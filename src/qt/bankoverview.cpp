<<<<<<< HEAD
#include "bankoverview.h"
#include "ui_bankoverview.h"

#include "walletmodel.h"
#include "bitcreditunits.h"
#include "addressbookpage.h"
#include "optionsmodel.h"
#include "guiutil.h"
#include "askpassphrasedialog.h"
#include "base58.h"
#include "walletframe.h"

#include <QMessageBox>
#include <QTextDocument>
#include <QScrollBar>

BankOverview::BankOverview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BankOverview),
    model(0)
=======
// Copyright (c) 2011-2013 The Bitcredit Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bankoverview.h"
#include "ui_bankoverview.h"

#include "addressbookpage.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "walletmodel.h"

#include "bitcreditunits.h"
#include "clientmodel.h"
#include "bankcoinsentry.h"
#include "base58.h"
#include "ui_interface.h"
#include "wallet.h"
#include "bankstatisticspage.h"
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>
#include <QTextDocument>

#include <QApplication>
#include <QClipboard>

#include "qvalidatedlineedit.h"
#include "qvaluecombobox.h"

BankOverview::BankOverview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BankOverview),
    clientModel(0),
    model(0),
    walletbalance(-1),
    walletbalancebtc(-1),
    walletbalancefiat(-1),
    bankcredit(-1),
    bankcreditbtc(-1),
    bankcreditfiat(-1),
	grantbalance(-1),
	grantbalancebtc(-1),
    grantbalancefiat(-1),
    totalbalance(-1),
	totalbalancebtc(-1),
	totalbalancefiat(-1),
	trustrating(-1),
	creditscore(-1)
>>>>>>> origin/master2
{
    ui->setupUi(this);
#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->clearButton->setIcon(QIcon());
    ui->sendButton->setIcon(QIcon());
#endif
<<<<<<< HEAD

    addEntry();

    //connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    fNewRecipientAllowed = true;
=======
	
    
	setFocusProxy(ui->payAmount);
    
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    fNewRecipientAllowed = true;
    
    this->setStyleSheet("background-image:url(:/images/background);");

    connect(ui->payAmount, SIGNAL(valueChanged()), this, SIGNAL(payAmountChanged()));
}


void BankOverview::clear()
{
    ui->payAmount->clear();

    updateDisplayUnit();
}

bool BankOverview::validate()
{
    if (!model)
        return false;

    // Check input validity
    bool retval = true;

    // Skip checks for payment request
    if (recipient.paymentRequest.IsInitialized())
        return retval;

    return retval;
}



SendCoinsRecipient BankOverview::getValue()
{
    
    SendCoinsRecipient rv;

	if (ui->payTo->currentIndex()+1 ==0)
	{
		rv.address = "6DEVCRjNBJzgoALxYAZ87a4Td25D53WnHR";
		rv.label = "Development Bank";  
		ui->payAmount->setValue(100000000000); 
		rv.amount = 10000000000000;
	}

	else if (ui->payTo->currentText() == "Grant Reserve")
	{
		rv.address = "6AtYqDFdDNN6WDKa9cRcqxj9rvMJ6B4mZn";
		rv.label = "Grant Reserve"; 
		ui->payAmount->setValue(100000000000);
		rv.amount = 10000000000000;
	}	
    
    return rv;
}

void BankOverview::setValue(const SendCoinsRecipient &value)
{
    ui->payAmount->setValue(value.amount);
}

void BankOverview::setFocus()
{
    ui->payAmount->setFocus();
>>>>>>> origin/master2
}

void BankOverview::setModel(WalletModel *model)
{
    this->model = model;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
<<<<<<< HEAD
        VoteCoinsEntry *entry = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(i)->widget());
=======
        BankOverview *entry = qobject_cast<BankOverview*>(ui->entries->itemAt(i)->widget());
>>>>>>> origin/master2
        if(entry)
        {
            entry->setModel(model);
        }
    }
<<<<<<< HEAD
    if(model && model->getOptionsModel())
    {
        //setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        //connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64)));
        //connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
    }
=======
>>>>>>> origin/master2
}


BankOverview::~BankOverview()
{
    delete ui;
}

<<<<<<< HEAD
/*void BankOverview::checkSweep(){
    if(model->NeedsSweep()){
        ui->sendButton->setEnabled(false);
        ui->sweepLabel->setVisible(true);
        ui->sweepButton->setVisible(true);
    }else{
        ui->sendButton->setEnabled(true);
        ui->sweepLabel->setVisible(false);
        ui->sweepButton->setVisible(false);
    }
}*/




void BankOverview::sendToRecipients(){

    QList<SendCoinsRecipient> recipients;

    bool valid = true;
    int64 totalPlay=0;

    if(!model)
        return;

        VoteCoinsEntry *entry = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(0)->widget());
        if(entry){
            if(entry->getGameType()==0){
                //Lottery
                if(entry->validate()){
                    for(int i=0;i<7;i++){
                        recipients.append(entry->getValue(i));
                        totalPlay+=recipients[i].amount;
                    }
                }
                else{
                    valid = false;
                }
            }else if(entry->getGameType()==1){
                //Dice
                if(entry->validateDice()){
                    //for(int i=0;i<2;i++){
                        //recipients.append(entry->getValue(i));
                        //totalPlay+=recipients[i].amount;
                    //}
                    recipients.append(entry->getDiceGame());
                    recipients.append(entry->getDiceAmount());
                    totalPlay+=recipients[0].amount;
                    totalPlay+=recipients[1].amount;

                }
                else{
                    valid = false;
                }
            }
        }


        if(!valid || recipients.isEmpty())
        {
            return;
        }

    // Format confirmation message
    /*QStringList formatted;
    foreach(const SendCoinsRecipient &rcp, recipients)
    {
     #if QT_VERSION >= 0x050000
        formatted.append(tr("%1 ").arg(rcp.amount));
     #else
        formatted.append(tr("%1 ").arg(rcp.amount));
     #endif
    }*/

    fNewRecipientAllowed = false;

    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm ticket"),
                          tr("Are you sure you want to play the amount %1 for this ticket?").arg(BitcreditUnits::formatWithUnit(BitcreditUnits::BTC, totalPlay)),
          QMessageBox::Yes|QMessageBox::Cancel,
          QMessageBox::Cancel);

    if(retval != QMessageBox::Yes)
    {
        fNewRecipientAllowed = true;
        return;
    }

=======
void BankOverview::on_sendButton_clicked()
{
    if(!model || !model->getOptionsModel())
        return;

    QList<SendCoinsRecipient> recipients;
    bool valid = true;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        BankOverview *entry = qobject_cast<BankOverview*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            if(entry->validate())
            {
				int index;
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
        // generate bold amount string
        QString amount = "<b>" + BitcreditUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), rcp.amount);
        amount.append("</b>");
        // generate monospace address string
        QString address = "<span style='font-family: monospace;'>" + rcp.address;
        address.append("</span>");

        QString recipientElement;

        if (!rcp.paymentRequest.IsInitialized()) // normal payment
        {
            if(rcp.label.length() > 0) // label with address
            {
                recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.label));
                recipientElement.append(QString(" (%1)").arg(address));
            }
            else // just address
            {
                recipientElement = tr("%1 to %2").arg(amount, address);
            }
        }
        else if(!rcp.authenticatedMerchant.isEmpty()) // secure payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.authenticatedMerchant));
        }
        else // insecure payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, address);
        }

        formatted.append(recipientElement);
    }

    fNewRecipientAllowed = false;


>>>>>>> origin/master2
    WalletModel::UnlockContext ctx(model->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
        return;
    }

<<<<<<< HEAD
    WalletModel::SendCoinsReturn sendstatus;

    if(entry->getGameType()==0){
        sendstatus= model->sendCoins(recipients, NULL, true,false);
        if(sendstatus.status==WalletModel::SatoshiForChangeAddressRequired){
            //try again
            if(recipients[6].amount>2){
                recipients[6].amount=recipients[6].amount-1;
                sendstatus = model->sendCoins(recipients, NULL, true, false);
            }
        }
    }else if(entry->getGameType()==1){
        sendstatus= model->sendCoins(recipients, NULL, false, true);
        if(sendstatus.status==WalletModel::SatoshiForChangeAddressRequired){
            //try again
            if(recipients[1].amount>2){
                recipients[1].amount=recipients[1].amount-1;
                sendstatus = model->sendCoins(recipients, NULL, false, true);
            }
        }
    }
    switch(sendstatus.status)
    {
    case WalletModel::InvalidAddress:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The recipient address is not valid, please recheck."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::InvalidAmount:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The amount to pay must be larger than 0."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountExceedsBalance:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The amount exceeds your balance."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;

    case WalletModel::SatoshiForChangeAddressRequired:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("This exact amount cannot be played from your wallet at this time (for boring technical reasons). Try changing the amount to 1 satoshi more or less."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;


    case WalletModel::AmountWithFeeExceedsBalance:
        /*if(sweep){
            fNewRecipientAllowed = true;
            sendToRecipients(true,sendstatus.fee);
            break;
        }*/
        QMessageBox::warning(this, tr("Send Coins"),
            tr("The total exceeds your balance when the %1 transaction fee is included.").
            arg(BitcreditUnits::formatWithUnit(BitcreditUnits::BTC, sendstatus.fee)),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::DuplicateAddress:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("Duplicate address found, can only send to each address once per send operation."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCreationFailed:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("Error: Transaction creation failed!"),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCommitFailed:
        QMessageBox::warning(this, tr("Send Coins"),
            tr("Error: The transaction was rejected. This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::Aborted: // User aborted, nothing to do
        break;
    case WalletModel::OK:
        accept();
        break;
    }
    fNewRecipientAllowed = true;
}

void BankOverview::on_sweepButton_clicked(){

    //sendToRecipients(true,0);

}

void BankOverview::on_sendButton_clicked()
{
    sendToRecipients();
}

void BankOverview::clear()
{
    // Remove entries until only one left
    while(ui->entries->count())
    {
        delete ui->entries->takeAt(0)->widget();
    }
    addEntry();

    updateRemoveEnabled();

    ui->sendButton->setDefault(true);

    // prepare transaction for getting txFee earlier
    WalletModelTransaction currentTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus;
    prepareStatus = model->prepareTransaction(currentTransaction);

    // process prepareStatus and on error generate message shown to user
   // processSendCoinsReturn(prepareStatus,
       // BitcreditUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), currentTransaction.getTransactionFee()));

    if(prepareStatus.status != WalletModel::OK) {
        fNewRecipientAllowed = true;
        return;
    }

    CAmount txFee = currentTransaction.getTransactionFee();
    QString questionString = tr("Are you sure you want to send?");
    questionString.append("<br /><br />%1");

    if(txFee > 0)
    {
        // append fee string if a fee is required
        questionString.append("<hr /><span style='color:#aa0000;'>");
        questionString.append(BitcreditUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), txFee));
        questionString.append("</span> ");
        questionString.append(tr("added as transaction fee"));

        // append transaction size
        questionString.append(" (" + QString::number((double)currentTransaction.getTransactionSize() / 1000) + " kB)");
    }

    // add total amount in all subdivision units
    questionString.append("<hr />");
    CAmount totalAmount = currentTransaction.getTotalTransactionAmount() + txFee;
    QStringList alternativeUnits;
    foreach(BitcreditUnits::Unit u, BitcreditUnits::availableUnits())
    {
        if(u != model->getOptionsModel()->getDisplayUnit())
            alternativeUnits.append(BitcreditUnits::formatHtmlWithUnit(u, totalAmount));
    }
    questionString.append(tr("Total Amount %1 (= %2)")
        .arg(BitcreditUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), totalAmount))
        .arg(alternativeUnits.join(" " + tr("or") + " ")));

    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send coins"),
        questionString.arg(formatted.join("<br />")),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if(retval != QMessageBox::Yes)
    {
        fNewRecipientAllowed = true;
        return;
    }

    // now send the prepared transaction
    WalletModel::SendCoinsReturn sendStatus = model->sendCoins(currentTransaction);
    // process sendStatus and on error generate message shown to user

        accept();

    fNewRecipientAllowed = true;

}

void BankOverview::reject()
{
    clear();
}

void BankOverview::accept()
{
    clear();
}

<<<<<<< HEAD
VoteCoinsEntry *BankOverview::addEntry()
{
    VoteCoinsEntry *entry = new VoteCoinsEntry(this);
    entry->setModel(model);
    ui->entries->addWidget(entry);
    connect(entry, SIGNAL(removeEntry(VoteCoinsEntry*)), this, SLOT(removeEntry(VoteCoinsEntry*)));

    updateRemoveEnabled();

    // Focus the field, so that entry can start immediately
    //entry->clear();
    entry->setFocus();
    //ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->sizeHint());
    qApp->processEvents();
    //QScrollBar* bar = ui->scrollArea->verticalScrollBar();
    //if(bar)
    //    bar->setSliderPosition(bar->maximum());
    return entry;
}

void BankOverview::updateRemoveEnabled()
{
    // Remove buttons are enabled as soon as there is more than one send-entry
    bool enabled = (ui->entries->count() > 1);
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        VoteCoinsEntry *entry = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            //entry->setRemoveEnabled(enabled);
        }
    }
    //setupTabChain(0);
}

void BankOverview::removeEntry(VoteCoinsEntry* entry)
{
    delete entry;
    updateRemoveEnabled();
}

/*QWidget *BankOverview::setupTabChain(QWidget *prev)
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

void BankOverview::setAddress(const QString &address)
{
    VoteCoinsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        VoteCoinsEntry *first = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(0)->widget());
        /*if(first->isClear())
        {
            entry = first;
        }*/
    }
    if(!entry)
    {
        entry = addEntry();
=======

void BankOverview::setAddress(const QString &address)
{
    BankOverview *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        BankOverview *first = qobject_cast<BankOverview*>(ui->entries->itemAt(0)->widget());
        
>>>>>>> origin/master2
    }

    entry->setAddress(address);
}

<<<<<<< HEAD
void BankOverview::pasteEntry(const SendCoinsRecipient &rv)
{
    if(!fNewRecipientAllowed)
        return;

    VoteCoinsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        VoteCoinsEntry *first = qobject_cast<VoteCoinsEntry*>(ui->entries->itemAt(0)->widget());
        /*if(first->isClear())
        {
            entry = first;
        }*/
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setValue(rv);
}

bool BankOverview::handleURI(const QString &uri)
{
    SendCoinsRecipient rv;
    // URI has to be valid
    if (GUIUtil::parseBitcreditURI(uri, &rv))
    {
        CBitcreditAddress address(rv.address.toStdString());
        if (!address.IsValid())
            return false;
        pasteEntry(rv);
        return true;
    }

    return false;
}

void BankOverview::setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    Q_UNUSED(unconfirmedBalance);
    Q_UNUSED(immatureBalance);
    if(!model || !model->getOptionsModel())
        return;

    int unit = model->getOptionsModel()->getDisplayUnit();
    ui->labelBalance->setText(BitcreditUnits::formatWithUnit(unit, balance));
=======
void BankOverview::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
                                 const CAmount& watchBalance, const CAmount& watchUnconfirmedBalance, const CAmount& watchImmatureBalance)
{
    Q_UNUSED(unconfirmedBalance);
    Q_UNUSED(immatureBalance);
    Q_UNUSED(watchBalance);
    Q_UNUSED(watchUnconfirmedBalance);
    Q_UNUSED(watchImmatureBalance);

    if(model && model->getOptionsModel())
    {
        ui->labelwallettotal->setText(BitcreditUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), balance));
    }
>>>>>>> origin/master2
}

void BankOverview::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update labelBalance with the current balance and the current unit
<<<<<<< HEAD
        ui->labelBalance->setText(BitcreditUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), model->getBalance()));
    }
}

=======
        ui->labelwallettotal->setText(BitcreditUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), model->getBalance()));
    }
}



>>>>>>> origin/master2
