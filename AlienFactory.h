#ifndef ALIENFACTORY_H
#define ALIENFACTORY_H

#include <memory>
#include "Alien.h"

class AlienFactory {
public:
    // Factory Method ce returnează un shared_ptr către o instanță de Alien
    static std::shared_ptr<Alien> createAlien(float x, float y, int row) {
        int alienType;
        if (row == 0) alienType = 3;
        else if (row <= 2) alienType = 2;
        else alienType = 1;

        return std::make_shared<Alien>(x, y, alienType); //Creează efectiv obiectul Alien în memoria Heap 
    }
};

#endif // ALIENFACTORY_H