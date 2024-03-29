#include "Game.h"
#include <fstream>
#include <iostream>

Game::Game(const std::string& config)
{
  init(config);
}

void Game::init(const std::string& path)
{
  // TODO: read in config file here
  std::ifstream fin(path);
  std::string first;
  int wWidth;
  int wHeight;
  int framerateLimit;
  bool fullscreen;

  std::string fontPath;
  int fontSize;
  int fontR;
  int fontG;
  int fontB;

  while (fin >> first)
  {
    if (first == "Window")
    {
      fin >> wWidth >> wHeight >> framerateLimit >> fullscreen;
      continue;
    }

    if (first == "Font")
    {
      fin >> fontPath >> fontSize >> fontR >> fontG >> fontB;
      continue;
    }

    if (first == "Player")
    {
      fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
      continue;
    }

    if (first == "Enemy")
    {
      fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
      continue;
    }

    if (first == "Bullet")
    {
      fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
      continue;
    }
  }

  if (fullscreen)
  {
    m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2", sf::Style::Fullscreen);
  }
  else {
    m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2");
  }
  m_window.setFramerateLimit(framerateLimit);

  if (!m_font.loadFromFile(fontPath))
  {
    std::cerr << "Could not load font!\n";
    exit(-1);
  }

  m_text.setFont(m_font);
  m_text.setCharacterSize(fontSize);
  m_text.setPosition(0, (float)m_text.getCharacterSize());
  spawnPlayer();
}

void Game::run()
{
  // TODO: Add pause functionality in here
  while (m_running)
  {
    std::cout << "Update" << "\n";
    m_entities.update();
    std::cout << "Render" << "\n";
    sRender();
    std::cout << "User input" << "\n";
    sUserInput();
    if (!m_paused)
    {
      std::cout << "Enemy Spawned" << "\n";
      sEnemySpawner();
      std::cout << "Movement" << "\n";
      sMovement();
      std::cout << "Collision" << "\n";
      sCollision();
      std::cout << "Lifespan" << "\n";
      sLifespan();
    }

    m_currentFrame++;
  }

}

void Game::setPaused(bool paused)
{
  m_paused = paused;
}

void Game::spawnPlayer()
{
  auto entity = m_entities.addEntity("player");

  float mx = m_window.getSize().x / 2.0f;
  float my = m_window.getSize().y / 2.0f;

  entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(1.0f, 1.0f), 0.0f);
  entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
  entity->cInput = std::make_shared<CInput>();

  m_player = entity;
}

void Game::spawnEnemy()
{
  auto entity = m_entities.addEntity("enemy");
  float ex = rand() % m_window.getSize().x;
  float ey = rand() % m_window.getSize().y;
  int red = rand() % 256;
  int green = rand() % 256;
  int blue = rand() % 256;
  int vertices = m_enemyConfig.VMIN + rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1);
  float speed = m_enemyConfig.SMIN + ((float)rand() / (RAND_MAX / (m_enemyConfig.SMAX - m_enemyConfig.SMIN)));
  float vx = (float)rand() / RAND_MAX;
  float vy = (float)rand() / RAND_MAX;
  Vec2 velocity = { vx, vy };
  velocity.normalize();
  velocity *= speed;
  entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), velocity, 0.0f);
  entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, sf::Color(red, green, blue), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
  entity->cScore = std::make_shared<CScore>(vertices * 100);
  m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
  auto bullet = m_entities.addEntity("bullet");
  Vec2 velocity = target - entity->cTransform->pos;
  velocity.normalize();
  velocity *= m_bulletConfig.S;
  std::cout << "Speed " << velocity.x << " " << velocity.y << "\n";
  bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0);
  bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
  bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
  // std::cout << bullet->cLifespan->remaining << "\n";
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{

}

void Game::sMovement()
{
  m_player->cTransform->velocity = { 0, 0 };

  if (m_player->cInput->up && m_player->cTransform->pos.y > m_playerConfig.CR)
  {
    m_player->cTransform->velocity.y = -1;
  }
  if (m_player->cInput->down && m_player->cTransform->pos.y < m_window.getSize().y - m_playerConfig.CR)
  {
    m_player->cTransform->velocity.y = 1;
  }
  if (m_player->cInput->left && m_player->cTransform->pos.x > m_playerConfig.CR)
  {
    m_player->cTransform->velocity.x = -1;
  }
  if (m_player->cInput->right && m_player->cTransform->pos.x < m_window.getSize().x - m_playerConfig.CR)
  {
    m_player->cTransform->velocity.x = 1;
  }

  m_player->cTransform->velocity.normalize();
  m_player->cTransform->velocity *= m_playerConfig.S;

  // m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
  // m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
  for (auto& e : m_entities.getEntities()) {
    if (e->cTransform) {
      if (e->tag() == "enemy") {
        if (e->cTransform->pos.y <= m_enemyConfig.CR || e->cTransform->pos.y >= m_window.getSize().y - m_enemyConfig.CR) {
          e->cTransform->velocity.y *= -1;
        }

        if (e->cTransform->pos.x <= m_enemyConfig.CR || e->cTransform->pos.x >= m_window.getSize().x - m_enemyConfig.CR) {
          e->cTransform->velocity.x *= -1;
        }
      }
      e->cTransform->pos.x += e->cTransform->velocity.x;
      e->cTransform->pos.y += e->cTransform->velocity.y;
    }
  }
}

void Game::sLifespan()
{
  for (auto& e : m_entities.getEntities()) {
    if (e->cLifespan) {
      if (e->cLifespan->remaining < 0) {
        e->destroy();
      }
      else {
        float alpha = (float)e->cLifespan->remaining / (float)e->cLifespan->total * 255.0f;
        e->cShape->circle.setFillColor(sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB, alpha));
        e->cLifespan->remaining--;
      }
    }
  }
}

void Game::sCollision()
{
  for (auto b : m_entities.getEntities("bullet"))
  {
    for (auto e : m_entities.getEntities("enemy"))
    {
      float distance = (b->cTransform->pos - e->cTransform->pos).length();
      if (distance < m_bulletConfig.CR + m_enemyConfig.CR) {
        m_score += e->cScore->score;
        e->destroy();
        b->destroy();
      }
    }
  }
}

void Game::sEnemySpawner()
{
  if (m_currentFrame - m_lastEnemySpawnTime >= 60) {
    spawnEnemy();
  }
}

void Game::sRender()
{
  m_window.clear();

  for (auto e : m_entities.getEntities())
  {
    e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
    e->cTransform->angle += 1.0f;
    e->cShape->circle.setRotation(e->cTransform->angle);
    m_window.draw(e->cShape->circle);
  }
  m_text.setString("Score: " + std::to_string(m_score));
  m_window.draw(m_text);
  m_window.display();
}

void Game::sUserInput()
{
  sf::Event event;
  while (m_window.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
    {
      m_running = false;
    }

    if (event.type == sf::Event::KeyPressed)
    {
      switch (event.key.code)
      {
      case sf::Keyboard::W:
        std::cout << "W Key Pressed\n";
        m_player->cInput->up = true;
        break;
      case sf::Keyboard::S:
        std::cout << "S Key Pressed\n";
        m_player->cInput->down = true;
        break;
      case sf::Keyboard::A:
        std::cout << "A Key Pressed\n";
        m_player->cInput->left = true;
        break;
      case sf::Keyboard::D:
        std::cout << "D Key Pressed\n";
        m_player->cInput->right = true;
        break;
      case sf::Keyboard::P:
        std::cout << "P Key Pressed\n";
        if (!m_paused) setPaused(true);
        else setPaused(false);
        break;
      default:
        break;
      }
    }

    if (event.type == sf::Event::KeyReleased)
    {
      switch (event.key.code)
      {
      case sf::Keyboard::W:
        std::cout << "W Key Released\n";
        m_player->cInput->up = false;
        break;
      case sf::Keyboard::S:
        std::cout << "S Key Released\n";
        m_player->cInput->down = false;
        break;
      case sf::Keyboard::A:
        std::cout << "A Key Released\n";
        m_player->cInput->left = false;
        break;
      case sf::Keyboard::D:
        std::cout << "D Key Released\n";
        m_player->cInput->right = false;
        break;
      default:
        break;
      }
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
      if (event.mouseButton.button == sf::Mouse::Left)
      {
        std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
        //call spawnBullet
        spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
      }

      if (event.mouseButton.button == sf::Mouse::Right)
      {
        std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
        //call spawnSpecialWeapon
      }
    }


  }
}