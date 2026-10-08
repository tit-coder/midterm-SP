#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Options opt;
int exit_status = 0;

void traverse_dir(const char *dirpath, bool print_dirname) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror(dirpath);
        exit_status = 1;
        return;
    }

    if (print_dirname) {
        printf("\n%s:\n", dirpath);
    }

    struct dirent *dp;
    FileInfo *files = NULL;
    int count = 0;

    while ((dp = readdir(dir)) != NULL) {
        if (!opt.a && !opt.A && dp->d_name[0] == '.') continue;
        if (opt.A && (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))) continue;

        FileInfo *tmp = realloc(files, sizeof(FileInfo) * (count + 1));
        if (!tmp) {
            perror("realloc");
            free(files);
            closedir(dir);
            exit(1);
        }
        files = tmp;
        files[count].name = strdup(dp->d_name);
        
        char path_buf[2048];
        snprintf(path_buf, sizeof(path_buf), "%s/%s", dirpath, dp->d_name);
        files[count].path = strdup(path_buf);

        if (lstat(files[count].path, &files[count].st) == -1) {
            files[count].stat_ok = false;
        } else {
            files[count].stat_ok = true;
        }
        count++;
    }
    closedir(dir);

    if (!opt.f) {
        qsort(files, count, sizeof(FileInfo), compare_files);
    }

    print_entries(files, count, true);

    if (opt.R) {
        for (int i = 0; i < count; i++) {
            if (S_ISDIR(files[i].st.st_mode) &&
                strcmp(files[i].name, ".") != 0 &&
                strcmp(files[i].name, "..") != 0) {
                traverse_dir(files[i].path, true);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(files[i].name);
        free(files[i].path);
    }
    free(files);
}

int main(int argc, char **argv) {
    memset(&opt, 0, sizeof(opt));
    opt.blocksize = 512;
    char *env_bs = getenv("BLOCKSIZE");
    if (env_bs) {
        long bs = atol(env_bs);
        if (bs > 0) opt.blocksize = bs;
    }

    int ch;
    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
            case 'A': opt.A = true; break;
            case 'a': opt.a = true; break;
            case 'c': opt.c = true; break;
            case 'd': opt.d = true; break;
            case 'F': opt.F = true; break;
            case 'f': opt.f = true; break;
            case 'h': opt.h = true; opt.k = false; break;
            case 'i': opt.i = true; break;
            case 'k': opt.k = true; opt.h = false; opt.blocksize = 1024; break;
            case 'l': opt.l = true; break;
            case 'n': opt.n = true; break;
            case 'q': opt.q = true; opt.w = false; break;
            case 'R': opt.R = true; break;
            case 'r': opt.r = true; break;
            case 'S': opt.S = true; break;
            case 's': opt.s = true; break;
            case 't': opt.t = true; break;
            case 'u': opt.u = true; break;
            case 'w': opt.w = true; opt.q = false; break;
            default:
                exit(1);
        }
    }

    if (geteuid() == 0 && !opt.a) opt.A = true;
    if (isatty(STDOUT_FILENO)) {
        if (!opt.w) opt.q = true;
    } else {
        if (!opt.q) opt.w = true;
    }

    int num_args = argc - optind;
    if (num_args == 0) {
        traverse_dir(".", false);
        return exit_status;
    }

    FileInfo *file_ops = NULL; int num_files = 0;
    FileInfo *dir_ops = NULL;  int num_dirs = 0;

    for (int i = optind; i < argc; i++) {
        struct stat st;
        if (lstat(argv[i], &st) == -1) {
            perror(argv[i]);
            exit_status = 1;
            continue;
        }

        if (S_ISDIR(st.st_mode) && !opt.d) {
            FileInfo *tmp = realloc(dir_ops, sizeof(FileInfo) * (num_dirs + 1));
            if (!tmp) exit(1);
            dir_ops = tmp;
            dir_ops[num_dirs].name = argv[i];
            dir_ops[num_dirs].path = argv[i];
            dir_ops[num_dirs].st = st;
            num_dirs++;
        } else {
            FileInfo *tmp = realloc(file_ops, sizeof(FileInfo) * (num_files + 1));
            if (!tmp) exit(1);
            file_ops = tmp;
            file_ops[num_files].name = argv[i];
            file_ops[num_files].path = argv[i];
            file_ops[num_files].st = st;
            num_files++;
        }
    }

    if (!opt.f) {
        if (num_files > 0) qsort(file_ops, num_files, sizeof(FileInfo), compare_files);
        if (num_dirs > 0) qsort(dir_ops, num_dirs, sizeof(FileInfo), compare_files);
    }

    if (num_files > 0) {
        print_entries(file_ops, num_files, false);
    }

    for (int i = 0; i < num_dirs; i++) {
        bool show_header = (num_args > 1);
        traverse_dir(dir_ops[i].name, show_header);
    }

    free(file_ops);
    free(dir_ops);
    return exit_status;
}
