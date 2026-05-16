//
// Created by Alexandra on 4/25/2026.
//
#include "PowerUp.h"
#include "Constants.h"

PowerUp::PowerUp(float x, float y, sf::Color color) : Entity(x, y), active(true) {
    shape.setSize({25.f, 25.f});
    shape.setFillColor(color);
    shape.setPosition(position);
}

void PowerUp::doUpdate() {
    position.y += 2.0f; // Viteza de cădere
    shape.setPosition(position);
    // Utilizare funcție liberă șablon
    if (isOut(position.y, static_cast<float>(Config::SCREEN_HEIGHT))) active = false;
}


void PowerUp::doDraw(sf::RenderWindow& window) {
    if (active) window.draw(shape);
}

sf::FloatRect PowerUp::getBounds() const {
    return shape.getGlobalBounds();
}

bool PowerUp::isExpired() const {
    return !active;
}

void SpeedPowerUp::applyEffect(Spaceship& ship) {
    ship.activateSpeedBoost(); 
}

void ClonePowerUp::applyEffect(Spaceship& ship) {
    ship.activateClone(); 
}