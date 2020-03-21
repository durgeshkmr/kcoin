// Copyright (c) 2017-2020 The KCOIN developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ZKCNCONTROLDIALOG_H
#define ZKCNCONTROLDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "zkcn/zerocoin.h"

class CZerocoinMint;
class WalletModel;

namespace Ui {
class ZKcnControlDialog;
}

class CZKcnControlWidgetItem : public QTreeWidgetItem
{
public:
    explicit CZKcnControlWidgetItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type) {}
    explicit CZKcnControlWidgetItem(int type = Type) : QTreeWidgetItem(type) {}
    explicit CZKcnControlWidgetItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type) {}

    bool operator<(const QTreeWidgetItem &other) const;
};

class ZKcnControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ZKcnControlDialog(QWidget *parent);
    ~ZKcnControlDialog();

    void setModel(WalletModel* model);

    static std::set<std::string> setSelectedMints;
    static std::set<CMintMeta> setMints;
    static std::vector<CMintMeta> GetSelectedMints();

private:
    Ui::ZKcnControlDialog *ui;
    WalletModel* model;

    void updateList();
    void updateLabels();

    enum {
        COLUMN_CHECKBOX,
        COLUMN_DENOMINATION,
        COLUMN_PUBCOIN,
        COLUMN_VERSION,
        COLUMN_CONFIRMATIONS,
        COLUMN_ISSPENDABLE
    };
    friend class CZKcnControlWidgetItem;

private Q_SLOTS:
    void updateSelection(QTreeWidgetItem* item, int column);
    void ButtonAllClicked();
};

#endif // ZKCNCONTROLDIALOG_H
