#ifndef STATS_H
#define STATS_H

#include <iostream>
#include <QObject>
#include <math.h>
#include "util.h"
#include "init.h"
#include "wallet.h"
#include "main.h"
#include "coins.h"
#include "rpcserver.h"


using namespace std;

int onemonth = 604800 *4;

class Stats : public QObject
{
  public:
  
	int totalnumtx()  //total number of chain transactions
	{
		int ttnmtx = chainActive.Tip()->nChainTx;
	
		return ttnmtx;
	} 

	int getNumTransactions() const //number of wallet transactions
	{
		//total number of transactions for the account
		int numTransactions = pwalletMain->mapWallet.size();

		return numTransactions; 
	}

	CAmount moneysupply()
	{
		CCoinsStats ss;
		FlushStateToDisk();
		
		if (pcoinsTip->GetStats(ss)) 
		{
		CAmount x =ss.nTotalAmount/100000000;
		return x;
		}
		
	}

	double blockreward (int nHeight, CAmount& nFees )
	{
		double x = GetBlockValue(nHeight, nFees);	
			return x;
	}
	
	
	

	double stage ()
	{
		int nHeight;
		if (chainActive.Tip()->nHeight<8000)
			return 0.0;
			
		if (chainActive.Tip()->nHeight<15000)
			return 1.0;
			
		if (chainActive.Tip()->nHeight<40000)
			return 1.3;
			
		if (chainActive.Tip()->nHeight<60000)
			return 2.0;
			
		if (chainActive.Tip()->nHeight<80000)
			return 4.0;

		//TODO: case for 80000 >= nHeight <= 100000
		
		if (chainActive.Tip()->nHeight>100000)
			return 10.0;				
	}



	double networktxpart() //wallet's network participation
	{
		double netpart = pwalletMain->mapWallet.size()/chainActive.Tip()->nChainTx;
	
		return netpart;
	}

	int lifetime() //wallet's lifetime
	{
		int creationdate  = pwalletMain->GetOldestKeyPoolTime();
		int lifespan = (GetTime() - creationdate)/3600;
    
		return lifespan;
	}

	int gbllifetime() //blockchain lifetime
	{
		int a  = GetTime() - 1418504572;
              
		return a/3600;
	}

	double freq () // transactional frequency
	{
		double a = (getNumTransactions()/ lifetime());
    
		return a;    
	}

	double glbfreq () //networks' transactional frequency 
	{
		double b = (chainActive.Tip()->nChainTx/ gbllifetime() );
    
		return b;    
	}
	

	int64_t balance() 
	{
		int64_t bal = pwalletMain->GetBalance();
	
		return bal;
	}
    
	double Gettrust()
	{
		double trust=0;
			{		
				{
					// lifetime carries up to 20 %
					if (lifetime() < 3* onemonth)
						trust+= 0.1;
					
					if (lifetime() > 3* onemonth && lifetime() < 4 * onemonth )
						trust+= 0.3;	
					
					if (lifetime() > 4* onemonth && lifetime() < 5 * onemonth )
						trust+= 0.5;
					
					if (lifetime() > 5 * onemonth && lifetime() < 6 * onemonth )
						trust+= 0.1;
					
					if (lifetime() > 6* onemonth && lifetime() < 12 * onemonth )
						trust+= 0.15;
					
					if (lifetime() > 12 * onemonth)
						trust+= 0.20;
					
				}	
				
				{//wallet useage up to 20%
					if (pwalletMain->mapWallet.size()<10)
						trust+= 0.01;
						
					if (pwalletMain->mapWallet.size()>10 && pwalletMain->mapWallet.size()<100)
						trust+= 0.02;	
						
					if (pwalletMain->mapWallet.size()>100 && pwalletMain->mapWallet.size()<250)
						trust+= 0.05;
						
					if (pwalletMain->mapWallet.size()>250 && pwalletMain->mapWallet.size()<500)
						trust+= 0.1;
						
					if (pwalletMain->mapWallet.size()>1000)
						trust+= 0.2;	
					
				}
				
				{//wallet balance up to 20%
	
					if(balance() < 1000)
						trust+= 0.01;
				
					if(balance() > 1000 && balance() < 1500)
						trust+= 0.02;
				
					if(balance() >1500 && balance() < 3000)
						trust+= 0.04;
				
					if(balance() >3000 && balance() < 5000)
						trust+= 0.06;
				
					if(balance() >5000 && balance() < 10000)
						trust+= 0.08;
				
					if(balance() >100000)
						trust+= 0.2;		
				
				}
				
				
				{ //network tx participation up to 20%
					if (networktxpart()< 0.00001)
						trust+=0.01;
					if (networktxpart()> 0.00001 && networktxpart()< 0.00005)
						trust+=0.03;
					if (networktxpart()> 0.00005 && networktxpart()< 0.0001)
						trust+=0.05;
					if (networktxpart()> 0.0001 && networktxpart()< 0.001)
						trust+=0.1;
					if (networktxpart()> 0.001)
						trust+=0.2;	
				}	
			
				/*{//is ICO or founder account 30% 
					if (Isfdraccount)
						trust += 0.30;
				}*/
				
			}
	}

	double Getmintrust()
	{
		double  x= 0.31;
		
		return x + stage(); 	 
	}

	double creditscore()
	{
		return Gettrust(); //unforntunately static as well since i have to build a transaction tracking system
	}

	double Getmincreditscore() const
	{
		return 0.5; 
	}

	double Getavecreditscore()
	{
		return Getmincreditscore();

	}

	double aveprice()
	{
		double  x=0;
	
		return x; 	 
	}

	double Getavetrust()
	{
		return Gettrust();
	}

	double Getgrossinterestrate()
	{
		return 0; //Disabled until PoS
	}

	double Getnetinterestrate()
	{
		return 0; //disabled until PoS
	}

	double Getinflationindex()
	{
		double a = moneysupply() - 10000000;
		
		return  a/ moneysupply();
	}

	int64_t Getvolume()
	{
		return moneysupply() -10000000 ; // need exchange to provide, unless we just consider volume per period within the client
	}

	int64_t Getgrantstotal()
	{
		return (moneysupply()- 10500000) * 0.1; // post v 30.10
	}

	int64_t Getbankreserve()
	{
		return (moneysupply()- 10500000) * 0.1; // post v 30.10
	}

	int64_t Getgblavailablecredit() 
	{
		
		return Getbankreserve();
	}

	int64_t Getglobaldebt()
	{
		return moneysupply()- Getbankreserve()-Getgrantstotal() ; //representing how much of the people's money the bank holds 
	}

	double Getgrantindex()
	{
		return Getgrantstotal()/moneysupply(); 
	}

	double Getgblmoneysupply()
	{
		return moneysupply(); //overall money supply 
	}
	
};

#endif
