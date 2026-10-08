#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/sysmacros.h>

static void print_sanitized_name(const char *name) {
    while (*name) {
        if (opt.q && !isprint((unsigned char)*name)) {
            putchar('?');
        } else {
            putchar(*name);
        }
        name++;
    }
}

static char get_file_suffix(mode_t mode) {
    if (!opt.F) return '\0';
    if (S_ISDIR(mode)) return '/';
    if (S_ISLNK(mode)) return '@';
    if (S_ISSOCK(mode)) return '=';
    if (S_ISFIFO(mode)) return '|';
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return '*';
    return '\0';
}

void print_file(const FileInfo *fi) {
    if (opt.i) {
        printf("%llu ", (unsigned long long)fi->st.st_ino);
    }
    if (opt.s) {
        long blocks = (fi->st.st_blocks * 512 + opt.blocksize - 1) / opt.blocksize;
        printf("%ld ", blocks);
    }

    if (opt.l || opt.n) {
        char modestr[11];
        mode_to_string(fi->st.st_mode, modestr);
        printf("%s %2ld ", modestr, (long)fi->st.st_nlink);

        if (opt.n) {
            printf("%u %u ", (unsigned int)fi->st.st_uid, (unsigned int)fi->st.st_gid);
        } else {
            struct passwd *pw = getpwuid(fi->st.st_uid);
            struct group *gr = getgrgid(fi->st.st_gid);
            if (pw) printf("%-8s ", pw->pw_name);
            else printf("%-8u ", (unsigned int)fi->st.st_uid);
            if (gr) printf("%-8s ", gr->gr_name);
            else printf("%-8u ", (unsigned int)fi->st.st_gid);
        }

        if (S_ISCHR(fi->st.st_mode) || S_ISBLK(fi->st.st_mode)) {
            printf("%3d, %3d ", major(fi->st.st_rdev), minor(fi->st.st_rdev));
        } else {
            char sz[32];
            format_size(fi->st.st_size, sz, sizeof(sz));
            printf("%8s ", sz);
        }

        time_t ftime = opt.u ? fi->st.st_atime : (opt.c ? fi->st.st_ctime : fi->st.st_mtime);
        struct tm *tm_info = localtime(&ftime);
        char time_buf[64];
        strftime(time_buf, sizeof(time_buf), "%b %e %H:%M", tm_info);
        printf("%s ", time_buf);
    }

    print_sanitized_name(fi->name);
    char suffix = get_file_suffix(fi->st.st_mode);
    if (suffix) putchar(suffix);

    if ((opt.l || opt.n) && S_ISLNK(fi->st.st_mode)) {
        char link_target[1024];
        ssize_t len = readlink(fi->path, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(" -> %s", link_target);
        }
    }
    putchar('\n');
}

void print_entries(FileInfo *files, int count, bool is_dir_listing) {
    if (is_dir_listing && (opt.s || opt.l || opt.n)) {
        long total_blocks = 0;
        for (int i = 0; i < count; i++) {
            total_blocks += (files[i].st.st_blocks * 512 + opt.blocksize - 1) / opt.blocksize;
        }
        printf("total %ld\n", total_blocks);
    }
    for (int i = 0; i < count; i++) {
        print_file(&files[i]);
    }
}
