/* 
 * File:   thesmurFS.h
 * Author: Jeff Hemmen - 50906132
 *
 */

#ifndef THESMURFS_H
#define	THESMURFS_H

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 26
#endif //FUSE_USE_VERSION

#define USE_HASH_MD5 5
#define USE_HASH_SHA1 1

#ifndef THESMURFS_USE_HASH
#define THESMURFS_USE_HASH USE_HASH_MD5
#endif

#if THESMURFS_USE_HASH == USE_HASH_MD5
#include <openssl/md5.h>
#define HASH_DIGEST_LENGTH MD5_DIGEST_LENGTH
#elif THESMURFS_USE_HASH == USE_HASH_SHA1
#include <openssl/sha.h>
#define HASH_DIGEST_LENGTH SHA_DIGEST_LENGTH
#endif

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <attr/xattr.h>
#include <unistd.h>


#ifdef	__cplusplus
extern "C" {
#endif

    
    /*CONFIGURATION*/
    #define CONF ((struct confStruct*) (fuse_get_context()->private_data))
    #define CONF_FILE_DEFAULT_PATH "~/.thesmurFS/conf"
    struct confStruct {
        FILE* logfile;
                
        int    argc;
        char** argv;
        
        char* mountdir;
        char* rootdir;
        int rootdirlen;
        mode_t mountdir_mod;
        
        char* xattr_namespace;
        char* xattr_name;
        
        char* logpath;
    };
    
    void loadconf(int, char**, struct confStruct*);
    
    /*ERROR REPORTING*/
    void fatal_error(char*, ...);
    
    /*FUSE STRUCTS*/
    struct fuse_operations thesmurFS_operations;

    /*LOGGING*/

    //  macro to log fields in structs.
    #define log_struct(st, field, format, typecast) \
    log_msg("    " #field " = " #format "\n", typecast st->field)

    FILE *log_open(char*);
    void log_fi (struct fuse_file_info *fi);
    void log_stat(struct stat *si);
    void log_statvfs(struct statvfs *sv);
    void log_utime(struct utimbuf *buf);

    void log_msg(const char *format, ...);
    
    /*STRING OPERATIONS*/
    int same_string(const char* a, const char* b);
    void make_parent_dir(char* dir);
    void concat_path(char*, char*, char*);
    int get_physical_path(const char*, char**);
    
    /*CORE FUNCTIONALITIES*/
    int update_hash_folder(char*, int);
    int update_hash_file(char*);

    /*HASH FUNCTIONALITIES*/
    unsigned char* hash_atom(const unsigned char*, unsigned long, unsigned char*);
    struct thesmurFS_CTX {
        #if   THESMURFS_USE_HASH == USE_HASH_MD5
        MD5_CTX ctx;
        #elif THESMURFS_USE_HASH == USE_HASH_SHA1
        SHA_CTX ctx;
        #endif
    };
    int hash_Init(struct thesmurFS_CTX*);
    int hash_Update(struct thesmurFS_CTX*, const void*, unsigned long);
    int hash_Final(unsigned char*, struct thesmurFS_CTX*);/**/
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* THESMURFS_H */

