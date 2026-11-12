# 方針
## 出力
### help
Options:
 -s Show statistics
 -f \<prefix> Find words starting with prefix
 -t \<n> Show top n most frequent words
 -w \<word> Search for specific word

### -s
=== TEXT STATISTICS ===
 Total characters: 52
 Total words: 9
 Unique words: 8
 Average word length: 4.56
  Longest word: "programming" (11 characters)
 Shortest word: "a" (1 character)

 Longest,Shortestは全列挙
 Total charactersは区切り文字も含む

 ### -f \<prefix>
 === WORDS STARTING WITH "pro" ===
Found 4 matches:
 \- "programming" (position 1)
 \- "problems" (position 12)
 \- "provide" (position 21)
 \- "programmers" (position 41)

 positionは全列挙

### -t \<n>
=== TOP 3 WORDS ===
1. "the" - 4 occurrences
2. "cat" - 2 occurrences
2. "sat" - 2 occurrences
3. "sit" - 1 occurrence

### -w \<word>
=== result of searching "you" ===
Found 2 words
  position 1,7

## 一時メモ
ノードを追加する関数
ノードを追加するかするかどうか検討するのはmain
ノードにpositionの追加が必要か判断する関数

## 構造体
typedef struct hash_node {
    char *word;
    // 単語の出現回数
    int count;
    // 単語の出現位置の配列へのポインタ
    int *positions;
    // 単語の出現位置の配列で現状何出現分の容量を保持しているか
    int capacity;
    struct hash_node *next;
} hash_node;