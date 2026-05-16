#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Constants.h"
#include "Exceptions.h"
#include <iostream>
#include <optional>

int main() {
    try {
        sf::RenderWindow window(sf::VideoMode({Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT}), "Space Invaders - POO");
        window.setFramerateLimit(60);
        Game game;

        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) window.close();
            }

            game.HandleInput();
            game.Update();

            window.clear(Config::BACKGROUND_COLOR);
            game.Draw(window);
            window.display();
        }
    }
    catch (const ConfigurationException& e) {
        std::cerr << "Eroare configuratie: " << e.what() << std::endl;
        return 3;
    }
    catch (const ResourceException& e) {
        std::cerr << "Eroare imagine: " << e.what() << std::endl;
        return 1;
    }
    catch (const GameException& e) {
        std::cerr << "Eroare joc: " << e.what() << std::endl;
        return 2;
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare standard: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}