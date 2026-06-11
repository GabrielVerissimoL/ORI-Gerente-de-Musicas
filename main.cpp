#include <iostream>
#include "music.hpp"

int main()
{
    music m1("Amargurado", "Tião Carreiro e Pardinho", "Sucessos", "url", "Sertanejo Raiz", 180000, 90.0, 3, 103);

    m1.print();

    return 0;
}