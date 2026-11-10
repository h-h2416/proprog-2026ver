#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_options(void);
int analyze_argment(int argc, char *argv[]);

int option_s_flag = 0;
int option_f_flag = 0;
int option_t_flag = 0;
int option_w_flag = 0;
char *prefix = NULL;
int top_n = 0;
char *specific_word = NULL;


int main(int argc, char *argv[]) {
    // 引数の解析
    char *filename = argv[1];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        perror("argument error:");
        return 1;
    }
    if (analyze_argment(argc, argv) == 1) {
        return 1;
    }
    

    


    return 0;
}

// オプションの表示
void print_options(void) {
    printf("Options:\n");
    printf(" %-15s %s\n", "-s", "Show statistics");
    printf(" %-15s %s\n", "-f <prefix>", "Find words starting with prefix");
    printf(" %-15s %s\n", "-t <n>", "Show top n most frequent words");
    printf(" %-15s %s\n", "-w <word>", "Search for specific word");
}

// コマンドで指定された引数の解析。解析成功したら 0、失敗したら 1 を返す。
/*
    int option_s_flag = 0;
    int option_f_flag = 0;
    int option_t_flag = 0;
    int option_w_flag = 0;
    char *prefix = NULL;
    int top_n = 0;
    char *specific_word = NULL;
    が更新される。
*/
int analyze_argment(int argc, char *argv[]) {
    // i = 0,1 は実行ファイル名と入力ファイル名なので、2から始める。
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            option_s_flag = 1;

        } else if (strcmp(argv[i], "-f") == 0) {
            option_f_flag = 1;
            if (i + 1 < argc) {
                prefix = argv[i + 1];
                i++;
            }

        } else if (strcmp(argv[i], "-t") == 0) {
            option_t_flag = 1;
            if (i + 1 < argc) {
                top_n = atoi(argv[i + 1]);
                i++;
            }
        
        } else if (strcmp(argv[i], "-w") == 0) {
            option_w_flag = 1;
            if (i + 1 < argc) {
                specific_word = argv[i + 1];
                i++;
            }
        
        } else {
            printf("Invalid option");
            print_options();
            return 1;
        }
    }
    return 0;
}
