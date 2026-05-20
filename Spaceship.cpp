#include "Spaceship.h"
#include "Constants.h"
#include "Exceptions.h"
#include "ResourceManager.h"
#include <iostream>

// Constructor
Spaceship::Spaceship() : Entity(0.f, 0.f), image(), sprite(image) {
    // Utilizare ResourceManager (Clasă Șablon + Singleton)
    auto& manager = ResourceManager<sf::Texture>::getInstance();
    manager.load("spaceship", "Graphics/spaceship.png");
    image = manager.get("spaceship");

    sprite.setTexture(image, true);

    position.x = (static_cast<float>(Config::SCREEN_WIDTH) - image.getSize().x) / 2.f;
    position.y = static_cast<float>(Config::SCREEN_HEIGHT) - image.getSize().y - 20.f;
    sprite.setPosition(position);
}

// Constructor de copiere
Spaceship::Spaceship(const Spaceship& other) : Entity(other), image(other.image), sprite(image) {
    sprite.setTexture(image, true);
    sprite.setPosition(position);
}

// Funcția swap
void swap(Spaceship& first, Spaceship& second) noexcept {
    using std::swap;
    swap(first.position, second.position);
    swap(first.image, second.image);
    first.sprite.setTexture(first.image, true);
    second.sprite.setTexture(second.image, true);
}

//operator de atribuire folosind copy-and-swap
Spaceship& Spaceship::operator=(Spaceship other) {
    swap(*this, other);
    return *this;
}


void Spaceship::doDraw(sf::RenderWindow& window) {
    window.draw(sprite);
    if (cloneActive && shipClone) {
        shipClone->draw(window);
    }
}

void Spaceship::doUpdate() {
    // Sincronizăm sprite-ul cu poziția logică
    sprite.setPosition(position);
}

void Spaceship::MoveLeft() {
    position.x -= 7; //mutam nava la stanga cu 7 pixeli
    if (position.x < 0) {  //daca depasim marginea 
        position.x = 0;
    }
}

void Spaceship::MoveRight() {
    position.x += 7;
    float limitaDreapta = static_cast<float>(Config::SCREEN_WIDTH) - image.getSize().x;
    if (position.x >= limitaDreapta) {
        position.x = limitaDreapta;
    }
}   

void Spaceship::fire() {
    if (fireClock.getElapsedTime().asSeconds() >= fireInterval) {
        // Instanțiere folosind clasa template cu argumentul implicit
        lasers.push_back(std::make_shared<Laser<>>(sf::Vector2f(position.x + 20, position.y), -10.f));
        if (cloneActive && shipClone) {
            lasers.push_back(std::make_shared<Laser<>>(sf::Vector2f(shipClone->position.x + 20, shipClone->position.y), -10.f));
        }
        fireClock.restart();
    }
}
void Spaceship::activateSpeedBoost() {
    speedBoostActive = true;
    fireInterval = 0.15f; //trage mult mai rapid
    powerUpClock.restart();
}

void Spaceship::activateClone() {
    cloneActive = true;
    // Cream o copie a navei curente
    auto duplicated = this->clone();
    shipClone = std::dynamic_pointer_cast<Spaceship>(duplicated);
    powerUpClock.restart();
}

void Spaceship::updatePowerUps() {
    //daca a expirat timpul
    if ((speedBoostActive || cloneActive) && powerUpClock.getElapsedTime().asSeconds() > 5.0f) {
        speedBoostActive = false;
        cloneActive = false;
        fireInterval = 0.5f; //revine la viteza normală
        shipClone = nullptr; //stergem clona
    }

    if (cloneActive && shipClone) {
        //clona urmărește nava la o distanță fixă (50 pixeli la dreapta)
        shipClone->position = sf::Vector2f(position.x + 50.f, position.y);
        shipClone->doUpdate();
    }
}
// Modificat pentru a se potrivi cu patternul Prototype din clasa de bază Entity
std::shared_ptr<Entity> Spaceship::clone() const {
    return std::make_shared<Spaceship>(*this);
}