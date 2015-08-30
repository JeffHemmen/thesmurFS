#include "thesmurFS.h"


int thesmurFS_getattr(const char *path, struct stat *statbuf) {
    char* physical_path;
    int r;
    //emergency_output("getattr");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = lstat(physical_path, statbuf);
    //free(physical_path);
    if(r == -1)
        return -errno;
    return r;
}

int thesmurFS_readlink(const char* path, char* link, size_t size) {
    char* physical_path;
    int r;
    //emergency_output("readlink");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = readlink(physical_path, link, size-1);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_mknod(const char* path, mode_t mod, dev_t dev) {
    char* physical_path;
    int r;
    //emergency_output("mknod");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = mknod(physical_path, mod, dev);
    if(r<0)
        return -errno;
    update_hash_file(physical_path);
    return r;
}

int thesmurFS_mkdir(const char* path, mode_t mod) {
    char* physical_path;
    int r;
    //emergency_output("mkdir");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = mkdir(physical_path, mod);
    if(r<0)
        return -errno;
    update_hash_folder(physical_path, 1);
    return r;
}

int thesmurFS_unlink(const char* path) {
    char* physical_path;
    char* path_copy;
    int r;
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = unlink(physical_path);
    if(r == -1)
        return -errno;
    
    path_copy = calloc(strlen(physical_path)+1, sizeof(char));
    memcpy(path_copy, physical_path, strlen(physical_path)+1);
    make_parent_dir(path_copy);
    update_hash_folder(path_copy, 1);
    free(path_copy);
    
    return r;
}

int thesmurFS_rmdir(const char* path) {
    char* physical_path;
    char* path_copy;
    int r;
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = rmdir(physical_path);
    if(r<0)
        return -errno;
    
    path_copy = calloc(strlen(physical_path)+1, sizeof(char));
    memcpy(path_copy, physical_path, strlen(physical_path)+1);
    make_parent_dir(path_copy);
    update_hash_folder(path_copy, 1);
    free(path_copy);
    
    return r;
}

int thesmurFS_symlink(const char* path, const char* link) {
    char* physical_path;
    char* physical_link;
    int r;
    //emergency_output("symlink");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    if(get_physical_path(link, &physical_link) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = symlink(physical_path, physical_link);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_rename(const char* path, const char* name) {
    char* physical_path;
    char* physical_name;
    int r;
    //emergency_output("rename");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    if(get_physical_path(name, &physical_name) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = rename(physical_path, physical_name);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_link(const char* path, const char* name) {
    char* physical_path;
    char* physical_name;
    int r;
    //emergency_output("link");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    if(get_physical_path(name, &physical_name) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = link(physical_path, physical_name);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_chmod(const char* path, mode_t mod) {
    char* physical_path;
    int r;
    //emergency_output("chmod");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = chmod(physical_path, mod);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_chown(const char* path, uid_t uid, gid_t gid) {
    char* physical_path;
    int r;
    //emergency_output("chown");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = chown(physical_path, uid, gid);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_truncate(const char* path, off_t size) {
    char* physical_path;
    char* path_copy;
    int r;
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    //emergency_output("truncating:");
    //emergency_output(path);
    //emergency_output_plus_n("new size", size);
    r = truncate(physical_path, size);
    //emergency_output_plus_n("truncate returns", r);
    if(r == -1) {
        return -errno;
        //emergency_output_plus_n("truncate errno", errno);
        //emergency_output("-----truncate");
    }
    //emergency_output("-----truncate");
    
    //emergency_output("truncate - update hash");
    //emergency_output(physical_path);
    path_copy = calloc(strlen(physical_path)+1, sizeof(char));
    memcpy(path_copy, physical_path, strlen(physical_path)+1);
    update_hash_file(path_copy);
    make_parent_dir(path_copy);
    update_hash_folder(path_copy, 1);
    free(path_copy);
    
    return r;
}

int thesmurFS_utime(const char* path, struct utimbuf *ubuf) {
    char* physical_path;
    int r;
    //emergency_output("utime");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = utime(physical_path, ubuf);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_open(const char* path, struct fuse_file_info *fi) {
    char* physical_path;
    int r;
    //emergency_output("open");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    //emergency_output_plus_n(physical_path, fi->flags);
    r = open(physical_path, fi->flags);
    //emergency_output_plus_n("open returns", r);
    if(r == -1)
        return -errno;
    fi->fh = r;
    //emergency_output("-----open");
    return 0;
}

int thesmurFS_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    int r;
    //emergency_output("read");
    //physical path already in fi->fh, no need to get_physical_path(...)
    r = read(fi->fh, buf, size);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    int r;
    //emergency_output("write");
    //physical path already in fi->fh, no need to get_physical_path(...)
    r = write(fi->fh, buf, size);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_statfs(const char* path, struct statvfs* statv) {
    char* physical_path;
    int r;
    //emergency_output("statfs");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = statvfs(physical_path, statv);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_flush(const char* path, struct fuse_file_info* fi) {
    //emergency_output("flush");
    //fflush(fi->fh);
    return 0;
}

int thesmurFS_release(const char* path, struct fuse_file_info *fi) {
    char* path_copy;
    int r;
    //emergency_output("release");
    //get_physical_path(path, &physical_path);
    r = close(fi->fh);
    if(r == -1)
        return -errno;
    
    
    get_physical_path(path, &path_copy);
    //emergency_output("release - update hash");
    //emergency_output(path_copy);
    update_hash_file(path_copy);
    make_parent_dir(path_copy);
    update_hash_folder(path_copy, 1);
    free(path_copy);
    
    return r;
}

int thesmurFS_fsync(const char* path, int datasync, struct fuse_file_info* fi) {
    char* physical_path;
    int r;
    //emergency_output("fsync");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    if(datasync)
        r = fdatasync(fi->fh);
    else
        r = fsync(fi->fh);
    return r;
}

int thesmurFS_setxattr(const char* path, const char* name, const char* value, size_t size, int flags) {
    char* physical_path;
    int r;
    /*emergency_output("setxattr");
    emergency_output(path);
    emergency_output(name);
    emergency_output(value);
    emergency_output_plus_n("size", size);
    emergency_output_plus_n("flags", flags);/**/
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = lsetxattr(physical_path, name, value, size, flags);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_getxattr(const char* path, const char* name, char* value, size_t size) {
    char* physical_path;
    int r;
    //emergency_output("getxattr");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = lgetxattr(physical_path, name, value, size);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_listxattr(const char* path, char* list, size_t size) {
    char* physical_path;
    int r;
    //emergency_output("listxattr");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = llistxattr(physical_path, list, size);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_removexattr(const char* path, const char* name) {
    char* physical_path;
    int r;
    //emergency_output("removexattr");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = lremovexattr(physical_path, name);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_opendir(const char* path, struct fuse_file_info* fi) {
    DIR* dp;
    char* physical_path;
    //emergency_output("opendir()");
    //emergency_output(path);
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    //emergency_output("physical path determined:");
    //emergency_output(physical_path);
    dp = opendir(physical_path);
    //emergency_output_plus_n("opendir returned", dp);
    if(dp==NULL)
        return -errno;
    fi->fh = (intptr_t) dp;
    //emergency_output_plus_n("file handler assigned", fi->fh);
    //emergency_output("-----opendir");
    return 0;
}

int thesmurFS_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
    DIR* dp;
    struct dirent* de;
    int ne=0;  //number of entries
    //physical path already in fi->fh, no need to get_physical_path
    //emergency_output("readdir");
    dp = (DIR*)(uintptr_t) fi->fh;
    //de = readdir(dp);
    //emergency_output_plus_n("de address", de);
    while(de = readdir(dp)) {   //while this isn't 0 (NULL)
        if(filler(buf, de->d_name, NULL, 0)) {
            return -ENOMEM;     //filler should return 0
        }
        ne++;
    }
    if(!ne) {    //no entries, must at least be one ('.')
        return -errno;
    }
    return 0;
}

int thesmurFS_releasedir(const char* path, struct fuse_file_info* fi) {
    int r;
    //emergency_output("releasing dir");
    //emergency_output_plus_n("fh", fi->fh);
    r = closedir((DIR*)(uintptr_t)fi->fh);
    //emergency_output_plus_n("releasedir result", r);
    //emergency_output("-----releasedir");
    if(r<0)
        return -errno;
    return r;
}

//not implmeted fsyncdir

void* thesmurFS_init(struct fuse_conn_info* conn) {
    //FUSE context available when this is called, return the private data
    log_msg("Initialising file system...");
    
    //some errnos
    /*emergency_output_plus_n("EACCES", EACCES);
    emergency_output_plus_n("EBADF", EBADF);
    emergency_output_plus_n("EFAULT", EFAULT);
    emergency_output_plus_n("ELOOP", ELOOP);
    emergency_output_plus_n("ENAMETOOLONG", ENAMETOOLONG);
    emergency_output_plus_n("ENOENT", ENOENT);
    emergency_output_plus_n("ENOMEM", ENOMEM);
    emergency_output_plus_n("ENOTDIR", ENOTDIR);
    emergency_output_plus_n("EOVERFLOW", EOVERFLOW);
    emergency_output_plus_n("ENOTSUP", ENOTSUP);*/
    
    return fuse_get_context()->private_data;
}

void gargamel(void* userdata) {
    //destroys thesmurFS
    log_msg("gargamelised...\n");
}

int thesmurFS_access(const char* path, int mask) {
    char* physical_path;
    int r;
    //emergency_output("access");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    r = access(physical_path, mask);
    if(r<0)
        return -errno;
    return r;
}

int thesmurFS_create(const char* path, mode_t mod, struct fuse_file_info* fi) {
    char* physical_path;
    char* path_copy;
    int fd;
    //emergency_output("create");
    if(get_physical_path(path, &physical_path) == -1) {
        //emergency_output("ENOMEM");
        return -errno;
    }
    fd = creat(physical_path, mod);
    if(fd<0)
        return -errno;
    fi->fh = fd;
    
    //emergency_output("create - update hash");
    //emergency_output(physical_path);
    path_copy = calloc(strlen(physical_path)+1, sizeof(char));
    memcpy(path_copy, physical_path, strlen(physical_path)+1);
    update_hash_file(path_copy);
    make_parent_dir(path_copy);
    update_hash_folder(path_copy, 1);
    free(path_copy);
    
    return 0;
}

int thesmurFS_ftruncate(const char* path, off_t size, struct fuse_file_info* fi) {
    int r;
    //emergency_output("ftruncating");
    //emergency_output_plus_n(path, fi->fh);
    //emergency_output_plus_n("size", size);
    r = ftruncate(fi->fh, size);
    //emergency_output_plus_n("ftruncate returns", r);
    //emergency_output("-----ftruncate");
    if(r == -1)
        return -errno;
    
    return r;
}

int thesmurFS_fgetattr(const char* path, struct stat* statbuf, struct fuse_file_info* fi) {
    int r;
    //emergency_output("fgetattr");
    r = fstat(fi->fh, statbuf);
    if(r<0)
        return -errno;
    return r;
}


struct fuse_operations thesmurFS_operations = {
    .getattr            = thesmurFS_getattr,
    .readlink           = thesmurFS_readlink,
    //.getdir             = NULL,
    .mknod              = thesmurFS_mknod,
    .mkdir              = thesmurFS_mkdir,
    .unlink             = thesmurFS_unlink,
    .rmdir              = thesmurFS_rmdir,
    .symlink            = thesmurFS_symlink,
    .rename             = thesmurFS_rename,
    .link               = thesmurFS_link,
    .chmod              = thesmurFS_chmod,
    .chown              = thesmurFS_chown,
    .truncate           = thesmurFS_truncate,
    .utime              = thesmurFS_utime,
    .open               = thesmurFS_open,
    .read               = thesmurFS_read,
    .write              = thesmurFS_write,
    /** Just a placeholder, don't set */ // huh???
    .statfs             = thesmurFS_statfs,
    .flush              = thesmurFS_flush,
    .release            = thesmurFS_release,
    .fsync              = thesmurFS_fsync,
    .setxattr           = thesmurFS_setxattr,
    .getxattr           = thesmurFS_getxattr,
    .listxattr          = thesmurFS_listxattr,
    .removexattr        = thesmurFS_removexattr,
    .opendir            = thesmurFS_opendir,
    .readdir            = thesmurFS_readdir,
    .releasedir         = thesmurFS_releasedir,
    .fsyncdir           = NULL,
    .init               = thesmurFS_init,
    .destroy            = gargamel,
    .access             = thesmurFS_access,
    .create             = thesmurFS_create,
    .ftruncate          = thesmurFS_ftruncate,
    .fgetattr           = thesmurFS_fgetattr
};