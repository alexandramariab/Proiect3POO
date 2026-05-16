//
// Created by Alexandra on 4/16/2026.
//

#ifndef ALIEN_H
#define ALIEN_H

#include "Entity.h"

class Alien : public Entity {
private:
    int type;
    bool alive;
    sf::Texture texture;
    sf::Sprite sprite;

    static float speed; // viteza comună
    static int direction; // 1 pentru dreapta, -1 pentru stânga

    void doUpdate() override;
    void doDraw(sf::RenderWindow& window) override;

public:
    Alien(float x, float y, int type);
    ~Alien() override = default;

    bool isAlive() const { return alive; }
    void moveDown(float distance);
    float getRightEdge() const;
    float getLeftEdge() const;
    sf::Vector2f getCenter() const;

    // DESIGN PATTERN: Prototype suprascriere
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Alien>(*this);
    }

    static void changeDirection();
    sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }

};

#endif//ALIEN_H
