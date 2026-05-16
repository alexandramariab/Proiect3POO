# Space Invaders - Implementare POO în C++

## 1. Enunțul Problemei

### Ce se dă:
Se cere implementarea unui joc **Space Invaders** folosind principiile Programării Orientate pe Obiecte. Jocul conține mai multe entități cu comportamente distincte care interacționează între ele: o navă spațială controlată de utilizator, alienii inamici care se deplasează și trag proiectile, laserele trase de ambele tabere, și obiecte cu efecte speciale (power-ups). Toate entitățile sunt caracterizate prin poziție pe ecran și comportament specific. Entitățile trebuie să se poată actualiza și desena pe ecran, iar interacțiunile (coliziunile) trebuie gestionate corespunzător. Jocul are logică de victorie (eliminarea tuturor inamicilor) și logică de înfrângere (lovirea navei sau trecerea inamicilor de ea).

### Ce se cere:
Să se implementeze arhitectura jocului folosind o ierarhie de clase cu bază proprie și cel puțin 3 clase derivate; să se gestioneze excepțiile (minim 3 tipuri distincte) pentru cazuri cum ar fi imposibilitatea încărcării resurselor; să se implementeze funcții virtuale și virtuale pure cu interfață non-virtuală; să se utilizeze obiecte cu pointeri de bază pentru a exploata polimorfismul; să se aplice pattern-uri avansate precum copy-and-swap și dynamic_cast pentru downcast; să se evite variabilele globale, să se folosească std::string și std::vector în loc de char* și vectori alocați manual; să se implementeze operatorul de stream (<<) pentru afișare; să se utilizeze date și funcții statice unde este cazul (de exemplu, pentru a gestiona stări comune ale unor entități); și să se demonstreze înțelegerea conceptelor POO prin utilizare reală și cu sens în contextul jocului.

---

## 2. Modul de Rezolvare

### 2.1 Arhitectura Claselor

#### **Clasa de Bază: `Entity`**
Clasa de bază (proprie, nu derivată din altă clasă predefinită) care definește interfața comună pentru toate entitățile jocului.

**Caracteristici:**
- **Atribute protejate:** 
  - `sf::Vector2f position` - poziția entității pe ecran
  - `static int entityCount` - variabilă statică pentru a urmări numărul total de entități create

- **Metode virtuale pure (private):**
  - `doUpdate()` - logică internă de actualizare
  - `doDraw(sf::RenderWindow&)` - logică internă de desenare
  - `getBounds()` - returnează dreptunghiul care încojură obiectul

- **Interfață non-virtuală (publică):**
  - `update()` - apelează `doUpdate()` (NVI - Non-Virtual Interface pattern)
  - `draw(sf::RenderWindow&)` - apelează `doDraw()`
  - `getEntityCount()` - metodă statică care returnează numărul de entități

- **Funcție virtuală de ajutor:**
  - `print(std::ostream&)` - pentru personalizarea afișării (implementată default)

- **Operator de stream:**
  - `operator<<` - afișează entitatea folosind metoda `print()` și moștenire virtuală

**Justificare:** Clasa de bază permite gestionarea uniformă a tuturor entităților prin pointeri de bază. Metodele statice urmăresc starea globală a sistemului fără a folosi variabile globale.

---

#### **Clasa Derivată 1: `Spaceship`**
Reprezintă nava spațială controlată de jucător.

**Atribute specifice:**
- `sf::Texture image` și `sf::Sprite sprite` - reprezentarea grafică
- `std::vector<std::shared_ptr<Laser>> lasers` - vectorul laserelor trase de navă
- `sf::Clock fireClock` - cronometru pentru controlul ratei de tragere
- `float fireInterval`, `bool speedBoostActive`, `bool cloneActive` - stări power-up
- `std::shared_ptr<Spaceship> shipClone` - pointer la nava secundară din power-up Clone

**Metode specifice:**
- `MoveLeft()`, `MoveRight()` - deplasare controlată de utilizator
- `fire()` - trage lasere (apelează `clone()` cu dynamic_cast pentru navă secundară)
- `activateSpeedBoost()`, `activateClone()` - activează power-up-uri
- `updatePowerUps()` - gestionează durata power-up-urilor

**Implementări virtuale:**
- `doUpdate()`, `doDraw()`, `getBounds()` - override-uri corespunzătoare

**Pattern copy-and-swap:**
- Constructor de copiere, operator= și funcție swap
- `clone() const` - returnează `std::shared_ptr<Spaceship>` (pentru a nu folosi polimorfismul întrucât nu este necesar pentru altele)

**Justificare:** Nava poate fi clonată (prin copy constructor și clone()), iar power-up-urile necesită gestionare de stări și timpi specifici.

---

#### **Clasa Derivată 2: `Alien`**
Reprezintă entitățile inamice care se deplasează și trag.

**Atribute specifice:**
- `int type` - tipul alienului (determină textura)
- `bool alive` - stare viață
- `sf::Texture texture` și `sf::Sprite sprite` - reprezentare grafică
- `static float speed` și `static int direction` - date statice pentru mișcare sincronă

**Metode specifice și statice:**
- `moveDown(float)` - coboară pe ecran
- `getRightEdge()`, `getLeftEdge()`, `getCenter()` - pozițiile de referință
- `static void changeDirection()` - inversează direcția pentru toți alienii odată

**Implementări virtuale:**
- `doUpdate()`, `doDraw()`, `getBounds()`

**Justificare:** Alienii trebuie să se mute sincron (de aceea `static` pentru viteză și direcție). Metoda `changeDirection()` modifica global direcția pentru toate entitățile de tipul Alien din cauza mecanicii jocului (când ating marginea, toți se întorc).

---

#### **Clasa Derivată 3: `Laser`**
Reprezintă proiectilele trase de navă sau alienii.

**Atribute specifice:**
- `float speed` - viteza deplasării pe verticală
- `bool active` - stare dacă laserul este activ
- `sf::RectangleShape shape` - geometria vizuală

**Metode virtuale:**
- `doUpdate()`, `doDraw()`, `getBounds()`

**Justificare:** Laser-urile sunt entități simple dar esențiale pentru mecaninica de coliziune. Sunt create prin `std::shared_ptr` și gestionate în vectori.

---

#### **Clasă de Bază Secundară: `PowerUp`**
O a 4-a clasă derivată din `Entity` care introduceți un alt nivel de ierarhie.

**Atribute:**
- `sf::RectangleShape shape` - reprezentare vizuală
- `bool active` - stare

**Metode virtuale pure:**
- `applyEffect(Spaceship&)` - efect specific al fiecărui bonus

**Implementări virtuale:**
- `doUpdate()`, `doDraw()`, `getBounds()`

---

#### **Clase Derivate din PowerUp:**
1. **`SpeedPowerUp`** - accelerează rata de tragere
   - `applyEffect()` - apelează `activateSpeedBoost()` pe navă

2. **`ClonePowerUp`** - creează o navă secundară care trage și ea
   - `applyEffect()` - apelează `activateClone()` pe navă

**Justificare:** Herența multiplă a nivelelor (Entity -> PowerUp -> SpeedPowerUp/ClonePowerUp) demonstrează polimorfismul în adâncime.

---

### 2.2 Gestionarea Excepțiilor

Implementată o ierarhie proprie de excepții derivată din `std::runtime_error`:

```cpp
class GameException : public std::runtime_error        // Bază proprie
class ResourceException : public GameException         // Erori de resurse
class GameplayException : public GameException         // Erori logică joc
class ConfigurationException : public GameException    // Erori configurație
```

**Utilizări:**
- `ResourceException` - aruncată în constructorii `Spaceship` și `Alien` dacă imaginile nu se încarcă
- `ConfigurationException` - pentru erori de configurare (dacă s-ar implementa)
- `GameplayException` - pentru erori de logică (extensibil)

**Try-catch în main():**
```cpp
try { ... }
catch (const ConfigurationException&) { ... }
catch (const ResourceException&) { ... }
catch (const GameException&) { ... }
catch (const std::exception&) { ... }
```

---

### 2.3 Utilizare Polimorfism și Dynamic Cast

#### **Pointeri de Bază cu Virtuale:**
În clasa `Game`, se gestiona o colecție:
```cpp
std::vector<std::shared_ptr<Entity>> allEntities;
```

Toate derivatele sunt stocate ca `shared_ptr<Entity>`, iar metodele virtuale sunt apelate prin acești pointeri:
```cpp
for (auto& entity : allEntities) {
    entity->update();           // Apel virtual
    entity->draw(window);       // Apel virtual
}
```

#### **Dynamic Cast cu Sens:**
```cpp
auto alienTarget = std::dynamic_pointer_cast<Alien>(entity);
if (alienTarget) {
    // Operații specifice alienilor (coliziuni, deszăpezire, etc.)
}

auto pUp = std::dynamic_pointer_cast<PowerUp>(entity);
if (pUp) {
    // Aplicare efect power-up
    pUp->applyEffect(spaceship);
}
```

**Justificare:** Downcast-ul este necesar pentru a executa operații specifice doar pentru anumite tipuri de entități fără a compromite design-ul polimorf.

---

### 2.4 Pattern Copy-and-Swap

Implementat pentru `Spaceship`:
```cpp
class Spaceship : public Entity {
    // Constructor de copiere
    Spaceship(const Spaceship& other);
    
    // Operator= cu copy-and-swap
    Spaceship& operator=(Spaceship other);
    
    // Funcție swap
    friend void swap(Spaceship& first, Spaceship& second) noexcept;
};
```

**Implementare:**
```cpp
Spaceship::Spaceship(const Spaceship& other) 
    : Entity(other), image(other.image), sprite(image) {
    sprite.setTexture(image, true);
    sprite.setPosition(position);
}

void swap(Spaceship& first, Spaceship& second) noexcept {
    using std::swap;
    swap(first.position, second.position);
    swap(first.image, second.image);
    first.sprite.setTexture(first.image, true);
    second.sprite.setTexture(second.image, true);
}

Spaceship& Spaceship::operator=(Spaceship other) {
    swap(*this, other);
    return *this;
}
```

**Justificare:** Pattern-ul copy-and-swap oferă garanție puternică de excepție și evită memory leaks.

---

### 2.5 Utilizare std::shared_ptr

Aplicat extensiv:
- `std::vector<std::shared_ptr<Laser>> lasers` - lasere din navă
- `std::vector<std::shared_ptr<Entity>> allEntities` - toate entitățile
- `std::vector<std::shared_ptr<Laser>> alienLasers` - lasere inamice
- `std::shared_ptr<Spaceship> shipClone` - nava secundară

**Avantaje:**
- Management automat al memoriei
- Evitarea memory leaks
- Ștergere automată când nu mai sunt referințe

---

### 2.6 Date și Funcții Statice

1. **`Entity::entityCount`** (static member)
   - Urmărește numărul total de entități viu în orice moment
   - Incrementat în constructor, decrementat în destructor

2. **`Entity::getEntityCount()`** (static method)
   - Returnează numărul curent de entități

3. **`Alien::speed`** (static member)
   - Viteza comună pentru toți alienii
   - Sincronizează mișcarea pe orizontală

4. **`Alien::direction`** (static member)
   - Direcție comună (1 pentru dreapta, -1 pentru stânga)

5. **`Alien::changeDirection()`** (static method)
   - Inversează direcția pentru toți alienii odată
   - Apelată când un alien atinge marginea

**Justificare:** Datele statice sincronizează comportamentul entităților de același tip fără a utiliza variabile globale.

---

### 2.7 Const-Correctness

Utilizat în mai multe locuri:
```cpp
bool isAlive() const { return alive; }
bool isActive() const { return active; }
sf::FloatRect getBounds() const override;
sf::Vector2f getCenter() const;
std::shared_ptr<Spaceship> clone() const;
static int getEntityCount();
virtual void print(std::ostream&) const;
```

---

### 2.8 Operator <<

Implementat cu friend function și metodă virtuală de ajutor:
```cpp
friend std::ostream& operator<<(std::ostream& os, const Entity& e) {
    e.print(os);
    return os;
}

virtual void print(std::ostream& os) const {
    os << "Entitate la pozitia [" << position.x << ", " << position.y << "]";
}
```

Permite afișarea poliorfă a tuturor entităților.

---

### 2.9 Funcții Virtuale Specifice Temei

1. **`doUpdate()` și `doDraw()`** (virtuale pure, private)
   - Logică specifică fiecărei entități
   - Apelate prin interfață non-virtuală `update()` și `draw()`

2. **`getBounds()`** (virtuală pură)
   - Returnează dreptunghiul care încojură obiectul
   - Utilizată pentru detecția coliziunilor

3. **`applyEffect()`** (virtuală pură în PowerUp)
   - Aplică efectul specific unui power-up
   - Implementată în `SpeedPowerUp` și `ClonePowerUp`

Toate sunt specifice temei și nu sunt simple citiri/afișări.

---

### 2.10 Evitarea Citirilor de la Tastatură

Jocul nu folosește `std::cin` pentru configurare. În schimb:
- Se folosește `sf::Keyboard::isKeyPressed()` doar pentru controlul în timp real al navei
- Parametrii de joc sunt hard-codați în constante
- Dimensiunile și pozițiile sunt stabilite la compilare

Dacă s-ar dori configurabilitate, s-ar folosi fișiere de configurare text, nu citiri interactive.

---

### 2.11 Elemente de Logică de Joc

#### **Actualizare și Desenare:**
- În `Game::Update()`: se actualizează poziții, se gestionează coliziunile
- În `Game::Draw()`: se desenează toate entitățile pe ecran

#### **Detecția Coliziunilor:**
- `Game::CheckForCollisions()` - verifică intersecțiile folosind `getBounds()`
- Cazuri: laser vs alien, laser inamicului vs navă, alien vs navă

#### **Logică de Câștig/Pierdere:**
- Jucător câștigă: eliminat toți alienii
- Jucător pierde: navă lovită de laser inamicului sau alien trece de ea

#### **Power-ups:**
- Apar random pe ecran
- Se colectează cu nava
- Au durată limitată (5 secunde)
- Tipuri: SpeedBoost (tragere mai rapidă) și Clone (navă secundară)

#### **Mișcare Sincronă:**
- Alienii se mută sincron pe orizontală (down odată ce ating marginea)
- Implementat cu date statice în clasa `Alien`

---

## 3. Conformitate cu Cerințele

| Cerință | Status | Detalii |
|---------|--------|---------|
| 1a. Split header/source | ✅ | Toate clasele au `.h` și `.cpp` separate |
| 1b. Clase mici în aceeași pereche | ✅ | `PowerUp.h/cpp` conține PowerUp și derivatele |
| 2a. Ierarhie (1 bază + 3+ derivate) | ✅ | Entity (bază) + Spaceship, Alien, Laser, PowerUp (4 derivate) |
| 2b. Bază proprie | ✅ | Entity nu este derivată din clasă predefinită |
| 2c. Virtuale și NVI | ✅ | Virtuale pure `doUpdate()`, `doDraw()`, `getBounds()` cu NVI |
| 2d. Constructor bază în derivate | ✅ | Toți constructorii derivatelor apelează Entity() |
| 2e. Pointer bază + downcast | ✅ | `vector<shared_ptr<Entity>>` cu `dynamic_pointer_cast` |
| 2e.1 Copy constructor/swap | ✅ | Implementat pentru Spaceship |
| 2e.2 dynamic_cast | ✅ | Utilizat în `Game::Update()` și `CheckForCollisions()` |
| 2e.3 std::string și vector | ✅ | Fără char* și vectori manuali |
| 2e.4 shared_ptr | ✅ | Utilizat pentru toate entitățile |
| 3a. Ierarhie excepții | ⚠️ | GameException, ResourceException, GameplayException, ConfigurationException |
| 3b. Throw/catch | ✅ | Throw în constructori, catch în main |
| 4. Date și funcții statice | ✅ | Entity::entityCount, Alien::speed, Alien::direction |
| 5. Const | ✅ | Multiple metode const |
| 6. Funcții nivel înalt | ✅ | Fire, MoveLeft, MoveRight, activateClone, etc. |
| 7. Operator << | ✅ | Implementat cu metodă virtuală print() |
| 8. Fără citiri tastatură | ✅ | Doar control real-time cu sf::Keyboard |
| 9. README | ✅ | Acest document |
| 11. Fără globale/cod fără sens | ✅ | Doar static members în clase |

---

## 4. Compilare și Rulare

### Dependențe:
- SFML 2.5+ (Simple and Fast Multimedia Library)
- C++17 sau mai nou
- CMake (opțional)

### Compilare:
```bash
mkdir build
cd build
cmake ..
make
```

### Rulare:
```bash
./untitled
```

### Fișiere Necesare:
- `Graphics/spaceship.png`
- `Graphics/alien_1.png`, `alien_2.png`, `alien_3.png`

---

## 5. Bibliografie și Resurse

1. **SFML Documentation**: https://www.sfml-dev.org/documentation/2.5.1/
2. **C++ Reference - std::shared_ptr**: https://en.cppreference.com/w/cpp/memory/shared_ptr
3. **C++ Reference - std::vector**: https://en.cppreference.com/w/cpp/container/vector
4. **Modern C++ Design Patterns**:
   - Non-Virtual Interface (NVI)
   - Copy-and-Swap
   - RAII (Resource Acquisition Is Initialization)
5. **Exception Handling in C++**: https://en.cppreference.com/w/cpp/language/exceptions
6. **Dynamic Casting in C++**: https://en.cppreference.com/w/cpp/language/dynamic_cast

---

## 6. Observații și Extinderi Posibile

1. **Configurabilitate:** S-ar putea adăuga un fișier de configurare (game_config.txt) pentru parametri ca viteza, intervale de foc, etc.
2. **Level-uri:** Ar putea fi implementate nivele cu dificultate crescândă
3. **Score:** Sistem de punctaj pentru inamici eliminați
4. **Animații:** Efecte vizuale pentru coliziuni, explosii, etc.
5. **Sunet:** Adăugare de efecte sonore cu SFML Audio

---

**Autor:** Alexandra  
**Data:** aprilie 2026  
**Limbaj:** C++ (C++17)  
**Framework:** SFML 2.5+
