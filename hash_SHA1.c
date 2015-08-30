#include "thesmurFS.h"

#if THESMURFS_USE_HASH == USE_HASH_SHA1

#ifndef HASH_SHA1_INC
#define HASH_SHA1_INC

#include <openssl/sha.h>

#ifndef HASH_DIGEST_LENGTH
#define HASH_DIGEST_LENGTH SHA_DIGEST_LENGTH
#endif

unsigned char* hash_atom(const unsigned char* d, unsigned long n, unsigned char* md) {
    return SHA1(d, n, md);
}

//struct thesmurFS_CTX; - already defined in header

int hash_Init(struct thesmurFS_CTX* str_ctx) {
    return SHA_Init(&(str_ctx->ctx));
}

int hash_Update(struct thesmurFS_CTX* str_ctx, const void* data, unsigned long len) {
    return SHA_Update(&(str_ctx->ctx), data, len);
}

int hash_Final(unsigned char* md, struct thesmurFS_CTX* str_ctx) {
    return SHA_Final(md, &(str_ctx->ctx));
}

#endif //HASH_SHA1_INC
#endif //THESMURFS_USE_HASH
