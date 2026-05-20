//
// Created by Alexandra on 5/16/2026.
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include "Exceptions.h"

template <typename T>
class ResourceManager {
private:
    std::map<std::string, T> resources;

    // Constructor privat pentru implementarea pattern-ului Singleton
    ResourceManager() = default;
    ~ResourceManager() = default;

public:
    // Ștergem constructorul de copiere și operatorul de atribuire pentru a asigura instanța unică
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Metodă statică pentru a obține instanța unică a managerului
    static ResourceManager& getInstance() {
        static ResourceManager instance; //va trăi pe tot parcursul rulării programului
        return instance;
    }

    // Funcție membru care depinde de T: încarcă o resursă generică din fișier
    void load(const std::string& name, const std::string& filepath) {
        if (resources.find(name) == resources.end()) { //dacă resursa nu a fost deja încărcată
            T resource;
            if (!resource.loadFromFile(filepath)) {
                throw ResourceException(filepath);
            }
            resources[name] = resource;
        }
    }

    // Funcție membru care depinde de T: returnează o referință către resursa stocată
    T& get(const std::string& name) {
        auto it = resources.find(name);
        if (it == resources.end()) {
            throw ResourceException(name + " (resursa nu a fost incarcata in manager)");
        }
        return it->second;
    }
};

#endif //RESOURCEMANAGER_H
