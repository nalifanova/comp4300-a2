#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include "Entity.h"
#include "EntityManager.h"

constexpr float kPi = 3.1415f;

struct WindowConfig
{
    int width, height, framerate;
    bool fullscreen;
};

struct FontConfig
{
    std::string filepath;
    int size;
    int16_t red, green, blue;
};


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

struct SpecialWeapon
{
    int cooldown = 0;
    int lastUsed = 0;
    int delta = 0;
};

struct Control
{
    bool showMovement = true;
    bool showRendering = true;
    bool showCollisiton = true;
    bool showLifespan = true;
    bool spawnEnemies = true;
    bool isPaused = false;
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

    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

    void sMovement();
    void sLifespan();

    void sCollision();
    void sEnemySpawner();
    void sGUI();
    void sRender();
    void sUserInput();

    template <typename T>
    static T random(T min, T max);

private:
    void m_checkBoundaries(const std::shared_ptr<Entity>& entity) const;
    void m_checkCollision(const std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& enemy);
    static std::string m_getTextLine(const std::shared_ptr<Entity>& entity);
    static void m_getEntityInfo(const std::shared_ptr<Entity>& entity);
    void m_showFinalMessage();
    void m_restartGame();

    sf::RenderWindow m_window;
    EntityManager m_entities;
    sf::Font m_font;
    sf::Text m_text;
    sf::Clock m_deltaClock;

    WindowConfig m_windowConfig{};
    FontConfig m_fontConfig{};
    PlayerConfig m_playerConfig{};
    EnemyConfig m_enemyConfig{};
    BulletConfig m_bulletConfig{};
    Control m_control;

    int m_score = 0;
    int m_currentFrame = 0;
    bool m_running = true;
    int m_lastEnemySpawnTime = 0;
    std::shared_ptr<Entity> m_player;
    int m_liveCounter = 0;
    int m_totalLives = 5;

    SpecialWeapon m_sWeapon;
};

#endif //GAME_H
