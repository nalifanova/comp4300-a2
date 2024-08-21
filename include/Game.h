#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S;
};

struct EnemyConfig
{
    int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};

struct BulletConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};

class Game
{

public:
    Game() = default;
    explicit Game(const std::string& config);
    ~Game() = default;

    void init(const std::string& path);
    void run();
    void setPaused(bool paused);

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);

    void smawnBullet(std::shared_ptr<Entity> entity, const Vec2& target);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

    void sMovement();
    void sLifespan();

    void sCollision();
    void sEnemySpawner();
    void sGUI();
    void sRender();
    void sUserInput();

private:
    void m_checkBoundaries();
    void m_getCollapsingHeaders();
    static std::string m_getTextLine(const std::shared_ptr<Entity>& entity);
    static void m_getEntityInfo(const std::shared_ptr<Entity>& entity);
    [[nodiscard]] static int intRand(int min, int max);

    sf::RenderWindow m_window; // the window we will draw to
    EntityManager m_entities; // vector of entities to maintain
    sf::Font m_font; // the font we will use to draw
    sf::Text m_text; // the score text to be drawn to the screen
    sf::Clock m_deltaClock;

    PlayerConfig m_playerConfig{};
    EnemyConfig m_enemyConfig{};
    BulletConfig m_bulletConfig{};

    int m_score = 0;
    int m_currentFrame = 0; /// ??????
    bool m_running = true;
    bool m_paused = false;
    int m_lastEnemySpawnTime = 0;
    std::shared_ptr<Entity> m_player;
};

#endif //GAME_H
