//
// Created by Alexandra on 4/14/2026.
//
#include "Entity.h"

int Entity::entityCount = 0;

//Constructorul
Entity::Entity(float x, float y) : position(x, y) {
    entityCount++;
}

//Destructorul
Entity::~Entity() {
    entityCount--;
}

// Implementarea metodelor statice
int Entity::getEntityCount() {
    return entityCount;
}

// NVI, aceste funcții sunt publice și apelează implementările private ale derivatelor
void Entity::update() {
    doUpdate(); //Execută implementarea specifică prin polimorfism la rulare
}
void Entity::draw(sf::RenderWindow& window) {
    doDraw(window);
} 