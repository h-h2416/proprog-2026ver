#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

void print_options(void);
int analyze_argment(int argc, char *argv[]);
uint32_t hash_calc(char *word);

int option_s_flag = 0;
int option_f_flag = 0;
int option_t_flag = 0;
int option_w_flag = 0;
char *prefix = NULL;
int top_n = 0;
char *specific_word = NULL;

// 1つのユニークな単語に対して1ノード
typedef struct hash_node {
    char *word;
    // 単語の出現回数
    int count;
    // 単語の出現位置の配列へのポインタ
    int *positions;
    // 単語の出現位置の配列で現状何箇所分の容量を保持しているか
    int capacity;
    struct hash_node *next;
} hash_node;

#define BUF_SIZE 1024 // テスト用に4にした。あとで1024に戻す
#define WORD_SIZE 128


int main(int argc, char *argv[]) {
    // 引数の解析
    char *filename = argv[1];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        perror("argument error");
        return 1;
    }
    if (analyze_argment(argc, argv) == 1) {
        return 1;
    }
    // ファイルの中身の処理
    int bytes_read = 0;
    char buf[BUF_SIZE];
    char word[WORD_SIZE];
    int word_len = 0;
    int pre_bytes_read = 0;
    while((bytes_read = fread(buf, 1, BUF_SIZE, fp)) > 0) {
        // bufの処理
        int i = 0;
        while(i < bytes_read) {
            if(isalpha((unsigned char) buf[i])) {
                word[word_len++] = buf[i++];
            }else {
                int end = word_len;
                while(i < bytes_read && !(isalpha((unsigned char) buf[i]))) {
                    i++;
                    word[end] = '\0';
                    word_len = 0;
                }
                if ((strcmp(word,"\0"))){
                    printf("%s\n",word); //デバッグ用
                    // 取り出したwordの処理

                } else{continue;}
            }
        }
        pre_bytes_read = bytes_read;
    }
    // EOFで終わった単語を処理するためにもう一回
    if ((strcmp(word,"\0")) && isalpha((unsigned char)buf[pre_bytes_read - 1])){
        // printf("%s\n",word); //デバッグ用
        // 取り出したwordの処理

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

// hash値を計算する関数
uint32_t hash_calc(char *word) {
    uint32_t hash = 5381;
    while(*word != '\0') {
        hash = hash * 33u + (unsigned char) *word;
        word++;
    }
    return hash % 1000000;
}