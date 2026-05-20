#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>

class Entity {
private:
    virtual void doUpdate() = 0; // Metodă privată pur virtuală pentru logica internă
    virtual void doDraw(sf::RenderWindow& window) = 0; // Metodă privată pur virtuală pentru desenare

protected:
    sf::Vector2f position; //vector de pozitie pt toate entitatile (are aza Ox si Oy)
    static int entityCount;
    // Metodă virtuală de ajutor pentru afișarea detaliilor obiectului
    virtual void print(std::ostream& os) const {
        os << "Entitate la pozitia [" << position.x << ", " << position.y << "]";
    }

public:
    Entity(float x, float y); // Constructor care setează poziția inițială a entității pe ecran
    virtual ~Entity(); //Destructor virtual

    //interfata non-virtuala, entity controleaza tot procesul
    void update();
    void draw(sf::RenderWindow& window);

    virtual sf::FloatRect getBounds() const = 0; //returneaza dreptunghiul care inconjoara obiectul

    // Design Pattern: Prototype - metodă necesară clonării polimorfice
    virtual std::shared_ptr<Entity> clone() const = 0;

    // Supraîncărcarea operatorului de afișare
    friend std::ostream& operator<<(std::ostream& os, const Entity& e) {
        e.print(os);
        return os;
    }
    static int getEntityCount();
};
// Funcție liberă șablon utilă pentru calcularea limitelor de ecran
template <typename T>
bool isOut(T currentY, T limitY, bool movingDown = true) {
    if (movingDown) {
        return currentY > limitY;
    }
    return currentY < limitY;
}
#endif //ENTITY_H