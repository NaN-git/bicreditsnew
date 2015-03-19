// Copyright (c) 2014 The ShadowCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef SDC_CORE_H
#define SDC_CORE_H

#include "util.h"
#include "serialize.h"
#include "primitives/transaction.h"

#include <stdlib.h> 
#include <stdio.h> 
#include <vector>
#include <inttypes.h>


class CKeyImageSpent
{
// stored in txdb, key is keyimage
public:
    CKeyImageSpent() {};
    
    CKeyImageSpent(uint256& txnHash_, uint32_t inputNo_, int64_t nValue_)
    {
        txnHash = txnHash_;
        inputNo = inputNo_;
        nValue  = nValue_;
    };
    
    uint256 txnHash;    // hash of spending transaction
    uint32_t inputNo;   // keyimage is for inputNo of txnHash
    int64_t nValue;     // reporting only
    
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(txnHash);
        READWRITE(inputNo);
        READWRITE(nValue);
    }
};

class CAnonOutput
{
// stored in txdb, key is pubkey
public:
    
    CAnonOutput() {};
    
    CAnonOutput(COutPoint& outpoint_, int64_t nValue_, int nBlockHeight_, uint8_t nCompromised_)
    {
        outpoint = outpoint_;
        nValue = nValue_;
        nBlockHeight = nBlockHeight_;
        nCompromised = nCompromised_;
    };
    
    COutPoint outpoint;
    int64_t nValue;         // rather store 2 bytes, digit + power 10 ?
    int nBlockHeight;
    uint8_t nCompromised;   // TODO: mark if output can be identified (spent with ringsig 1)
    
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(outpoint);
        READWRITE(nValue);
        READWRITE(nBlockHeight);
        READWRITE(nCompromised);
    }
};

class CAnonOutputCount
{ // CountAllAnonOutputs
public:
    
    CAnonOutputCount()
    {
        nValue = 0;
        nExists = 0;
        nSpends = 0;
        nOwned = 0;
        nLeastDepth = 0;
    }

    CAnonOutputCount(int64_t nValue_, int nExists_, int nSpends_, int nOwned_, int nLeastDepth_)
    {
        nValue = nValue_;
        nExists = nExists_;
        nSpends = nSpends_;
        nOwned = nOwned_;
        nLeastDepth = nLeastDepth_;
    }
    
    void set(int64_t nValue_, int nExists_, int nSpends_, int nOwned_, int nLeastDepth_)
    {
        nValue = nValue_;
        nExists = nExists_;
        nSpends = nSpends_;
        nOwned = nOwned_;
        nLeastDepth = nLeastDepth_;
    }
    
    void addCoin(int nCoinDepth, int64_t nCoinValue)
    {
        nExists++;
        nValue = nCoinValue;
        if (nCoinDepth < nLeastDepth)
            nLeastDepth = nCoinDepth;
    }
    
    void updateDepth(int nCoinDepth, int64_t nCoinValue)
    {
        nValue = nCoinValue;
        if (nLeastDepth == 0
            || nCoinDepth < nLeastDepth)
            nLeastDepth = nCoinDepth;
    }
    
    void incSpends(int64_t nCoinValue)
    {
        nSpends++;
        nValue = nCoinValue;
    }
    
    void decSpends(int64_t nCoinValue)
    {
        nSpends--;
        nValue = nCoinValue;
    }
    
    void incExists(int64_t nCoinValue)
    {
        nExists++;
        nValue = nCoinValue;
    }
    
    void decExists(int64_t nCoinValue)
    {
        nExists--;
        nValue = nCoinValue;
    }
    
    
    int64_t nValue;
    int nExists;
    int nSpends;
    int nOwned; // todo
    int nLeastDepth;
    
};

#endif  // SDC_CORE_H

