/* 
 * File:   main.c
 * Author: jeff
 *
 * Created on April 19, 2012, 3:25 AM
 */

#include "thesmurFS.h"

/*
 * 
 */
int main(int argc, char** argv) {

    char* test1;
    char* test2;
    
    struct confStruct* conf = (struct confStruct*) calloc(sizeof(struct confStruct), 1);
    //struct dataStruct* data = (struct dataStruct*) calloc(sizeof(struct dataStruct), 1);
    int r=0;
    
    
    loadconf(argc, argv, conf);
    printf("Rootdir:\t%s\nMountdir:\t%s\n", conf->rootdir, conf->mountdir);
    
    //check if mountdir exists, create otherwise
    if(access(conf->mountdir, F_OK))
        r=mkdir(conf->mountdir, conf->mountdir_mod);
        if(r != 0)
            fprintf(stderr, "Could not create mount point directory.\n\tError: %i\n", r);
    
    //check if mountdir is writeable, change otherwise
    if(access(conf->mountdir, R_OK|W_OK|X_OK))
            fprintf(stderr, "Need RWX access rights on mount point directory.\n");
    
    conf->logfile = log_open(conf->logpath);
    
    printf("456\n");
    
    printf("argc = %d\n", conf->argc);
    for(r=0; r<conf->argc; r++)
        printf("\targv[%i] = %s\n", r, conf->argv[r]);
        
    printf("about to call fuse_main...");
    r = fuse_main(conf->argc, conf->argv, &thesmurFS_operations, conf);
    fprintf(stderr, "fuse_main returned");
    fprintf(stderr, "ret %i\n", r);
    
    if(r != 0)
        fprintf(stderr, "fuse_main returned with %i.\n", r);
    
    printf("Ok.\n");
    return 0;
}

