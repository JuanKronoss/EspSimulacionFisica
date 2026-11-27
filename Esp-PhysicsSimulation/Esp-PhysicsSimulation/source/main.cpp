#include <SFML/Graphics.hpp>
#include "Game.h"

int32
main()
{
  Game myGame("PhysSim", 1280, 720);
  myGame.run();

  return 0;
}