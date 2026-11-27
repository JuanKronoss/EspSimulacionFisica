/**
 * Includes
 */

#include "Game.h"

#include "ResourceManager.h"
#include "PhysicsManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Actor.h"
#include "SpriteRendererComponent.h"
#include "BoxColliderComponent.h"

constexpr float FPS = 60.0f;
constexpr float FRAME_RATE = 1.0f / FPS;

Game::Game(const String& title, uint16 windowWidth, uint16 windowHeight, int16 posX, int16 posY)
{
  uint32 style = sf::Style::Titlebar | sf::Style::Close; // Create a window with a title bar and close button, but no resize option
  m_pWindow = make_shared<sf::RenderWindow>(sf::VideoMode({ windowWidth, windowHeight }), title, style);
  //m_pWindow->setPosition({ posX, posY });
  initSystems();
  loadResources();
}

void
Game::initSystems()
{
  float windowWidth = static_cast<float>(m_pWindow->getSize().x);
  float windowHeight = static_cast<float>(m_pWindow->getSize().y);

  ResourceManager::startUp(); // Start up the ResourceManager module to manage game resources such as textures and sounds
  PhysicsManager::startUp(); // Start up the PhysicsManager module to handle physics calculations and collision detection
  SceneManager::startUp(); // Start up the SceneManager module to manage game scenes and transitions
}

void
Game::shutDownSystems()
{
  ResourceManager::shutDown(); // Shut down the ResourceManager module and free any resources it is using
  PhysicsManager::shutDown(); // Shut down the PhysicsManager module and free any resources it is using
  SceneManager::shutDown(); // Shut down the SceneManager module and free any resources it is using
}

void
Game::loadResources()
{
  SceneManager& sceneManager = SceneManager::instance();
  sceneManager.createScene("TestScene");
}

void
Game::run()
{
  sf::Clock clock;
  SceneManager& sceneMan = SceneManager::instance();
  PhysicsManager& physicsMan = PhysicsManager::instance();
  while (m_pWindow->isOpen()) {

    sf::Time elapsed = clock.restart();
    float deltaTime = elapsed.asSeconds();

    handleEventsAndInput();

    m_pWindow->clear();

    if (!m_isPaused) {
      // Normal Update
      updateScene(*sceneMan.getActiveScene(), deltaTime);
      physicsMan.handleCollisions(sceneMan.getActiveScene()->getActors());

      // Fixed Update
      static float accumulator = 0.0f;
      accumulator += deltaTime;
      while (accumulator >= FRAME_RATE) {
        updateScene(*sceneMan.getActiveScene(), FRAME_RATE);
        physicsMan.handleCollisions(sceneMan.getActiveScene()->getActors());
        accumulator -= FRAME_RATE;
      }
    }

    renderScene(*sceneMan.getActiveScene());
    renderUI();
    sceneMan.getActiveScene()->destroyMarkedActors(); // Destroy any actors that were marked for destruction during the update or collision handling
    m_pWindow->display();

  }
}

void
Game::handleEventsAndInput()
{
  SceneManager& sceneMan = SceneManager::instance();
  while (const Optional<sf::Event> event = m_pWindow->pollEvent()) {

    if (event->is<sf::Event::Closed>()) {
      m_pWindow->close();
    }
    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

      if (keyPressed->code == sf::Keyboard::Key::Escape) {
        m_pWindow->close(); // Close the window when 'Escape' is pressed
      }

      if (keyPressed->scancode == sf::Keyboard::Scancode::P) {
          m_isPaused = !m_isPaused; // Toggle pause state when 'P' is pressed
      }

    }

  }
}

void
Game::updateScene(const Scene& scene, const float deltaTime)
{
  for (const auto& actor : scene.getActors()) {
    actor->update(deltaTime);
  }
}

void
Game::fixedUpdateScene(const Scene& scene, const float fixedDeltaTime)
{
  for (const auto& actor : scene.getActors()) {
    actor->fixedUpdate(fixedDeltaTime);
  }
}

void
Game::renderScene(const Scene& scene)
{
  auto actorsInDrawingOrder = scene.getActorsInDrawingOrder();
  for (const auto& actor : actorsInDrawingOrder) {
    WPtr<SpriteRendererComponent> spriteRenderer = actor->getComponent<SpriteRendererComponent>();
    if (!spriteRenderer.expired()) {
      m_pWindow->draw(spriteRenderer.lock()->getSprite());
    }

    //Debug rendering of box colliders
    if (m_isDebugMode) {
      WPtr<BoxColliderComponent> boxCollider = actor->getComponent<BoxColliderComponent>();
      if (!boxCollider.expired()) {
        auto pBoxCollider = boxCollider.lock();
        if (!pBoxCollider->isActive()) {
          continue; // Skip rendering inactive box colliders
        }
        sf::FloatRect bounds = pBoxCollider->getBounds();
        sf::RectangleShape debugShape(sf::Vector2f(bounds.size.x, bounds.size.y));
        debugShape.setPosition(sf::Vector2f(bounds.position.x, bounds.position.y));
        debugShape.setFillColor(sf::Color::Transparent);
        debugShape.setOutlineColor(sf::Color::Red);
        debugShape.setOutlineThickness(1.0f);
        m_pWindow->draw(debugShape);
      }
    }
  }
}

