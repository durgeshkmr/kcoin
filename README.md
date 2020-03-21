KCOIN Core integration/staging repository
=====================================

[![Build Status](https://travis-ci.org/KCOIN-Project/KCOIN.svg?branch=master)](https://travis-ci.org/KCOIN-Project/KCOIN) [![GitHub version](https://badge.fury.io/gh/KCOIN-Project%2FKCOIN.svg)](https://badge.fury.io/gh/KCOIN-Project%2FKCOIN)

KCOIN is an open source crypto-currency focused on fast private transactions using the Zerocoin protocol, with low transaction fees & environmental footprint.  It utilizes the first ever anonymous proof of stake protocol, called zPoS, combined with regular PoS and masternodes for securing its network. zPoS incentivises using the privacy features available in KCOIN by granting a higher block reward for zPoS over regular PoS and masternodes. In practice KCOIN has between 4 to 10 times higher use of it's privacy features in comparison to other coins that combine public and private transactions. This is thanks to innovations like zPoS and integrating the Zerocoin protocol into light/mobile wallets, allowing for a complete and lightweight privacy protocol that can be used on the go. As well as security improvements like [deterministic generation of zKCN for easy backups.](https://www.reddit.com/r/kcoin/comments/8gbjf7/how_to_use_deterministic_zerocoin_generation/)
The goal of KCOIN is to achieve a decentralized sustainable crypto currency with near instant full-time private transactions, fair governance and community intelligence.
- Anonymized transactions & consensus using the [_Zerocoin Protocol_](http://www.kcoin.org/zkcn) and [zPoS](https://kcoin.org/zpos/).
- light/mobile wallet privacy using the [Zerocoin Light Node Protocol](https://kcoin.org/wp-content/uploads/2018/11/Zerocoin_Light_Node_Protocol.pdf)
- Fast transactions featuring guaranteed zero confirmation transactions, we call it _SwiftX_.
- Decentralized blockchain voting utilizing Masternode technology to form a DAO. The blockchain will distribute monthly treasury funds based on successful proposals submitted by the community and voted on by the DAO.

More information at [kcoin.org](http://www.kcoin.org) Visit our ANN thread at [BitcoinTalk](http://www.bitcointalk.org/index.php?topic=1262920)

### Coin Specs
<table>
<tr><td>Algo</td><td>Quark</td></tr>
<tr><td>Block Time</td><td>60 Seconds</td></tr>
<tr><td>Difficulty Retargeting</td><td>Every Block</td></tr>
<tr><td>Max Coin Supply (PoW Phase)</td><td>43,199,500 KCN</td></tr>
<tr><td>Max Coin Supply (PoS Phase)</td><td>Infinite</td></tr>
<tr><td>Premine</td><td>60,000 KCN*</td></tr>
</table>

*60,000 KCN Premine was burned in block [279917](http://www.presstab.pw/phpexplorer/KCOIN/block.php?blockhash=206d9cfe859798a0b0898ab00d7300be94de0f5469bb446cecb41c3e173a57e0)

### Reward Distribution

<table>
<th colspan=4>Genesis Block</th>
<tr><th>Block Height</th><th>Reward Amount</th><th>Notes</th></tr>
<tr><td>1</td><td>60,000 KCN</td><td>Initial Pre-mine, burnt in block <a href="http://www.presstab.pw/phpexplorer/KCOIN/block.php?blockhash=206d9cfe859798a0b0898ab00d7300be94de0f5469bb446cecb41c3e173a57e0">279917</a></td></tr>
</table>

### PoW Rewards Breakdown

<table>
<th>Block Height</th><th>Masternodes</th><th>Miner</th><th>Budget</th>
<tr><td>2-43200</td><td>20% (50 KCN)</td><td>80% (200 KCN)</td><td>N/A</td></tr>
<tr><td>43201-151200</td><td>20% (50 KCN)</td><td>70% (200 KCN)</td><td>10% (25 KCN)</td></tr>
<tr><td>151201-259200</td><td>45% (22.5 KCN)</td><td>45% (22.5 KCN)</td><td>10% (5 KCN)</td></tr>
</table>

### PoS Rewards Breakdown

<table>
<th>Phase</th><th>Block Height</th><th>Reward</th><th>Masternodes & Stakers</th><th>Budget</th>
<tr><td>Phase 0</td><td>259201-302399</td><td>50 KCN</td><td>90% (45 KCN)</td><td>10% (5 KCN)</td></tr>
<tr><td>Phase 1</td><td>302400-345599</td><td>45 KCN</td><td>90% (40.5 KCN)</td><td>10% (4.5 KCN)</td></tr>
<tr><td>Phase 2</td><td>345600-388799</td><td>40 KCN</td><td>90% (36 KCN)</td><td>10% (4 KCN)</td></tr>
<tr><td>Phase 3</td><td>388800-431999</td><td>35 KCN</td><td>90% (31.5 KCN)</td><td>10% (3.5 KCN)</td></tr>
<tr><td>Phase 4</td><td>432000-475199</td><td>30 KCN</td><td>90% (27 KCN)</td><td>10% (3 KCN)</td></tr>
<tr><td>Phase 5</td><td>475200-518399</td><td>25 KCN</td><td>90% (22.5 KCN)</td><td>10% (2.5 KCN)</td></tr>
<tr><td>Phase 6</td><td>518400-561599</td><td>20 KCN</td><td>90% (18 KCN)</td><td>10% (2 KCN)</td></tr>
<tr><td>Phase 7</td><td>561600-604799</td><td>15 KCN</td><td>90% (13.5 KCN)</td><td>10% (1.5 KCN)</td></tr>
<tr><td>Phase 8</td><td>604800-647999</td><td>10 KCN</td><td>90% (9 KCN)</td><td>10% (1 KCN)</td></tr>
<tr><td>Phase 9</td><td>648000-1154203</td><td>5 KCN</td><td>90% (4.5 KCN)</td><td>10% (0.5 KCN)</td></tr>
<tr><td>Phase X</td><td>1154203-∞</td><td>6 KCN</td><td>84% (5 KCN/zKCN)</td><td>16% (1 KCN)</td></tr>
</table>
