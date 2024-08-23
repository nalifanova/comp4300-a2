#include "Game.h"

#include <fstream>
#include <random>

#include "FileUtils.h"

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& path)
{
    const auto filePath = getFullPath(path);
    std::cout << "Contig file: " << filePath << "\n";
    std::ifstream fin(filePath);
    std::string token = "none";

    while (fin >> token)
    {
        // Window W H FL FS
        if (token == "Window")
        {
            fin >> m_windowConfig.width >> m_windowConfig.height >> m_windowConfig.framerate
                >> m_windowConfig.fullscreen;
            m_sWeapon.cooldown = m_windowConfig.framerate * 5; // 5 sec
        }
        // Font F S R G B
        else if (token == "Font")
        {
            fin >> m_fontConfig.filepath >> m_fontConfig.size
                >> m_fontConfig.red >> m_fontConfig.green >> m_fontConfig.blue;
        }
        // Player SR CR S FR FG FB OR OG OB OT V
        else if (token == "Player")
        {
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S
                >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
                >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
                >> m_playerConfig.OT >> m_playerConfig.V;
        }
        // Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SP
        else if (token == "Enemy")
        {
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
                >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT
                >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
        }
        // Bullet SR CR S FR FG FB OR OG OB OT V L
        else if (token == "Bullet")
        {
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S
                >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
                >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB
                >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }

    // set up default window parameters
    m_window.create(sf::VideoMode(m_windowConfig.width, m_windowConfig.height), "Assigment 02",
                    m_windowConfig.fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    m_window.setFramerateLimit(m_windowConfig.framerate);

    // set up font
    loadFont(m_font, m_fontConfig.filepath);

    m_text.setFont(m_font);
    m_text.setCharacterSize(m_fontConfig.size);
    m_text.setFillColor(sf::Color(m_fontConfig.red, m_fontConfig.green, m_fontConfig.blue));

    std::ignore = ImGui::SFML::Init(m_window);

    spawnPlayer();
    // init random seed
    srand(static_cast<int>(time(nullptr)));
}

void Game::run()
{
    while (m_running)
    {
        if (!m_control.isPaused && m_liveCounter < m_totalLives)
        {
            m_entities.update();
            if (m_control.spawnEnemies)
                sEnemySpawner();
            if (m_control.showLifespan)
                sLifespan();
            if (m_control.showMovement)
                sMovement();
            if (m_control.showCollisiton)
                sCollision();
            m_currentFrame++;
            m_sWeapon.delta = m_currentFrame - m_sWeapon.lastUsed;
        }

        // required update call to imgui
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        sUserInput();
        sGUI();
        sRender();
    }
}

void Game::setPaused(const bool paused)
{
    m_control.isPaused = paused;
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    //  --------- transform ---------
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(static_cast<float>(m_window.getSize().x) / 2.f, static_cast<float>(m_window.getSize().y) / 2.f),
        Vec2(0.0f, 0.0f),
        0.0f);

    // --------- shape ---------
    auto fillColor = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
    auto outlineColor = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);

    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
                                              fillColor, outlineColor, m_playerConfig.OT);
    // --------- collisiton ---------
    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
    m_liveCounter += 1;
}

void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");

    // --------- transform ---------
    const float x = random(m_enemyConfig.CR, m_windowConfig.width - m_enemyConfig.CR - 1);
    const float y = random(m_enemyConfig.CR, m_windowConfig.height - m_enemyConfig.CR - 1);
    const float angle = random(0.0f, kPi * 2.0f); // 2 * pi = 180'

    Vec2 velocity(cos(angle), sin(angle));
    velocity *= random(m_enemyConfig.SMIN, m_enemyConfig.SMAX);

    entity->cTransform = std::make_shared<CTransform>(Vec2(static_cast<float>(x), static_cast<float>(y))
                                                    , velocity, angle);
    // --------- shape ---------
    int vertices = random(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
    auto fillColor = sf::Color(random(0, 255), random(0, 255), random(0, 255));
    auto outlineColor = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, fillColor, outlineColor,
                                              m_enemyConfig.OT);
    // --------- collision ---------
    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

    // --------- score ---------
    entity->cScore = std::make_shared<CScore>(vertices * 100);

    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity entity) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
    auto vertices = entity->cShape->circle.getPointCount();

    for (size_t i = 0; i < vertices; i++)
    {
        auto small = m_entities.addEntity("small");
        /*
         * 1 radian = 180°/π = 57° 16′ (approx)
         * we have      θ = 360° / 6  = 60°
         *              60° * π / 180°
         * Vec2 velocity(Speed * cos(θ), Speed * sin(θ));
         */
        const float angle = 360.0f / vertices * i;
        const float radian = angle * kPi / 180;

        Vec2 position = entity->cTransform->pos;
        Vec2 velocity(cos(radian), sin(radian));
        velocity *= random(m_enemyConfig.SMIN, m_enemyConfig.SMAX);

        small->cTransform = std::make_shared<CTransform>(position, velocity, 0.0f);
        small->cShape = std::make_shared<CShape>(
            entity->cShape->circle.getRadius() / 2.0f,
            vertices,
            entity->cShape->circle.getFillColor(),
            entity->cShape->circle.getOutlineColor(),
            entity->cShape->circle.getOutlineThickness()
            );
        small->cCollision = std::make_shared<CCollision>(entity->cCollision->radius / 2.0f);
        small->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
        small->cScore = std::make_shared<CScore>(vertices * entity->cScore->score * 2);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
    auto bullet = m_entities.addEntity("bullet");

    // --------- transform --------  entity is an m_player
    Vec2 velocity = target - entity->cTransform->pos;
    std::ignore = velocity.normalize();
    velocity *= m_bulletConfig.S / 2.0f;
    bullet->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos), velocity, 0.0f);

    // --------- shape ---------
    auto fillColor = sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
    auto outlineColor = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);

    bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, fillColor, outlineColor,
                                              m_bulletConfig.OT);

    // --------- collision ---------
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

    // --------- lifespan ---------
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    const size_t vertices = 18;
    for (size_t i = 0; i < vertices; i++)
    {
        auto ghost = m_entities.addEntity("bullet");
        const float angle = 360.0f / vertices * i;
        const float radian = angle * kPi / 180;

        Vec2 position = entity->cTransform->pos;
        Vec2 velocity(cos(radian), sin(radian));
        velocity *= m_bulletConfig.S;

        ghost->cTransform = std::make_shared<CTransform>(position, velocity, 0.0f);
        ghost->cShape = std::make_shared<CShape>(
            m_bulletConfig.SR,
            entity->cShape->circle.getPointCount(),
            entity->cShape->circle.getFillColor(),
            entity->cShape->circle.getOutlineColor(),
            entity->cShape->circle.getOutlineThickness()
            );
        ghost->cCollision = std::make_shared<CCollision>(m_player->cCollision->radius);
        ghost->cLifespan = std::make_shared<CLifespan>(m_windowConfig.framerate / 3);
    }
    m_sWeapon.lastUsed = m_currentFrame;
}

void Game::sMovement()
{
    for (auto& el: m_entities.getEntities())
    {
        if (el->cInput)
        {
            m_checkBoundaries(m_player);

            el->cTransform->velocity.x = static_cast<float>(el->cInput->right - el->cInput->left);
            el->cTransform->velocity.y = static_cast<float>(el->cInput->down - el->cInput->up);

            el->cTransform->pos += el->cTransform->velocity * m_playerConfig.S;
        }
        else
        {
            m_checkBoundaries(el); // bullets also can bounce ;D
        }
        el->cTransform->pos += el->cTransform->velocity;
        el->cShape->circle.setPosition(el->cTransform->pos.x, el->cTransform->pos.y);
    }
}

void Game::sLifespan()
{
    for (auto& el: m_entities.getEntities())
    {
        if (el->cLifespan) // bullets and small enemies
        {
            el->cLifespan->remaining -= 1;
            auto color = el->cShape->circle.getFillColor();
            auto alpha = 255 * el->cLifespan->remaining / el->cLifespan->total;
            el->cShape->circle.setFillColor(sf::Color(color.r, color.g, color.b, alpha));

            color = el->cShape->circle.getOutlineColor();
            alpha = 255 * el->cLifespan->remaining / el->cLifespan->total;
            el->cShape->circle.setOutlineColor(sf::Color(color.r, color.g, color.b, alpha));

            if (el->cLifespan->remaining <= 0) { el->destroy(); }
        }
    }
}

void Game::sCollision()
{
    for (auto& enemy: m_entities.getEntities("enemy"))
    {
        m_checkCollision(m_player, enemy);
    }

    for (const auto& bullet: m_entities.getEntities("bullet"))
    {
        for (const auto& enemy: m_entities.getEntities("enemy"))
        {
            m_checkCollision(bullet, enemy);
        }

        for (const auto& small: m_entities.getEntities("small"))
        {
            m_checkCollision(bullet, small);
        }
    }
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI)
    {
        if (m_entities.getEntities().size() <= 20) // FOR DEBUGGING
        {
            spawnEnemy();
        }
    }
}

void Game::sGUI()
{
    ImGui::Begin("Geometry Wars");

    // ImGui::ShowDemoWindow();
    const char* names[2] = {"Systems", "Entity Manager"};
    static bool opened[2] = {true, true}; // Persistent user state

    if (ImGui::BeginTabBar("DebugTabBar", true))
    {
        for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
            if (opened[n] && ImGui::BeginTabItem(names[n], &opened[n], ImGuiTabItemFlags_None))
            {
                if (n == 0)
                {
                    ImGui::Text("This is the %s tab!", names[n]);
                    ImGui::Checkbox("Paused", &m_control.isPaused);
                    ImGui::Checkbox("Movement", &m_control.showMovement);
                    ImGui::Checkbox("Lifespan", &m_control.showLifespan);
                    ImGui::Checkbox("Collision", &m_control.showCollisiton);
                    ImGui::Checkbox("Spawning", &m_control.spawnEnemies);
                    ImGui::Checkbox("Rendering", &m_control.showRendering);
                }
                else
                {
                    if (ImGui::TreeNode("Entities by Tag"))
                    {
                        for (auto& [tag, EntityVector]: m_entities.getEntityMap())
                        {
                            if (ImGui::CollapsingHeader(tag.c_str()))
                            {
                                for (auto& el: EntityVector)
                                    m_getEntityInfo(el);
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("All Entities"))
                    {
                        for (auto& e: m_entities.getEntities())
                        {
                            m_getEntityInfo(e);
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::EndTabItem();
            }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Game::sRender()
{
    m_window.clear();

    if (m_control.showRendering)
    {
        // draw the entity's sf::CircleShape
        for (const auto& el: m_entities.getEntities())
        {
            el->cShape->circle.setPosition(el->cTransform->pos.x, el->cTransform->pos.y);
            el->cTransform->angle += 1.0f;
            el->cShape->circle.setRotation(el->cTransform->angle);

            m_window.draw(el->cShape->circle);
        }
        const std::string sWeaponMsg = (m_sWeapon.delta > m_windowConfig.framerate * 5) ? "ready" : "not ready";
        m_text.setString("Score: " + std::to_string(m_score)
            + "\nLives: " + std::to_string(m_totalLives - m_liveCounter) + "/" + std::to_string(m_totalLives)
            + "\nSpecial weapon: " + sWeaponMsg);
    }

    if (m_liveCounter >= m_totalLives)
    {
        m_showFinalMessage();
    }
    m_window.draw(m_text);

    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event)) // why poll and not wait? think
    {
        // pass the event to imgui to be parsed
        ImGui::SFML::ProcessEvent(m_window, event);

        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:
                    m_running = false;
                    break;
                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = true;
                    break;
                case sf::Keyboard::P:
                    m_control.isPaused = !m_control.isPaused;
                    break;
                default:
                    break;
            }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = false;
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && !m_control.isPaused)
        {
            // this line ignores mouse events if ImGui is the thing being clicked
            if (ImGui::GetIO().WantCaptureMouse) { continue; }

            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // call spawnBullet here
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right && m_sWeapon.delta > m_sWeapon.cooldown)
            {
                std::cout << "Left mouse button clicked at (" << event.mouseButton.x << ", " << event.mouseButton.x
                    << ")\n";
                // call spawnSpecialWeapon here
                spawnSpecialWeapon(m_player);
            }
        }
    }
}

void Game::m_checkBoundaries(const std::shared_ptr<Entity>& entity) const
{
    // be sure to use the collision radius, NOT the shape radius
    const auto position = entity->cTransform->pos;
    const auto collisionRadius = entity->cCollision->radius;

    const auto winWidth = static_cast<float>(m_window.getSize().x);
    const auto winHeight = static_cast<float>(m_window.getSize().y);

    if (entity->tag() == "player")
    {
        if (position.x < collisionRadius) { m_player->cTransform->pos.x = collisionRadius; }
        if (position.y < collisionRadius) { m_player->cTransform->pos.y = collisionRadius; }

        if (position.x + collisionRadius > winWidth) { m_player->cTransform->pos.x = winWidth - collisionRadius; }
        if (position.y + collisionRadius > winHeight) { m_player->cTransform->pos.y = winHeight - collisionRadius; }
    }
    else
    {
        if (position.x + collisionRadius >= winWidth || position.x <= collisionRadius)
        {
            entity->cTransform->velocity.x *= -1.0f;
        }

        if (position.y + collisionRadius >= winHeight || position.y <= collisionRadius)
        {
            entity->cTransform->velocity.y *= -1.0f;
        }
    }
}

void Game::m_checkCollision(const std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& enemy)
{
    // entity can be a bullet or a player
    if (const auto distance = entity->cTransform->pos.dist(enemy->cTransform->pos);
        distance < entity->cCollision->radius + enemy->cCollision->radius)
    {
        if (entity->tag() == "bullet")
        {
            m_score += enemy->cScore->score;
            // if (m_score >= 5000) // Just a bonus for fun
            // {
            //     m_liveCounter -= 1;
            //     m_score -= 5000;
            // }
        }

        entity->destroy();
        if (!entity->cLifespan)
        {
            spawnPlayer();
        }

        if (!enemy->cLifespan)
        {
            spawnSmallEnemies(enemy);
        }
        enemy->destroy();
    }
}

// GUI debug

std::string Game::m_getTextLine(const std::shared_ptr<Entity>& entity)
{
    const auto circle = entity->cShape->circle;
    const std::string entityText = "\t" + std::to_string(entity->id()) + "\t" + entity->tag()
        + "\t(" + std::to_string(static_cast<int>(circle.getPosition().x)) + ", "
        + std::to_string(static_cast<int>(circle.getPosition().y)) + ")\n";
    return entityText;
}

void Game::m_getEntityInfo(const std::shared_ptr<Entity>& entity)
{
    const auto entityText = m_getTextLine(entity);
    std::string label = "D##" + std::to_string(entity->id());
    if (ImGui::Button(label.c_str()))
    {
        std::cout << "I clicked on the button " << label << "\n";
        entity->destroy();
    }
    ImGui::SameLine();
    ImGui::Text("%s", entityText.c_str());
}

void Game::m_showFinalMessage()
{
    auto finalText = m_text;
    finalText.setString("Game Over!");
    const auto length = finalText.getGlobalBounds().width / 2.0f;
    constexpr int size = 60;
    finalText.setCharacterSize(size);
    finalText.setFillColor(sf::Color::Red);
    finalText.setPosition(sf::Vector2f(static_cast<float>(m_windowConfig.width) / 2.0f - length * 2.4f,
                                       static_cast<float>(m_windowConfig.height) / 2.0f - size / 2.0f));
    m_window.draw(finalText);
}

/**
 * Divident / divisor = quotient
 */
template <typename T>
T Game::random(const T min, const T max)
{
    // % works only with ints
    int divisor = 1;
    auto tmp = (1 + max - min);
    if constexpr (std::is_integral<T>::value)
        divisor = tmp;
    if constexpr (std::is_floating_point<T>::value)
        divisor = static_cast<int>(tmp);
    return min + rand() % divisor;
}
