// Copyright (c) 2014 The Memorycoin developers
#include "voting.h"
#include "base58.h"
#include "utilstrencodings.h"
#include "chain.h"
#include "main.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>

#include <sys/stat.h>
#include <stdio.h>

using namespace boost;
using namespace std;

CCriticalSection grantdb;
std::map<int, std::string > awardWinners;
std::map<std::string,int64_t > grantAwards;
std::map<std::string,int64_t>::iterator gait;



//SECTION: GrantPrefixes and Grant Block Intervals

CAmount GetGrantValue(int nHeight, int64_t nFees)
{
    int64_t grantaward=5*COIN;
    return grantaward;
}

static string GRANTPREFIX="6BCR";

static const int64_t GRANTBLOCKINTERVAL = 5;

static int numberOfOffices = 8;
string electedOffices[9];

//= {"ceo","cto","cso","bnk","esw","nsr","crf","dev", "XFT"};


//Implement in memory for now - this will cause slow startup as recalculation of all votes takes place every startup. 
//These should be persisted in a database or on disk

int64_t grantDatabaseBlockHeight=-1; //How many blocks processed for grant allocation purposes


std::map<std::string,int64_t> balances; //Balances as at grant allocation block point

std::map<std::string,std::map<int64_t,std::string> > votingPreferences[7]; //Voting prefs as at grant allocation block point

//These do not need to persist. They are necessarily rebuilt when required
CBlockIndex* gdBlockPointer = NULL;
std::map<std::string,std::map<int64_t, std::string> >::iterator ballotit;
std::map<std::string,std::map<int64_t, std::string> > ballots;
//std::map<std::string,std::map<int64_t, std::string> > newBallotObject;
std::map<std::string,int64_t > ballotBalances;
std::map<std::string,double > ballotWeights;
std::map<int64_t, std::string>::iterator svpit;
ofstream grantAwardsOutput;
bool debugVote = false;
bool debugVoteExtra = false;
std::map<std::string, int64_t>::iterator svpit3;
std::map<int64_t, std::string>::iterator svpit4;
std::map<std::string,int64_t>::iterator it;
std::map<int64_t,std::string>::iterator it2;

std::map<std::string,std::map<int64_t,std::string> >::iterator vpit;

std::map<std::string,int64_t > wastedVotes; //Report on Votes that were wasted

std::map<std::string,std::map<int64_t,std::string> > electedVotes; //Report on where votes went

std::map<std::string,std::map<int64_t,std::string> > supportVotes; //Report on support for candidates

bool isGrantAwardBlock(int64_t nHeight){
	//NOTE: CALLED EVERY BLOCK. (Minimize computations here.)
	//printf("isGrantAwardBlock");
	
	if ( chainActive.Tip()->nHeight > 70000 && (chainActive.Tip()->nHeight % 5 == 0))
	//Grants were not being rewarded...
	{
		printf(" === Bitcredit Client === \n Is (%d) a grant block? : Yes \n", nHeight);
		return true;
	}else{
		printf(" === Bitcredit Client === \n Is (%d) a grant block? : No \n", nHeight);
		return false;
	}
}

//NOTE: Bitcredit V4+: Serialization of Grant DB will soon be depreciated. Serialization is too slow.
//TODO: Bitcredit V4 will require BOOST Serialization i.e. http://www.codeproject.com/Articles/225988/A-practical-guide-to-Cplusplus-serialization
//NOTE: More optimizations required here to improve saving of the DB.
//TODO: This could be compressed and/or encrypted to save space.
void serializeGrantDB(string filename){
		//NOTE: Debug Information
		printf(" === Bitcredit Client === \nSerialize Grant Info Database: Current Grant Database Block Height: %llu\n",grantDatabaseBlockHeight);
		
		//NOTE: Setup the filestream object and open the file.
		ofstream grantdb;
		grantdb.open (filename.c_str(), ios::trunc);
		
		//SECTION: Height of latest grant block
		//
		//NOTE: This writes the latest grant DB height on the first line of the file to be saved.
		//NOTE:  Grant DB height = the latest multiple of 20 (election block)
		grantdb << grantDatabaseBlockHeight << "\n";
		
		//SECTION: Balances
		//
		//NOTE: This code writes the second line of the grantdb.txt file located in "%APPDATA%/Bitcredit/grantdb.dat"(Windows).
		//NOTE: How many items are in the grant DB array:
		grantdb << balances.size()<< "\n";
		
		//NOTE: Loop that writes all the balances, including the key and the value
		//NOTE: Address,(skip line) amount of coins "detected" at address.
		//
		//TODO: These databases are written in order -- and alphabetically organized but optimized in any fashion.
		for( it = balances.begin();
			it != balances.end();
			++it)
		{
			grantdb << it->first << "\n" << it->second<< "\n";
		}
		
		//SECTION: Voting Preferences
		//
		//NOTE: Loop that first writes the size of the Voting Preference database for the line - Per round, for how many offices there are.
        for( int i = 0;
			i < numberOfOffices;
			i++)
		{
			//NOTE: Unusually small, there are a limited amount of voters in Bitcredit.
			//DESIGN?: Voting function isn't used by the general public, mostly by VIP and Shareholders who want to carry an opinion in the block-chain.
            grantdb << votingPreferences[i].size()<< "\n";
			
			//NOTE: votingPreferences is a large array.
			//
            for( vpit = votingPreferences[i].begin();
				vpit != votingPreferences[i].end();
				++vpit)
			{
				//NOTE: Address voted from: (key)
                grantdb << (vpit->first) << "\n";
				
				//NOTE: How many preferences are located in the array?
                grantdb << vpit->second.size() << "\n";
				
				//NOTE: If there are any other votes in the respective voting preference database, also list them here.
                for( it2 = vpit->second.begin();
					it2 != vpit->second.end();
					++it2 )
				{
					//NOTE: List the preference first, and then the office voted for.
                    grantdb << it2->first << "\n" << it2->second<< "\n";
                }
            }
        }
		
		grantdb.flush();
		grantdb.close();
}

//SECTION - Deserialize the Grant DB.
//
bool deSerializeGrantDB( string filename, int64_t maxWanted ){
	//NOTE: This takes a while to load Grant DB.
	//TODO: Disable debug information.
	printf(" === Bitcredit Client === \nDe-Serialize Grant Info Database\n Max Blocks Wanted: %d\n", maxWanted);
	
	std::string line;
	std::string line2;
	ifstream myfile;
	
	//NOTE: Open the file using ifstream object.
	myfile.open (filename.c_str());	
	
	//ifstream myfile ("grantsdb.dat");
	//NOTE: above line is commented out because of extra check to make sure grantDB can be opened.
	
	//NOTE: Rare error... only happens if client has a corrupt grantDB or does not exist.
	//NOTE: Cannot open file. Looks like we have to start all over again.
	if ( !myfile.is_open() ){
	
		printf("Could not load Grant Info Database from %s, Max Wanted: %d\n",filename.c_str(), maxWanted);
		
		return false;
	
	}else if (myfile.is_open()){
		//NOTE: Retrieve the latest grant database height.
		//NOTE: Line #1
		getline ( myfile, line );
		//Get latest grant height
		grantDatabaseBlockHeight = atoi64( line.c_str() );
		
		//TODO: Remove debug information.
		printf("Deserialize Grant Info Database Found.\n Height %llu, Max Wanted %d\n",grantDatabaseBlockHeight, maxWanted);
		
		//NOTE: This condition disables the deserialization of the Grant DB.
		//NOTE: maxWanted is an int64_t passed to this function.
		//NOTE: Only reads the first line before reading any more. Decreases load on non-grant blocks.
        if( grantDatabaseBlockHeight > maxWanted ){
            //NOTE: Don't load and reset grantDatabaseBlockHeight variable.
			//NOTE: The block-chain was cleared out and not the grant database. Re-do Everything.
            grantDatabaseBlockHeight=-1;
            myfile.close();
			
            return false;
        }
		//SECTION: Load Balances from file.
		//
		//NOTE: This line completely clears out the balances array.
		//TODO: SERVER LOAD?!
		balances.clear();
		
		//NOTE: Line #2
		//NOTE: Size of balances is a large array, 267K lines
		//NOTE: Saves too much info.
		//TODO: How can we make this smaller?
		getline( myfile, line );
		int64_t balancesSize = atoi64( line.c_str() );
		
		//NOTE: This loop reads the third and fourth line.
		for( int i = 0;
			i < balancesSize;
			i++)
		{
			getline( myfile, line );
			getline( myfile, line2 );
			//NOTE: Set balances[1st line] to the 2nd read line.
			balances[ line ] = atoi64( line2.c_str() );
		}
		
		//SECTION: Voting Preferences
		//
		//NOTE: Loop through all the offices that are currently active.
		//TODO: If new officers are added, there could be an issue.
        for( int i = 0;
			i < numberOfOffices;
			i++)
		{
			//NOTE: Working with a clear slate.
            votingPreferences[ i ].clear();
			//NOTE: Starts at the balancesSize*2 + 3 in the grantdb.dat file.
			//NOTE: This is the size of the votingPreferences database. (who voted for who).
            getline( myfile, line );
            int64_t votingPreferencesSize = atoi64( line.c_str() );
            
			//NOTE: Good enough loop.
			for( int k = 0;
				k < votingPreferencesSize;
				k++)
			{
				//NOTE: line is the wallet address that voted for a specific candidate.
                getline( myfile, line );
                std::string vpAddress = line;
				//NOTE: line retrieves the size of the address' preference file.
                getline( myfile, line );
                int64_t vpAddressSize = atoi64( line.c_str() );
				
				//NOTE: loop through data in address' preference 'array'.
                for( int j = 0;
					j < vpAddressSize;
					j++)
				{
					//NOTE: Retrieve vote preference.
                    getline( myfile, line );
					
					//NOTE: Retrieve voting address associated with preference.
                    getline( myfile, line2 );
                    
					//NOTE: Associate this data with the votingPreferences Array.
					votingPreferences[ i ][ vpAddress ][ atoi64( line.c_str() ) ] = line2;
                }
		
            }
        }
		
		myfile.close();
		
		//Set the pointer to next block to process
		//NOTE: This loop sets the next block to process.
		//NOTE: First sets the gdBlockPointer variable to the genesis (1) block.
		//NOTE: If pindexGenesisBlock is not set after declaration, gdBlockPointer will be NULL.
		gdBlockPointer = chainActive.Genesis();
		
		//NOTE: This loop checks if the blocks are valid and the next one is available.
		for( int i = 0;
			i < grantDatabaseBlockHeight;
			i++)
		{
			//NOTE: Check if gdBlockPointer is defined and not NULL.
            if( gdBlockPointer->pskip == NULL ){
				printf("Insufficent number of blocks loaded %s\n", filename.c_str() );
				return false;
			}
			
			gdBlockPointer = gdBlockPointer->pskip;
		}
		//NOTE: Everything worked out fine. database has been opened, all data has been loaded, closed, and the next block is available.
	
		return true;
	}
}


bool getGrantAwards(int64_t nHeight){
	//nHeight is the current block height
	if( !isGrantAwardBlock( nHeight ) ){
		printf("Error - calling getgrantawards for non grant award block, nHeight requested: %d", nHeight);
		return false;
	}else{
		return ensureGrantDatabaseUptoDate( nHeight );
	}
}

bool ensureGrantDatabaseUptoDate(int64_t nHeight){
	//NOTE: This sets up the initial database
	printf(" === Grant Database Block Height %d === \n", grantDatabaseBlockHeight);
    //This should always be true on startup
    if( grantDatabaseBlockHeight == -1 ){
			//string newCV="xxx";
		//NOTE: voting prefix is second part after (originally MVTE)
		
		
        //Only count custom vote if re-indexing
		//NOTE: We will note be using a custom vote prefix any longer.
		 /*
         if(getGrantDatabaseBlockHeight()==-1 && (GetBoolArg("-reindex"),false)){
            newCV=GetArg("-customvoteprefix",newCV);
            printf("customvoteprefix:%s\n",newCV.c_str());
        } 
		*/
		electedOffices[0] = "ceo";
		electedOffices[1] = "cto";
		electedOffices[2] = "cno";
		electedOffices[3] = "cmo";
		electedOffices[4] = "cso";
		electedOffices[5] = "cha";
		electedOffices[6] = "xxx";
		
		//electedOffices[6] = newCV;
		//6th [debug] is not necessary.
        //electedOffices[6]=newCV;
    }
	
	//NOTE: set up option for V3
    //NOTE: nHeight is the current block height
    //NOTE: (v2) requiredgrantdatabaseheight is 20 less than the current block
	
	//NOTE: Call v3 grant rewards AGAIN after 60 blocks.
	//NOTE: Grants of the new supply will be granted to candidates on the hard-fork block.
	int64_t requiredGrantDatabaseHeight =nHeight-GRANTBLOCKINTERVAL;
	
	
 	printf("=== Bitcredit Client: Making sure that Grant Database is up to date. ===\n=== Required Height of Database: %lld, Height requested from: %d ===\n",requiredGrantDatabaseHeight, nHeight);
    //Maybe we don't have to count votes from the start - let's check if there's a recent vote database stored
    if( grantDatabaseBlockHeight == -1){
			deSerializeGrantDB( ( GetDataDir() / "blocks/grantdb.dat" ).string().c_str(), requiredGrantDatabaseHeight );
	}
 
 
 //NOTE: This has been removed... why?
 /*
    if( grantDatabaseBlockHeight > requiredGrantDatabaseHeight ){
        printf("Grant database has processed too many blocks. Needs to be rebuilt. %lld", nHeight );
        
		balances.clear();
        
		for( int i = 0;
			i < numberOfOffices + 1;
			i++)
		{
            votingPreferences[ i ].clear();
        }
        gdBlockPointer = pindexGenesisBlock;
        grantDatabaseBlockHeight = -1;
		
	}
*/	
    while( grantDatabaseBlockHeight < requiredGrantDatabaseHeight ){
        processNextBlockIntoGrantDatabase();
	}
	
    return true;
		
}

int getOfficeNumberFromAddress(string grantVoteAddress, int64_t nHeight){
	//printf("getOfficeNumberFromAddress\n");
	if (!startsWith( grantVoteAddress.c_str(), "MVTE" ) )
	{
		//printf("getOfficeNumberFromAddress: Fail - 1\n");
		return -1;
	}
	
	//NOTE: this loop is horrible
	//TODO: CASE SYSTEM
	for( int i = 0;
		i < numberOfOffices + 1;
		i++)
	{
	//printf("substring %s\n",grantVoteAddress.substr(4,3).c_str());
	//NOTE: Not changing prefix.
		if ( grantVoteAddress.substr(4,3) == electedOffices[i] )
		{
			//printf("getOfficeNumberFromAddress: Pass\n");
			return i;
		}
	}
	//NOTE: Negative
	//printf("getOfficeNumberFromAddress: Fail2\n");
	return -1;
}

void printVotingPrefs(std::string address){

    //I don't know why, this is compiling, but crashing
    /*std::map<int64_t, std::string> thisBallot=ballots.find(address)->second;
    if(thisBallot.begin()==thisBallot.end()){
        printf("No Voting Preferences\n");
        return;
    }
    int pref=1;
    for(svpit4=thisBallot.begin();svpit4!=thisBallot.end();++svpit4){
        printf("Preference: (%d) %llu %s \n",pref, svpit4->first/COIN,svpit4->second.c_str());
        pref++;
    }*/

    //This is slow and iterates too much, but on the plus side it doesn't crash the program.
    //This crash probably caused by eliminate candidate corrupting the ballot structure.
    //Should be safe to use more efficient code after fork
	
    int pref = 1;
    for( ballotit = ballots.begin(); 
		ballotit != ballots.end();
		++ballotit){
	if( address == ballotit->first )
		{
           for( svpit4 = ballotit->second.begin();
				svpit4 != ballotit->second.end();
				++svpit4)
			{
				grantAwardsOutput<<"--Preference "<<pref<<" "<<svpit4->first<<" "<<svpit4->second.c_str()<<" \n";
				pref++;
			}
		}
	}
		
}

void processNextBlockIntoGrantDatabase(){

	printf(" === Bitcredit Client === \n Processing the Next Block into the Grant Database for Block: %d\n",grantDatabaseBlockHeight+1);
	
	//NOTE: Process the latest block.
	CBlock block;
	CBlockUndo undo;
	CDiskBlockPos pos = gdBlockPointer->GetUndoPos();
	//If it's the first block, we'll start with the Genesis Block
	if( gdBlockPointer != NULL ){
		gdBlockPointer = gdBlockPointer->pskip;
	}else{
		gdBlockPointer = chainActive.Genesis();
	}
	
	!undo.ReadFromDisk(pos, gdBlockPointer->pprev->GetBlockHash());
	//ReadBlockFromDisk(block, gdBlockPointer);
        //block.ReadFromDisk(gdBlockPointer,true); //Litecoin codebase method
	
	
	//Look at all transactions in the block to update balances and see if they contain voting preferences
	//NOTE: This should be done on a NEEDto basis.
	for (unsigned int i = 0; 
		i < block.vtx.size(); 
		i++)
	{
		std::map<std::string,int64_t > votes;
		std::map<std::string,int64_t >::iterator votesit;
		
		//Deal with outputs first - increase balances AND note what the votes are
		//the file has 350k lines as of this moment.
		for (unsigned int j = 0; 
			j < block.vtx[i].vout.size();
			j++)
		{
			CTxDestination address;
			ExtractDestination( block.vtx[ i ].vout[ j ].scriptPubKey, address );
			string receiveAddress = CBitcreditAddress( address ).ToString().c_str();
			
			//Grab the amount of coins "Received to this address"
			//NOTE:
			//https://bitcointalk.org/index.php?topic=115488.msg1246315#msg1246315
			int64_t theAmount = block.vtx[ i ].vout[ j ].nValue;
			
			//Update balance - if no previous balance, should start at 0
			balances[ receiveAddress ] = balances[ receiveAddress ] + theAmount;				
			
			//Note any voting preferences made in the outputs
			//NOTE: THIS IS CALLED FOR EVERY TRANSACTION (wow.)
			
			//Spammed...
			//printf("Note any voting preferences made in the outputs\n");

			if(
				//NOTE: Easiest checks first.
				//NOTE: Is the amount between 1 and 9 satoshi's ??
				theAmount < 10 
			&& 
				theAmount > 0
			&& 
				startsWith( receiveAddress.c_str(), "MVTE" ) )
			{
				printf(" == Bitcredit Client === \nWe found a vote!\n: Vote found -- Candidate: %s, Preference: %llu\n ======\n",receiveAddress.c_str() , theAmount);
				votes[ receiveAddress ] = theAmount;
			}
		}
		
		//Deal with the inputs - reduce balances AND apply voting preferences noted in the outputs.
		//
		for ( unsigned int j = 0; 
			j < block.vtx[ i ].vin.size();
			j++ )
		{
			if( !(block.vtx[ i ].IsCoinBase() ) )
			{
				//NOTE: No input to reduce if it is a coinbase transaction.
				
				CTransaction txPrev;
				uint256 hashBlock;
				
				GetTransaction( block.vtx[ i ].vin[ j ].prevout.hash, txPrev, hashBlock );
				
				CTxDestination source;
				
				ExtractDestination( txPrev.vout[ block.vtx[ i ].vin[ j ].prevout.n ].scriptPubKey, source );
				
				string spendAddress = CBitcreditAddress( source ).ToString().c_str();
				
				int64_t theAmount = txPrev.vout[ block.vtx[ i ].vin[ j ].prevout.n ].nValue;
				
				//Reduce balance
				balances[ spendAddress ] = balances[ spendAddress ] - theAmount;
	
				//If any of the outputs were votes
				//NOTE: Run through the 'votes' table.
				//NOTE: first= Key, second= Value
				for( votesit = votes.begin();
					votesit != votes.end(); 
					++votesit)
				{
					//NOTE: (Why are we still debugging?)
                    printf(" === Bitcredit Client === \nVote found: %s, %llu\n",votesit->first.c_str(),votesit->second);
					string grantVoteAddress = ( votesit->first );	 
					int electedOfficeNumber = getOfficeNumberFromAddress( grantVoteAddress, gdBlockPointer->nHeight );
					
					if( electedOfficeNumber > -1 ){
						//NOTE: running the grant database is memory intensive and can cause an issue of taking up too much memory on the node.
						//TODO: There may be a better way to set this database up.
                        //printf("Vote added: %d %s, %llu\n",electedOfficeNumber,votesit->first.c_str(),votesit->second);
                        votingPreferences[ electedOfficeNumber ][ spendAddress ][ votesit->second ] = grantVoteAddress;
						
                        //printf("Voting Preference Size: %d \n",votingPreferences[electedOfficeNumber].size());
                    }
					
				}
				
			}
			
		}
		
	}
	//SECTION: processNextBlockIntoGrantDatabase function
	//
	
	//NOTE: INCREASE THE GRANTDATABASEBLOCK HEIGHT
	//NOTE: Run after the above loop scans through the size of the block for every transaction.
	grantDatabaseBlockHeight++;
	
	printf("Block has been processed. Grant Database Block Height is now updated to Block # %d\n", grantDatabaseBlockHeight);
	if ( isGrantAwardBlock( grantDatabaseBlockHeight + GRANTBLOCKINTERVAL ) ) 
	{
		getGrantAwardsFromDatabaseForBlock( grantDatabaseBlockHeight + GRANTBLOCKINTERVAL );
		
	}
	
	//NOTE: This is always serialized or saved at the end.
	serializeGrantDB( (GetDataDir() / "blocks/grantdb.dat" ).string().c_str() );
}


void printCandidateSupport(){
	std::map<int64_t,std::string>::reverse_iterator itpv2;
	
	grantAwardsOutput<<"\nWinner Support: \n";
		
	for( ballotit = supportVotes.begin(); 
		ballotit != supportVotes.end(); 
		++ballotit){
		grantAwardsOutput<<"\n--"<<ballotit->first<<" \n";
		for( itpv2 = ballotit->second.rbegin();
			itpv2 != ballotit->second.rend();
			++itpv2)
		{
			grantAwardsOutput<<"-->("<< itpv2->first/COIN <<"/"<<balances[itpv2->second.c_str()]/COIN <<") "<<itpv2->second.c_str()<<" \n";
		}	
	}
}

void printBalances( int64_t howMany, bool printVoting, bool printWasted ){
	grantAwardsOutput<<"---Current Balances------\n";
	std::multimap<int64_t, std::string > sortByBalance;
	
	std::map<std::string,int64_t>::iterator itpv;
	
	std::map<int64_t,std::string>::reverse_iterator itpv2;
	
	for(itpv=balances.begin();
		itpv!=balances.end();
		++itpv)
	{
		//int amt=(it->second)/COIN;
		//sortByBalance[it->second]=it->first;
		if( itpv->second > COIN ){
			sortByBalance.insert( pair<int64_t, std::string>( itpv->second, itpv->first ) );
		}
	}
	
	//printf("%d addresses with balances. Printing Top %d\n",balances.size(),howMany);
	std::multimap<int64_t, std::string >::reverse_iterator sbbit;
	int64_t count = 0;
	for (sbbit =  sortByBalance.rbegin();
		sbbit !=  sortByBalance.rend();
		++sbbit)
	{
		if( howMany > count ){
			grantAwardsOutput<<"\n->Balance:"<<sbbit->first/COIN<<" - "<<sbbit->second.c_str()<<"\n";
			if( printWasted ){
				for( itpv2 = electedVotes[ sbbit->second.c_str() ].rbegin();
					itpv2!=electedVotes[ sbbit->second.c_str() ].rend();
					++itpv2)
				{
					grantAwardsOutput << "---->" << itpv2->first/COIN << " supported " << itpv2->second << "\n";		
				}
				
				if( wastedVotes[ sbbit->second.c_str()] / COIN > 0 ){
					grantAwardsOutput<<"---->"<<wastedVotes[sbbit->second.c_str()]/COIN<<"  wasted (Add More Preferences)\n";
				}
				
				if( votingPreferences[ 0 ].find( sbbit->second.c_str() ) == votingPreferences[ 0 ].end() ){
					grantAwardsOutput<<"---->No Vote: (Add Some Voting Preferences)\n";
				}
			}
			
			if( printVoting ){
				try{
					printVotingPrefs( sbbit->second );
				}catch ( std::exception &e ) {
					grantAwardsOutput<<"Print Voting Prefs Exception\n";
				}
			}
			count++;
		}
	}
	grantAwardsOutput<<"---End Balances------\n";
}


bool getGrantAwardsFromDatabaseForBlock(int64_t nHeight){
	
    printf( " === Bitcredit Client === \n getGrantAwardsFromDatabaseForBlock %llu\n", nHeight );
	if (( grantDatabaseBlockHeight != nHeight - GRANTBLOCKINTERVAL ))
	{ 
        printf(" === Bitcredit Client === \ngetGrantAwardsFromDatabase is being called when no awards are due. %llu %llu\n",grantDatabaseBlockHeight,nHeight);

		return false;
	}
	
	debugVote = GetBoolArg("-debugvote", false);
	if( debugVote ){
		std::stringstream sstm;
		sstm << "award" << setw(8) << std::setfill('0') << nHeight << ".txt";
		string filename = sstm.str();
		//printf("%s\n",filename.c_str());
		//mkdir((GetDataDir() / "grantawards").string().c_str());
		mkdir((GetDataDir() / "grantawards").string().c_str()
#ifndef _WIN32
		,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
#endif
		);
		grantAwardsOutput.open ((GetDataDir() / "grantawards" / filename).string().c_str(), ios::trunc);}
	//save to disk
	
	if( debugVote ){
		grantAwardsOutput << "-------------:\nElection Count:\n-------------:\n\n";
	}
	//Clear from last time, ensure nothing left over

	awardWinners.clear();
	grantAwards.clear();

		
	for( int i = 0;
		i < numberOfOffices + 1;
		i++ )
	{
	//NOTE: CLEAR EVERYTHING HERE.
		ballots.clear();
		ballotBalances.clear();
		ballotWeights.clear();
		wastedVotes.clear();
		electedVotes.clear();
		supportVotes.clear();
		
		//Iterate through every vote
		for( vpit = votingPreferences[i].begin();
			vpit != votingPreferences[i].end();
			++vpit)
		{
			int64_t voterBalance = balances[ vpit->first ];
			//Ignore balances of 0 - they play no part.
			if( voterBalance > 0 ){
				ballotBalances[ vpit->first ] = voterBalance;
				ballotWeights[ vpit->first ] = 1.0;
				
				//Order preferences by coins sent - lowest number of coins has top preference
				for( it2 = vpit->second.begin();
					it2 != vpit->second.end();
					++it2)
				{
					//Where a voter has voted for more than one preference with the same amount, only the last one (alphabetically) will be valid. The others are discarded.
					ballots[ vpit->first ][ it2->first ] = it2->second;
				}
			}
		}
		//if(debugVote)printBalances(100,true,false);
		//TODO: decrease intensity of this function.
		getWinnersFromBallots( nHeight, i );

		//At this point, we know the vote winners - now to see if grants are to be awarded
		//nheight is the current blockheight
		if( i < numberOfOffices ){
			for(int i=0;i<1;i++){
				grantAwards[ awardWinners[ i ] ] = grantAwards[ awardWinners[ i ] ] + GetGrantValue( nHeight, 0 );
						
				if( debugVote ){
					grantAwardsOutput << "Add grant award to Block "<<awardWinners[0].c_str()<<" ("<<GetGrantValue(nHeight, 0)/COIN<<")\n";
				}
			}		
		}
	
		if( debugVote ){
			printCandidateSupport();
		}
	}	
	//if(debugVote)printBalances(100,false,true);
	if( debugVote ){
		grantAwardsOutput.close();
	}
	
	return true;
}

void getWinnersFromBallots( int64_t nHeight, int officeNumber ){

	if( debugVote ){
		grantAwardsOutput <<"\n\n\n--------"<< electedOffices[ officeNumber ]<<"--------\n";
	}
	if( debugVoteExtra ){
		printBallots();
	}
	//Calculate Total in all balances
	int64_t tally = 0;
	for( it = balances.begin();
		it != balances.end();
		++it)
	{
		tally = tally + it->second;
	}
	
	if( debugVote ){
		grantAwardsOutput <<"Total coin issued: " << tally/COIN <<"\n";
	}
	//Calculate Total of balances of voters
	int64_t totalOfVoterBalances=0;
	
	for( it = ballotBalances.begin();
		it != ballotBalances.end();
		++it)
	{
		totalOfVoterBalances = totalOfVoterBalances + it->second;
	}
	
	if( debugVote ){
		grantAwardsOutput <<"Total of Voters' Balances: "<<totalOfVoterBalances/COIN<<"\n";
	}
	//Turnout
	if( debugVote ){
		grantAwardsOutput <<"Percentage of total issued coin voting: "<<(totalOfVoterBalances*100)/tally<<" percent\n";
	}
	//Calculate Droop Quota
	int64_t droopQuota = (totalOfVoterBalances/2) + 1;
	//int64_t droopQuota = ( totalOfVoterBalances / ( 1 ) ) + 1;
	if( debugVote ){
		grantAwardsOutput <<"Droop Quota: "<<droopQuota/COIN<<"\n";
	}
	//Conduct voting rounds until all grants are awarded
	for(int i = 1;
		i > 0;
		i--)
	{
		string electedCandidate;
		
		int voteRoundNumber = 0;
		if( debugVote ){
			grantAwardsOutput <<"-------------:\nRound:"<<1-i<<"\n";
		}
		
		if( debugVoteExtra ){
			printBallots();
		}
		
		do{
			//if(debugVote)grantAwardsOutput <<"-------------:\nElimination Round:%d\n",voteRoundNumber);
			electedCandidate = electOrEliminate( droopQuota, i );
			voteRoundNumber++;
		}while( electedCandidate == "" );
		
		awardWinners[ ( i - 1 ) *- 1 ] = electedCandidate;
	}
	//if(debugVote)grantAwardsOutput <<"--------End Grant Voting--------\n";
	
}

//Sum total of first preferences
std::map<std::string,int64_t > preferenceCount;
int64_t numberCandidatesEliminated = 0;	
		
string electOrEliminate( int64_t droopQuota, unsigned int requiredCandidates ){

	std::map<std::string,int64_t >::iterator tpcit;
	
	//Recalculate the preferences each time as winners and losers are removed from ballots.
	preferenceCount.clear();
	
	//Calculate support for each candidate. The balance X the weighting for each voter is applied to the total for the candidate currently at the top of the voter's ballot  
	for( ballotit = ballots.begin();
		ballotit != ballots.end();
		++ballotit)
	{
		//Check: Multiplying int64_t by double here, and representing answer as int64_t.
		preferenceCount[ ballotit->second.begin()->second ] += ( ballotBalances[ ballotit->first ] * ballotWeights[ ballotit->first ] );
	}
	
	//if(debugVote)grantAwardsOutput <<"Number of Remaining Candidates: %d\n",preferenceCount.size());
	
	//Find out which remaining candidate has the greatest and least support
	string topOfThePoll;
	int64_t topOfThePollAmount = 0;	
	string bottomOfThePoll;
	int64_t bottomOfThePollAmount = 9223372036854775807;

		
	for( tpcit = preferenceCount.begin();
		tpcit != preferenceCount.end();
		++tpcit)
	{
		//Check:When competing candidates have equal votes, the first (sorted by Map) will be chosen for top and bottom of the poll.
		if( tpcit->second > topOfThePollAmount ){
			topOfThePollAmount = tpcit->second;
			topOfThePoll = tpcit->first;
		}
		
		if( tpcit->second < bottomOfThePollAmount ){
			bottomOfThePollAmount = tpcit->second;
			bottomOfThePoll = tpcit->first;
		}
		//if(tpcit->second>droopQuota/10){
		//	if(debugVote)grantAwardsOutput <<"Support: "<<tpcit->first<<"("<<tpcit->second/COIN<<")\n";
		//}
	}
	
	//Purely for debugging/information
	if( topOfThePollAmount >= droopQuota 
	|| 
		requiredCandidates >= preferenceCount.size() 
	||
		bottomOfThePollAmount > droopQuota / 10)
	{
		if( debugVote ){
			grantAwardsOutput <<"Candidates with votes equalling more than 10% of Droop quota\n";
		}
		
		for( tpcit = preferenceCount.begin();
			tpcit != preferenceCount.end();
			++tpcit)
		{
			if( tpcit->second > droopQuota / 10 ){
				if( debugVote ){
					grantAwardsOutput <<"Support: "<<tpcit->first<<" ("<<tpcit->second/COIN<<")\n";
				}
			}
		}
	}
	//if(debugVote)grantAwardsOutput <<"Bottom Preference Votes: %s %llu\n",bottomOfThePoll.c_str(),bottomOfThePollAmount/COIN);
	
	if( topOfThePollAmount == 0 ){
		//No ballots left -end - 
		if( debugVote ){
			grantAwardsOutput <<"No Candidates with support remaining. Grant awarded to unspendable address MVTEceo1111111111111111111111TvNrt\n";
		}
		
		return "MVTEceo1111111111111111111111TvNrt";
	}
	
	if( topOfThePollAmount >= droopQuota
	|| 
		requiredCandidates >= preferenceCount.size() )
	{
		
		//Note: This is a simplified Gregory Transfer Value - ignoring ballots where there are no other hopefuls.
		double gregorySurplusTransferValue = ( (double)topOfThePollAmount - (double)droopQuota ) / (double)topOfThePollAmount;
		
		//Don't want this value to be negative when candidates are elected with less than a quota
		if( gregorySurplusTransferValue < 0 ){
			gregorySurplusTransferValue = 0;
		}
		
		electCandidate( topOfThePoll, gregorySurplusTransferValue, (requiredCandidates == 1) );
		
		//if(debugVote)grantAwardsOutput <<"Top Preference Votes: %s %llu\n",topOfThePoll.c_str(),topOfThePollAmount/COIN);	
		if( debugVote ){
			if( numberCandidatesEliminated > 0 ){
				grantAwardsOutput <<"Candidates Eliminated ("<<numberCandidatesEliminated<<")\n\n";
				numberCandidatesEliminated = 0;
			}
			grantAwardsOutput <<"Candidate Elected: "<<topOfThePoll.c_str()<<" ("<<topOfThePollAmount/COIN<<")\n";
			grantAwardsOutput <<"Surplus Transfer Value: "<<gregorySurplusTransferValue<<"\n";				
		}
		return topOfThePoll;
		
	}else{
		eliminateCandidate( bottomOfThePoll, false );
		if( debugVote ){
			if( bottomOfThePollAmount > droopQuota / 10 ){
				if( numberCandidatesEliminated > 0 ){
					grantAwardsOutput <<"Candidates Eliminated ("<<numberCandidatesEliminated<<")\n";
					numberCandidatesEliminated = 0;
				}
				
				grantAwardsOutput <<"Candidate Eliminated: "<<bottomOfThePoll.c_str()<<" ("<<bottomOfThePollAmount/COIN<<")\n\n";
			}else{
				numberCandidatesEliminated++;
			}
		}
		return "";
	}	
	
	
}

std::map<int64_t, std::string>::iterator svpit2;

void electCandidate( string topOfThePoll, 
	double gregorySurplusTransferValue,
	bool isLastCandidate )
{
	
	//Apply fraction to weights where the candidate was top of the preference list
	for( ballotit = ballots.begin();
		ballotit != ballots.end();
		++ballotit)
	{
		svpit2 = ballotit->second.begin();
		if( svpit2->second == topOfThePoll ){
			//Record how many votes went towards electing this candidate for each user
			electedVotes[ ballotit->first ][ balances[ ballotit->first ]*( ballotWeights[ ballotit->first ] * ( 1 - gregorySurplusTransferValue ) ) ] = svpit2->second;
			//Record the support for each candidate elected
			supportVotes[ topOfThePoll ][ balances[ ballotit->first ] * ( ballotWeights[ ballotit->first ] * ( 1 - gregorySurplusTransferValue ) ) ] = ballotit->first;
			
			//This voter had the elected candidate at the top of the ballot. Adjust weight for future preferences.
			ballotWeights[ballotit->first]=ballotWeights[ballotit->first]*gregorySurplusTransferValue;
		}
	}
	
	//Remove candidate from all ballots - this includes where he may be far down the list of preferences
	eliminateCandidate( topOfThePoll, isLastCandidate );
}

void eliminateCandidate( string removeid, bool isLastCandidate ){

	std::map<std::string, int64_t> ballotsToRemove;
	std::map<std::string, int64_t>::iterator btrit;
	
	//Remove candidate from all ballots - note the candidate may be way down the preference list
	for( ballotit = ballots.begin(); 
		ballotit != ballots.end();
		++ballotit)
	{
		int64_t markForRemoval = 0;
			
		for( svpit2 = ballotit->second.begin();
			svpit2 != ballotit->second.end();
			++svpit2)
		{
			if( svpit2->second == removeid ){
				//if(debugVote)grantAwardsOutput <<"1. Mark for Removal From Ballot: %s %d \n",removeid.c_str(),svpit2->first);
				markForRemoval = svpit2->first;
			}
		}
		
		if( markForRemoval != 0 ){
			ballotit->second.erase( markForRemoval );
		}
		
		//Make a note of ballot to remove
		if( ballotit->second.size() == 0 ){
			if( !isLastCandidate ){
				wastedVotes[ ballotit->first ] = ( ballotBalances[ ballotit->first ] * ballotWeights[ ballotit->first ] );
			}
			ballotsToRemove[ ballotit->first ] = 1;
		}
	}	
	
	for( btrit = ballotsToRemove.begin();
		btrit != ballotsToRemove.end();
		++btrit)
	{
		ballots.erase( btrit->first );
	}
	
}



void printBallots(){
	printf("Current Ballot State\n");
	int cutOff = 0;
	for( ballotit = ballots.begin();
		ballotit != ballots.end();
		++ballotit)
	{
		//if(cutOff<10){
			printf("Voter: %s Balance: %llu Weight: %f Total: %f\n",
				ballotit->first.c_str(),
				ballotBalances[ ballotit->first ] / COIN,
				ballotWeights[ ballotit->first ],
				( ballotBalances[ ballotit->first ] / COIN ) * ballotWeights[ ballotit->first ] 
				);
			int cutOff2 = 0;
			//if(cutOff2<5){
				for( svpit2 = ballotit->second.begin();
					svpit2 != ballotit->second.end();
					++svpit2)
				{
					printf( "Preference: (%d) %llu %s \n",
						cutOff2,
						svpit2->first,
						svpit2->second.c_str()
						);
				}
			/* }*/
			cutOff2++;
		/* }*/
		cutOff++;
	}
}

bool startsWith( const char *str, const char *pre )
{
    size_t lenpre = strlen( pre ),
           lenstr = strlen( str );
		   
    return lenstr < lenpre ? false : strncmp( pre, str, lenpre ) == 0;
}
