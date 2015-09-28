#include <sys/timeb.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include "utils.h"
#include "bytecode.h"
#include "vm.h"

typedef enum {
	GET_GLOBAL_OP_QUICK = DROP_OP+1,
	SET_GLOBAL_OP_QUICK,
	GOTO_OP_QUICK,
	BRANCH_OP_QUICK,
	CALL_OP_QUICK,
} QOpCode;

typedef struct {
  OpCode tag;
  int idx;
} GetGlobalQuickIns;

typedef struct {
  OpCode tag;
  int idx;
} SetGlobalQuickIns;

typedef struct {
  OpCode tag;
  int addr;
} GotoQuickIns;

typedef struct {
  OpCode tag;
  int addr;
} BranchQuickIns;

typedef struct {
  OpCode tag;
  int idx;
  int arity;
} CallQuickIns;

// #define DEBUG 1

// Copied from sys/time.h
// to not pollute our #define space
// WAS NOT WRITTEN BY LG OR BM - rights belong to original author
#define timersub(a, b, result)                  \
  do {                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;            \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;            \
    if ((result)->tv_usec < 0) {                \
      --(result)->tv_sec;                  \
      (result)->tv_usec += 1000000;                \
    }                        \
  } while (0)

//============================================================
//=========== HASH TABLE IMPLEMENTATION ======================
//============================================================

/*
Copyright (c) 2003-2014, Troy D. Hanson     http://troydhanson.github.com/uthash/
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef UTHASH_H
#define UTHASH_H

#include <string.h>   /* memcmp,strlen */
#include <stddef.h>   /* ptrdiff_t */
#include <stdlib.h>   /* exit() */

/* These macros use decltype or the earlier __typeof GNU extension.
   As decltype is only available in newer compilers (VS2010 or gcc 4.3+
   when compiling c++ source) this code uses whatever method is needed
   or, for VS2008 where neither is available, uses casting workarounds. */
#if defined(_MSC_VER)   /* MS compiler */
#if _MSC_VER >= 1600 && defined(__cplusplus)  /* VS2010 or newer in C++ mode */
#define DECLTYPE(x) (decltype(x))
#else                   /* VS2008 or older (or VS2010 in C mode) */
#define NO_DECLTYPE
#define DECLTYPE(x)
#endif
#elif defined(__BORLANDC__) || defined(__LCC__) || defined(__WATCOMC__)
#define NO_DECLTYPE
#define DECLTYPE(x)
#else                   /* GNU, Sun and other compilers */
#define DECLTYPE(x) (__typeof(x))
#endif

#ifdef NO_DECLTYPE
#define DECLTYPE_ASSIGN(dst,src)                                                 \
do {                                                                             \
  char **_da_dst = (char**)(&(dst));                                             \
  *_da_dst = (char*)(src);                                                       \
} while(0)
#else
#define DECLTYPE_ASSIGN(dst,src)                                                 \
do {                                                                             \
  (dst) = DECLTYPE(dst)(src);                                                    \
} while(0)
#endif

/* a number of the hash function use uint32_t which isn't defined on Pre VS2010 */
#if defined (_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <stdint.h>
#elif defined(__WATCOMC__)
#include <stdint.h>
#else
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#endif
#else
#include <stdint.h>
#endif

#define UTHASH_VERSION 1.9.9

#ifndef uthash_fatal
#define uthash_fatal(msg) exit(-1)        /* fatal error (out of memory,etc) */
#endif
#ifndef uthash_malloc
#define uthash_malloc(sz) malloc(sz)      /* malloc fcn                      */
#endif
#ifndef uthash_free
#define uthash_free(ptr,sz) free(ptr)     /* free fcn                        */
#endif

#ifndef uthash_noexpand_fyi
#define uthash_noexpand_fyi(tbl)          /* can be defined to log noexpand  */
#endif
#ifndef uthash_expand_fyi
#define uthash_expand_fyi(tbl)            /* can be defined to log expands   */
#endif

/* initial number of buckets */
#define HASH_INITIAL_NUM_BUCKETS 32U     /* initial number of buckets        */
#define HASH_INITIAL_NUM_BUCKETS_LOG2 5U /* lg2 of initial number of buckets */
#define HASH_BKT_CAPACITY_THRESH 10U     /* expand when bucket count reaches */

/* calculate the element whose hash handle address is hhe */
#define ELMT_FROM_HH(tbl,hhp) ((void*)(((char*)(hhp)) - ((tbl)->hho)))

#define HASH_FIND(hh,head,keyptr,keylen,out)                                     \
do {                                                                             \
  out=NULL;                                                                      \
  if (head != NULL) {                                                            \
     unsigned _hf_bkt,_hf_hashv;                                                 \
     HASH_FCN(keyptr,keylen, (head)->hh.tbl->num_buckets, _hf_hashv, _hf_bkt);   \
     if (HASH_BLOOM_TEST((head)->hh.tbl, _hf_hashv) != 0) {                      \
       HASH_FIND_IN_BKT((head)->hh.tbl, hh, (head)->hh.tbl->buckets[ _hf_bkt ],  \
                        keyptr,keylen,out);                                      \
     }                                                                           \
  }                                                                              \
} while (0)

#ifdef HASH_BLOOM
#define HASH_BLOOM_BITLEN (1UL << HASH_BLOOM)
#define HASH_BLOOM_BYTELEN (HASH_BLOOM_BITLEN/8UL) + (((HASH_BLOOM_BITLEN%8UL)!=0UL) ? 1UL : 0UL)
#define HASH_BLOOM_MAKE(tbl)                                                     \
do {                                                                             \
  (tbl)->bloom_nbits = HASH_BLOOM;                                               \
  (tbl)->bloom_bv = (uint8_t*)uthash_malloc(HASH_BLOOM_BYTELEN);                 \
  if (!((tbl)->bloom_bv))  { uthash_fatal( "out of memory"); }                   \
  memset((tbl)->bloom_bv, 0, HASH_BLOOM_BYTELEN);                                \
  (tbl)->bloom_sig = HASH_BLOOM_SIGNATURE;                                       \
} while (0)

#define HASH_BLOOM_FREE(tbl)                                                     \
do {                                                                             \
  uthash_free((tbl)->bloom_bv, HASH_BLOOM_BYTELEN);                              \
} while (0)

#define HASH_BLOOM_BITSET(bv,idx) (bv[(idx)/8U] |= (1U << ((idx)%8U)))
#define HASH_BLOOM_BITTEST(bv,idx) (bv[(idx)/8U] & (1U << ((idx)%8U)))

#define HASH_BLOOM_ADD(tbl,hashv)                                                \
  HASH_BLOOM_BITSET((tbl)->bloom_bv, (hashv & (uint32_t)((1ULL << (tbl)->bloom_nbits) - 1U)))

#define HASH_BLOOM_TEST(tbl,hashv)                                               \
  HASH_BLOOM_BITTEST((tbl)->bloom_bv, (hashv & (uint32_t)((1ULL << (tbl)->bloom_nbits) - 1U)))

#else
#define HASH_BLOOM_MAKE(tbl)
#define HASH_BLOOM_FREE(tbl)
#define HASH_BLOOM_ADD(tbl,hashv)
#define HASH_BLOOM_TEST(tbl,hashv) (1)
#define HASH_BLOOM_BYTELEN 0U
#endif

#define HASH_MAKE_TABLE(hh,head)                                                 \
do {                                                                             \
  (head)->hh.tbl = (UT_hash_table*)uthash_malloc(                                \
                  sizeof(UT_hash_table));                                        \
  if (!((head)->hh.tbl))  { uthash_fatal( "out of memory"); }                    \
  memset((head)->hh.tbl, 0, sizeof(UT_hash_table));                              \
  (head)->hh.tbl->tail = &((head)->hh);                                          \
  (head)->hh.tbl->num_buckets = HASH_INITIAL_NUM_BUCKETS;                        \
  (head)->hh.tbl->log2_num_buckets = HASH_INITIAL_NUM_BUCKETS_LOG2;              \
  (head)->hh.tbl->hho = (char*)(&(head)->hh) - (char*)(head);                    \
  (head)->hh.tbl->buckets = (UT_hash_bucket*)uthash_malloc(                      \
          HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));               \
  if (! (head)->hh.tbl->buckets) { uthash_fatal( "out of memory"); }             \
  memset((head)->hh.tbl->buckets, 0,                                             \
          HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));               \
  HASH_BLOOM_MAKE((head)->hh.tbl);                                               \
  (head)->hh.tbl->signature = HASH_SIGNATURE;                                    \
} while(0)

#define HASH_ADD(hh,head,fieldname,keylen_in,add)                                \
        HASH_ADD_KEYPTR(hh,head,&((add)->fieldname),keylen_in,add)

#define HASH_REPLACE(hh,head,fieldname,keylen_in,add,replaced)                   \
do {                                                                             \
  replaced=NULL;                                                                 \
  HASH_FIND(hh,head,&((add)->fieldname),keylen_in,replaced);                     \
  if (replaced!=NULL) {                                                          \
     HASH_DELETE(hh,head,replaced);                                              \
  }                                                                              \
  HASH_ADD(hh,head,fieldname,keylen_in,add);                                     \
} while(0)

#define HASH_ADD_KEYPTR(hh,head,keyptr,keylen_in,add)                            \
do {                                                                             \
 unsigned _ha_bkt;                                                               \
 (add)->hh.next = NULL;                                                          \
 (add)->hh.key = (char*)(keyptr);                                                \
 (add)->hh.keylen = (unsigned)(keylen_in);                                       \
 if (!(head)) {                                                                  \
    head = (add);                                                                \
    (head)->hh.prev = NULL;                                                      \
    HASH_MAKE_TABLE(hh,head);                                                    \
 } else {                                                                        \
    (head)->hh.tbl->tail->next = (add);                                          \
    (add)->hh.prev = ELMT_FROM_HH((head)->hh.tbl, (head)->hh.tbl->tail);         \
    (head)->hh.tbl->tail = &((add)->hh);                                         \
 }                                                                               \
 (head)->hh.tbl->num_items++;                                                    \
 (add)->hh.tbl = (head)->hh.tbl;                                                 \
 HASH_FCN(keyptr,keylen_in, (head)->hh.tbl->num_buckets,                         \
         (add)->hh.hashv, _ha_bkt);                                              \
 HASH_ADD_TO_BKT((head)->hh.tbl->buckets[_ha_bkt],&(add)->hh);                   \
 HASH_BLOOM_ADD((head)->hh.tbl,(add)->hh.hashv);                                 \
 HASH_EMIT_KEY(hh,head,keyptr,keylen_in);                                        \
 HASH_FSCK(hh,head);                                                             \
} while(0)

#define HASH_TO_BKT( hashv, num_bkts, bkt )                                      \
do {                                                                             \
  bkt = ((hashv) & ((num_bkts) - 1U));                                           \
} while(0)

/* delete "delptr" from the hash table.
 * "the usual" patch-up process for the app-order doubly-linked-list.
 * The use of _hd_hh_del below deserves special explanation.
 * These used to be expressed using (delptr) but that led to a bug
 * if someone used the same symbol for the head and deletee, like
 *  HASH_DELETE(hh,users,users);
 * We want that to work, but by changing the head (users) below
 * we were forfeiting our ability to further refer to the deletee (users)
 * in the patch-up process. Solution: use scratch space to
 * copy the deletee pointer, then the latter references are via that
 * scratch pointer rather than through the repointed (users) symbol.
 */
#define HASH_DELETE(hh,head,delptr)                                              \
do {                                                                             \
    struct UT_hash_handle *_hd_hh_del;                                           \
    if ( ((delptr)->hh.prev == NULL) && ((delptr)->hh.next == NULL) )  {         \
        uthash_free((head)->hh.tbl->buckets,                                     \
                    (head)->hh.tbl->num_buckets*sizeof(struct UT_hash_bucket) ); \
        HASH_BLOOM_FREE((head)->hh.tbl);                                         \
        uthash_free((head)->hh.tbl, sizeof(UT_hash_table));                      \
        head = NULL;                                                             \
    } else {                                                                     \
        unsigned _hd_bkt;                                                        \
        _hd_hh_del = &((delptr)->hh);                                            \
        if ((delptr) == ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->tail)) {     \
            (head)->hh.tbl->tail =                                               \
                (UT_hash_handle*)((ptrdiff_t)((delptr)->hh.prev) +               \
                (head)->hh.tbl->hho);                                            \
        }                                                                        \
        if ((delptr)->hh.prev != NULL) {                                         \
            ((UT_hash_handle*)((ptrdiff_t)((delptr)->hh.prev) +                  \
                    (head)->hh.tbl->hho))->next = (delptr)->hh.next;             \
        } else {                                                                 \
            DECLTYPE_ASSIGN(head,(delptr)->hh.next);                             \
        }                                                                        \
        if (_hd_hh_del->next != NULL) {                                          \
            ((UT_hash_handle*)((ptrdiff_t)_hd_hh_del->next +                     \
                    (head)->hh.tbl->hho))->prev =                                \
                    _hd_hh_del->prev;                                            \
        }                                                                        \
        HASH_TO_BKT( _hd_hh_del->hashv, (head)->hh.tbl->num_buckets, _hd_bkt);   \
        HASH_DEL_IN_BKT(hh,(head)->hh.tbl->buckets[_hd_bkt], _hd_hh_del);        \
        (head)->hh.tbl->num_items--;                                             \
    }                                                                            \
    HASH_FSCK(hh,head);                                                          \
} while (0)


/* convenience forms of HASH_FIND/HASH_ADD/HASH_DEL */
#define HASH_FIND_STR(head,findstr,out)                                          \
    HASH_FIND(hh,head,findstr,(unsigned)strlen(findstr),out)
#define HASH_ADD_STR(head,strfield,add)                                          \
    HASH_ADD(hh,head,strfield[0],(unsigned int)strlen(add->strfield),add)
#define HASH_REPLACE_STR(head,strfield,add,replaced)                             \
    HASH_REPLACE(hh,head,strfield[0],(unsigned)strlen(add->strfield),add,replaced)
#define HASH_FIND_INT(head,findint,out)                                          \
    HASH_FIND(hh,head,findint,sizeof(int),out)
#define HASH_ADD_INT(head,intfield,add)                                          \
    HASH_ADD(hh,head,intfield,sizeof(int),add)
#define HASH_REPLACE_INT(head,intfield,add,replaced)                             \
    HASH_REPLACE(hh,head,intfield,sizeof(int),add,replaced)
#define HASH_FIND_PTR(head,findptr,out)                                          \
    HASH_FIND(hh,head,findptr,sizeof(void *),out)
#define HASH_ADD_PTR(head,ptrfield,add)                                          \
    HASH_ADD(hh,head,ptrfield,sizeof(void *),add)
#define HASH_REPLACE_PTR(head,ptrfield,add,replaced)                             \
    HASH_REPLACE(hh,head,ptrfield,sizeof(void *),add,replaced)
#define HASH_DEL(head,delptr)                                                    \
    HASH_DELETE(hh,head,delptr)

/* HASH_FSCK checks hash integrity on every add/delete when HASH_DEBUG is defined.
 * This is for uthash developer only; it compiles away if HASH_DEBUG isn't defined.
 */
#ifdef HASH_DEBUG
#define HASH_OOPS(...) do { fprintf(stderr,__VA_ARGS__); exit(-1); } while (0)
#define HASH_FSCK(hh,head)                                                       \
do {                                                                             \
    struct UT_hash_handle *_thh;                                                 \
    if (head) {                                                                  \
        unsigned _bkt_i;                                                         \
        unsigned _count;                                                         \
        char *_prev;                                                             \
        _count = 0;                                                              \
        for( _bkt_i = 0; _bkt_i < (head)->hh.tbl->num_buckets; _bkt_i++) {       \
            unsigned _bkt_count = 0;                                             \
            _thh = (head)->hh.tbl->buckets[_bkt_i].hh_head;                      \
            _prev = NULL;                                                        \
            while (_thh) {                                                       \
               if (_prev != (char*)(_thh->hh_prev)) {                            \
                   HASH_OOPS("invalid hh_prev %p, actual %p\n",                  \
                    _thh->hh_prev, _prev );                                      \
               }                                                                 \
               _bkt_count++;                                                     \
               _prev = (char*)(_thh);                                            \
               _thh = _thh->hh_next;                                             \
            }                                                                    \
            _count += _bkt_count;                                                \
            if ((head)->hh.tbl->buckets[_bkt_i].count !=  _bkt_count) {          \
               HASH_OOPS("invalid bucket count %u, actual %u\n",                 \
                (head)->hh.tbl->buckets[_bkt_i].count, _bkt_count);              \
            }                                                                    \
        }                                                                        \
        if (_count != (head)->hh.tbl->num_items) {                               \
            HASH_OOPS("invalid hh item count %u, actual %u\n",                   \
                (head)->hh.tbl->num_items, _count );                             \
        }                                                                        \
        /* traverse hh in app order; check next/prev integrity, count */         \
        _count = 0;                                                              \
        _prev = NULL;                                                            \
        _thh =  &(head)->hh;                                                     \
        while (_thh) {                                                           \
           _count++;                                                             \
           if (_prev !=(char*)(_thh->prev)) {                                    \
              HASH_OOPS("invalid prev %p, actual %p\n",                          \
                    _thh->prev, _prev );                                         \
           }                                                                     \
           _prev = (char*)ELMT_FROM_HH((head)->hh.tbl, _thh);                    \
           _thh = ( _thh->next ?  (UT_hash_handle*)((char*)(_thh->next) +        \
                                  (head)->hh.tbl->hho) : NULL );                 \
        }                                                                        \
        if (_count != (head)->hh.tbl->num_items) {                               \
            HASH_OOPS("invalid app item count %u, actual %u\n",                  \
                (head)->hh.tbl->num_items, _count );                             \
        }                                                                        \
    }                                                                            \
} while (0)
#else
#define HASH_FSCK(hh,head)
#endif

/* When compiled with -DHASH_EMIT_KEYS, length-prefixed keys are emitted to
 * the descriptor to which this macro is defined for tuning the hash function.
 * The app can #include <unistd.h> to get the prototype for write(2). */
#ifdef HASH_EMIT_KEYS
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)                                   \
do {                                                                             \
    unsigned _klen = fieldlen;                                                   \
    write(HASH_EMIT_KEYS, &_klen, sizeof(_klen));                                \
    write(HASH_EMIT_KEYS, keyptr, (unsigned long)fieldlen);                      \
} while (0)
#else
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)
#endif

/* default to Jenkin's hash unless overridden e.g. DHASH_FUNCTION=HASH_SAX */
#ifdef HASH_FUNCTION
#define HASH_FCN HASH_FUNCTION
#else
#define HASH_FCN HASH_JEN
#endif

/* The Bernstein hash function, used in Perl prior to v5.6. Note (x<<5+x)=x*33. */
#define HASH_BER(key,keylen,num_bkts,hashv,bkt)                                  \
do {                                                                             \
  unsigned _hb_keylen=(unsigned)keylen;                                          \
  const unsigned char *_hb_key=(const unsigned char*)(key);                      \
  (hashv) = 0;                                                                   \
  while (_hb_keylen-- != 0U) {                                                   \
      (hashv) = (((hashv) << 5) + (hashv)) + *_hb_key++;                         \
  }                                                                              \
  bkt = (hashv) & (num_bkts-1U);                                                 \
} while (0)


/* SAX/FNV/OAT/JEN hash functions are macro variants of those listed at
 * http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx */
#define HASH_SAX(key,keylen,num_bkts,hashv,bkt)                                  \
do {                                                                             \
  unsigned _sx_i;                                                                \
  const unsigned char *_hs_key=(const unsigned char*)(key);                      \
  hashv = 0;                                                                     \
  for(_sx_i=0; _sx_i < keylen; _sx_i++) {                                        \
      hashv ^= (hashv << 5) + (hashv >> 2) + _hs_key[_sx_i];                     \
  }                                                                              \
  bkt = hashv & (num_bkts-1U);                                                   \
} while (0)
/* FNV-1a variation */
#define HASH_FNV(key,keylen,num_bkts,hashv,bkt)                                  \
do {                                                                             \
  unsigned _fn_i;                                                                \
  const unsigned char *_hf_key=(const unsigned char*)(key);                      \
  hashv = 2166136261U;                                                           \
  for(_fn_i=0; _fn_i < keylen; _fn_i++) {                                        \
      hashv = hashv ^ _hf_key[_fn_i];                                            \
      hashv = hashv * 16777619U;                                                 \
  }                                                                              \
  bkt = hashv & (num_bkts-1U);                                                   \
} while(0)

#define HASH_OAT(key,keylen,num_bkts,hashv,bkt)                                  \
do {                                                                             \
  unsigned _ho_i;                                                                \
  const unsigned char *_ho_key=(const unsigned char*)(key);                      \
  hashv = 0;                                                                     \
  for(_ho_i=0; _ho_i < keylen; _ho_i++) {                                        \
      hashv += _ho_key[_ho_i];                                                   \
      hashv += (hashv << 10);                                                    \
      hashv ^= (hashv >> 6);                                                     \
  }                                                                              \
  hashv += (hashv << 3);                                                         \
  hashv ^= (hashv >> 11);                                                        \
  hashv += (hashv << 15);                                                        \
  bkt = hashv & (num_bkts-1U);                                                   \
} while(0)

#define HASH_JEN_MIX(a,b,c)                                                      \
do {                                                                             \
  a -= b; a -= c; a ^= ( c >> 13 );                                              \
  b -= c; b -= a; b ^= ( a << 8 );                                               \
  c -= a; c -= b; c ^= ( b >> 13 );                                              \
  a -= b; a -= c; a ^= ( c >> 12 );                                              \
  b -= c; b -= a; b ^= ( a << 16 );                                              \
  c -= a; c -= b; c ^= ( b >> 5 );                                               \
  a -= b; a -= c; a ^= ( c >> 3 );                                               \
  b -= c; b -= a; b ^= ( a << 10 );                                              \
  c -= a; c -= b; c ^= ( b >> 15 );                                              \
} while (0)

#define HASH_JEN(key,keylen,num_bkts,hashv,bkt)                                  \
do {                                                                             \
  unsigned _hj_i,_hj_j,_hj_k;                                                    \
  unsigned const char *_hj_key=(unsigned const char*)(key);                      \
  hashv = 0xfeedbeefu;                                                           \
  _hj_i = _hj_j = 0x9e3779b9u;                                                   \
  _hj_k = (unsigned)(keylen);                                                    \
  while (_hj_k >= 12U) {                                                         \
    _hj_i +=    (_hj_key[0] + ( (unsigned)_hj_key[1] << 8 )                      \
        + ( (unsigned)_hj_key[2] << 16 )                                         \
        + ( (unsigned)_hj_key[3] << 24 ) );                                      \
    _hj_j +=    (_hj_key[4] + ( (unsigned)_hj_key[5] << 8 )                      \
        + ( (unsigned)_hj_key[6] << 16 )                                         \
        + ( (unsigned)_hj_key[7] << 24 ) );                                      \
    hashv += (_hj_key[8] + ( (unsigned)_hj_key[9] << 8 )                         \
        + ( (unsigned)_hj_key[10] << 16 )                                        \
        + ( (unsigned)_hj_key[11] << 24 ) );                                     \
                                                                                 \
     HASH_JEN_MIX(_hj_i, _hj_j, hashv);                                          \
                                                                                 \
     _hj_key += 12;                                                              \
     _hj_k -= 12U;                                                               \
  }                                                                              \
  hashv += (unsigned)(keylen);                                                   \
  switch ( _hj_k ) {                                                             \
     case 11: hashv += ( (unsigned)_hj_key[10] << 24 ); /* FALLTHROUGH */        \
     case 10: hashv += ( (unsigned)_hj_key[9] << 16 );  /* FALLTHROUGH */        \
     case 9:  hashv += ( (unsigned)_hj_key[8] << 8 );   /* FALLTHROUGH */        \
     case 8:  _hj_j += ( (unsigned)_hj_key[7] << 24 );  /* FALLTHROUGH */        \
     case 7:  _hj_j += ( (unsigned)_hj_key[6] << 16 );  /* FALLTHROUGH */        \
     case 6:  _hj_j += ( (unsigned)_hj_key[5] << 8 );   /* FALLTHROUGH */        \
     case 5:  _hj_j += _hj_key[4];                      /* FALLTHROUGH */        \
     case 4:  _hj_i += ( (unsigned)_hj_key[3] << 24 );  /* FALLTHROUGH */        \
     case 3:  _hj_i += ( (unsigned)_hj_key[2] << 16 );  /* FALLTHROUGH */        \
     case 2:  _hj_i += ( (unsigned)_hj_key[1] << 8 );   /* FALLTHROUGH */        \
     case 1:  _hj_i += _hj_key[0];                                               \
  }                                                                              \
  HASH_JEN_MIX(_hj_i, _hj_j, hashv);                                             \
  bkt = hashv & (num_bkts-1U);                                                   \
} while(0)

/* The Paul Hsieh hash function */
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__)             \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)             \
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif
#define HASH_SFH(key,keylen,num_bkts,hashv,bkt)                                  \
do {                                                                             \
  unsigned const char *_sfh_key=(unsigned const char*)(key);                     \
  uint32_t _sfh_tmp, _sfh_len = (uint32_t)keylen;                                \
                                                                                 \
  unsigned _sfh_rem = _sfh_len & 3U;                                             \
  _sfh_len >>= 2;                                                                \
  hashv = 0xcafebabeu;                                                           \
                                                                                 \
  /* Main loop */                                                                \
  for (;_sfh_len > 0U; _sfh_len--) {                                             \
    hashv    += get16bits (_sfh_key);                                            \
    _sfh_tmp  = ((uint32_t)(get16bits (_sfh_key+2)) << 11) ^ hashv;              \
    hashv     = (hashv << 16) ^ _sfh_tmp;                                        \
    _sfh_key += 2U*sizeof (uint16_t);                                            \
    hashv    += hashv >> 11;                                                     \
  }                                                                              \
                                                                                 \
  /* Handle end cases */                                                         \
  switch (_sfh_rem) {                                                            \
    case 3: hashv += get16bits (_sfh_key);                                       \
            hashv ^= hashv << 16;                                                \
            hashv ^= (uint32_t)(_sfh_key[sizeof (uint16_t)]) << 18;              \
            hashv += hashv >> 11;                                                \
            break;                                                               \
    case 2: hashv += get16bits (_sfh_key);                                       \
            hashv ^= hashv << 11;                                                \
            hashv += hashv >> 17;                                                \
            break;                                                               \
    case 1: hashv += *_sfh_key;                                                  \
            hashv ^= hashv << 10;                                                \
            hashv += hashv >> 1;                                                 \
  }                                                                              \
                                                                                 \
    /* Force "avalanching" of final 127 bits */                                  \
    hashv ^= hashv << 3;                                                         \
    hashv += hashv >> 5;                                                         \
    hashv ^= hashv << 4;                                                         \
    hashv += hashv >> 17;                                                        \
    hashv ^= hashv << 25;                                                        \
    hashv += hashv >> 6;                                                         \
    bkt = hashv & (num_bkts-1U);                                                 \
} while(0)

#ifdef HASH_USING_NO_STRICT_ALIASING
/* The MurmurHash exploits some CPU's (x86,x86_64) tolerance for unaligned reads.
 * For other types of CPU's (e.g. Sparc) an unaligned read causes a bus error.
 * MurmurHash uses the faster approach only on CPU's where we know it's safe.
 *
 * Note the preprocessor built-in defines can be emitted using:
 *
 *   gcc -m64 -dM -E - < /dev/null                  (on gcc)
 *   cc -## a.c (where a.c is a simple test file)   (Sun Studio)
 */
#if (defined(__i386__) || defined(__x86_64__)  || defined(_M_IX86))
#define MUR_GETBLOCK(p,i) p[i]
#else /* non intel */
#define MUR_PLUS0_ALIGNED(p) (((unsigned long)p & 3UL) == 0UL)
#define MUR_PLUS1_ALIGNED(p) (((unsigned long)p & 3UL) == 1UL)
#define MUR_PLUS2_ALIGNED(p) (((unsigned long)p & 3UL) == 2UL)
#define MUR_PLUS3_ALIGNED(p) (((unsigned long)p & 3UL) == 3UL)
#define WP(p) ((uint32_t*)((unsigned long)(p) & ~3UL))
#if (defined(__BIG_ENDIAN__) || defined(SPARC) || defined(__ppc__) || defined(__ppc64__))
#define MUR_THREE_ONE(p) ((((*WP(p))&0x00ffffff) << 8) | (((*(WP(p)+1))&0xff000000) >> 24))
#define MUR_TWO_TWO(p)   ((((*WP(p))&0x0000ffff) <<16) | (((*(WP(p)+1))&0xffff0000) >> 16))
#define MUR_ONE_THREE(p) ((((*WP(p))&0x000000ff) <<24) | (((*(WP(p)+1))&0xffffff00) >>  8))
#else /* assume little endian non-intel */
#define MUR_THREE_ONE(p) ((((*WP(p))&0xffffff00) >> 8) | (((*(WP(p)+1))&0x000000ff) << 24))
#define MUR_TWO_TWO(p)   ((((*WP(p))&0xffff0000) >>16) | (((*(WP(p)+1))&0x0000ffff) << 16))
#define MUR_ONE_THREE(p) ((((*WP(p))&0xff000000) >>24) | (((*(WP(p)+1))&0x00ffffff) <<  8))
#endif
#define MUR_GETBLOCK(p,i) (MUR_PLUS0_ALIGNED(p) ? ((p)[i]) :           \
                            (MUR_PLUS1_ALIGNED(p) ? MUR_THREE_ONE(p) : \
                             (MUR_PLUS2_ALIGNED(p) ? MUR_TWO_TWO(p) :  \
                                                      MUR_ONE_THREE(p))))
#endif
#define MUR_ROTL32(x,r) (((x) << (r)) | ((x) >> (32 - (r))))
#define MUR_FMIX(_h) \
do {                 \
  _h ^= _h >> 16;    \
  _h *= 0x85ebca6bu; \
  _h ^= _h >> 13;    \
  _h *= 0xc2b2ae35u; \
  _h ^= _h >> 16;    \
} while(0)

#define HASH_MUR(key,keylen,num_bkts,hashv,bkt)                        \
do {                                                                   \
  const uint8_t *_mur_data = (const uint8_t*)(key);                    \
  const int _mur_nblocks = (int)(keylen) / 4;                          \
  uint32_t _mur_h1 = 0xf88D5353u;                                      \
  uint32_t _mur_c1 = 0xcc9e2d51u;                                      \
  uint32_t _mur_c2 = 0x1b873593u;                                      \
  uint32_t _mur_k1 = 0;                                                \
  const uint8_t *_mur_tail;                                            \
  const uint32_t *_mur_blocks = (const uint32_t*)(_mur_data+(_mur_nblocks*4)); \
  int _mur_i;                                                          \
  for(_mur_i = -_mur_nblocks; _mur_i!=0; _mur_i++) {                   \
    _mur_k1 = MUR_GETBLOCK(_mur_blocks,_mur_i);                        \
    _mur_k1 *= _mur_c1;                                                \
    _mur_k1 = MUR_ROTL32(_mur_k1,15);                                  \
    _mur_k1 *= _mur_c2;                                                \
    _mur_h1 ^= _mur_k1;                                                \
    _mur_h1 = MUR_ROTL32(_mur_h1,13);                                  \
    _mur_h1 = (_mur_h1*5U) + 0xe6546b64u;                              \
  }                                                                    \
  _mur_tail = (const uint8_t*)(_mur_data + (_mur_nblocks*4));          \
  _mur_k1=0;                                                           \
  switch((keylen) & 3U) {                                              \
    case 3: _mur_k1 ^= (uint32_t)_mur_tail[2] << 16; /* FALLTHROUGH */ \
    case 2: _mur_k1 ^= (uint32_t)_mur_tail[1] << 8;  /* FALLTHROUGH */ \
    case 1: _mur_k1 ^= (uint32_t)_mur_tail[0];                         \
    _mur_k1 *= _mur_c1;                                                \
    _mur_k1 = MUR_ROTL32(_mur_k1,15);                                  \
    _mur_k1 *= _mur_c2;                                                \
    _mur_h1 ^= _mur_k1;                                                \
  }                                                                    \
  _mur_h1 ^= (uint32_t)(keylen);                                       \
  MUR_FMIX(_mur_h1);                                                   \
  hashv = _mur_h1;                                                     \
  bkt = hashv & (num_bkts-1U);                                         \
} while(0)
#endif  /* HASH_USING_NO_STRICT_ALIASING */

/* key comparison function; return 0 if keys equal */
#define HASH_KEYCMP(a,b,len) memcmp(a,b,(unsigned long)(len))

/* iterate over items in a known bucket to find desired item */
#define HASH_FIND_IN_BKT(tbl,hh,head,keyptr,keylen_in,out)                       \
do {                                                                             \
 if (head.hh_head != NULL) { DECLTYPE_ASSIGN(out,ELMT_FROM_HH(tbl,head.hh_head)); } \
 else { out=NULL; }                                                              \
 while (out != NULL) {                                                           \
    if ((out)->hh.keylen == (keylen_in)) {                                       \
        if ((HASH_KEYCMP((out)->hh.key,keyptr,keylen_in)) == 0) { break; }         \
    }                                                                            \
    if ((out)->hh.hh_next != NULL) { DECLTYPE_ASSIGN(out,ELMT_FROM_HH(tbl,(out)->hh.hh_next)); } \
    else { out = NULL; }                                                         \
 }                                                                               \
} while(0)

/* add an item to a bucket  */
#define HASH_ADD_TO_BKT(head,addhh)                                              \
do {                                                                             \
 head.count++;                                                                   \
 (addhh)->hh_next = head.hh_head;                                                \
 (addhh)->hh_prev = NULL;                                                        \
 if (head.hh_head != NULL) { (head).hh_head->hh_prev = (addhh); }                \
 (head).hh_head=addhh;                                                           \
 if ((head.count >= ((head.expand_mult+1U) * HASH_BKT_CAPACITY_THRESH))          \
     && ((addhh)->tbl->noexpand != 1U)) {                                        \
       HASH_EXPAND_BUCKETS((addhh)->tbl);                                        \
 }                                                                               \
} while(0)

/* remove an item from a given bucket */
#define HASH_DEL_IN_BKT(hh,head,hh_del)                                          \
    (head).count--;                                                              \
    if ((head).hh_head == hh_del) {                                              \
      (head).hh_head = hh_del->hh_next;                                          \
    }                                                                            \
    if (hh_del->hh_prev) {                                                       \
        hh_del->hh_prev->hh_next = hh_del->hh_next;                              \
    }                                                                            \
    if (hh_del->hh_next) {                                                       \
        hh_del->hh_next->hh_prev = hh_del->hh_prev;                              \
    }

/* Bucket expansion has the effect of doubling the number of buckets
 * and redistributing the items into the new buckets. Ideally the
 * items will distribute more or less evenly into the new buckets
 * (the extent to which this is true is a measure of the quality of
 * the hash function as it applies to the key domain).
 *
 * With the items distributed into more buckets, the chain length
 * (item count) in each bucket is reduced. Thus by expanding buckets
 * the hash keeps a bound on the chain length. This bounded chain
 * length is the essence of how a hash provides constant time lookup.
 *
 * The calculation of tbl->ideal_chain_maxlen below deserves some
 * explanation. First, keep in mind that we're calculating the ideal
 * maximum chain length based on the *new* (doubled) bucket count.
 * In fractions this is just n/b (n=number of items,b=new num buckets).
 * Since the ideal chain length is an integer, we want to calculate
 * ceil(n/b). We don't depend on floating point arithmetic in this
 * hash, so to calculate ceil(n/b) with integers we could write
 *
 *      ceil(n/b) = (n/b) + ((n%b)?1:0)
 *
 * and in fact a previous version of this hash did just that.
 * But now we have improved things a bit by recognizing that b is
 * always a power of two. We keep its base 2 log handy (call it lb),
 * so now we can write this with a bit shift and logical AND:
 *
 *      ceil(n/b) = (n>>lb) + ( (n & (b-1)) ? 1:0)
 *
 */
#define HASH_EXPAND_BUCKETS(tbl)                                                 \
do {                                                                             \
    unsigned _he_bkt;                                                            \
    unsigned _he_bkt_i;                                                          \
    struct UT_hash_handle *_he_thh, *_he_hh_nxt;                                 \
    UT_hash_bucket *_he_new_buckets, *_he_newbkt;                                \
    _he_new_buckets = (UT_hash_bucket*)uthash_malloc(                            \
             2UL * tbl->num_buckets * sizeof(struct UT_hash_bucket));            \
    if (!_he_new_buckets) { uthash_fatal( "out of memory"); }                    \
    memset(_he_new_buckets, 0,                                                   \
            2UL * tbl->num_buckets * sizeof(struct UT_hash_bucket));             \
    tbl->ideal_chain_maxlen =                                                    \
       (tbl->num_items >> (tbl->log2_num_buckets+1U)) +                          \
       (((tbl->num_items & ((tbl->num_buckets*2U)-1U)) != 0U) ? 1U : 0U);        \
    tbl->nonideal_items = 0;                                                     \
    for(_he_bkt_i = 0; _he_bkt_i < tbl->num_buckets; _he_bkt_i++)                \
    {                                                                            \
        _he_thh = tbl->buckets[ _he_bkt_i ].hh_head;                             \
        while (_he_thh != NULL) {                                                \
           _he_hh_nxt = _he_thh->hh_next;                                        \
           HASH_TO_BKT( _he_thh->hashv, tbl->num_buckets*2U, _he_bkt);           \
           _he_newbkt = &(_he_new_buckets[ _he_bkt ]);                           \
           if (++(_he_newbkt->count) > tbl->ideal_chain_maxlen) {                \
             tbl->nonideal_items++;                                              \
             _he_newbkt->expand_mult = _he_newbkt->count /                       \
                                        tbl->ideal_chain_maxlen;                 \
           }                                                                     \
           _he_thh->hh_prev = NULL;                                              \
           _he_thh->hh_next = _he_newbkt->hh_head;                               \
           if (_he_newbkt->hh_head != NULL) { _he_newbkt->hh_head->hh_prev =     \
                _he_thh; }                                                       \
           _he_newbkt->hh_head = _he_thh;                                        \
           _he_thh = _he_hh_nxt;                                                 \
        }                                                                        \
    }                                                                            \
    uthash_free( tbl->buckets, tbl->num_buckets*sizeof(struct UT_hash_bucket) ); \
    tbl->num_buckets *= 2U;                                                      \
    tbl->log2_num_buckets++;                                                     \
    tbl->buckets = _he_new_buckets;                                              \
    tbl->ineff_expands = (tbl->nonideal_items > (tbl->num_items >> 1)) ?         \
        (tbl->ineff_expands+1U) : 0U;                                            \
    if (tbl->ineff_expands > 1U) {                                               \
        tbl->noexpand=1;                                                         \
        uthash_noexpand_fyi(tbl);                                                \
    }                                                                            \
    uthash_expand_fyi(tbl);                                                      \
} while(0)


/* This is an adaptation of Simon Tatham's O(n log(n)) mergesort */
/* Note that HASH_SORT assumes the hash handle name to be hh.
 * HASH_SRT was added to allow the hash handle name to be passed in. */
#define HASH_SORT(head,cmpfcn) HASH_SRT(hh,head,cmpfcn)
#define HASH_SRT(hh,head,cmpfcn)                                                 \
do {                                                                             \
  unsigned _hs_i;                                                                \
  unsigned _hs_looping,_hs_nmerges,_hs_insize,_hs_psize,_hs_qsize;               \
  struct UT_hash_handle *_hs_p, *_hs_q, *_hs_e, *_hs_list, *_hs_tail;            \
  if (head != NULL) {                                                            \
      _hs_insize = 1;                                                            \
      _hs_looping = 1;                                                           \
      _hs_list = &((head)->hh);                                                  \
      while (_hs_looping != 0U) {                                                \
          _hs_p = _hs_list;                                                      \
          _hs_list = NULL;                                                       \
          _hs_tail = NULL;                                                       \
          _hs_nmerges = 0;                                                       \
          while (_hs_p != NULL) {                                                \
              _hs_nmerges++;                                                     \
              _hs_q = _hs_p;                                                     \
              _hs_psize = 0;                                                     \
              for ( _hs_i = 0; _hs_i  < _hs_insize; _hs_i++ ) {                  \
                  _hs_psize++;                                                   \
                  _hs_q = (UT_hash_handle*)((_hs_q->next != NULL) ?              \
                          ((void*)((char*)(_hs_q->next) +                        \
                          (head)->hh.tbl->hho)) : NULL);                         \
                  if (! (_hs_q) ) { break; }                                     \
              }                                                                  \
              _hs_qsize = _hs_insize;                                            \
              while ((_hs_psize > 0U) || ((_hs_qsize > 0U) && (_hs_q != NULL))) {\
                  if (_hs_psize == 0U) {                                         \
                      _hs_e = _hs_q;                                             \
                      _hs_q = (UT_hash_handle*)((_hs_q->next != NULL) ?          \
                              ((void*)((char*)(_hs_q->next) +                    \
                              (head)->hh.tbl->hho)) : NULL);                     \
                      _hs_qsize--;                                               \
                  } else if ( (_hs_qsize == 0U) || (_hs_q == NULL) ) {           \
                      _hs_e = _hs_p;                                             \
                      if (_hs_p != NULL){                                        \
                        _hs_p = (UT_hash_handle*)((_hs_p->next != NULL) ?        \
                                ((void*)((char*)(_hs_p->next) +                  \
                                (head)->hh.tbl->hho)) : NULL);                   \
                       }                                                         \
                      _hs_psize--;                                               \
                  } else if ((                                                   \
                      cmpfcn(DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl,_hs_p)), \
                             DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl,_hs_q))) \
                             ) <= 0) {                                           \
                      _hs_e = _hs_p;                                             \
                      if (_hs_p != NULL){                                        \
                        _hs_p = (UT_hash_handle*)((_hs_p->next != NULL) ?        \
                               ((void*)((char*)(_hs_p->next) +                   \
                               (head)->hh.tbl->hho)) : NULL);                    \
                       }                                                         \
                      _hs_psize--;                                               \
                  } else {                                                       \
                      _hs_e = _hs_q;                                             \
                      _hs_q = (UT_hash_handle*)((_hs_q->next != NULL) ?          \
                              ((void*)((char*)(_hs_q->next) +                    \
                              (head)->hh.tbl->hho)) : NULL);                     \
                      _hs_qsize--;                                               \
                  }                                                              \
                  if ( _hs_tail != NULL ) {                                      \
                      _hs_tail->next = ((_hs_e != NULL) ?                        \
                            ELMT_FROM_HH((head)->hh.tbl,_hs_e) : NULL);          \
                  } else {                                                       \
                      _hs_list = _hs_e;                                          \
                  }                                                              \
                  if (_hs_e != NULL) {                                           \
                  _hs_e->prev = ((_hs_tail != NULL) ?                            \
                     ELMT_FROM_HH((head)->hh.tbl,_hs_tail) : NULL);              \
                  }                                                              \
                  _hs_tail = _hs_e;                                              \
              }                                                                  \
              _hs_p = _hs_q;                                                     \
          }                                                                      \
          if (_hs_tail != NULL){                                                 \
            _hs_tail->next = NULL;                                               \
          }                                                                      \
          if ( _hs_nmerges <= 1U ) {                                             \
              _hs_looping=0;                                                     \
              (head)->hh.tbl->tail = _hs_tail;                                   \
              DECLTYPE_ASSIGN(head,ELMT_FROM_HH((head)->hh.tbl, _hs_list));      \
          }                                                                      \
          _hs_insize *= 2U;                                                      \
      }                                                                          \
      HASH_FSCK(hh,head);                                                        \
 }                                                                               \
} while (0)

/* This function selects items from one hash into another hash.
 * The end result is that the selected items have dual presence
 * in both hashes. There is no copy of the items made; rather
 * they are added into the new hash through a secondary hash
 * hash handle that must be present in the structure. */
#define HASH_SELECT(hh_dst, dst, hh_src, src, cond)                              \
do {                                                                             \
  unsigned _src_bkt, _dst_bkt;                                                   \
  void *_last_elt=NULL, *_elt;                                                   \
  UT_hash_handle *_src_hh, *_dst_hh, *_last_elt_hh=NULL;                         \
  ptrdiff_t _dst_hho = ((char*)(&(dst)->hh_dst) - (char*)(dst));                 \
  if (src != NULL) {                                                             \
    for(_src_bkt=0; _src_bkt < (src)->hh_src.tbl->num_buckets; _src_bkt++) {     \
      for(_src_hh = (src)->hh_src.tbl->buckets[_src_bkt].hh_head;                \
          _src_hh != NULL;                                                       \
          _src_hh = _src_hh->hh_next) {                                          \
          _elt = ELMT_FROM_HH((src)->hh_src.tbl, _src_hh);                       \
          if (cond(_elt)) {                                                      \
            _dst_hh = (UT_hash_handle*)(((char*)_elt) + _dst_hho);               \
            _dst_hh->key = _src_hh->key;                                         \
            _dst_hh->keylen = _src_hh->keylen;                                   \
            _dst_hh->hashv = _src_hh->hashv;                                     \
            _dst_hh->prev = _last_elt;                                           \
            _dst_hh->next = NULL;                                                \
            if (_last_elt_hh != NULL) { _last_elt_hh->next = _elt; }             \
            if (dst == NULL) {                                                   \
              DECLTYPE_ASSIGN(dst,_elt);                                         \
              HASH_MAKE_TABLE(hh_dst,dst);                                       \
            } else {                                                             \
              _dst_hh->tbl = (dst)->hh_dst.tbl;                                  \
            }                                                                    \
            HASH_TO_BKT(_dst_hh->hashv, _dst_hh->tbl->num_buckets, _dst_bkt);    \
            HASH_ADD_TO_BKT(_dst_hh->tbl->buckets[_dst_bkt],_dst_hh);            \
            (dst)->hh_dst.tbl->num_items++;                                      \
            _last_elt = _elt;                                                    \
            _last_elt_hh = _dst_hh;                                              \
          }                                                                      \
      }                                                                          \
    }                                                                            \
  }                                                                              \
  HASH_FSCK(hh_dst,dst);                                                         \
} while (0)

#define HASH_CLEAR(hh,head)                                                      \
do {                                                                             \
  if (head != NULL) {                                                            \
    uthash_free((head)->hh.tbl->buckets,                                         \
                (head)->hh.tbl->num_buckets*sizeof(struct UT_hash_bucket));      \
    HASH_BLOOM_FREE((head)->hh.tbl);                                             \
    uthash_free((head)->hh.tbl, sizeof(UT_hash_table));                          \
    (head)=NULL;                                                                 \
  }                                                                              \
} while(0)

#define HASH_OVERHEAD(hh,head)                                                   \
 ((head != NULL) ? (                                                             \
 (size_t)(((head)->hh.tbl->num_items   * sizeof(UT_hash_handle))   +             \
          ((head)->hh.tbl->num_buckets * sizeof(UT_hash_bucket))   +             \
           sizeof(UT_hash_table)                                   +             \
           (HASH_BLOOM_BYTELEN))) : 0U)

#ifdef NO_DECLTYPE
#define HASH_ITER(hh,head,el,tmp)                                                \
for(((el)=(head)), ((*(char**)(&(tmp)))=(char*)((head!=NULL)?(head)->hh.next:NULL)); \
  (el) != NULL; ((el)=(tmp)), ((*(char**)(&(tmp)))=(char*)((tmp!=NULL)?(tmp)->hh.next:NULL)))
#else
#define HASH_ITER(hh,head,el,tmp)                                                \
for(((el)=(head)), ((tmp)=DECLTYPE(el)((head!=NULL)?(head)->hh.next:NULL));      \
  (el) != NULL; ((el)=(tmp)), ((tmp)=DECLTYPE(el)((tmp!=NULL)?(tmp)->hh.next:NULL)))
#endif

/* obtain a count of items in the hash */
#define HASH_COUNT(head) HASH_CNT(hh,head)
#define HASH_CNT(hh,head) ((head != NULL)?((head)->hh.tbl->num_items):0U)

typedef struct UT_hash_bucket {
  struct UT_hash_handle *hh_head;
  unsigned count;

  /* expand_mult is normally set to 0. In this situation, the max chain length
   * threshold is enforced at its default value, HASH_BKT_CAPACITY_THRESH. (If
   * the bucket's chain exceeds this length, bucket expansion is triggered).
   * However, setting expand_mult to a non-zero value delays bucket expansion
   * (that would be triggered by additions to this particular bucket)
   * until its chain length reaches a *multiple* of HASH_BKT_CAPACITY_THRESH.
   * (The multiplier is simply expand_mult+1). The whole idea of this
   * multiplier is to reduce bucket expansions, since they are expensive, in
   * situations where we know that a particular bucket tends to be overused.
   * It is better to let its chain length grow to a longer yet-still-bounded
   * value, than to do an O(n) bucket expansion too often.
   */
  unsigned expand_mult;

} UT_hash_bucket;

/* random signature used only to find hash tables in external analysis */
#define HASH_SIGNATURE 0xa0111fe1u
#define HASH_BLOOM_SIGNATURE 0xb12220f2u

typedef struct UT_hash_table {
  UT_hash_bucket *buckets;
  unsigned num_buckets, log2_num_buckets;
  unsigned num_items;
  struct UT_hash_handle *tail; /* tail hh in app order, for fast append    */
  ptrdiff_t hho; /* hash handle offset (byte pos of hash handle in element */

  /* in an ideal situation (all buckets used equally), no bucket would have
   * more than ceil(#items/#buckets) items. that's the ideal chain length. */
  unsigned ideal_chain_maxlen;

  /* nonideal_items is the number of items in the hash whose chain position
   * exceeds the ideal chain maxlen. these items pay the penalty for an uneven
   * hash distribution; reaching them in a chain traversal takes >ideal steps */
  unsigned nonideal_items;

  /* ineffective expands occur when a bucket doubling was performed, but
   * afterward, more than half the items in the hash had nonideal chain
   * positions. If this happens on two consecutive expansions we inhibit any
   * further expansion, as it's not helping; this happens when the hash
   * function isn't a good fit for the key domain. When expansion is inhibited
   * the hash will still work, albeit no longer in constant time. */
  unsigned ineff_expands, noexpand;

  uint32_t signature; /* used only to find hash tables in external analysis */
#ifdef HASH_BLOOM
  uint32_t bloom_sig; /* used only to test bloom exists in external analysis */
  uint8_t *bloom_bv;
  uint8_t bloom_nbits;
#endif

} UT_hash_table;

typedef struct UT_hash_handle {
  struct UT_hash_table *tbl;
  void *prev;                       /* prev element in app order      */
  void *next;                       /* next element in app order      */
  struct UT_hash_handle *hh_prev;   /* previous hh in bucket order    */
  struct UT_hash_handle *hh_next;   /* next hh in bucket order        */
  void *key;                        /* ptr to enclosing struct's key  */
  unsigned keylen;                  /* enclosing struct's key len     */
  unsigned hashv;                   /* result of hash-fcn(key)        */
} UT_hash_handle;

#endif /* UTHASH_H */

//============================================================
//=============== BASIC DATA STRUCTURE =======================
//============================================================


typedef int ValIndex;

typedef struct {
  const char *key;  // hashtable item key
  int value;    // hashtable item value
  UT_hash_handle hh;  // used for internal book keeping, ignore it
} Hashable;

typedef enum {
  INT_OBJ,
  NULL_OBJ,
  STR_OBJ,
  METHOD_OBJ,
  SLOT_OBJ,
  CLASS_OBJ,
  OBJ_OBJ,
  ARRAY_OBJ
} ObjTag;

typedef struct {
  ObjTag tag;
} RValue;

typedef struct {
  ValTag tag;
} NullValue;

/* reuse the value structure from bytecode.h

typedef struct {
  ValTag tag;
} Value;

typedef struct {
  ValTag tag;
  int value;
} IntValue;

typedef struct {
  ValTag tag;
  char* value;
} StringValue;

typedef struct {
  ValTag tag;
  int name;
  int nargs;
  int nlocals;
  Vector* code;
} MethodValue;

typedef struct {
  ValTag tag;
  int name;
} SlotValue;

typedef struct {
  ValTag tag;
  Vector* slots;
} ClassValue;
*/

typedef struct {
  ObjTag tag;
  Vector* v;
} ArrayValue;

typedef struct {
  ValTag tag;
  // a vector of slot indices
  Vector* slots;
  // a hash table that maps a name to index;
  Hashable* name_to_slot_table;
} IdxClassValue;

typedef struct {
  ValTag tag;
  int name;
  Value* value;
} RSlotValue;

typedef struct {
  ObjTag tag;
  Vector* slot_vec_ptr;
  Value* parent_obj_ptr;
  IdxClassValue* class_ptr;
} ObjectValue;

typedef struct {
  // the values of all local variables
  // defined in the function
  Vector* slot_vec_ptr;
  // address of the call byte
  // code instruction, this is
  // also the return address
  ByteIns* call_ins_idx;
  // function slot pointer
  MethodValue* func_ptr;
} _Frame;

typedef struct {
  // the values of all local variables
  // defined in the function
  Vector* slot_vec_ptr;
  // address of the call byte
  // code instruction, this is
  // also the return address
  int call_ins_idx;
  // function slot pointer
  MethodValue* func_ptr;
  // caller's frame
  _Frame* call_frame_ptr;
} Frame;

// hash table operations
ValIndex find_item (Hashable* table, char* key);
void add_item (Hashable* table, char* key, ValIndex value);

// global data structure operations

// operand stack operations
Value* stack_pop ();
Value* stack_peek ();
void stack_push (Value* val);
Vector* get_operand_stack ();

int get_entry_function ();
Vector* get_global_slots ();
Vector* get_constant_pool ();
Value* get_val_constant(int idx);
void associate_labels(Value *val);
void set_global_slots (Vector* slots);
void set_entry_function (int entry_index);
void addto_constant_pool (Vector* values, Value* v);
IdxClassValue* create_class (Vector* values, ClassValue* v2);

// frame operations
Frame* get_cur_frame ();
Frame* get_root_frame ();
void set_cur_frame (Frame* new_frame);
Value* get_frame_slot (Frame* frame, int idx);
void set_frame_slot (Frame* frame, int idx, Value* val);

// byte code interpreter operations
void exec_prog (Program* p);
void exec_ins (ByteIns* ins);

void exec_array_op ();
void exec_return_op ();
void exec_lit_op (LitIns* i);
void exec_slot_op (SlotIns* i);
void exec_goto_op (GotoIns* i);
void exec_label_op (LabelIns* i);
void exec_branch_op (BranchIns* i);
void exec_object_op (ObjectIns* i);
void exec_printf_op (PrintfIns* i);
void exec_set_slot_op (SetSlotIns* i);
void exec_set_local_op (SetLocalIns* i);
void exec_call_slot_op (CallSlotIns* i);
void exec_get_local_op (GetLocalIns* i);
void exec_set_global_op (SetGlobalIns* i);
void process_label_op (LabelIns * i, int i_ptr);
void call_func (MethodValue * function_slot, int arity);

// slot related operations
RSlotValue* copy_var_slot (SlotValue* slot);
int get_num_var_slots (IdxClassValue* class_val);
Value* find_slot_by_name(ObjectValue* receiver, char* function_name);

Frame* create_frame (int call_ins_idx,
                     MethodValue* func_ptr, Frame* call_frame);

// util operations
char* intToString (int i);
char* toString (Value *val_ptr);
char* copy_string (const char *string);
char* arrayToString (ArrayValue *obj_ptr);
char* str_replace (char *orig, char *rep, char *with);

int obj_type (Value* o);
NullValue* make_null_obj ();
IntValue* make_int_obj (int value);
IntValue* array_length (ArrayValue* array);
Value* array_get (ArrayValue* a, IntValue* i);
ArrayValue* make_array_obj (int length, Value* init);
NullValue* array_set (ArrayValue* a, IntValue* i, Value* v);

Value* eq(IntValue* x, IntValue* y);
Value* lt(IntValue* x, IntValue* y);
Value* le(IntValue* x, IntValue* y);
Value* gt(IntValue* x, IntValue* y);
Value* ge(IntValue* x, IntValue* y);
IntValue* int_obj_add (IntValue* x, IntValue* y);
IntValue* int_obj_sub (IntValue* x, IntValue* y);
IntValue* int_obj_mul (IntValue* x, IntValue* y);
IntValue* int_obj_div (IntValue* x, IntValue* y);
IntValue* int_obj_mod (IntValue* x, IntValue* y);

// assert functions
void assert_obj_obj (Value* ptr);
void assert_not_null (void* ptr);
IntValue* to_int_val (Value* val);
RSlotValue* to_slot_val (Value* val);
MethodValue* to_function_val (Value* val);
void exp_assert(int i, const char * fmt, ...);

// free functions
void free_frame(Frame* frame);

#ifndef PRE_SUBMIT
// statistics data structure
typedef struct {
  struct timeval total_time;               // total time in ms
  long total_method_call;        // # of method calls
  long total_int_method_call;    // # of method calls with integer receiver
  long total_array_method_call;  // # of method calls with array receiver
  long total_envobj_method_call; // # of method calls with env obj receiver
  struct timeval total_time_lookup_entry;  // total time in ms spend looking
  // up an entry in env obj
} Stat;

// collect statistics operations
void init_stat ();
void print_help ();
int is_collect_stat ();
void write_stat (char* filename);
void set_collect_stat (int flag);
void inc_total_time (const struct timeval *total_time);
void inc_entry_lookup_time (const struct timeval* time);
void inc_method_call (Value* receiver_ptr);
void start_time_counter (struct timeval *t);
void end_time_counter (struct timeval *start, struct timeval *end, struct timeval* diff);
#endif // PRE_SUBMIT

//============================================================
//============== HASHTABLE ENCAPSULATION =====================
//============================================================

Hashable* init_hashtable () {
  Hashable* table = NULL;
  Hashable* item_ptr = (Hashable*)malloc(sizeof(Hashable));
  item_ptr->key = "*&(&^&*";
  item_ptr->value = -1;
  HASH_ADD_KEYPTR(hh, table, item_ptr->key,
                  strlen(item_ptr->key), item_ptr);
  return table;
}

void add_item(Hashable* table, char* key, ValIndex value) {
  Hashable* item_ptr = (Hashable*)malloc(sizeof(Hashable));
  item_ptr->key = key;
  item_ptr->value = value;
  HASH_ADD_KEYPTR(hh, table, item_ptr->key, strlen(item_ptr->key), item_ptr);
}

ValIndex find_item(Hashable* table, char* key) {
  Hashable* item;
  HASH_FIND_STR(table, key, item);
  if (item == NULL) {
    return -1;
  }
  return item->value;
}

//============================================================
//========== GLOBAL DATA STRUCTURE OPERATIONS ================
//============================================================

Value* find_slot_by_name(ObjectValue* obj, char* function_name) {
  struct timeval start, end;
  // start collecting time
  if (is_collect_stat())
    start_time_counter(&start);

  Value* ret = NULL;
  int ret_val_idx = find_item(obj->class_ptr->name_to_slot_table,
                              function_name);
  if (ret_val_idx != -1) {
    ret = (Value*)vector_get(obj->slot_vec_ptr, ret_val_idx);
  } else {
    // search in the parent object
    // assumes that in Feeny, object can only inherit from
    // user defined object or null object
    Value* parent = obj->parent_obj_ptr;
    if (parent->tag == OBJ_OBJ) {
      ret = find_slot_by_name((ObjectValue*)parent,
                               function_name);
    } else {
      if (parent->tag != NULL_OBJ) {
        printf("Error[2]: find_slot_by_name.\n");
        exit(-1);
      }
      ret = NULL;
    }
  }

  // end collecting time
  if (is_collect_stat()) {
    struct timeval time;
    end_time_counter(&start, &end, &time);
    inc_entry_lookup_time(&time);
  }
  return ret;
}

// count the number of slots in the class
// that are variable slots
int get_num_var_slots (IdxClassValue * class_val) {
  int var_slot_count = 0;
  for (int i = 0; i < class_val->slots->size; i++) {
    int slot_idx = (int)vector_get(class_val->slots, i);
    Value* slot = get_val_constant(slot_idx);
    assert_not_null(slot);
    if (slot->tag == SLOT_VAL) {
      var_slot_count++;
    }
  }
  return var_slot_count;
}

RSlotValue* copy_var_slot (SlotValue * slot) {
  RSlotValue* rSlot =
    (RSlotValue*)malloc(sizeof(RSlotValue));
  rSlot->tag = slot->tag;
  rSlot->name = slot->name;
  rSlot->value = NULL;
  return rSlot;
}

Vector* get_constant_pool () {
  static Vector* constant_pool = NULL;
  if (constant_pool == NULL) {
    constant_pool = make_vector();
  }
  return constant_pool;
}

// get value from the constant pool
Value* get_val_constant(int idx) {
  Vector* constant_pool = get_constant_pool();
  return vector_get(constant_pool, idx);
}

// returns the string content
// indexed by idx in the constant pool
char* get_str_constant(int idx) {
  Value* val = get_val_constant(idx);
  if (val == NULL || val->tag != STR_OBJ) {
    printf("Error: get string constant.\n");
    exit(-1);
  }
  return ((StringValue*)val)->value;
}

// instructor pointer
static int inst_ptr = -1;
// hashtable that associate a string label
// with an instruction address
static Hashable* inst_label_table = NULL;
Hashable* get_inst_label_table () {
  if (inst_label_table == NULL)
    inst_label_table = init_hashtable();
  return inst_label_table;
}
void set_label_addr (char* label, int addr) {
  add_item(get_inst_label_table(), label, addr);
}
int get_label_addr (char* label) {
  return find_item(get_inst_label_table(), label);
}

// hashtable that maps string names to global
// slots' index in the constant pool
// each index is guaranteed to refer to either
// a Slot object or a Method object
static Hashable* global_slot_table = NULL;
static Vector* global_slots = NULL;
void set_global_slots (Vector * slots) {
  int val_idx, name_idx;
  Value* value = NULL;
  char* name_str = NULL;
  global_slots = slots;
  // establish string name to
  // slot index hashtable
  global_slot_table = init_hashtable();
  for (int i = 0; i < slots->size; i++) {
    val_idx = (int)vector_get(slots, i);
    value = get_val_constant(val_idx);
    name_idx = ((SlotValue*)value)->name;
    name_str = get_str_constant(name_idx);
    add_item(global_slot_table, name_str, val_idx);
  }
}
Vector* get_global_slots () {
  return global_slots;
}

Value* get_global_slot_by_name (char* name) {
  int slot_idx = find_item(global_slot_table, name);
  if (slot_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
  }
  Value* val = get_val_constant(slot_idx);
  if (val == NULL) {
    printf("Error[2]: get global slot by name.\n");
    exit(-1);
  }
  return val;
}

Value* get_global_slot_by_idx (int slot_idx) {
  Value* val = get_val_constant(slot_idx);
  if (val == NULL) {
    printf("Error[2]: get global slot by name.\n");
    exit(-1);
  }
  return val;
}

static int entry_function_index = -1;
int get_entry_function () {
  return entry_function_index;
}
void set_entry_function (int entry_index) {
  entry_function_index = entry_index;
}

Hashable* get_global_var_map () {
  static Hashable* global_var_map = NULL;
  if (global_var_map == NULL) {
    global_var_map = init_hashtable();
  }
  return global_var_map;
}

// get instruction label table
Hashable* get_ins_label_table () {
  static Hashable* ins_label_table = NULL;
  if (ins_label_table == NULL) {
    ins_label_table = init_hashtable();
  }
  return ins_label_table;
}

static Vector* operand_stack = NULL;
Vector* get_operand_stack () {
  if (operand_stack == NULL)
    operand_stack = make_vector();
  return operand_stack;
}

void stack_push (Value * val) {
  vector_add(operand_stack, val);
}
Value* stack_pop () {
  if (operand_stack->size == 0) {
    return NULL;
  }
  return vector_pop(operand_stack);
}
Value* stack_peek () {
  return vector_peek(operand_stack);
}

static Frame* current_frame_ptr = NULL;
Frame* get_root_frame () {
  static Frame* root_frame_ptr = NULL;
  if (root_frame_ptr == NULL) {
    root_frame_ptr = create_frame(-1, NULL, NULL);
    current_frame_ptr = root_frame_ptr;
  }
  return root_frame_ptr;
}
Frame* get_cur_frame () {
  return current_frame_ptr;
}

void set_cur_frame (Frame * new_frame) {
  /*if (current_frame_ptr != (Frame*)(new_frame->call_frame_ptr)) {
    printf("Error: set_cur_frame.\n");
    exit(-1);
  }*/
  current_frame_ptr = new_frame;
}

void set_frame_slot (Frame * frame, int idx, Value * val) {
  if (idx == frame->slot_vec_ptr->size) {
    vector_add(frame->slot_vec_ptr, val);
  } else if (0 <= idx && idx < frame->slot_vec_ptr->size) {
    vector_set(frame->slot_vec_ptr, idx, val);
  } else {
    printf("Error: set frame value.\n");
    exit(-1);
  }
}

Value* get_frame_slot (Frame * frame, int idx) {
  if (idx >= frame->slot_vec_ptr->size) {
    printf("Error: get frame value.\n");
    exit(-1);
  }
  return vector_get(frame->slot_vec_ptr, idx);
}

Frame* create_frame (int call_ins_idx,
                     MethodValue * func_ptr, Frame * call_frame) {
  Frame* ret = malloc(sizeof(Frame));
  ret->slot_vec_ptr = make_vector();
  ret->call_ins_idx = call_ins_idx;
  ret->func_ptr = func_ptr;
  ret->call_frame_ptr = (_Frame*)call_frame;
  return ret;
}

//============================================================
//=============== BYTE CODE INTERPRETER ======================
//============================================================

// Sets the i’th slot in the current
// local frame to the top value in
// the operand stack.
void exec_set_local_op (SetLocalIns * i) {
  // slot_idx is the index of the slot
  // in the current local frame
  int slot_idx = i->idx;
  Value* val = stack_peek();
  if (val == NULL) {
    printf("Error: set local op.\n");
    exit(-1);
  }
  Frame* frame = get_cur_frame();
  set_frame_slot(frame, slot_idx, val);
  // NOTE: do not pop from the stack
  inst_ptr++;
}

// Retrieves the i’th slot in the
// current local frame and pushes
// it onto the operand stack.
void exec_get_local_op (GetLocalIns * i) {
  // slot_idx is the index of the slot
  // in the current local frame
  int slot_idx = i->idx;
  Frame* frame = get_cur_frame();
  if (frame == NULL) {
    printf("Error: get local op.\n");
    exit(-1);
  }
  Value* val = get_frame_slot(frame, slot_idx);
  stack_push(val);
  inst_ptr++;
}

// In-place makes a quick version of the global ins
void make_set_global_quick(SetGlobalIns* i){
  int name_idx = i->name;
  char* name_str = get_str_constant(name_idx);

  name_idx = find_item(global_slot_table, name_str);
  if (name_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
  }
  i->tag = (OpCode)SET_GLOBAL_OP_QUICK;
  i->name = name_idx;
}

// Sets the global variable with name
// specified by the String object at index
// i to the top value in the operand stack.
void exec_set_global_op (SetGlobalIns * i) {
	make_set_global_quick(i);
}

// Sets the global variable with name
// specified by the String object at index
// i to the top value in the operand stack.
void exec_set_global_quick_op (SetGlobalQuickIns * i) {
  int name_idx = i->idx;
  Value* val = stack_peek();
  if (val == NULL) {
    printf("Error: set global op.\n");
    exit(-1);
  }
  Value* slot = get_global_slot_by_idx(name_idx);
  if (slot == NULL || slot->tag != SLOT_OBJ) {
    printf("Error[2]: set global op.\n");
    exit(-1);
  }
  RSlotValue* rSlot = (RSlotValue*)slot;
  rSlot->value = val;
  // NOTE: do not pop from the stack
  inst_ptr++;
}

// In-place makes a quick version of the global ins
void make_get_global_quick(GetGlobalIns* i){
  int name_idx = i->name;
  char* name_str = get_str_constant(name_idx);

  name_idx = find_item(global_slot_table, name_str);
  if (name_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
  }
  i->tag = (OpCode)GET_GLOBAL_OP_QUICK;
  i->name = name_idx;
}

// Retrieves the value of the
// global variable with name specified
// by the String object at index i, and
// pushes it onto the operand stack.
void exec_get_global_op (GetGlobalIns * i) {
  make_get_global_quick(i);
}

// Retrieves the value of the
// global variable with name specified
// by the String object at index i, and
// pushes it onto the operand stack.
void exec_get_global_quick_op (GetGlobalQuickIns * i) {
  int idx = i->idx;
  Value* slot = get_global_slot_by_idx(idx);
  if (slot == NULL || slot->tag != SLOT_OBJ) {
    printf("Error: get global op.\n");
    exit(-1);
  }
  RSlotValue* rSlot = (RSlotValue*)slot;
  Value* val = rSlot->value;
  if (val == NULL) {
    printf("Error[2]: get global op.\n");
    exit(-1);
  }
  stack_push(val);
  inst_ptr++;
}

void make_branch_quick(BranchIns *i){
  int label_idx = i->name;
  char* label_str = get_str_constant(label_idx);
  int addr = get_label_addr(label_str);

  i->name = addr;
  i->tag = (OpCode)BRANCH_OP_QUICK;
}

// Pops a value from the operand
// stack. If this value is not Null,
// then sets the instruction pointer to
// the instruction address associated
// with the name given by the String
// object at index i.
void exec_branch_op (BranchIns * i) {
	make_branch_quick(i);
}


void exec_branch_quick_op (BranchQuickIns * i) {
  Value* val = stack_pop();
  if (val == NULL) {
    printf("Error: branch op.\n");
    exit(-1);
  }
  if (val->tag != NULL_OBJ) {
    inst_ptr = i->addr;
  } else {
    inst_ptr++;
  }
}

void make_goto_quick(GotoIns *i){
  int label_idx = i->name;
  char* label_str = get_str_constant(label_idx);
  int addr = get_label_addr(label_str);

  i->name = addr;
  i->tag = (OpCode)GOTO_OP_QUICK;
}

// Sets the instruction pointer to the instruction
// address associated with the name given by
// the String object at index i.
void exec_goto_op (GotoIns * i) {
  make_goto_quick(i);
}

void exec_goto_quick_op (GotoQuickIns *i){
	inst_ptr = i->addr;
}

// Pops and discards the top value from
// the operand stack.
void exec_drop_op () {
  stack_pop();
  inst_ptr++;
}

// Retrieves the object at index i in
// the constant pool, which refers to
// either an Int object or a Null object,
// and pushes it onto the operand stack.
void exec_lit_op (LitIns * i) {
  int val_idx = i->idx;
  Value* val = get_val_constant(val_idx);
  if (val == NULL || (val->tag != INT_OBJ && val->tag != NULL_OBJ)) {
    printf("Error in LIT_OP.\n");
    exit(-1);
  }
  stack_push(val);
  inst_ptr++;
}

// Associates a name with the address of
// this instruction. The name is given by
// the String object at index i.
void process_label_op (LabelIns * i, int i_ptr) {
  int label_idx = i->name;
  char* label_str = get_str_constant(label_idx);
  set_label_addr(label_str, i_ptr);
}

void exec_label_op (LabelIns * i) {
  inst_ptr++;
}

// First pops the initializing value from
// the operand stack, and then pops the
// length of the array from the operand
// stack. Creates a new array with the
// given length, with each element initizlied
// to the given value, and pushes the array
// onto the operand stack.
void exec_array_op () {
  Value* init_value = stack_pop();
  assert_not_null(init_value);
  Value* length = stack_pop();
  IntValue* len = to_int_val(length);
  Value* array = (Value*)make_array_obj(len->value, init_value);
  stack_push(array);
  inst_ptr++;
}

// Pops n values from the operand
// stack, and then prints them out according to the given format string.
// Arguments are spliced in from the deepest value in the stack (last
// popped) to the shallowest value in the stack (first popped). Null is
// then pushed onto the operand stack.
void exec_printf_op (PrintfIns * i) {
  int format_idx = i->format;
  int arity = i->arity;
  char* format_str = get_str_constant(format_idx);
  char replace_char[] = "~";
  // replace all ~ in format string into the corresponding arguments
  char *new_str = copy_string(format_str);

  Vector* args = make_vector();
  int n = arity;
  while (n-- > 0) {
    vector_add(args, stack_pop());
  }
  if (args->size != arity) {
    printf("Error: exec_printf_op.\n");
    exit(-1);
  }
  n = arity;
  while (n-- > 0) {
    char *old_str = new_str;
    // pop one elemet from the stack
    Value* val = vector_pop(args);
    char *arg_str = toString(val);
    new_str = str_replace(old_str, replace_char, arg_str);
    if (arg_str != NULL) free(arg_str);
    if (old_str != NULL) free(old_str);
  }
  if (new_str != NULL) {
    printf("%s", new_str);
    free(new_str);
  }
  vector_free(args);
  inst_ptr++;
}

// Retrieves the Class object at index c.
// Suppose that the class object contains n Slot objects, and m Method
// objects. This instruction will pop n values from the operand stack for
// use as the initial values of the variable slots in the object, and then an
// additional value for use as the parent of the object. The first variable
// slot is initialized to the deepest value on the stack (last popped) and the
// last variable slot is initialized to the shallowest value on the stack (first
// popped). A new object is created with the variable slots indicated by
// the Class object, initialized to the given values on the stack, with the
// method slots indicated by the Class object, and with the given parent
// object, and is pushed onto the operand stack.
void exec_object_op (ObjectIns * i) {
  int class_idx = i->class;
  Value* class = get_val_constant(class_idx);
  if (class == NULL || class->tag != CLASS_VAL) {
    printf("Error: exec_object_op.\n");
    exit(-1);
  }
  IdxClassValue* class_val = (IdxClassValue*)class;
  int num_slots = get_num_var_slots(class_val);
  // init new object value
  ObjectValue* obj = (ObjectValue*)malloc(sizeof(ObjectValue));
  obj->tag = OBJ_OBJ;
  obj->slot_vec_ptr = make_vector();
  vector_set_length(obj->slot_vec_ptr, class_val->slots->size, NULL);
  // iterate from last slot to the first slot
  for (int i = class_val->slots->size - 1; i >= 0 ; i--) {
    int slot_idx = (int)vector_get(class_val->slots, i);
    Value* slot_in_class = get_val_constant(slot_idx);
    assert_not_null(slot_in_class);
    if (slot_in_class->tag == METHOD_VAL) {
      // copy method slot
      vector_set(obj->slot_vec_ptr, i, slot_in_class);
    } else if (slot_in_class->tag == SLOT_VAL) {
      // copy var slot
      RSlotValue* new_slot =
        copy_var_slot((SlotValue*)slot_in_class);
      // first var slot will be initialized
      // to the deepest value on the stack
      new_slot->value = stack_pop();
      if (new_slot->value == NULL) {
        printf("Error[2]: exec_object_op.\n");
        exit(-1);
      }
      vector_set(obj->slot_vec_ptr, i, new_slot);
    } else {
      printf("Error[3]: exec_object_op.\n");
      exit(-1);
    }
  }
  obj->parent_obj_ptr = stack_pop();
  obj->class_ptr = class_val;
  stack_push((Value*)obj);
  inst_ptr++;
}

// Pops a value from the operand
// stack assuming it is an object. Retrieves the value in the object stored
// at the variable slot with name given by the String object at index i,
// and pushes it onto the operand stack.
void exec_slot_op (SlotIns* i) {
  int name_idx = i->name;
  char* slot_name = get_str_constant(name_idx);
  assert_not_null(slot_name);
  // pop the object to get from
  Value* obj = stack_pop();
  assert_obj_obj(obj);
  Value* slot = find_slot_by_name((ObjectValue*)obj, slot_name);
  RSlotValue* slotVal = to_slot_val(slot);
  stack_push(slotVal->value);
  inst_ptr++;
}

// Pops the value to store, x, from
// the operand stack, and then pops the object to store it into. Stores
// x into the object at the variable slot with name given by the String
// object at index i. x is then pushed onto the operand stack.
void exec_set_slot_op (SetSlotIns * i) {
  int name_idx = i->name;
  char* slot_name = get_str_constant(name_idx);
  assert_not_null(slot_name);
  // pop the value to be stored
  Value* val = stack_pop();
  assert_not_null(val);
  // pop the object to store into
  Value* obj = stack_pop();
  assert_obj_obj(obj);
  Value* slot = find_slot_by_name((ObjectValue*)obj, slot_name);
  RSlotValue* slotVal = to_slot_val(slot);
  slotVal->value = val;
  stack_push(val);
  inst_ptr++;
}

void call_func (MethodValue * function_slot, int arity) {
  // pops arguments from the stack
  Vector* tmp_vec = make_vector();
  // create a new local frame
  Frame* local_frame = create_frame(inst_ptr,
                                    function_slot, get_cur_frame());
  // the first arity slots are the arguments
  int n = arity;
  while (n-- > 0) {
    vector_add(tmp_vec, stack_pop());
  }
  n = arity;
  while (n-- > 0) {
    vector_add(local_frame->slot_vec_ptr, vector_pop(tmp_vec));
  }
  vector_free(tmp_vec);
  set_cur_frame(local_frame);
  // set the instructor pointer to the first
  // instruction in the function body
  inst_ptr = 0;
}

void make_call_quick(CallIns * i){
  int function_name_idx = i->name;
  int arity = i->arity;

  char* function_name = get_str_constant(function_name_idx);
  int name_idx = find_item(global_slot_table, function_name);
  if (name_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
  }

  i->tag = (OpCode)CALL_OP_QUICK;
  i->name = name_idx;
}

// Pops n values from the operand stack
// for the arguments to the call. The name of the function to call is given
// by the String object at index i. A new local frame is created for the context
// of the call. The first n slots in the frame holds the argument values
// starting with the deepest value on the stack (last popped) and ending
// with the shallowest value on the stack (first popped). The new local
// frame has the current frame as its parent, and the current instruction
// pointer as the return address. Execution proceeds by registering the
// newly created frame as the current frame, and setting the instruction
// pointer to the address of the body of the function.
void exec_call_op (CallIns * i) {
  make_call_quick(i);
}

void exec_call_quick_op(CallQuickIns * i){
  Value* function_slot = get_global_slot_by_idx(i->idx);
  if (function_slot == NULL || function_slot->tag != METHOD_VAL) {
    printf("Error: exec_call_op.\n");
    exit(-1);
  }
  call_func((MethodValue*)function_slot, i->arity);
  // instruction pointer is updated in call_func
}

// Registers the parent frame of the
// current local frame as the new current
// frame. Execution proceeds by
// setting the instruction pointer to the
// return address stored in the current
// local frame. The local frame is no longer
// used after a Return instruction, and any
// storage allocated for it may be reclaimed.
void exec_return_op () {
  Frame* frame = get_cur_frame();
  inst_ptr = frame->call_ins_idx + 1;
  Frame* call_frame = (Frame*)frame->call_frame_ptr;
  set_cur_frame(call_frame);
  free_frame(frame);
}

// Pops n values from the
// operand stack for the arguments to the call. Then pops the receiver
// object from the operand stack. The name of the method to call is given
// by the String object at index i. If the receiver is an integer or array,
// then the result of the method call (as specified by the semantics of
// Feeny) is pushed onto the operand stack. If the receiver is an object,
// then a new local frame is created for the context of the call. Slot 0
// in the new local frame holds the receiver object, and the following n
// slots holds the argument values starting with the deepest value on the
// stack (last popped) and ending with the shallowest value on the stack
// (first popped). The new local frame has the current frame as its parent,
// and the current instruction pointer as the return address. Execution
// proceeds by registering the newly created frame as the current frame,
// and setting the instruction pointer to the address of the body of the
// method.
void exec_call_slot_op (CallSlotIns * i) {
  int method_name_idx = i->name;
  char* method_name = get_str_constant(method_name_idx);
  int arity = i->arity > 0 ? i->arity - 1 : 0;
  // pops arguments from the stack
  Vector* tmp_arg_vec = make_vector();
  // the first arity slots are the arguments
  int n = arity;
  while (n-- > 0) {
    vector_add(tmp_arg_vec, stack_pop());
  }
  // pop the receiver object
  Value* receiver_ptr = stack_pop();
  assert_not_null(receiver_ptr);
  exp_assert(obj_type(receiver_ptr) != NULL_OBJ,
             "Null object does not have methods!");
  Value* retVal = NULL;
  inc_method_call(receiver_ptr);

  switch ((ObjTag)obj_type(receiver_ptr)) {
  // handle built in functions
  case INT_OBJ: {
    Value* arg;
    exp_assert(arity == 1 && obj_type(arg = vector_get(tmp_arg_vec, 0)) == INT_VAL,
               "native int function error - %s",
               arity != 1 ? "not enough arguments!" :
               "wrong argument type!");
    if (!strcmp(method_name, "add")) {
      retVal = (Value*) int_obj_add((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "sub")) {
      retVal = (Value*) int_obj_sub((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "mul")) {
      retVal = (Value*) int_obj_mul((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "div")) {
      retVal = (Value*) int_obj_div((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "mod")) {
      retVal = (Value*) int_obj_mod((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "gt")) {
      retVal = (Value*) gt((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "ge")) {
      retVal = (Value*) ge((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "lt")) {
      retVal = (Value*) lt((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "le")) {
      retVal = (Value*) le((IntValue*) receiver_ptr, (IntValue*)arg);
    } else if (!strcmp(method_name, "eq")) {
      retVal = (Value*) eq((IntValue*) receiver_ptr, (IntValue*)arg);
    } else {
      exp_assert(0, "unknown native int function");
    }
    stack_push(retVal);
    inst_ptr++;
    break;
  }
  case ARRAY_OBJ: {
    Value* result = NULL;
    if (!strcmp(method_name, "length") && arity == 0) {
      result = (Value*)array_length((ArrayValue*) receiver_ptr);
    } else {
      Vector* args = make_vector();
      n = arity;
      while (n-- > 0) {
        vector_add(args, vector_pop(tmp_arg_vec));
      }
      Value* first_arg;
      exp_assert(arity > 0 && obj_type(first_arg = vector_get(args, 0)) == INT_OBJ,
                 "native array function error - %s",
                 arity < 1 ? "not enough arguments!" :
                 "incorrect argument type!");
      if (!strcmp(method_name, "set") && arity == 2) {
        result = (Value*)array_set((ArrayValue*)receiver_ptr,
                                   (IntValue*)first_arg, vector_get(args, 1));
      } else if (!strcmp(method_name, "get") && arity == 1) {
        result = (Value*)array_get((ArrayValue*)receiver_ptr, (IntValue*)first_arg);
      } else {
        exp_assert(0, "unknown native array function");
      }
      vector_free(args);
    }
    stack_push(result);
    inst_ptr++;
    break;
  }
  case OBJ_OBJ: {
    Value* slot = find_slot_by_name((ObjectValue*)receiver_ptr, method_name);
    assert_not_null(slot);
    MethodValue* method_slot = to_function_val(slot);
    Vector* args = make_vector();
    // create a new local frame
    Frame* local_frame = create_frame(inst_ptr,
                                      method_slot, get_cur_frame());
    // the slot 0 is the receiver object
    vector_add(local_frame->slot_vec_ptr, receiver_ptr);
    n = arity;
    while (n-- > 0) {
      vector_add(local_frame->slot_vec_ptr, vector_pop(tmp_arg_vec));
    }
    set_cur_frame(local_frame);
    inst_ptr = 0;
    vector_free(args);
    break;
  }
  default:
    printf("receiver_ptr->tag: %d\n", receiver_ptr->tag);
    exp_assert(0, "Cannot call method on null object!");
    exit(-1);
  }
  vector_free(tmp_arg_vec);
}

void exec_ins (ByteIns * ins) {
  switch (ins->tag) {
  case LABEL_OP: {
    LabelIns* i = (LabelIns*)ins;
    exec_label_op(i);
    break;
  }
  case LIT_OP: {
    LitIns* i = (LitIns*)ins;
    exec_lit_op(i);
    break;
  }
  case PRINTF_OP: {
    PrintfIns* i = (PrintfIns*)ins;
    exec_printf_op(i);
    break;
  }
  case ARRAY_OP: {
    exec_array_op();
    break;
  }
  case OBJECT_OP: {
    ObjectIns* i = (ObjectIns*)ins;
    exec_object_op(i);
    break;
  }
  case SLOT_OP: {
    SlotIns* i = (SlotIns*)ins;
    exec_slot_op(i);
    break;
  }
  case SET_SLOT_OP: {
    SetSlotIns* i = (SetSlotIns*)ins;
    exec_set_slot_op(i);
    break;
  }
  case CALL_SLOT_OP: {
    CallSlotIns* i = (CallSlotIns*)ins;
    exec_call_slot_op(i);
    break;
  }
  case CALL_OP: {
    CallIns* i = (CallIns*)ins;
    exec_call_op(i);
    //break;
  }
  case CALL_OP_QUICK: {
    CallQuickIns* i = (CallQuickIns*)ins;
    exec_call_quick_op(i);
    break;
  }
  case SET_LOCAL_OP: {
    SetLocalIns* i = (SetLocalIns*)ins;
    exec_set_local_op(i);
    break;
  }
  case GET_LOCAL_OP: {
    GetLocalIns* i = (GetLocalIns*)ins;
    exec_get_local_op(i);
    break;
  }
  case SET_GLOBAL_OP: {
    SetGlobalIns* i = (SetGlobalIns*)ins;
    exec_set_global_op(i);
    //break; // exec_set_global_op transforms the type
  }
  case SET_GLOBAL_OP_QUICK: {
    SetGlobalQuickIns* i = (SetGlobalQuickIns*)ins;
    exec_set_global_quick_op(i);
    break;
  }
  case GET_GLOBAL_OP: {
    GetGlobalIns* i = (GetGlobalIns*)ins;
    exec_get_global_op(i);
    //break; // exec_get_global_op transforms the type
  }
  case GET_GLOBAL_OP_QUICK: {
    GetGlobalQuickIns* i = (GetGlobalQuickIns*)ins;
    exec_get_global_quick_op(i);
    break;
  }
  case BRANCH_OP: {
    BranchIns* i = (BranchIns*)ins;
    exec_branch_op(i);
    //break; // exec_branch_op transforms the type
  }
  case BRANCH_OP_QUICK: {
    BranchQuickIns* i = (BranchQuickIns*)ins;
    exec_branch_quick_op(i);
    break;
  }
  case GOTO_OP: {
    GotoIns* i = (GotoIns*)ins;
    exec_goto_op(i);
    //break; // exec_goto_op transforms the type
  }
  case GOTO_OP_QUICK: {
    GotoQuickIns* i = (GotoQuickIns*) ins;
	 exec_goto_quick_op(i);
	 break;
  }
  case RETURN_OP: {
    exec_return_op();
    break;
  }
  case DROP_OP: {
    exec_drop_op();
    break;
  }
  default: {
    printf("Unknown instruction with tag: %u\n", ins->tag);
    exit(-1);
  }
  }
}

IdxClassValue* create_class(Vector * values, ClassValue * v2) {
  IdxClassValue* new_v =
    (IdxClassValue*)malloc(sizeof(IdxClassValue));
  new_v->tag = v2->tag;
  new_v->slots = v2->slots;
  new_v->name_to_slot_table = init_hashtable();
  // iterate over all slots
  for (int i = 0; i < v2->slots->size; i++) {
    // get the slot value
    int slot_index = (int)vector_get(v2->slots, i);
    Value* value = vector_get(values, slot_index);
    if (value->tag != SLOT_OBJ && value->tag != METHOD_OBJ) {
      printf("non-slot or method value in class\n");
      exit(-1);
    }
    int name_index = ((SlotValue*)value)->name;
    // get the string value of name
    value = vector_get(values, name_index);
    if (value->tag != STR_OBJ) {
      printf("Slot value name is not a string.\n");
      exit(-1);
    }
    char* name_ptr = ((StringValue*)value)->value;
    // map the string name to the slot index in new_v->slots
    add_item(new_v->name_to_slot_table, name_ptr, i);
  }
  return new_v;
}

void associate_labels(Value *val) {
  MethodValue* method = to_function_val(val);
  for (int i = 0; i < method->code->size; i++) {
    ByteIns* ins = (ByteIns*)vector_get(method->code, i);
    if (ins->tag == LABEL_OP) {
      process_label_op((LabelIns*)ins, i);
    }
  }
}

// add values in the byte code syntax tree into
// the runtime constatnt pool
void addto_constant_pool (Vector * values, Value * v) {
  Vector* constant_pool = get_constant_pool();
  switch (v->tag) {
  case INT_VAL:
  case NULL_VAL:
  case STRING_VAL:
  case METHOD_VAL: {
    vector_add(constant_pool, v);
    // preprocess to construct label table
    if (v->tag == METHOD_VAL)
      associate_labels(v);
    break;
  }
  case SLOT_VAL: {
    SlotValue* v2 = (SlotValue*)v;
    RSlotValue* new_v = (RSlotValue*)malloc(sizeof(RSlotValue));
    new_v->tag = v2->tag;
    new_v->name = v2->name;
    new_v->value = NULL;
    vector_add(constant_pool, new_v);
    break;
  }
  case CLASS_VAL: {
    ClassValue* v2 = (ClassValue*)v;
    IdxClassValue* new_v = create_class(values, v2);
    vector_add(constant_pool, new_v);
    break;
  }
  default:
    printf("Value with unknown tag: %d\n", v->tag);
    exit(-1);
  }
}

void start_exec() {
  // init the root frame
  get_root_frame();
  // init the global var table
  get_global_var_map ();
  // init the instruction label table
  get_ins_label_table ();
  // init the operand stack
  get_operand_stack ();
  // call the entry function
  Value* slot = get_val_constant(get_entry_function());
  call_func(to_function_val(slot), 0);
  // start executing the program
  while (1) {
    // get the current instruction
    Frame* cur_frame = get_cur_frame();
    if (cur_frame == get_root_frame()) {
      break; // execution stops
    }
    Vector* code = cur_frame->func_ptr->code;
    if (inst_ptr < 0 || inst_ptr >= code->size) {
      printf("Error: wrong inst_ptr: %d.\n", inst_ptr);
      exit(-1);
    }
#ifdef DEBUG
    printf("%d, %d\n", code->size, inst_ptr);
    printf("stack size: %d\n", operand_stack->size);
#endif
    ByteIns* ins = vector_get(code, inst_ptr);
#ifdef DEBUG
    print_ins(ins);
    printf("\n");
#endif
    exec_ins(ins);
  }
}

void exec_prog (Program * p) {
  // constant pool
  Vector* vs = p->values;
  for (int i = 0; i < vs->size; i++) {
    addto_constant_pool(vs, vector_get(vs, i));
  }
  // global slot
  set_global_slots(p->slots);
  // entry function
  set_entry_function(p->entry);
  // start exec the program
  start_exec();
}

void interpret_bc (Program * p) {
  /*printf("Interpreting Bytecode Program:\n");
  print_prog(p);
  printf("\n");*/
  exec_prog(p);
}


//============================================================
//==================== COLLECT STAT ==========================
//============================================================

void print_help () {
  printf("Command line:\n\tcfeeny filename [-s path]\n\n\t-s print statistics to the specified file\n");
}


static int collectStat = 0;
static Stat* stat = NULL;

void set_collect_stat (int flag) {
  collectStat = flag;
  if (flag) {
    init_stat();
  }
}

int is_collect_stat () {
  return collectStat != 0;
}

void init_stat () {
  stat = malloc(sizeof(Stat));
  stat->total_time.tv_usec = 0;
  stat->total_time.tv_sec = 0;
  stat->total_method_call = 0;
  stat->total_int_method_call = 0;
  stat->total_array_method_call = 0;
  stat->total_envobj_method_call = 0;
  stat->total_time_lookup_entry.tv_sec = 0;
  stat->total_time_lookup_entry.tv_usec = 0;
}

void start_time_counter (struct timeval *t) {
  gettimeofday(t, NULL);
}

void end_time_counter (struct timeval *start, struct timeval *end, struct timeval* result) {
  gettimeofday(end, NULL);
  timersub(end, start, result);
}

void inc_total_time (const struct timeval *total_time) {
  if (collectStat == 0 || stat == NULL)
    return;
  stat->total_time.tv_sec += total_time->tv_sec;
  stat->total_time.tv_usec += total_time->tv_usec;
}

void inc_entry_lookup_time (const struct timeval* time) {
  if (collectStat == 0 || stat == NULL)
    return;
  stat->total_time_lookup_entry.tv_sec += time->tv_sec;
  stat->total_time_lookup_entry.tv_usec += time->tv_usec;
}

void inc_method_call (Value* receiver_ptr) {
  if (receiver_ptr == NULL
          || collectStat == 0
          || stat == NULL)
    return;

  stat->total_method_call++;
  switch ((ObjTag)receiver_ptr->tag) {
  case INT_OBJ:
    stat->total_int_method_call++;
    break;
  case ARRAY_OBJ:
    stat->total_array_method_call++;
    break;
  case OBJ_OBJ:
    stat->total_envobj_method_call++;
    break;
  default:
    break;
  }
}

void write_stat (char* filename) {
  if (!is_collect_stat() || stat == NULL) return;
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  fprintf(f, "Total Time: %f ms\n", 0.0+stat->total_time.tv_sec*1000.0+stat->total_time.tv_usec/1000.0);
  fprintf(f, "Total Time Lookup Entry: %f ms\n", 0.0+stat->total_time_lookup_entry.tv_sec*1000.0+stat->total_time_lookup_entry.tv_usec/1000.0);
  fprintf(f, "Total Method Call: %ld\n", stat->total_method_call);
  fprintf(f, "Total Integer Method Call: %ld\n", stat->total_int_method_call);
  fprintf(f, "Total Array Method Call: %ld\n", stat->total_array_method_call);
  fprintf(f, "Total EnvObj Method Call: %ld\n", stat->total_envobj_method_call);

  fclose(f);
}

//============================================================
//================== UTIL FUNCITONS ==========================
//============================================================

void free_frame(Frame* frame) {
  vector_free(frame->slot_vec_ptr);
  free(frame);
}

MethodValue* to_function_val (Value* val) {
  if (val == NULL || val->tag != METHOD_VAL) {
    printf("Error: not a function value.\n");
    exit(-1);
  }
  return (MethodValue*) val;
}

RSlotValue* to_slot_val (Value* val) {
  if (val == NULL || val->tag != SLOT_VAL) {
    printf("Error: not a var slot.\n");
    exit(-1);
  }
  return (RSlotValue*) val;
}

IntValue* to_int_val (Value* val) {
  if (val == NULL || val->tag != INT_VAL) {
    printf("Error: to_int_val.\n");
    exit(-1);
  }
  return (IntValue*)val;
}

void assert_not_null (void* ptr) {
  if (ptr == NULL) {
    printf("Error: assert_not_null.\n");
    exit(-1);
  }
}

void assert_obj_obj (Value* ptr) {
  if (ptr == NULL || ptr->tag != OBJ_OBJ) {
    printf("Error: assert_obj_obj.\n");
    exit(-1);
  }
}

void exp_assert (int i, const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  if (!i) {
    printf("Encountered error: ");
    vprintf(fmt, args);
    printf("\n");
    exit(-1);
  }
  va_end(args);
}

int obj_type (Value * o) {
  return o->tag;
}

NullValue* make_null_obj () {
  static NullValue n = {NULL_VAL};
  return &n;
}

IntValue* make_int_obj (int value) {
  //cache for memory usage - could make the cache larger
  static int cache_initted = 0;
  static IntValue cached[101];
  if (! cache_initted) {
    for (int i = 0; i < sizeof(cached) / sizeof(IntValue); ++i) {
      cached[i].tag = INT_VAL;
      cached[i].value = i;
    }
    cache_initted = 1;
  }
  if (value >= 0 && value < sizeof(cached) / sizeof(IntValue)) {
    return &cached[value];
  }

  IntValue* t = malloc(sizeof(IntValue));
  t->tag = INT_VAL;
  t->value = value;
  return t;
}

IntValue* array_length (ArrayValue* array) {
  return make_int_obj(array->v->size);
}

NullValue* array_set (ArrayValue* a, IntValue* i, Value* v) {
  if (i->value >= a->v->size || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->v->size, i->value);
    exit(-1);
  }

  vector_set(a->v, i->value, v);
  return make_null_obj();
}

Value* array_get (ArrayValue* a, IntValue* i) {
  if (i->value >= a->v->size || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->v->size, i->value);
    exit(-1);
  }
  return vector_get(a->v, i->value);
}

ArrayValue* make_array_obj(int length, Value* init) {
  ArrayValue* t = malloc(sizeof(ArrayValue));
  t->tag = ARRAY_OBJ;
  t->v = make_vector();
  for (int i = 0; i < length; ++i) {
    vector_add(t->v, init);
  }
  return t;
}

IntValue* int_obj_add (IntValue * x, IntValue * y) {
  return make_int_obj(x->value + y->value);
}
IntValue* int_obj_sub (IntValue * x, IntValue * y) {
  return make_int_obj(x->value - y->value);
}
IntValue* int_obj_mul (IntValue * x, IntValue * y) {
  return make_int_obj(x->value * y->value);
}
IntValue* int_obj_div (IntValue * x, IntValue * y) {
  return make_int_obj(x->value / y->value);
}
IntValue* int_obj_mod (IntValue * x, IntValue * y) {
  return make_int_obj(x->value % y->value);
}

Value* eq(IntValue * x, IntValue * y) {
  return x->value == y->value ?
         (Value*)make_int_obj(0) : (Value*)make_null_obj();
}
Value* lt(IntValue * x, IntValue * y) {
  return x->value < y->value ?
         (Value*)make_int_obj(0) : (Value*)make_null_obj();
}
Value* le(IntValue * x, IntValue * y) {
  return x->value <= y->value ?
         (Value*)make_int_obj(0) : (Value*)make_null_obj();
}
Value* gt(IntValue * x, IntValue * y) {
  return x->value > y->value ?
         (Value*)make_int_obj(0) : (Value*)make_null_obj();
}
Value* ge(IntValue * x, IntValue * y) {
  return x->value >= y->value ?
         (Value*)make_int_obj(0) : (Value*)make_null_obj();
}

// convert an interpreter object into a string
char* toString (Value * val_ptr) {
  switch (((RValue*)val_ptr)->tag) {
  case INT_OBJ: {
    return intToString(((IntValue*)val_ptr)->value);
  }
  case OBJ_OBJ: {
    char *result = malloc(9 * sizeof(char));
    strcpy(result, "[OBJECT]");
    return result;
  }
  case NULL_OBJ: {
    char *result = malloc(5 * sizeof(char));
    strcpy(result, "Null");
    return result;
  }
  case ARRAY_OBJ: {
    return arrayToString((ArrayValue*)val_ptr);
  }
  default:
    printf("Error: toString.\n");
    exit(-1);
  }
  return NULL;
}

char* arrayToString (ArrayValue *obj_ptr) {
  char** strs = malloc(sizeof(char*) * obj_ptr->v->size);
  int size_of_str = 1; //opening brace
  for (int i = 0; i < obj_ptr->v->size; ++i) {
    strs[i] = toString(vector_get(obj_ptr->v, i));
    size_of_str += strlen(strs[i]);
    if (i) {
      size_of_str += 1; //space
    }
  }
  size_of_str += 1; // closing brace
  size_of_str += 1; // null at end of string

  char *result = malloc(size_of_str * sizeof(char));
  *result = '\0';

  char* t = result;
  t = strcat(t, "[");
  for (int i = 0; i < obj_ptr->v->size; ++i) {
    t = strcat(t, strs[i]);
  }
  t = strcat(t, "]");
  return result;
}

char* intToString(int i) {
  char* b = malloc(20 * sizeof(char));
  char const digit[] = "0123456789";
  char* p = b;
  if (i < 0) {
    *p++ = '-';
    i *= -1;
  }
  int shifter = i;
  do { //Move to where representation ends
    ++p;
    shifter = shifter / 10;
  } while (shifter);
  *p = '\0';
  do { //Move back, inserting digits as u go
    *--p = digit[i % 10];
    i = i / 10;
  } while (i);
  return b;
}

char* copy_string (const char *string) {
  char *stringcopy = malloc (1 + strlen (string));
  if (stringcopy) {
    strcpy (stringcopy, string);
  } else {
    fprintf (stderr, "malloc failure!");
  }
  return stringcopy;
}

// You must free the result if result is non-NULL.
// only replace the first occurrance of *rep
char* str_replace (char *orig, char *rep, char *with) {
  char *result; // the return string
  char *ins;    // the next insert point
  char *tmp;    // varies
  int len_rep;  // length of rep
  int len_with; // length of with
  int len_front; // distance between rep and end of last rep
  int count;    // number of replacements

  if (!orig)
    return NULL;
  if (!rep)
    rep = "";
  len_rep = strlen(rep);
  if (!with)
    with = "";
  len_with = strlen(with);

  ins = orig;
  for (count = 0; (tmp = strstr(ins, rep)); ++count) {
    ins = tmp + len_rep;
  }
  if (count > 0) {
    count = 1;
  }
  // first time through the loop, all the variable are set correctly
  // from here on,
  //    tmp points to the end of the result string
  //    ins points to the next occurrence of rep in orig
  //    orig points to the remainder of orig after "end of rep"
  tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

  if (!result)
    return NULL;

  while (count--) {
    ins = strstr(orig, rep);
    len_front = ins - orig;
    tmp = strncpy(tmp, orig, len_front) + len_front;
    tmp = strcpy(tmp, with) + len_with;
    orig += len_front + len_rep; // move to next "end of rep"
  }
  strcpy(tmp, orig);
  return result;
}

