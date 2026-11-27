/**
 * Includes
 */

#include "SceneManager.h"
#include "Scene.h"

void
SceneManager::createScene(const String& name)
{
  // Avoid creating a scene with the same name as an existing scene
  auto it = std::find_if(m_pScenes.begin(), m_pScenes.end(),
                         [&name](const SPtr<Scene>& scene) {
                           return scene->getName() == name;
    });
  if (it != m_pScenes.end()) {
    throw std::invalid_argument("Cannot create scene: a scene with the name '" + name + "' already exists.");
  }

  SPtr<Scene> pScene = make_shared<Scene>(name);
  m_pScenes.push_back(pScene);
  if (!m_pActiveScene) {
    m_pActiveScene = pScene; // Set the first created scene as the active scene by default
  }
}

void
SceneManager::createSceneFromFile(const String& name, const String& _dataFile)
{
  // Avoid creating a scene with the same name as an existing scene
  auto it = std::find_if(m_pScenes.begin(), m_pScenes.end(),
    [&name](const SPtr<Scene>& scene) {
      return scene->getName() == name;
    });
  if (it != m_pScenes.end()) {
    throw std::invalid_argument("Cannot create scene: a scene with the name '" + name + "' already exists.");
  }

  // Avoid creating a scene with the same data file as an existing scene
  auto itFile = std::find_if(m_pScenes.begin(), m_pScenes.end(),
    [&_dataFile](const SPtr<Scene>& scene) {
      return scene->getDataPath() == _dataFile;
    });
  if (itFile != m_pScenes.end()) {
    throw std::invalid_argument("Cannot create scene: a scene with the data file '" + _dataFile + "' already exists.");
  }

  SPtr<Scene> pScene = make_shared<Scene>(name, _dataFile);
  m_pScenes.push_back(pScene);
  if (!m_pActiveScene) {
    m_pActiveScene = pScene; // Set the first created scene as the active scene by default
  }
}

void
SceneManager::addScene(const WPtr<Scene>& wpScene, const bool makeActive)
{
  if (wpScene.expired()) {
    throw std::invalid_argument("Cannot add scene: the provided scene pointer is expired.");
  }
  SPtr<Scene> pScene = wpScene.lock();
  if (find(m_pScenes.begin(), m_pScenes.end(), pScene) != m_pScenes.end()) {
    throw std::invalid_argument("Cannot add scene: the provided scene is already managed by this SceneManager.");
  }

  m_pScenes.push_back(pScene);
  if (!m_pActiveScene || makeActive) {
    m_pActiveScene = pScene; // Set the first added scene as the active scene by default
  }
}

void
SceneManager::setActiveScene(const WPtr<Scene>& scene, const bool reload)
{
  if (scene.expired()) {
    throw std::invalid_argument("Cannot set active scene: the provided scene pointer is expired.");
  }
  SPtr<Scene> spScene = scene.lock();
  auto it = std::find(m_pScenes.begin(), m_pScenes.end(), spScene);
  if (it == m_pScenes.end()) {
    throw std::invalid_argument("Cannot set active scene: the provided scene is not managed by this SceneManager.");
  }
  m_pActiveScene = spScene;
  if (reload) {
    m_pActiveScene->reload(); // Reload the active scene to reset its state when switching to it
  }
}

void
SceneManager::setActiveScene(const String& _dataFile, const bool reload)
{
  auto it = std::find_if(m_pScenes.begin(), m_pScenes.end(),
                         [&_dataFile](const SPtr<Scene>& scene) {
                           return scene->getDataPath() == _dataFile;
                         });
  if (it == m_pScenes.end()) {
    throw std::invalid_argument("Cannot set active scene: no scene with the specified data file was found.");
  }
  m_pActiveScene = *it;
  if (reload) {
    m_pActiveScene->reload(); // Reload the active scene to reset its state when switching to it
  }
}
