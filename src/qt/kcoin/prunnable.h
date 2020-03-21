// Copyright (c) 2019 The KCOIN developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KCOIN_CORE_NEW_GUI_PRUNNABLE_H
#define KCOIN_CORE_NEW_GUI_PRUNNABLE_H

class Runnable {
public:
    virtual void run(int type) = 0;
    virtual void onError(QString error, int type) = 0;
};

#endif //KCOIN_CORE_NEW_GUI_PRUNNABLE_H
