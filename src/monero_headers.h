//
// Created by mwo on 5/11/15.
//

#ifndef XMREG03_MONERO_HEADERS_H_H
#define XMREG03_MONERO_HEADERS_H_H

#define DB_LMDB   2
#define BLOCKCHAIN_DB DB_LMDB

#include "cryptonote_core/cryptonote_basic.h"
#include "cryptonote_core/blockchain_storage.h"
#include "cryptonote_core/blockchain.h"

#include "blockchain_db/lmdb/db_lmdb.h"


#include "common/base58.h"

extern "C" {
    #include "crypto/crypto-ops.h"
    #include "crypto/keccak.h"
}



#endif //XMREG03_MONERO_HEADERS_H_H

