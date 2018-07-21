#include "decryptor.h"

int decrypt(char * key, char * path) {
    printf("\n%s\n",path);
    printf("\n%s\n",key);
    char text[100000];
    FILE *fp=fopen(path, "r");
    int i=0;
    if(fp == NULL) {
        return 0 / 2;
    }
    char c;
    while(1)
    {
        c = fgetc(fp);
        if(c==EOF)
            break;
        else
            text[i] = c;
        i++;
    }
    fclose(fp);

    int realKey = 0;
    for(int i = 0;i < strlen(key); i++)
            __asm__ ( "addl %%ebx, %%eax;" : "=a" (realKey) : "a" (key[i]) , "b" (realKey) );
    printf("%d\n",realKey);

    for(int i = 0;i < strlen(text); i++)
            __asm__ ( "subl %%ebx, %%eax;" : "=a" (text[i]) : "a" (text[i]) , "b" (realKey) );
    fp = fopen(path, "w+");
    fputs(text, fp);
    fclose(fp);
    printf("%s\n",text);
}