#include "thesmurFS.h"


//compares two strings - strings will not be modified - 0=strings differ; 1=same string
int same_string(const char* a, const char* b) {
    int i;
    for(i = 0; a[i] && (a[i] == b[i]); i++);
    return a[i] == b[i];
}

//transforms an input string into its parent (or containing) directory
//input string is modified!!
void make_parent_dir(char* dir) {
    int n;
    for(n=0; dir[n]; n++);
    while(dir[n] != '/')
        n--;
    dir[n] = 0;
}

//cocatenates two paths - buffer must be pre-allocated
//this can then all be freed by whoever allocated it!!
void concat_path(char* dir, char* file, char* buf) {
    //buf is supposed to hold strlen(dir)+strlen(file)+2 characters al least!
    //and pre-filled with zeroes! (calloc != malloc)
    int dirlen;
    int filelen;
    for(dirlen = 0; dir[dirlen]; dirlen++);
    if(dir[dirlen-1] == '/') {
        dirlen--;
    }
    for(filelen = 0; file[filelen]; filelen++);
    if(file[0] == '/') {
        filelen--;
        file ++;        //cut off first character of string!
    }
    
    
    //now we have dir = "/some/directoty" and file "some.file"
    
    memcpy(buf,          dir,  dirlen);
    memcpy(buf+dirlen+1, file, filelen);
    buf[dirlen] = '/';
    buf[dirlen+1+filelen] = 0;  //better be sure
}

//transforms a relative thesmurFS path into an absolute path
//absolute_path will contains a pointer to allocated memory
//should be freed after use - safe!
int get_physical_path(const char* relative_path, char** absolute_path) {
    int rpl=-1; //relative path length
    int apl;    //absolute path length
    
    //emergency_output("\tget_physical_path-----");
    if(fuse_get_context() == NULL)
        ;//emergency_output("\tfuse_get_context = null");
    struct confStruct* conf = (struct confStruct*)(fuse_get_context()->private_data);
    if(conf == NULL)
        ;//emergency_output("\tconf = null");
    
    //emergency_output("\tconf not NULL");
    //emergency_output("\trelative path:");
    //emergency_output(relative_path);
    
    while(relative_path[++rpl]);  //length of relative path
    //emergency_output_plus_n("\tlength", rpl);
    apl = rpl+(conf->rootdirlen)+1;  //length of absolute path
        //printf("rpl=%i; apl=%i\n", rpl, apl);
    //emergency_output_plus_n("\tapl", apl);
    *absolute_path = (char*)calloc(apl, sizeof(char));
    if(*absolute_path == NULL) {
        //emergency_output("\tcalloc failed");
        *absolute_path = NULL;
        errno = ENOMEM;
        return -1;
    }
    //emergency_output_plus_n("alloc", *absolute_path);
    memcpy(*absolute_path, conf->rootdir, conf->rootdirlen);
        //printf("1: %s\n", *absolute_path);
    memcpy(*absolute_path+conf->rootdirlen, relative_path, rpl);
    (*absolute_path)[apl-1] = '\0';
    //emergency_output(*absolute_path);
        //printf("2: %s\n", *absolute_path);
    //emergency_output("\tget_physical_path OVER");
    return 0;
}

//returns the name of the file when given a full path
//returns allocated string, safe to free!!
char* get_file_name(char* fullpath) {
    int a, b, c;
    char* res;

    a = strlen(fullpath);
    if(fullpath[a-1] == '/')
        fullpath[--a];
    for(b = a-1; fullpath[b]!='/'; b--);
    b++;
    c = a-b+1;
    res = (char*) calloc(c, sizeof(char));
    memcpy(res, fullpath+b, c-1);
    res[c-1] = 0;

    return res;
}