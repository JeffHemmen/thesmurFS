#include "thesmurFS.h"

#define LINEBUFFER_LENGTH 256

void dump_conf(struct confStruct* conf) {
    printf("%s|\n%s|\n%s|\n%s|\n%s|\n%s|\n%s|\n%s|\n", conf->mountdir, conf->rootdir, conf->argv[0], conf->argv[1], conf->argv[2], conf->xattr_name, conf->xattr_namespace, conf->logpath);
}

void conf_read_string(char* source, char** dest) {
    int l = strlen(source);
    *dest = calloc(l, sizeof(char));
    memcpy(*dest, source, l);
    (*dest)[l-1] = 0;
}

void tokenise_fuse_args(struct confStruct* conf, char* longStr) {
    //conf->argv = calloc(34, sizeof(char*));
    char* arg;
    //conf->argv[0] = "thesmurFS";
    conf->argc++;
    for(arg = strtok(longStr, " "); arg; arg = strtok(0, " ")) {
        conf->argv[conf->argc++] = arg;
    }
    conf->argv[conf->argc++] = conf->mountdir;
}

void loadconf(int argc, char** argv, struct confStruct* conf) {
    char* confFile;
    char* fuse_args;
    char line[LINEBUFFER_LENGTH];
    
    conf->argv = calloc(34, sizeof(char*));     //max number of possible options
    conf->argv[0] = argv[0];
    
    if(argc == 2)
        confFile = argv[1];
    else
        confFile = CONF_FILE_DEFAULT_PATH;
    
    printf("Loading conf file \"%s\"\n", confFile);
    
    FILE *fp = fopen(confFile, "r");
    if(fp == 0) {
        fprintf(stderr, "Could not open configuration file \"%s\".\nExiting.", confFile);
        fprintf(stdout, "Could not open configuration file \"%s\".\nExiting.", confFile);
        exit(1);
    }
    
    while(fgets(line, LINEBUFFER_LENGTH, fp)) { 

        if(line == 0 || line[0] == EOF)
            break;
        if(line[0] == '#')
            continue;   //comment
        if(line[0] == 0)
            break;      //eof
        if(line[0] == 'r') {    //root dir
            conf_read_string(line+1, &(conf->rootdir));
            conf->rootdirlen = strlen(conf->rootdir);
        }
        if(line[0] == 'm') {    //mount point
            conf_read_string(line+1, &(conf->mountdir));
        }
        if(line[0] == 'x') {    //xattr namespace
            int l;
            conf_read_string(line+1, &(conf->xattr_namespace));
            l = strlen(conf->xattr_namespace);
            conf->xattr_name = calloc(l+5, sizeof(char));
            memcpy(conf->xattr_name, conf->xattr_namespace, l);
            memcpy(conf->xattr_name+l, ".hash", 5);
            conf->xattr_name[l+5] = 0;   //calloc takes care of this but better be safe
        }
        if(line[0] == 'l') {
            conf_read_string(line+1, &(conf->logpath));
        }
        if(line[0] == 'f') {
            conf_read_string(line+1, &fuse_args);
        }
    }
    
    tokenise_fuse_args(conf, fuse_args);       //needs fuse_args AND conf->mountdir to be set!
    
    
    //testing...
    /*conf->mountdir = "/home/jeff/thesmurFS";
    conf->mountdir_mod = S_IRWXU;
        
    conf->rootdir  = "/home/jeff/.thesmurFS/data";
    conf->rootdirlen = 26;
    
    conf->argc = 3;
    conf->argv = (char**) calloc(conf->argc*sizeof(char*), 1);
    conf->argv[0] = argv[0];
    conf->argv[1] = "-d";
    conf->argv[2] = conf->mountdir;
    
    conf->xattr_namespace = "user.lu.citrusfruit.thesmurFS";
    conf->xattr_name = "user.lu.citrusfruit.thesmurFS.hash";
    
    conf->logpath = "/home/jeff/.thesmurFS/log";
    */
    fprintf(stdout, "Configuration file loaded...!\n");
    dump_conf(conf);
    printf("%s|\n", fuse_args);
}