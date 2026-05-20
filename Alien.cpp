//
// Created by Alexandra on 4/16/2026.
//
#include "Alien.h"
#include "Constants.h"
#include <iostream>
#include "Exceptions.h"
#include "ResourceManager.h"

float Alien::speed = 2.0f;
int Alien::direction = 1;

Alien::Alien(float x, float y, int type) : Entity(x, y), type(type), alive(true), texture(), sprite(texture) {
    std::string path = "Graphics/alien_" + std::to_string(type) + ".png";

    // Înlocuire cu ResourceManager (Clasă șablon + Singleton) pentru eficientizare
    auto& manager = ResourceManager<sf::Texture>::getInstance(); //cere instanța unică a managerului de resurse
    manager.load("alien_" + std::to_string(type), path);
    texture = manager.get("alien_" + std::to_string(type));

    sprite.setTexture(texture, true);
    sprite.setPosition(position);
}

void Alien::doUpdate() {
    position.x += speed * direction;
    sprite.setPosition(position);
}
void Alien::moveDown(float distance) {
    position.y += distance;
    sprite.setPosition(position);
}

void Alien::changeDirection() {
    direction *= -1; //inversam directia de mers
}

float Alien::getRightEdge() const {
    auto& manager = ResourceManager<sf::Texture>::getInstance();
    std::string textureKey = "alien_" + std::to_string(type);
    return position.x + manager.get(textureKey).getSize().x;
}

float Alien::getLeftEdge() const {
    return position.x;
}

void Alien::doDraw(sf::RenderWindow& window) {
    if (alive) window.draw(sprite);
}

sf::Vector2f Alien::getCenter() const {
    auto& manager = ResourceManager<sf::Texture>::getInstance();
    std::string textureKey = "alien_" + std::to_string(type);
    return { position.x + manager.get(textureKey).getSize().x / 2.f, position.y + manager.get(textureKey).getSize().y };
}