#include <stdio.h>
#include "music.h"

int main()
{
    music *m = createmusic("Bohemian Rhapsody", "Queen", "A Night at the Opera",
                          "http://url.com", "Rock", 354000, 98.5, 101, 1);

    printmusic(*m);
    freemusic(m);

    return 0;
}