#ifndef LASER_H
#define LASER_H

#include "Entity.h"
#include "Constants.h"

// Am transformat clasa existentă într-o clasă template.
// T poate fi tipul numeric pentru viteză (float, double) sau un tip pentru proprietăți.
template <typename T = float>
class Laser : public Entity {
private:
    T speed;
    bool active;
    sf::RectangleShape shape;

    void doUpdate() override {
        position.y += static_cast<float>(speed); // Folosește atributul de tip T
        shape.setPosition(position);

        //Utilizarea funcției libere șablon isOut
        if (position.y < 0 || isOut(position.y, static_cast<float>(Config::SCREEN_HEIGHT))) {
            active = false;
        }
    }

    void doDraw(sf::RenderWindow& window) override {
        if (active) {
            window.draw(shape);
        }
    }

public:
    //Funcție membru care depinde de T
    Laser(sf::Vector2f pos, T speedVal): Entity(pos.x, pos.y), speed(speedVal), active(true) {

        shape.setSize({4.f, 15.f});
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(position);
    }

    ~Laser() override = default;

    bool isActive() const { return active; }
    sf::FloatRect getBounds() const override { return shape.getGlobalBounds(); }

    // Implementare Prototype (Design Pattern)
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Laser<T>>(*this);
    }
};

#endif // LASER_H