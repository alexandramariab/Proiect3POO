#include "Game.h"
#include "Constants.h"
#include <iostream>
#include "Exceptions.h"
#include <algorithm>
#include "PowerUp.h"
#include <random>
#include "AlienFactory.h"

Game::Game() : run(true) {
    initAliens();
}

Game::~Game() {}

void Game::Draw(sf::RenderWindow& window) {
    spaceship.draw(window);
    for (auto& entity : allEntities) {
        entity->draw(window);
    }
    for (auto& laser : spaceship.getLasers()) laser->draw(window);
    for (auto& laser : alienLasers) laser->draw(window);
}

void Game::HandleInput() { // Gestionarea intrărilor de la tastatură

    if (!run) return; // Controlul navei este blocat dacă jocul s-a terminat

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        spaceship.MoveLeft();
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        spaceship.MoveRight();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        spaceship.fire();
    }
}

void Game::Update() { //Actualizează starea tuturor obiectelor la fiecare cadru
    if (!run) return;

    spaceship.update();
    spaceship.updatePowerUps();

    // generam random power-up-uri
    if (rand() % 1000 == 0) {
        float spawnX = static_cast<float>(rand() % (Config::SCREEN_WIDTH - 50) + 25);
        auto isClone = (rand() % 2 == 0);

        if (isClone) {
            auto pUp = std::make_shared<ClonePowerUp>(spawnX, -30.f);
            allEntities.push_back(pUp);
        } else {
            auto pUp = std::make_shared<SpeedPowerUp>(spawnX, -30.f);
            allEntities.push_back(pUp);
        }
    }

    bool hitEdge = false;

    for (auto it = allEntities.begin(); it != allEntities.end(); ) {
        auto entity = *it;
        entity->update();

        auto pUp = std::dynamic_pointer_cast<PowerUp>(entity);
        auto alien = std::dynamic_pointer_cast<Alien>(entity);

        if (pUp) { //Verificăm dacă jucătorul a colectat bonusul
            auto shipBounds = spaceship.getBounds();
            auto pUpBounds = pUp->getBounds();
            auto isCollected = shipBounds.findIntersection(pUpBounds);
            auto isOut = pUp->isExpired();

            if (isCollected) {
                pUp->applyEffect(spaceship);
                it = allEntities.erase(it);
                continue;
            }

            if (isOut) {
                it = allEntities.erase(it);
                continue;
            }
        }

        if (alien) {
            auto right = alien->getRightEdge();
            auto left = alien->getLeftEdge();

            if (right > Config::SCREEN_WIDTH || left < 0) { //daca a atins marginea
                hitEdge = true;
            }
        }
        ++it;
    }

    if (hitEdge) {
        Alien::changeDirection(); 
        for (auto& entity : allEntities) {
            if (auto a = std::dynamic_pointer_cast<Alien>(entity))
                a->moveDown(20.f);
        }
    }

    //logica de tragere pt aliens
    auto timeElapsed = alienFireClock.getElapsedTime().asSeconds();
    if (timeElapsed > fireInterval && !allEntities.empty()) {
        std::vector<std::shared_ptr<Alien>> aliensOnly; // Filtrăm doar Alieni din allEntities
        for (auto& e : allEntities) {
            auto a = std::dynamic_pointer_cast<Alien>(e);
            if (a) aliensOnly.push_back(a);
        }

        if (!aliensOnly.empty()) {
            int randomIndex = rand() % aliensOnly.size(); // Alegem un alien random pentru a trage
            // MODIFICARE: Alocare folosind clasa template Laser<>
            alienLasers.push_back(std::make_shared<Laser<>>(aliensOnly[randomIndex]->getCenter(), 5.f));
        }
        alienFireClock.restart();
    }

    //actualizăm laserele navei
    auto& lasers = spaceship.getLasers();
    auto it = lasers.begin();
    while (it != lasers.end()) {
        (*it)->update(); //mișcăm laserul

        if (!(*it)->isActive()) {
            it = lasers.erase(it); 
        } else {
            ++it;
        }
    }

    //actualizăm laserele alienilor
    auto itAlien = alienLasers.begin();
    while (itAlien != alienLasers.end()) {
        (*itAlien)->update(); 

        if (!(*itAlien)->isActive()) {
            itAlien = alienLasers.erase(itAlien);
        } else {
            ++itAlien;
        }
    }

    CheckForCollisions();

    //conditia de victorie
    bool anyAliensLeft = false;
    for (auto& entity : allEntities) {
        if (std::dynamic_pointer_cast<Alien>(entity)) {
            anyAliensLeft = true;
            break; 
        }
    }

    if (!anyAliensLeft) {
        run = false;
        std::cout << "FELICITARI! Ai castigat!" << std::endl;
    }
}

void Game::initAliens() {
    allEntities.clear();
    for (int row = 0; row < 5; row++) {
        for (int column = 0; column < 10; column++) {
            float x = 75.f + column * 55.f;
            float y = 110.f + row * 55.f;

            // Cerință: Utilizarea clasei Factory (AlienFactory)
            allEntities.push_back(AlienFactory::createAlien(x, y, row));
        }
    }
}


void Game::CheckForCollisions() {
    auto& sLasers = spaceship.getLasers();

    // Laserele navei lovesc alienii
    for (auto lIt = sLasers.begin(); lIt != sLasers.end(); ) {
        auto laser = *lIt;
        bool hitSomething = false;

        for (auto eIt = allEntities.begin(); eIt != allEntities.end(); ) {
            auto entity = *eIt;
            auto alienTarget = std::dynamic_pointer_cast<Alien>(entity);

            if (alienTarget) { 
                auto lBounds = laser->getBounds();
                auto aBounds = alienTarget->getBounds();
                auto isHit = lBounds.findIntersection(aBounds);

                if (isHit) {
                    eIt = allEntities.erase(eIt);
                    hitSomething = true;
                    break;
                } else {
                    ++eIt;
                }
            } else {
                ++eIt;
            }
        }
        if (hitSomething) lIt = sLasers.erase(lIt);
        else ++lIt;
    }

    //laserele alienilor vs nava
    for (auto& laser : alienLasers) {
        auto lBounds = laser->getBounds();
        auto sBounds = spaceship.getBounds();
        auto hitShip = lBounds.findIntersection(sBounds);

        if (hitShip) {
            run = false;
            std::cout << "GAME OVER: Ai fost impuscat!" << std::endl;
            return;
        }
    }

    // daca alienii ating nava direct sau trec de ea, pierdem jocul
    for (auto& entity : allEntities) {
        auto alien = std::dynamic_pointer_cast<Alien>(entity);

        if (alien) {
            auto aBounds = alien->getBounds();
            auto sBounds = spaceship.getBounds();

            auto collisionWithShip = aBounds.findIntersection(sBounds);
            //verificăm dacă partea de jos a alienului a trecut de nava
            auto passedLimit = (aBounds.position.y + aBounds.size.y >= sBounds.position.y);

            if (collisionWithShip || passedLimit) {
                run = false;
                std::cout << "GAME OVER: Invazia a reusit!" << std::endl;
                return;
            }
        }
    }
}