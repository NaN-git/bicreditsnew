// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Copyright (c) 2013-2014 Memorycoin Dev Team

#include "votecoinsdialog.h"
#include "ui_votecoinsdialog.h"

#include "walletmodel.h"
#include "bitcreditunits.h"
#include "addressbookpage.h"
#include "optionsmodel.h"
#include "votecoinsentry.h"
#include "guiutil.h"
#include "askpassphrasedialog.h"
#include "base58.h"
#include "clientmodel.h"

#include <QMessageBox>
#include <QTextDocument>
#include <QScrollBar>

VoteCoinsDialog::VoteCoinsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VoteCoinsDialog),
    model(0)
{
    ui->setupUi(this);
#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->addButton->setIcon(QIcon());
    ui->clearButton->setIcon(QIcon());
    ui->sendButton->setIcon(QIcon());
#endif

    addEntry();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    fNewRecipientAllowed = true;
    
    this->setStyleSheet("background-image:url(:/images/background);");
}

void VoteCoinsDialog::setModel(WalletModel *model)
{
    this->model = model;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        VoteCoinsEntry *entry = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setModel(model);
        }
    }
    if(model && model->getOptionsModel())
    {
        //setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        //connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64)));
        //connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
    }
}


VoteCoinsDialog::~VoteCoinsDialog()
{
    delete ui;
}

void VoteCoinsDialog::checkSweep(){
   
        ui->sendButton->setEnabled(true);
        ui->sweepLabel->setVisible(false);
        

}

void VoteCoinsDialog::sendToRecipients(bool sweep, qint64 sweepFee){

    QList<SendCoinsRecipient> recipients;

        bool valid = true;

        if(!model)
            return;

        for(int i = 0; i < ui->entries->count(); ++i)
        {
            VoteCoinsEntry *entry = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(i)->widget());
            if(entry)
            {
                if(entry->validate())
                {
                    recipients.append(entry->getValue());
                }
                else
                {
                    valid = false;
                }
            }
        }

        if(!valid || recipients.isEmpty())
        {
            return;
        }
    

    // Format confirmation message
    QStringList formatted;
    foreach(const SendCoinsRecipient &rcp, recipients)
    {
     #if QT_VERSION >= 0x050000
        formatted.append(tr("<b>%1</b> to %2 (%3)").arg(BitcreditUnits::formatWithUnit(BitcreditUnits::BCR, rcp.amount), rcp.label.toHtmlEscaped(), rcp.address));
     #else
        formatted.append(tr("<b>%1</b> to %2 (%3)").arg(BitcreditUnits::formatWithUnit(BitcreditUnits::BCR, rcp.amount), Qt::escape(rcp.label), rcp.address));
     #endif
    }

    fNewRecipientAllowed = false;

    WalletModel::UnlockContext ctx(model->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
        return;
    }

    WalletModelTransaction currentTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus;

	processSendCoinsReturn(prepareStatus,
        BitcreditUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), currentTransaction.getTransactionFee()));

    WalletModel::SendCoinsReturn sendstatus = model->sendCoins(currentTransaction);
    processSendCoinsReturn(sendstatus);

    if (sendstatus.status == WalletModel::OK)
    {
        accept();
 
    }
    
    fNewRecipientAllowed = true;
}

void VoteCoinsDialog::on_sweepButton_clicked(){

    sendToRecipients(true,0);

}

void VoteCoinsDialog::on_sendButton_clicked()
{
    sendToRecipients(false,0);
}

void VoteCoinsDialog::clear()
{
    // Remove entries until only one left
    while(ui->entries->count())
    {
        delete ui->entries->takeAt(0)->widget();
    }
    addEntry();

    updateRemoveEnabled();

    ui->sendButton->setDefault(true);
}

void VoteCoinsDialog::reject()
{
    clear();
}

void VoteCoinsDialog::accept()
{
    clear();
}

VoteCoinsEntry *VoteCoinsDialog::addEntry()
{
    VoteCoinsEntry *entry = new VoteCoinsEntry(this);
    entry->setModel(model);
    ui->entries->addWidget(entry);
    connect(entry, SIGNAL(removeEntry(VoteCoinsEntry*)), this, SLOT(removeEntry(VoteCoinsEntry*)));

    updateRemoveEnabled();

    // Focus the field, so that entry can start immediately
    entry->clear();
    entry->setFocus();
    //ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->sizeHint());
    qApp->processEvents();
    //QScrollBar* bar = ui->scrollArea->verticalScrollBar();
    //if(bar)
    //    bar->setSliderPosition(bar->maximum());
    return entry;
}

void VoteCoinsDialog::updateRemoveEnabled()
{
    // Remove buttons are enabled as soon as there is more than one send-entry
    bool enabled = (ui->entries->count() > 1);
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        VoteCoinsEntry *entry = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setRemoveEnabled(enabled);
        }
    }
    //setupTabChain(0);
}

void VoteCoinsDialog::removeEntry(VoteCoinsEntry* entry)
{
    delete entry;
    updateRemoveEnabled();
}

/*QWidget *VoteCoinsDialog::setupTabChain(QWidget *prev)
{
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        VoteCoinsEntry *entry = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            prev = entry->setupTabChain(prev);
        }
    }
    QWidget::setTabOrder(prev, ui->addButton);
    QWidget::setTabOrder(ui->addButton, ui->sendButton);
    return ui->sendButton;
}*/

void VoteCoinsDialog::setAddress(const QString &address)
{
    VoteCoinsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        VoteCoinsEntry *first = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setAddress(address);
}

void VoteCoinsDialog::pasteEntry(const SendCoinsRecipient &rv)
{
    if(!fNewRecipientAllowed)
        return;

    VoteCoinsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        VoteCoinsEntry *first = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setValue(rv);
}

void VoteCoinsDialog::processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg)
{
    QPair<QString, CClientUIInterface::MessageBoxFlags> msgParams;
    // Default to a warning message, override if error message is needed
    msgParams.second = CClientUIInterface::MSG_WARNING;

    // This comment is specific to SendCoinsDialog usage of WalletModel::SendCoinsReturn.
    // WalletModel::TransactionCommitFailed is used only in WalletModel::sendCoins()
    // all others are used only in WalletModel::prepareTransaction()
    switch(sendCoinsReturn.status)
    {
    case WalletModel::InvalidAddress:
        msgParams.first = tr("The recipient address is not valid, please recheck.");
        break;
    case WalletModel::InvalidAmount:
        msgParams.first = tr("The amount to pay must be larger than 0.");
        break;
    case WalletModel::AmountExceedsBalance:
        msgParams.first = tr("The amount exceeds your balance.");
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        msgParams.first = tr("The total exceeds your balance when the %1 transaction fee is included.").arg(msgArg);
        break;
    case WalletModel::DuplicateAddress:
        msgParams.first = tr("Duplicate address found, can only send to each address once per send operation.");
        break;
    case WalletModel::TransactionCreationFailed:
        msgParams.first = tr("Transaction creation failed!");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::TransactionCommitFailed:
        msgParams.first = tr("The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::AnonymizeOnlyUnlocked:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("Error: The wallet was unlocked only to anonymize coins."),
            QMessageBox::Ok, QMessageBox::Ok);
		break;
    case WalletModel::InsaneFee:
        msgParams.first = tr("A fee higher than %1 is considered an insanely high fee.").arg(BitcreditUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), 10000000));
        break;
    // included to prevent a compiler warning.
    case WalletModel::OK:
    default:
        return;
    }

    //emit message(tr("Send Coins"), msgParams.first, msgParams.second);
}
