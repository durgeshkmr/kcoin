// Copyright (c) 2020 The KCOIN developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "zerocoin_verify.h"

#include "chainparams.h"
#include "consensus/consensus.h"
#include "guiinterface.h"        // for ui_interface
#include "init.h"                // for ShutdownRequested()
#include "invalid.h"
#include "main.h"
#include "script/interpreter.h"
#include "spork.h"               // for sporkManager
#include "txdb.h"
#include "utilmoneystr.h"        // for FormatMoney

bool CheckZerocoinSpend(const CTransaction& tx, bool fVerifySignature, CValidationState& state, bool fFakeSerialAttack)
{
    //max needed non-mint outputs should be 2 - one for redemption address and a possible 2nd for change
    if (tx.vout.size() > 2) {
        int outs = 0;
        for (const CTxOut& out : tx.vout) {
            if (out.IsZerocoinMint())
                continue;
            outs++;
        }
        if (outs > 2 && !tx.IsCoinStake())
            return state.DoS(100, error("CheckZerocoinSpend(): over two non-mint outputs in a zerocoinspend transaction"));
    }

    //compute the txout hash that is used for the zerocoinspend signatures
    CMutableTransaction txTemp;
    for (const CTxOut& out : tx.vout) {
        txTemp.vout.push_back(out);
    }
    uint256 hashTxOut = txTemp.GetHash();

    bool fValidated = false;
    const Consensus::Params& consensus = Params().GetConsensus();
    std::set<CBigNum> serials;
    CAmount nTotalRedeemed = 0;
    for (const CTxIn& txin : tx.vin) {

        //only check txin that is a zcspend
        bool isPublicSpend = txin.IsZerocoinPublicSpend();
        if (!txin.IsZerocoinSpend() && !isPublicSpend)
            continue;

        libzerocoin::CoinSpend newSpend;
        CTxOut prevOut;
        if (isPublicSpend) {
            if(!GetOutput(txin.prevout.hash, txin.prevout.n, state, prevOut)){
                return state.DoS(100, error("CheckZerocoinSpend(): public zerocoin spend prev output not found, prevTx %s, index %d", txin.prevout.hash.GetHex(), txin.prevout.n));
            }
            libzerocoin::ZerocoinParams* params = consensus.Zerocoin_Params(false);
            PublicCoinSpend publicSpend(params);
            if (!ZKCNModule::parseCoinSpend(txin, tx, prevOut, publicSpend)){
                return state.DoS(100, error("CheckZerocoinSpend(): public zerocoin spend parse failed"));
            }
            newSpend = publicSpend;
        } else {
            newSpend = TxInToZerocoinSpend(txin);
        }

        //check that the denomination is valid
        if (newSpend.getDenomination() == libzerocoin::ZQ_ERROR)
            return state.DoS(100, error("Zerocoinspend does not have the correct denomination"));

        //check that denomination is what it claims to be in nSequence
        if (newSpend.getDenomination() != txin.nSequence)
            return state.DoS(100, error("Zerocoinspend nSequence denomination does not match CoinSpend"));

        //make sure the txout has not changed
        if (newSpend.getTxOutHash() != hashTxOut)
            return state.DoS(100, error("Zerocoinspend does not use the same txout that was used in the SoK"));

        if (isPublicSpend) {
            libzerocoin::ZerocoinParams* params = consensus.Zerocoin_Params(false);
            PublicCoinSpend ret(params);
            if (!ZKCNModule::validateInput(txin, prevOut, tx, ret)){
                return state.DoS(100, error("CheckZerocoinSpend(): public zerocoin spend did not verify"));
            }
        }

        if (serials.count(newSpend.getCoinSerialNumber()))
            return state.DoS(100, error("Zerocoinspend serial is used twice in the same tx"));
        serials.insert(newSpend.getCoinSerialNumber());

        //make sure that there is no over redemption of coins
        nTotalRedeemed += libzerocoin::ZerocoinDenominationToAmount(newSpend.getDenomination());
        fValidated = true;
    }

    if (!tx.IsCoinStake() && nTotalRedeemed < tx.GetValueOut()) {
        LogPrintf("redeemed = %s , spend = %s \n", FormatMoney(nTotalRedeemed), FormatMoney(tx.GetValueOut()));
        return state.DoS(100, error("Transaction spend more than was redeemed in zerocoins"));
    }

    return fValidated;
}

bool isBlockBetweenFakeSerialAttackRange(int nHeight)
{
    if (Params().NetworkID() != CBaseChainParams::MAIN)
        return false;

    return nHeight <= Params().GetConsensus().height_last_ZC_WrappedSerials;
}

bool CheckPublicCoinSpendEnforced(int blockHeight, bool isPublicSpend)
{
    if (blockHeight >= Params().GetConsensus().height_start_ZC_PublicSpends) {
        // reject old coin spend
        if (!isPublicSpend) {
            return error("%s: failed to add block with older zc spend version", __func__);
        }

    } else {
        if (isPublicSpend) {
            return error("%s: failed to add block, public spend enforcement not activated", __func__);
        }
    }
    return true;
}

int CurrentPublicCoinSpendVersion() {
    return sporkManager.IsSporkActive(SPORK_18_ZEROCOIN_PUBLICSPEND_V4) ? 4 : 3;
}

bool CheckPublicCoinSpendVersion(int version) {
    return version == CurrentPublicCoinSpendVersion();
}

bool ContextualCheckZerocoinSpend(const CTransaction& tx, const libzerocoin::CoinSpend* spend, int nHeight, const uint256& hashBlock)
{
    if(!ContextualCheckZerocoinSpendNoSerialCheck(tx, spend, nHeight, hashBlock)){
        return false;
    }

    //Reject serial's that are already in the blockchain
    int nHeightTx = 0;
    if (IsSerialInBlockchain(spend->getCoinSerialNumber(), nHeightTx))
        return error("%s : zKCN spend with serial %s is already in block %d\n", __func__,
                     spend->getCoinSerialNumber().GetHex(), nHeightTx);

    return true;
}

bool ContextualCheckZerocoinSpendNoSerialCheck(const CTransaction& tx, const libzerocoin::CoinSpend* spend, int nHeight, const uint256& hashBlock)
{
    const Consensus::Params& consensus = Params().GetConsensus();
    //Check to see if the zKCN is properly signed
    if (nHeight >= consensus.height_start_ZC_SerialsV2) {
        try {
            if (!spend->HasValidSignature())
                return error("%s: V2 zKCN spend does not have a valid signature\n", __func__);
        } catch (const libzerocoin::InvalidSerialException& e) {
            // Check if we are in the range of the attack
            if(!isBlockBetweenFakeSerialAttackRange(nHeight))
                return error("%s: Invalid serial detected, txid %s, in block %d\n", __func__, tx.GetHash().GetHex(), nHeight);
            else
                LogPrintf("%s: Invalid serial detected within range in block %d\n", __func__, nHeight);
        }

        libzerocoin::SpendType expectedType = libzerocoin::SpendType::SPEND;
        if (tx.IsCoinStake())
            expectedType = libzerocoin::SpendType::STAKE;
        if (spend->getSpendType() != expectedType) {
            return error("%s: trying to spend zKCN without the correct spend type. txid=%s\n", __func__,
                         tx.GetHash().GetHex());
        }
    }

    bool fUseV1Params = spend->getCoinVersion() < libzerocoin::PrivateCoin::PUBKEY_VERSION;

    //Reject serial's that are not in the acceptable value range
    if (!spend->HasValidSerial(consensus.Zerocoin_Params(fUseV1Params)))  {
        // Up until this block our chain was not checking serials correctly..
        if (!isBlockBetweenFakeSerialAttackRange(nHeight))
            return error("%s : zKCN spend with serial %s from tx %s is not in valid range\n", __func__,
                     spend->getCoinSerialNumber().GetHex(), tx.GetHash().GetHex());
        else
            LogPrintf("%s:: HasValidSerial :: Invalid serial detected within range in block %d\n", __func__, nHeight);
    }


    return true;
}

void AddWrappedSerialsInflation()
{
    const int height_end_attack = Params().GetConsensus().height_last_ZC_WrappedSerials;
    CBlockIndex* pindex = chainActive[height_end_attack];
    if (!pindex) return;
    const int chainHeight = chainActive.Height();
    if (pindex->nHeight > chainHeight) return;

    uiInterface.ShowProgress(_("Adding Wrapped Serials supply..."), 0);
    while (true) {
        if (pindex->nHeight % 1000 == 0) {
            LogPrintf("%s : block %d...\n", __func__, pindex->nHeight);
            int percent = std::max(1, std::min(99, (int)((double)(pindex->nHeight - height_end_attack) * 100 / (chainHeight - height_end_attack))));
            uiInterface.ShowProgress(_("Adding Wrapped Serials supply..."), percent);
        }

        // Add inflated denominations to block index mapSupply
        for (auto denom : libzerocoin::zerocoinDenomList) {
            pindex->mapZerocoinSupply.at(denom) += GetWrapppedSerialInflation(denom);
        }
        // Update current block index to disk
        assert(pblocktree->WriteBlockIndex(CDiskBlockIndex(pindex)));
        // next block
        if (pindex->nHeight < chainHeight)
            pindex = chainActive.Next(pindex);
        else
            break;
    }
    uiInterface.ShowProgress("", 100);
}

bool RecalculateKCNSupply(int nHeightStart, bool fSkipZkcn)
{
    const Consensus::Params& consensus = Params().GetConsensus();
    const int chainHeight = chainActive.Height();
    if (nHeightStart > chainHeight)
        return false;

    CBlockIndex* pindex = chainActive[nHeightStart];
    CAmount nSupplyPrev = pindex->pprev->nMoneySupply;
    if (nHeightStart == consensus.height_start_ZC)
        nSupplyPrev = CAmount(5449796547496199);

    uiInterface.ShowProgress(_("Recalculating KCN supply..."), 0);
    while (true) {
        if (pindex->nHeight % 1000 == 0) {
            LogPrintf("%s : block %d...\n", __func__, pindex->nHeight);
            int percent = std::max(1, std::min(99, (int)((double)((pindex->nHeight - nHeightStart) * 100) / (chainHeight - nHeightStart))));
            uiInterface.ShowProgress(_("Recalculating KCN supply..."), percent);
        }

        CBlock block;
        assert(ReadBlockFromDisk(block, pindex));

        CAmount nValueIn = 0;
        CAmount nValueOut = 0;
        for (const CTransaction& tx : block.vtx) {
            for (unsigned int i = 0; i < tx.vin.size(); i++) {
                if (tx.IsCoinBase())
                    break;

                if (tx.vin[i].IsZerocoinSpend()) {
                    nValueIn += tx.vin[i].nSequence * COIN;
                    continue;
                }

                COutPoint prevout = tx.vin[i].prevout;
                CTransaction txPrev;
                uint256 hashBlock;
                assert(GetTransaction(prevout.hash, txPrev, hashBlock, true));
                nValueIn += txPrev.vout[prevout.n].nValue;
            }

            for (unsigned int i = 0; i < tx.vout.size(); i++) {
                if (i == 0 && tx.IsCoinStake())
                    continue;

                nValueOut += tx.vout[i].nValue;
            }
        }

        // Rewrite money supply
        pindex->nMoneySupply = nSupplyPrev + nValueOut - nValueIn;
        nSupplyPrev = pindex->nMoneySupply;

        // Rewrite zkcn supply too
        if (!fSkipZkcn && pindex->nHeight >= consensus.height_start_ZC) {
            UpdateZKCNSupply(block, pindex, true);
        }

        // Add fraudulent funds to the supply and remove any recovered funds.
        if (pindex->nHeight == consensus.height_ZC_RecalcAccumulators) {
            const CAmount nInvalidAmountFiltered = 268200*COIN;    //Amount of invalid coins filtered through exchanges, that should be considered valid
            LogPrintf("%s : Original money supply=%s\n", __func__, FormatMoney(pindex->nMoneySupply));

            pindex->nMoneySupply += nInvalidAmountFiltered;
            LogPrintf("%s : Adding filtered funds to supply + %s : supply=%s\n", __func__, FormatMoney(nInvalidAmountFiltered), FormatMoney(pindex->nMoneySupply));

            CAmount nLocked = GetInvalidUTXOValue();
            pindex->nMoneySupply -= nLocked;
            LogPrintf("%s : Removing locked from supply - %s : supply=%s\n", __func__, FormatMoney(nLocked), FormatMoney(pindex->nMoneySupply));
        }

        assert(pblocktree->WriteBlockIndex(CDiskBlockIndex(pindex)));

        // Stop if shutdown was requested
        if (ShutdownRequested()) return false;

        if (pindex->nHeight < chainHeight)
            pindex = chainActive.Next(pindex);
        else
            break;
    }
    uiInterface.ShowProgress("", 100);
    return true;
}

bool UpdateZKCNSupply(const CBlock& block, CBlockIndex* pindex, bool fJustCheck)
{
    const Consensus::Params& consensus = Params().GetConsensus();
    if (pindex->nHeight < consensus.height_start_ZC)
        return true;

    //Reset the supply to previous block
    pindex->mapZerocoinSupply = pindex->pprev->mapZerocoinSupply;

    //Add mints to zKCN supply (mints are forever disabled after last checkpoint)
    if (pindex->nHeight < consensus.height_last_ZC_AccumCheckpoint) {
        std::list<CZerocoinMint> listMints;
        std::set<uint256> setAddedToWallet;
        BlockToZerocoinMintList(block, listMints, true);
        for (const auto& m : listMints) {
            pindex->mapZerocoinSupply.at(m.GetDenomination())++;
            //Remove any of our own mints from the mintpool
            if (!fJustCheck && pwalletMain) {
                if (pwalletMain->IsMyMint(m.GetValue())) {
                    pwalletMain->UpdateMint(m.GetValue(), pindex->nHeight, m.GetTxHash(), m.GetDenomination());
                    // Add the transaction to the wallet
                    for (auto& tx : block.vtx) {
                        uint256 txid = tx.GetHash();
                        if (setAddedToWallet.count(txid))
                            continue;
                        if (txid == m.GetTxHash()) {
                            CWalletTx wtx(pwalletMain, tx);
                            wtx.nTimeReceived = block.GetBlockTime();
                            wtx.SetMerkleBranch(block);
                            pwalletMain->AddToWallet(wtx, false, nullptr);
                            setAddedToWallet.insert(txid);
                        }
                    }
                }
            }
        }
    }

    //Remove spends from zKCN supply
    std::list<libzerocoin::CoinDenomination> listDenomsSpent = ZerocoinSpendListFromBlock(block, true);
    for (const auto& denom : listDenomsSpent) {
        pindex->mapZerocoinSupply.at(denom)--;
        // zerocoin failsafe
        if (pindex->mapZerocoinSupply.at(denom) < 0)
            return error("Block contains zerocoins that spend more than are in the available supply to spend");
    }

    // Update Wrapped Serials amount
    // A one-time event where only the zKCN supply was off (due to serial duplication off-chain on main net)
    if (Params().NetworkID() == CBaseChainParams::MAIN && pindex->nHeight == consensus.height_last_ZC_WrappedSerials + 1
            && pindex->GetZerocoinSupply() < consensus.ZC_WrappedSerialsSupply + GetWrapppedSerialInflationAmount()) {
        for (const auto& denom : libzerocoin::zerocoinDenomList)
            pindex->mapZerocoinSupply.at(denom) += GetWrapppedSerialInflation(denom);
    }

    for (const auto& denom : libzerocoin::zerocoinDenomList)
        LogPrint("zero", "%s coins for denomination %d pubcoin %s\n", __func__, denom, pindex->mapZerocoinSupply.at(denom));

    return true;
}

CAmount GetInvalidUTXOValue()
{
    CAmount nValue = 0;
    for (auto out : invalid_out::setInvalidOutPoints) {
        bool fSpent = false;
        CCoinsViewCache cache(pcoinsTip);
        const CCoins *coins = cache.AccessCoins(out.hash);
        if(!coins || !coins->IsAvailable(out.n))
            fSpent = true;

        if (!fSpent)
            nValue += coins->vout[out.n].nValue;
    }

    return nValue;
}
