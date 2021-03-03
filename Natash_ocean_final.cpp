#include "Nat_ocean.h"
using namespace std;

int main()
{
    txCreateWindow(800, 600);
    COcean ocean;

    const int NUMBER_OF_STONES = 7;
    const int NUMBER_OF_GRASS  = 9;
    const int NUMBER_OF_FISHES = 20;

    for(int i = 0; i < NUMBER_OF_STONES; i++)
    {
        ocean.addObject(STONE, randInt(0, SCREEN_WIDTH), randInt(0, SCREEN_HEIGHT), randInt(20, 50));
    }

    for(int i = 0; i < NUMBER_OF_GRASS; i++)
    {
        ocean.addObject(GRASS, randInt(0, SCREEN_WIDTH), SCREEN_HEIGHT, 80);
    }

    for(int i = 0; i < NUMBER_OF_FISHES; i++)
    {
        ocean.addObject(FISH, randInt(0, SCREEN_WIDTH), randInt(0, SCREEN_HEIGHT), 20, randInt(-10, 10), randInt(2, 90));
    }

    ocean.addObject(HUNTER, 300, 300, 60, 10);

    while(!GetAsyncKeyState(VK_ESCAPE))
    {
        txSetFillColor(RGB(135, 232, 237));
        txClear();
        ocean.update();
        txSleep(50);
    }
    return 0;
}
