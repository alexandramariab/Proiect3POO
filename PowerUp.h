#ifndef POWERUP_H
#define POWERUP_H

#include "Entity.h"
#include "Spaceship.h"

class PowerUp : public Entity {
private:
    sf::RectangleShape shape; 
    bool active;              
    
    void doUpdate() override;
    void doDraw(sf::RenderWindow& window) override;

public:
    
    PowerUp(float x, float y, sf::Color color);
    virtual ~PowerUp() override = default;
    
    virtual void applyEffect(Spaceship& ship) = 0;
    
    sf::FloatRect getBounds() const override;
    bool isExpired() const;

};


class SpeedPowerUp : public PowerUp {
public:
    SpeedPowerUp(float x, float y) : PowerUp(x, y, sf::Color::Cyan) {}
    
    void applyEffect(Spaceship& ship) override;
    
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<SpeedPowerUp>(*this);
    }
};

class ClonePowerUp : public PowerUp {
public:
    ClonePowerUp(float x, float y) : PowerUp(x, y, sf::Color::Magenta) {}
    
    void applyEffect(Spaceship& ship) override;

    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<ClonePowerUp>(*this);
    }
};

#endif//POWERUP_H