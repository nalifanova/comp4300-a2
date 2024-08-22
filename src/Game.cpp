#include "Game.h"

#include <fstream>
#include <random>
#include <type_traits>

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
                >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SP;
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
    m_text.setString("Score: " + std::to_string(m_score));

    std::ignore = ImGui::SFML::Init(m_window);

    spawnPlayer();
    // init random seed
    srand(static_cast<int>(time(nullptr)));
}

void Game::run()
{
    // TODO: add pause functionality in here
    //      some systems should function while paused (rendering)
    //      some systems shouldn't (move/input)

    while (m_running)
    {
        if (!m_paused)
        {
            m_entities.update();
            sEnemySpawner();
            sLifespan();
            sMovement();
            sCollision();
        }

        // required update call to imgui
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        sUserInput();
        sGUI();
        sRender();

        // increment the current frame
        // may need be moved when pause implemented
        m_currentFrame++;
    }
}

void Game::setPaused(const bool paused)
{
    m_paused = paused;
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    //  --------- transform ---------
    //The player must spawn in the center of the screen at the beginning of the game
    auto position = Vec2(static_cast<float>(m_window.getSize().x) / 2.f,
                         static_cast<float>(m_window.getSize().y) / 2.f);
    entity->cTransform = std::make_shared<CTransform>(position, Vec2(0.0f, 0.0f), 0.0f);
    std::cout << "Player position (" << (m_window.getSize().x / 2.f) << ", " << (m_window.getSize().y / 2.f) << ")\n";

    // --------- shape ---------
    auto fillColor = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
    auto outlineColor = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);
    // radius, points - vertices
    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
                                              fillColor, outlineColor, m_playerConfig.OT);

    // --------- collisiton ---------
    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");

    // --------- transform ---------
    const float x = random(m_enemyConfig.CR, m_windowConfig.width - m_enemyConfig.CR - 1);
    const float y = random(m_enemyConfig.CR, m_windowConfig.height - m_enemyConfig.CR - 1);

    const float angle = random(0.0f, 3.1415f * 2.0f); // 2 * pi = 180'
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
    // TODO: spawn small enemies at the location of the input enemy entity

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worth double points of the original enemy
}

void Game::smawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
    // TODO: implement the spawning of a bullet which travels toward target
    //      - bullet speed is given as a scalar speed
    //      - you must set the velocity by using formula in notes
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // TODO: implement your own special weapon
}

void Game::sMovement()
{
    m_checkBoundaries();

    // TODO: implement all entity movement in this function
    //       you should read the m_player->cInput component to determine if the player is moving

    // Sample movement speed update
    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

}

void Game::sLifespan()
{
    // TODO: implement all lifespan functionality
    //
    // for all entities
    //      if entity has no lifespan component, skip it
    //      if entity has > 0 remanining lifespan, substract 1
    //      if it has lifespan and is alive
    //          scale its alpha channel properly
    //      of it has lifespan and its time is up
    //          destroy the entity
}

void Game::sCollision()
{
    // TODO: implement all proper collisions between entities
    //      be sure to use the collision radius, NOT the shape radius
    // Tip: foor lop with bullet
}

void Game::sEnemySpawner()
{
    // TODO: code which implements enemy spawning should go here
    if (m_currentFrame - m_lastEnemySpawnTime >= 10)
    {
        if (m_entities.getEntities().size() <= 20)
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
                    ImGui::Checkbox("Movement", &m_paused);
                    ImGui::BulletText("Lifespan");
                    ImGui::BulletText("Collision");
                    ImGui::BulletText("Spawning");
                    ImGui::BulletText("Rendering");
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

    // draw the entity's sf::CircleShape
    for (const auto& el: m_entities.getEntities())
    {
        el->cShape->circle.setPosition(el->cTransform->pos.x, el->cTransform->pos.y);
        el->cTransform->angle += 1.0f;
        el->cShape->circle.setRotation(el->cTransform->angle);

        m_window.draw(el->cShape->circle);
    }
    // display score
    m_window.draw(m_text);

    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput()
{
    // TODO: handle user input here
    //      note that you should only be setting the player's input component variables here
    //      you should not implement the player's movement logic here
    //      the movement system will read the variables you set in this function

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
                    m_paused = !m_paused;
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

        if (event.type == sf::Event::MouseButtonPressed)
        {
            // this line ignores mouse events if ImGui is the thing being clicked
            if (ImGui::GetIO().WantCaptureMouse) { continue; }

            if (event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << "Left mouse button clicked at (" << event.mouseButton.x << ", " << event.mouseButton.x
                    << ")\n";
                // call spawnBullet here
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                std::cout << "Left mouse button clicked at (" << event.mouseButton.x << ", " << event.mouseButton.x
                    << ")\n";
                // call spawnSpecialWeapon here
            }

        }
    }
}

void Game::m_checkBoundaries()
{
    const auto position = m_player->cShape->circle.getPosition();
    const auto bounds = m_player->cShape->circle.getLocalBounds();

    // check width boundaries
    if (position.x + 0.5 * bounds.width >= static_cast<float>(m_window.getSize().x)
        || position.x <= 0.5 * bounds.width)
    {
        m_player->cTransform->velocity.x *= -1.0f;
    }

    // check height boundaries
    if (position.y + 0.5 * bounds.height >= static_cast<float>(m_window.getSize().y)
        || position.y <= 0.5 * bounds.height)
    {
        m_player->cTransform->velocity.y *= -1.0f;
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
