#ifndef GAME_H
#define GAME_H
#include <SFML/Graphics.hpp>
#include "Spaceship.h"
#include <vector>
#include <memory>
#include "Alien.h"
#include "Laser.h"

class Game {
private:
    Spaceship spaceship;
    std::vector<std::shared_ptr<Entity>> allEntities;
    // Vectorul de proiectile inamice folosește acum clasa template
    std::vector<std::shared_ptr<Laser<>>> alienLasers;
    sf::Clock alienFireClock;
    float fireInterval = 1.2f;
    void initAliens(); 
    void CheckForCollisions();
    bool run;

public:
    Game();
    ~Game();
    void Draw(sf::RenderWindow& window); 
    void Update();
    void HandleInput();

};
#endif//GAME_H