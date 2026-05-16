#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include "Laser.h"
#include <vector>
#include <memory>

// Cerința 2.a: Moștenire
class Spaceship : public Entity {
private:
    sf::Texture image;
    sf::Sprite sprite; // Obiectul grafic care permite desenarea și poziționarea imaginii
    // MODIFICARE: Vectorul reține acum instanțieri ale clasei template (Laser<>)
    std::vector<std::shared_ptr<Laser<>>> lasers;
    sf::Clock fireClock; // Cronometru intern pentru a preveni tragerea prea rapida

    void doUpdate() override;
    void doDraw(sf::RenderWindow& window) override;
    float fireInterval = 0.5f; //Timpul min dintre 2 focuri succesive
    bool speedBoostActive = false;
    bool cloneActive = false;
    sf::Clock powerUpClock; //Durata de viata a bonusurilor active
    std::shared_ptr<Spaceship> shipClone = nullptr; // Pointer către nava secundară creată de bonusul Clone

public:
    Spaceship(); //constructor
    ~Spaceship() override= default; //destructor
    Spaceship(const Spaceship& other); //constructor de copiere
    Spaceship& operator=(Spaceship other); //operator de atribuire 
    friend void swap(Spaceship& first, Spaceship& second) noexcept; //funcție de swap pentru copy-and-swap
   
    void MoveLeft();
    void MoveRight();

    void fire();
    // MODIFICARE: Returnează referință la vectorul de clase template
    std::vector<std::shared_ptr<Laser<>>>& getLasers() { return lasers; }

    // DESIGN PATTERN: Prototype (Suprascriere metodă clonare)
    std::shared_ptr<Entity> clone() const override;
    sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }
    void activateSpeedBoost();
    void activateClone();
    void updatePowerUps(); // Verifică dacă timpul bonusurilor a expirat

};

#endif//SPACESHIP_H