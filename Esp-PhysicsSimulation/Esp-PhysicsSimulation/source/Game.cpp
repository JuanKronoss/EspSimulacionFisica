/**
 * Includes
 */

#include "Game.h"

#include <SFML/Graphics.hpp>

constexpr float FPS = 60.0f;
constexpr float FRAME_RATE = 1.0f / FPS;

Game::Game(const String& title, uint16 windowWidth, uint16 windowHeight, int16 posX, int16 posY)
{
  uint32 style = sf::Style::Titlebar | sf::Style::Close; // Create a window with a title bar and close button, but no resize option
  m_pWindow = make_shared<sf::RenderWindow>(sf::VideoMode({ windowWidth, windowHeight }), title, style);
  m_pWindow->setPosition({ posX, posY });
  initSystems();
  loadResources();
  subscribeToPlayerEvent();
  loadMods();
}