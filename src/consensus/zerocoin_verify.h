// Copyright (c) 2020 The KCOIN developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KCOIN_CONSENSUS_ZEROCOIN_VERIFY_H
#define KCOIN_CONSENSUS_ZEROCOIN_VERIFY_H

#include "consensus/consensus.h"
#include "main.h"
#include "script/interpreter.h"
#include "zkcnchain.h"

/** Context-independent validity checks */
bool CheckZerocoinSpend(const CTransaction& tx, bool fVerifySignature, CValidationState& state, bool fFakeSerialAttack = false);
// Fake Serial attack Range
bool isBlockBetweenFakeSerialAttackRange(int nHeight);
// Public coin spend
bool CheckPublicCoinSpendEnforced(int blockHeight, bool isPublicSpend);
int CurrentPublicCoinSpendVersion();
bool CheckPublicCoinSpendVersion(int version);
bool ContextualCheckZerocoinSpend(const CTransaction& tx, const libzerocoin::CoinSpend* spend, int nHeight, const uint256& hashBlock);
bool ContextualCheckZerocoinSpendNoSerialCheck(const CTransaction& tx, const libzerocoin::CoinSpend* spend, int nHeight, const uint256& hashBlock);
void AddWrappedSerialsInflation();
bool RecalculateKCNSupply(int nHeightStart, bool fSkipZkcn = true);
bool UpdateZKCNSupply(const CBlock& block, CBlockIndex* pindex, bool fJustCheck);
CAmount GetInvalidUTXOValue();

#endif //KCOIN_CONSENSUS_ZEROCOIN_VERIFY_H
