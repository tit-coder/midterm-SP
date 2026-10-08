#ifndef LS_H
#define LS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    char *name;
    char *path;
    struct stat st;
    bool stat_ok;
} FileInfo;

typedef struct {
    bool A; bool a; bool c; bool d; bool F; bool f;
    bool h; bool i; bool k; bool l; bool n; bool q;
    bool R; bool r; bool S; bool s; bool t; bool u; bool w;
    long blocksize;
} Options;

extern Options opt;
extern int exit_status;

void traverse_dir(const char *dirpath, bool print_dirname);
void print_entries(FileInfo *files, int count, bool is_dir_listing);
void print_file(const FileInfo *fi);
int compare_files(const void *a, const void *b);
void mode_to_string(mode_t mode, char *str);
void format_size(off_t size, char *buf, size_t buf_len);

#endif
