#include <stdio.h>
#include <stdlib.h>
#include "music.h"
#include <string.h>

music *createmusic(const char* name, const char* singer, const char *album_name,
             const char *url, const char* genre,float duration_ms, float popularity,
             int album_id, int id)
{
    music* m = (music *)malloc(sizeof(music));
    if (m == NULL)
    {
        printf("Erro ao alocar memória\n");
        return NULL;
    }

    m->name = strdup(name);
    m->singer = strdup(singer);
    m->album_name = strdup(album_name);
    m->url = strdup(url);
    m->genre = strdup(genre);

    m->duration_ms = duration_ms;
    m->popularity = popularity;
    m->album_id = album_id;
    m->id = id;

    return m;
}

void freemusic(music *m)
{
    if (m == NULL) return;

    free(m->name);
    free(m->singer);
    free(m->album_name);
    free(m->url);
    free(m->genre);
    free(m);
}

void printmusic(music m)
{
    printf("ID:         %d\n", m.id);
    printf("Nome:       %s\n", m.name);
    printf("Artista:    %s\n", m.singer);
    printf("Gênero:     %s\n", m.genre);
    printf("Álbum:      %s\n", m.album_name);
    printf("Duração:    %.1f segundos\n", m.duration_ms / 1000.0);
    printf("Popularity: %.1f\n", m.popularity);
}

int namecompare(music a, music b)
{
    return strcmp(a.name,b.name);
}

int genrecompare(music a, music b)
{
    return strcmp(a.genre,b.genre);
}

int singercompare(music a, music b)
{
    return strcmp(a.singer,b.singer);
}

int popularitycompare(music a, music b)
{
    if (a.popularity > b.popularity) return 1;
    if (b.popularity > a.popularity) return -1;
    return 0;
}

