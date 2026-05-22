#ifndef MUSIC_H
#define MUSIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    char *name;
    char *singer;
    char *album_name;
    char *url;
    char *genre;
    float duration_ms;
    float popularity;
    int album_id;
    int id;
} music ;

void printmusic(music m);

music *createmusic(const char* name, const char* singer, const char *album_name,
             const char *url, const char* genre,float duration_ms, float popularity,
             int album_id, int id);

void freemusic(music *m);
int namecompare(music a, music b);
int genrecompare(music a, music b);
int singercompare(music a, music b);
int popularitycompare(music a, music b);

#endif