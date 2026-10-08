#include "ls.h"
#include <string.h>

static time_t get_file_time(const struct stat *st) {
    if (opt.u) return st->st_atime;
    if (opt.c) return st->st_ctime;
    return st->st_mtime;
}

int compare_files(const void *a, const void *b) {
    const FileInfo *fa = (const FileInfo *)a;
    const FileInfo *fb = (const FileInfo *)b;

    int res = 0;
    if (opt.S) {
        if (fa->st.st_size < fb->st.st_size) res = 1;
        else if (fa->st.st_size > fb->st.st_size) res = -1;
    } else if (opt.t) {
        time_t ta = get_file_time(&fa->st);
        time_t tb = get_file_time(&fb->st);
        if (ta < tb) res = 1;
        else if (ta > tb) res = -1;
    }

    if (res == 0) {
        res = strcmp(fa->name, fb->name);
    }

    return opt.r ? -res : res;
}
