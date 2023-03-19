#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void **createTable(int tableSize){
	return calloc(sizeof(void*),tableSize);
}

int checkTheLength(int number){
    int counter = 0;
    while(number > 0){
        counter+=1;
        number/=10;
    }
    return counter;
}

char *readFromTempFile(char *tmp_file_name, int file_size, int name_size){

    char* tmp_path = calloc(100,sizeof(char));
    for (int i = 0; i < name_size; i++) {
        tmp_path[i] = tmp_file_name[i];
    }

    FILE *tmp_file;
    tmp_file = fopen(tmp_path, "r");

    if (tmp_file == NULL)
    {
        printf("There is a problem with opening temporary file\n");
        exit(EXIT_FAILURE);
    }

    char curr_char;

    char *data = calloc(file_size,sizeof(char));
    int iterator = 0;

    while ((curr_char = fgetc(tmp_file)) != EOF){
        data[iterator] = curr_char;
        iterator++;
    }

    free(tmp_path);
    return data;
}

void fileStats(char* filePath, int currPathLength){

    FILE *current_file;
    char new_path[100];

    for (int i = 0; i < currPathLength; i++) {
        new_path[i] = filePath[i];
    }

    current_file = fopen(new_path, "r");

    if (current_file == NULL)
    {
        printf("There is a problem with opening file\n");
        exit(EXIT_FAILURE);
    }

    char curr_char;
    int characters, words, lines;
    characters = words = lines = 0;

    while ((curr_char = fgetc(current_file)) != EOF)
    {
        characters++;

        if (curr_char == '\n' || curr_char == '\0')
            lines++;

        if (curr_char == ' ' || curr_char == '\n' || curr_char == '\0')
            words++;
    }

    if (characters > 0)
    {
        words++;
        lines++;
    }

    printf("\n Number of chars: %d",characters);
    printf("\n Number of words: %d",words);
    printf("\n Number of lines: %d",lines);
    printf("\n");

    int chars_len = checkTheLength(characters), words_len = checkTheLength(words), lines_len = checkTheLength(lines);
    int summary_len = chars_len + words_len + lines_len;
    int table_len = summary_len+6;
    //printf("%d\n", table_len);

    char* filestats = calloc(table_len, sizeof(char));
    int index = 0;

    filestats[index] = 'c';
    filestats[index+1] = ':';
    index+=2;
    int op_chars = characters;
    int op_chars_len = chars_len;
    while(op_chars>0){
        filestats[index+op_chars_len-1] = op_chars%10+'0';
        op_chars/=10;
        op_chars_len--;
    }
    index+=chars_len;

    filestats[index] = 'w';
    filestats[index+1] = ':';
    index+=2;
    int op_words = words;
    int op_words_len = words_len;
    while(op_words>0){
        filestats[index+op_words_len-1] = op_words%10+'0';
        op_words/=10;
        op_words_len--;
    }
    index += words_len;

    filestats[index] = 'l';
    filestats[index+1] = ':';
    index+=2;
    int op_lines = lines;
    int op_lines_len = lines_len;
    while(op_lines>0){
        filestats[index+op_lines_len-1] = op_lines%10+'0';
        op_lines/=10;
        op_lines_len--;
    }


    int newFile;
    char file_name[] = "./fileXXXXXX";
    newFile = mkstemp(file_name);

    int saved;
    saved = write(newFile,filestats,table_len);

    if(saved == table_len) {

    }else {
        printf("There was problem with write function [Error code = %d]\n",saved);
    }

    char *tableLocation = readFromTempFile(file_name,table_len,12);

    for(int i = 0; i < table_len; i++){
        printf("%c",tableLocation[i]);
    }
    printf("\n");

    free(filestats);

}

void freeMemory(void *index){
    free(index);
}

