// Copyright (c) 2014 The Memorycoin developers
#include "sync.h"
#include "util.h"
#include "amount.h"


#include <algorithm>
#include <exception>
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include <boost/unordered_map.hpp>

#include <list>
using namespace std;
extern CCriticalSection grantdb;
extern std::map<std::string,int64_t > grantAwards;
extern std::map<std::string,int64_t>::iterator gait;
CAmount GetGrantValue(int nHeight, int64_t nFees);
bool isGrantAwardBlock(int64_t nHeight);
bool getGrantAwards(int64_t nHeight);
int64_t getGrantDatabaseBlockHeight();
void processNextBlockIntoGrantDatabase();
bool getGrantAwardsFromDatabaseForBlock(int64_t nHeight);
bool ensureGrantDatabaseUptoDate(int64_t nHeight);
bool startsWith(const char *str, const char *pre);
void getWinnersFromBallots(int64_t nHeight,int officeNumber);
string electOrEliminate(int64_t droopQuota,  unsigned int requiredCandidates);
void electCandidate(string topOfThePoll, double gregorySurplusTransferValue,bool isLastCandidate);
void eliminateCandidate(string topOfThePoll,bool isLastCandidate);
void printBallots();

bool deSerializeGrantDB(string filename);
