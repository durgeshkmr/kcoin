// Copyright (c) 2019 The KCOIN developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COINCONTROLKCNWIDGET_H
#define COINCONTROLKCNWIDGET_H

#include <QDialog>

namespace Ui {
class CoinControlKcnWidget;
}

class CoinControlKcnWidget : public QDialog
{
    Q_OBJECT

public:
    explicit CoinControlKcnWidget(QWidget *parent = nullptr);
    ~CoinControlKcnWidget();

private:
    Ui::CoinControlKcnWidget *ui;
};

#endif // COINCONTROLKCNWIDGET_H
