#include "Game.h"

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& path)
{
    // TODO: read in config file here
    //       use the premade PlayerConfig, EnemyConfig, BulletConfig variables

    // set up default window parameters
    m_window.create(sf::VideoMode(1200, 620), "Assigment 02");
    m_window.setFramerateLimit(60);

    std::ignore = ImGui::SFML::Init(m_window);

    spawnPlayer();
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
            // update the entity manager
            m_entities.update();

            sEnemySpawner();
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

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
    // TODO: Finish adding all properties of the player with the correct values from t..

    // We create every entity by calling EntityManager.addEntity(tag)
    // This returns an std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    // Give this entity a Transform so it spawns at (200, 200) with velicity (1, 1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(200.0f, 200.0f), Vec2(1.0f, 1.0f), 0.0f);

    // The entity's shape will have radius 32, 8 sides, dark gray fill, and red outline of thickness 4
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    // collision shape
    entity->cCollision = std::make_shared<CCollision>(32);

    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

void Game::spawnEnemy()
{
    // TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    //       the enemy must be spawned completely within the bounds of the window

    auto entity = m_entities.addEntity("enemy");
    int vertices = 3;
    entity->cTransform = std::make_shared<CTransform>(Vec2(intRand(100, 800) * 1.0f, intRand(50, 400) * 1.0f)
                                                    , Vec2(1.0f, 1.0f), 2.0f);

    // Enemy 32 32 3 3 255 255 255 2 3 8 90 60
    entity->cShape = std::make_shared<CShape>(32.0f, vertices, sf::Color(255, 255, 255), sf::Color(0, 255, 0), 4.0f);
    entity->cShape->circle.setPosition(entity->cTransform->pos.x, entity->cTransform->pos.y);

    entity->cCollision = std::make_shared<CCollision>(32.0f);
    entity->cScore = std::make_shared<CScore>(vertices * 100);

    std::cout << "entity: " << entity->tag() << ", id = " << entity->id()
        << ", pos(" << entity->cShape->circle.getPosition().x << ", "
        << entity->cShape->circle.getPosition().x << ")\n";

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
    // TODO: change the code below to draw ALL of the entities
    //      sample drawing of the player Entity that we have created
    m_window.clear();

    // -- here
    // set the position of the shape based on the entity's transform->pos
    m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

    // set the rotation of the shape based on the entity's transform->angle
    m_player->cTransform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);

    // draw the entity's sf::CircleShape
    for (const auto& el: m_entities.getEntities())
    {
        m_window.draw(el->cShape->circle);
    }

    // DEBUG
    if (!m_font.loadFromFile("../../assets/fonts/tech.ttf"))
    {
        std::cerr << "Could not load font!\n";
        exit(-1);
    }
    m_text.setFont(m_font);
    m_text.setString("Score: " + std::to_string(m_score));
    m_text.setFillColor(sf::Color::White);
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

int Game::intRand(const int min, const int max)
{
    return min + rand() % (1 + max - min);
}
