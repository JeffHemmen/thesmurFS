#include "thesmurFS.h"

int get_file_hash(char* physical_path, char** fileHash) {
    int vallen;
    vallen = lgetxattr(physical_path, CONF->xattr_name, *fileHash, 0);
    if(vallen == -1)
        return -1;
    *fileHash = (char*) malloc(vallen);
    vallen = lgetxattr(physical_path, CONF->xattr_name, *fileHash, vallen);
    return vallen;      //size or -1
}

int update_hash_folder(char* physical_path, int up_rec) {
    DIR *curr = opendir(physical_path);
    struct confStruct* conf = (struct confStruct*)(fuse_get_context()->private_data);
    struct dirent* file;
    char* fileHash;
    char* foldername;
    char* oldHash;
    char folderHash[HASH_DIGEST_LENGTH];
    int vallen, r;
    int ppl = strlen(physical_path);
    char* absolute_local_file;
    struct thesmurFS_CTX ctx;
    if(curr == NULL)
        return -1;
    hash_Init(&ctx);
    foldername = get_file_name(physical_path);
    hash_Update(&ctx, foldername, strlen(foldername));
    free(foldername);
    while(file = readdir(curr)) {
        if(same_string(file->d_name, ".") || same_string(file->d_name, ".."))
            continue;
        absolute_local_file = (char*) calloc(ppl+strlen(file->d_name)+2, sizeof(char));
        concat_path(physical_path, file->d_name, absolute_local_file);
        //vallen = lgetxattr(absolute_local_file, CONF->xattr_name, fileHash, 0);
        vallen = get_file_hash(absolute_local_file, &fileHash); //allocates fileHash, can be freed afterwards
        if(vallen == -1) {
            if(errno == ENOATTR) {
                if(file->d_type == DT_DIR)              //directory
                    update_hash_folder(absolute_local_file, 0);
                else if(file->d_type == DT_LNK)         //symlink
                    update_hash_link(absolute_local_file);
                else if(file->d_type == DT_REG)         //regular file?
                    update_hash_file(absolute_local_file);
                else                                    //special file - not supported
                    continue;
                vallen = get_file_hash(absolute_local_file, &fileHash);
            }
        }
        if(vallen == -1)
            continue;
        hash_Update(&ctx, fileHash, vallen);
        free(fileHash);
        free(absolute_local_file);
    }
    closedir(curr);
    hash_Final(folderHash, &ctx);
    
    r = get_file_hash(physical_path, &oldHash);
    if(r != -1 && same_string(oldHash, folderHash)) {        //'string' as general data here... it's all just bytes!
        return 0;       //ok, no need to update this OR the parent hashes
    }
        
    r = lsetxattr(physical_path, CONF->xattr_name, folderHash, HASH_DIGEST_LENGTH, 0);
    if(!up_rec)
        return 0;   //success
    if(same_string(physical_path, conf->rootdir))
        return 0;   //success
    make_parent_dir(physical_path);
    return update_hash_folder(physical_path, 1);
}

int update_hash_file(char* physical_path) {
    int r, fh, s;
    struct thesmurFS_CTX ctx;
    char* filename;
    unsigned int buff_size = 4000;
    unsigned char data[buff_size];
    
    fh = open(physical_path, O_RDONLY);
    if(fh == -1) {
        //log error here, journaling?
        //;
        return -1;
    }
    hash_Init(&ctx);
    filename = get_file_name(physical_path);
    hash_Update(&ctx, filename, strlen(filename));
    free(filename);
    do {
        s = read(fh, data, buff_size);
        hash_Update(&ctx, data, s);
    } while(s>0);
    
    if(s == -1) {

        return -1;
    }
    
    if(close(fh) == -1) {
        //log
    }
    
    if(hash_Final(data, &ctx)  == 0) {
        //log
        return -1;
    }

    r = lsetxattr(physical_path, CONF->xattr_name, data, HASH_DIGEST_LENGTH, 0);
    if(r == -1) {
        //log
        return -1;
    }
    
    return 0;
}

int update_hash_link(char* physical_path) {
    int r;
    struct thesmurFS_CTX ctx;
    char* filename;
    char* linkptr;
    unsigned char data[HASH_DIGEST_LENGTH];
    ssize_t s;
    
    s = readlink(physical_path, 0, 0);
    if(s == 0)
        return -1;
    hash_Init(&ctx);
    filename = get_file_name(physical_path);
    hash_Update(&ctx, filename, strlen(filename));
    free(filename);

    linkptr = calloc(s, sizeof(char));
    readlink(physical_path, linkptr, s);
    
    hash_Update(&ctx, linkptr, s);
    
    if(hash_Final(data, &ctx)  == 0)
        return -1;
    r = lsetxattr(physical_path, CONF->xattr_name, data, HASH_DIGEST_LENGTH, 0);
    if(r == -1)
        return -1;
    return 0;
}