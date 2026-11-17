#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>


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
    // 単語の出現位置の配列で現状何インデックス分の容量を保持しているか
    int capacity;
    struct hash_node *next;
} hash_node;

#define BUF_SIZE 1024 // テスト用に4にした。あとで1024に戻す
#define WORD_SIZE 128
#define HASH_TABLE_SIZE 1000000 // 1000000


void print_options(void);
int analyze_argment(int argc, char *argv[]);
uint32_t hash_calc(char *word);
void add_hash_node(hash_node *hash_table[], hash_node *tail_node, char *word, int word_position);
void add_word(hash_node *hash_table[], char *word, int word_position);
void print_s_flag(hash_node *hash_table[], int position_count);
void string_tolower(char *word);
void print_f_flag(hash_node *hash_table[]);
void print_t_flag(hash_node *hash_table[]);
void print_w_flag(hash_node *hash_table[]);
void free_hash_table(hash_node *hash_table[]);
void min_heap(hash_node *hash_table[], hash_node *heap_table[]);
void exchange_hash_node (hash_node **ptr1, hash_node **ptr2);
void heap_arrange(hash_node *heap_table[]);
void bubble_sort(hash_node *heap_table[]);


int main(int argc, char *argv[]) {
    // 引数の解析
    hash_node *hash_table[HASH_TABLE_SIZE] = {NULL};
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
    int position_count = 1;
    int word_position = 1;
    while((bytes_read = fread(buf, 1, BUF_SIZE, fp)) > 0) {
        // bufの処理
        int i = 0;
        while(i < bytes_read) {
            if(isalpha((unsigned char) buf[i])) {
                // printf("%d\n", position_count); // デバッグ用
                word[word_len++] = buf[i++];
                position_count++;
            }else {
                int end = word_len;
                while(i < bytes_read && !(isalpha((unsigned char) buf[i]))) {
                    i++;
                    position_count++;
                    word[end] = '\0';
                    word_len = 0;
                }
                if ((strcmp(word,"\0"))){
                // printf("%s\n",word); //デバッグ用
                    //printf("%d\n", position_count); // デバッグ用
                    // 取り出したwordの処理
                    //printf("%d\n", word_position); // デバッグ用
                    string_tolower(word);
                    add_word(hash_table, word, word_position);
                    word_position = position_count;
                } else{continue;}
            }
        }
        pre_bytes_read = bytes_read;
    }
    // EOFで終わった単語を処理するためにもう一回
    if ((strcmp(word,"\0")) && isalpha((unsigned char)buf[pre_bytes_read - 1])){
        // printf("%s\n",word); //デバッグ用
        //printf("%d\n", position_count); // デバッグ用
        // 取り出したwordの処理
        // printf("%d\n", word_position); // デバッグ用
        string_tolower(word);
        add_word(hash_table, word, word_position);
        word_position = position_count;
    }
    // オプション処理
    if (option_s_flag){
        print_s_flag(hash_table, position_count);
    }

    if (option_f_flag){
        print_f_flag(hash_table);
    }

    if (option_t_flag){
        print_t_flag(hash_table);
    }

    if (option_w_flag){
        print_w_flag(hash_table);
    }

    free_hash_table(hash_table);
    // デバッグ用。構造体の中身を表示
    # if 0
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            printf("word=%s, count=%d, positions=%p, capacity=%d\n",
            node->word != NULL ? node->word : "(null)",
            node->count,
            (void *)node->positions,
            node->capacity);

            node = node->next;
        }
    }
    # endif
    
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
    return hash % HASH_TABLE_SIZE;
}

// 追加の必要性が確定した前提
// 衝突してなければtail_nodeはNULL、衝突してるときはhash_tableは使わない
void add_hash_node(hash_node *hash_table[], hash_node *tail_node, char *word, int word_position) {
    hash_node *tmp_node = NULL;
    if (tail_node == NULL) {
        int hash_index = hash_calc(word);
        hash_table[hash_index] = malloc(sizeof(hash_node));
        if (hash_table[hash_index] == NULL) {
            perror("malloc");
        }
        tmp_node = hash_table[hash_index];
    } else {
        tail_node -> next = malloc(sizeof(hash_node));
        if (tail_node -> next == NULL) {
            perror("malloc");
        }
        tmp_node = tail_node -> next;
    
    }
    // 構造体の中身の更新
    tmp_node -> word = malloc((strlen(word) + 1) * sizeof(char));
    if( tmp_node -> word == NULL) perror("malloc");
    strcpy(tmp_node -> word, word);
    tmp_node -> count = 1;
    tmp_node -> positions = malloc(sizeof(int) * 8);
    if(tmp_node -> positions == NULL) perror("malloc");
    tmp_node -> positions[0] = word_position;
    tmp_node -> capacity = 7;
    tmp_node -> next = NULL;
}

// 得た単語の処理をする関数
void add_word(hash_node *hash_table[], char *word, int word_position) {
    uint32_t hash_index = hash_calc(word);
    // テーブルのインデックスがまだ埋まってなかった時
    if (hash_table[hash_index] == NULL) {
        add_hash_node(hash_table, NULL, word, word_position);
    // 埋まってた時
    } else {
        hash_node *aim_node = hash_table[hash_index];
        while(aim_node -> next != NULL && strcmp(word, aim_node -> word)) {
            aim_node = aim_node -> next;
        }
        // 次のノードがないかつ単語は該当しない
        if (aim_node -> next == NULL && strcmp(word, aim_node -> word)) {
            add_hash_node(hash_table, aim_node, word, word_position);
        }
        // 該当する単語があった
        else if (!strcmp(word, aim_node -> word)) {
            aim_node -> count++;
            // capacity 8n-1,count 8n+1でないとき
            if (aim_node -> capacity != aim_node -> count - 2) {
                aim_node -> positions[aim_node -> count - 1] = word_position; 
            } else {
                // count 8n+1のとき
                aim_node -> capacity += 8;
                int *tmp = realloc(aim_node -> positions, aim_node -> capacity * sizeof(int));
                if (tmp == NULL) perror("realloc");
                aim_node -> positions = tmp;
                aim_node -> positions[aim_node -> count - 1] = word_position; 

            }
        }
    }
}

void print_s_flag (hash_node *hash_table[], int position_count) {
    int total_words = 0;
    int unique_words = 0;
    int word_length_total = 0;
    int longest_word_length = 1;
    int shortest_word_length = INT_MAX;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            total_words += node -> count;
            unique_words ++;
            word_length_total += node -> count * strlen(node -> word);
            if (longest_word_length < strlen(node -> word)) {
                longest_word_length = strlen(node -> word);
            }
            if (shortest_word_length > strlen(node -> word)) {
                shortest_word_length= strlen(node -> word);
            }
            node = node -> next;
        }
    }
    printf("===TEXT STATISTICS===\n");
    printf("Total characters: %d\n", position_count - 1);
    printf("Total words: %d\n", total_words);
    printf("Unique words: %d\n", unique_words);
    printf("Average word length: %.2f\n", 
        (double)word_length_total / total_words);
    printf("Longest word: ");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            if(strlen(node -> word) == longest_word_length) {
                printf("%s ", node -> word);
            }
            node = node -> next;
        }
    }
    printf("(%d character%s)\n", 
        longest_word_length, longest_word_length == 1 ? "" : "s");

    printf("Shortest word: ");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            if(strlen(node -> word) == shortest_word_length) {
                printf("%s ", node -> word);
            }
            node = node -> next;
        }
    }
    printf("(%d character%s)\n", 
        shortest_word_length, shortest_word_length == 1 ? "" : "s");
}

void string_tolower(char *word) {
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = (char) tolower((unsigned char)word[i]);
    }
}

void print_f_flag(hash_node *hash_table[]) {
    printf("=== WORDS STARTING WITH \"%s\" ===\n", prefix);
    int match_count = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            if (strncmp(node -> word, prefix, strlen(prefix)) == 0) {
                match_count += node -> count;
            }
            node = node -> next;
        }
    }
    printf("Found %d matches:\n", match_count);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            if (strncmp(node -> word, prefix, strlen(prefix)) == 0) {
                printf("- \"%s\" (position ", node -> word);
                for (int j = 0; j < node -> count - 1; j++) {
                    printf("%d, ",node -> positions[j]);
                }
                printf("%d)\n", node -> positions[node -> count - 1]);
            }

            node = node -> next;
        }
    }
}

void print_t_flag(hash_node *hash_table[]) {
    printf("=== TOP %d WORDS ===\n", top_n);
    hash_node **heap_table = malloc(sizeof(hash_node*) * top_n);
    for(int i = 0; i < top_n; i++){
        heap_table[i] = NULL;
    }
    min_heap(hash_table, heap_table);
    bubble_sort(heap_table);
    int min_count = heap_table[0] -> count;
    int heap_len = 0;
    while(heap_table[heap_len] != NULL){
        heap_len++;
    }
    int ranking = 1;
    int index = heap_len - 1;
    while(index >= 0 && heap_table[index] -> count != min_count){
        int occurrence = heap_table[index] -> count;
        while(occurrence == heap_table[index] -> count){
            printf("%d.\"%s\"- %d occurrence%s\n",ranking, heap_table[index] -> word,
                heap_table[index] -> count, heap_table[index] -> count == 1? "":"s");
            index--;
        }
        ranking++;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            if(hash_table[i] -> count == min_count){
                printf("%d.\"%s\"- %d occurrence%s\n",ranking, hash_table[i] -> word,
                    hash_table[i] -> count, hash_table[i] -> count == 1? "":"s");
            }
            node = node -> next;
        }
    }
    free(heap_table);
}

void print_w_flag(hash_node *hash_table[]) {
    printf("===RESULT OF SEARCHING \"%s\"===\n", specific_word);
    int exist_flag = 0;
    int count = 0;
    hash_node *node;
    int i = 0;
    // 最後まで捜査し終えてないかつ単語が見つかっていない
    while(i < HASH_TABLE_SIZE) {
        node = hash_table[i];
        while(node != NULL) {
            if (strcmp(specific_word, node -> word) == 0) {
                exist_flag = 1;
                count = node -> count;
                break;
            }
            node = node -> next;
        }
        if (node != NULL && strcmp(specific_word, node -> word) == 0) break;
        i++;
    }
    if(exist_flag) {
        printf("Found %d word%s (position ", count, count == 1 ? "" : "s");
        for (int j = 0; j < node -> count - 1; j++) {
            printf("%d, ",node -> positions[j]);
        }
        printf("%d)\n", node -> positions[node -> count - 1]);
    } else {
        printf("Found %d word\n", count);
    }

}

void free_hash_table(hash_node *hash_table[]) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            free(node -> word);
            free(node -> positions);
            hash_node *tmp = node;
            node = node -> next;
            free(tmp);
        }
    }
}

// heap_tableの配列は確保されていてNULL前提、要素数top_n
void min_heap(hash_node *hash_table[], hash_node *heap_table[]){
    int heap_count = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_node *node = hash_table[i];
        while(node != NULL) {
            // heap_tableが埋まってない時
            if (heap_count < top_n) {
                heap_table[heap_count] = node;
                int index = heap_count;
                // 並べ替え
                while(index != 0 && heap_table[index] -> count < heap_table[(index-1)/2] -> count) {
                    exchange_hash_node(&heap_table[index], &heap_table[(index-1)/2]);
                    index = (index-1)/2;
                }
                heap_count++;
            // heap_tableが埋まってる時
            } else {
                if (node -> count > heap_table[0] -> count){
                    heap_table[0] = node;
                    // 並べ替え
                    heap_arrange(heap_table);
                }
            }
            node = node -> next;
        }
    }
}

void exchange_hash_node (hash_node **ptr1, hash_node **ptr2) {
    hash_node *tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
}

void heap_arrange(hash_node *heap_table[]){
    int index = 0;
    int change_flag = 1;
    
    while(2 * index + 1 < top_n) {                       
            // 左側の子のほうが小さい時
        if(2 * index + 2 < top_n
            && heap_table[2 * index + 1] -> count <= heap_table[2 * index + 2] -> count
            && heap_table[index] -> count > heap_table[2 * index + 1] -> count){
            exchange_hash_node(&heap_table[index], &heap_table[2*index+1]);
        
        // 右側の子のほうが小さい時 
        }else if(2 * index + 2 < top_n
            && heap_table[2 * index + 1] -> count > heap_table[2 * index + 2] -> count
            && heap_table[index] -> count > heap_table[2 * index + 2] -> count){
            exchange_hash_node(&heap_table[index], &heap_table[2*index+2]);
        // 2 * index + 2の要素がない時
        }else if(heap_table[index] -> count > heap_table[2 * index + 1] -> count) {
            exchange_hash_node(&heap_table[index], &heap_table[2*index+1]);
        // 入れ替え終わりの判定
        }else {
            change_flag = 0;
        }
        if(change_flag == 0){
            break;
        }
        index = 2 * index + 1;
    }
}

void bubble_sort(hash_node *heap_table[]) {
    int heap_len = 0;
    while(heap_table[heap_len] != NULL){
        heap_len++;
    }
    for(int i = 0; i < heap_len - 1; i++) {
        for (int j = heap_len - 1; j >= i + 1; j--) {
            if (heap_table[j] -> count < heap_table[j - 1] -> count) {
                exchange_hash_node(&heap_table[j],&heap_table[j-1]);
            }
        }
    }
}
