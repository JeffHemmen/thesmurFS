#include "thesmurFS.h"

#if THESMURFS_USE_HASH == USE_HASH_MD5

#ifndef HASH_MD5_INC
#define HASH_MD5_INC

#include <openssl/md5.h>

#ifndef HASH_DIGEST_LENGTH
#define HASH_DIGEST_LENGTH MD5_DIGEST_LENGTH
#endif

unsigned char* hash_atom(const unsigned char* d, unsigned long n, unsigned char* md) {
    return MD5(d, n, md);
}

//struct thesmurFS_CTX; - already defined in header

int hash_Init(struct thesmurFS_CTX* str_ctx) {
    return MD5_Init(&(str_ctx->ctx));
}

int hash_Update(struct thesmurFS_CTX* str_ctx, const void* data, unsigned long len) {
    return MD5_Update(&(str_ctx->ctx), data, len);
}

int hash_Final(unsigned char* md, struct thesmurFS_CTX* str_ctx) {
    return MD5_Final(md, &(str_ctx->ctx));
}

#endif //HASH_MD5_INC
#endif //THESMURFS_USE_HASH