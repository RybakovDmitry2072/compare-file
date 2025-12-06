#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int compare_files(char *file1, char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    
    if (f1 == NULL || f2 == NULL) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }
    
    int c1, c2;
    int same = 1;
    
    while (1) {
        c1 = fgetc(f1);
        c2 = fgetc(f2);
        
        if (c1 != c2) {
            same = 0;
            break;
        }
        
        if (c1 == EOF) break;
    }
    
    fclose(f1);
    fclose(f2);
    return same;
}

void process_dir(char *dir_path, char files[][MAX_PATH], int *count) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) return;
    
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[MAX_PATH];
        sprintf(full_path, "%s/%s", dir_path, entry->d_name);
        
        struct stat file_info;
        if (stat(full_path, &file_info) == -1) continue;
        
        if (S_ISDIR(file_info.st_mode)) {
            process_dir(full_path, files, count);
        } else if (S_ISREG(file_info.st_mode)) {
            if (*count < 1000) {
                sprintf(files[*count], "%s", full_path);
                (*count)++;
            }
        }
    }
    
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        return 1;
    }
    
    char files1[1000][MAX_PATH];
    char files2[1000][MAX_PATH];
    int count1 = 0, count2 = 0;
    
    process_dir(argv[1], files1, &count1);
    printf("В папке 1 найдено %d файлов\n", count1);
    
    process_dir(argv[2], files2, &count2);
    printf("В папке 2 найдено %d файлов\n", count2);
    
    FILE *result = fopen(argv[3], "w");
    if (result == NULL) {
        printf("Не могу создать файл %s\n", argv[3]);
        return 1;
    }
    
    printf("\nПоиск одинаковых файлов...\n");
    
    int found = 0;
    
    for (int i = 0; i < count1; i++) {
        for (int j = 0; j < count2; j++) {
            if (compare_files(files1[i], files2[j])) {
                found++;
                
                struct stat info;
                stat(files1[i], &info);
                
                printf("\n%d. %s\n", found, files1[i]);
                printf("   Размер: %ld байт\n", info.st_size);
                printf("   Права: %o\n", info.st_mode & 0777);
                printf("   Inode: %lu\n", info.st_ino);
                
                fprintf(result, "%d. %s\n", found, files1[i]);
                fprintf(result, "   Размер: %ld байт\n", info.st_size);
                fprintf(result, "   Права: %o\n", info.st_mode & 0777);
                fprintf(result, "   Inode: %lu\n\n", info.st_ino);
            }
        }
    }
    
    printf("\nНайдено одинаковых файлов: %d\n", found);
    fprintf(result, "Всего найдено: %d\n", found);
    
    fclose(result);
    return 0;
}
