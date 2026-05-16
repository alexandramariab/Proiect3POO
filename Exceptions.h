#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

//baza proprie pentru excepții
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& msg) : std::runtime_error(msg) {}
}; 

//excepție pentru imagini
class ResourceException : public GameException {
public:
    explicit ResourceException(const std::string& resourcePath) 
        : GameException("Eroare critica: Nu s-a putut incarca resursa de la: " + resourcePath) {}
};

//excepție pentru logica jocului
class GameplayException : public GameException {
public:
    explicit GameplayException(const std::string& msg) : GameException("Eroare Gameplay: " + msg) {}
};

//excepție pentru configurație
class ConfigurationException : public GameException {
public:
    explicit ConfigurationException(const std::string& msg) 
        : GameException("Eroare Configuratie: " + msg) {}
};

#endif//EXCEPTIONS_H