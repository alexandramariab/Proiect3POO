# Space Invaders - Implementare POO în C++ (Proiect 3)

# Review — Proiect 3 POO: Space Invaders
Proiectul 3 la Programare Orientată pe Obiecte este versiunea finală și cea mai complexă a jocului Space Invaders implementat în C++, folosind biblioteca grafică SFML. Ideea jocului a rămas aceeași ca în proiectele anterioare: jucătorul controlează o navă spațială care trebuie să distrugă o formație de inamici înainte ca aceștia să ajungă la ea. Alienii se mișcă împreună pe orizontală, își schimbă direcția când ating marginea ecranului, coboară treptat și trag proiectile aleatoriu. Pe lângă jocul clasic, implementarea include și obiecte bonus (power-ups) care cad din partea de sus a ecranului și pot fi colectate pentru avantaje temporare. Față de proiectele anterioare, Proiectul 3 adaugă mai multe elemente noi, precum clase și funcții template, design pattern-uri și o structură mai clară și mai ușor de extins.

Baza întregului proiect este clasa abstractă Entity, care servește drept clasă de bază pentru toate entitățile din joc. Aceasta nu moștenește nicio clasă predefinită și definește interfața comună prin care orice entitate poate fi actualizată și desenată pe ecran. Mai exact, Entity conține două metode virtuale pure private: doUpdate() și doDraw(), apelate prin metodele publice update() și draw(), după modelul Non-Virtual Interface. Această separare dintre interfața publică și implementarea privată permite clasei de bază să controleze fluxul de execuție fără să expună detaliile interne. Tot în Entity există și atributul static entityCount, care este incrementat în constructor și decrementat în destructor și urmărește numărul total de entități existente în joc, fără a folosi variabile globale. În acest proiect, clasa de bază declară și metoda virtuală pură clone() const, ceea ce obligă toate clasele derivate să implementeze clonarea polimorfă și stă la baza pattern-ului Prototype folosit în proiect.

De la Entity derivează direct patru clase principale: Spaceship, Alien, Laser și PowerUp, iar din PowerUp derivează mai departe SpeedPowerUp și ClonePowerUp. Toate entitățile sunt gestionate prin pointeri de bază de tipul shared_ptr<Entity>, stocați într-un vector unic allEntities din clasa Game. Aceasta înseamnă că aceeași buclă de actualizare și desenare funcționează corect pentru orice tip de entitate, fără cod separat pentru fiecare tip. Acolo unde este nevoie de comportament specific unui subtip, de exemplu, pentru a verifica dacă un alien a atins marginea ecranului sau dacă un power-up a fost colectat, se folosește dynamic_pointer_cast, cu verificarea rezultatului înainte de orice operație.

Spaceship reprezintă nava controlată de jucător și este, probabil, clasa cu cele mai multe responsabilități. Mișcarea navei este controlată prin sf::Keyboard::isKeyPressed(), apelat din Game::HandleInput(), care apelează MoveLeft() sau MoveRight(). Tragerea se face prin metoda fire(), care respectă un interval minim de timp între focuri pentru a nu permite tragerea continuă necontrolată. În plus, nava implementează pattern-ul copy-and-swap: are un constructor de copiere explicit, un operator de atribuire care primește parametrul prin valoare, și o funcție swap() declarată friend, asigurând că orice copier, inclusiv cea realizată la activarea power-up-ului Clone, replică corect starea internă, inclusiv textura SFML. Față de proiectul 2, Spaceship nu mai apelează direct loadFromFile() pentru a-și încărca textura, ci lasă această responsabilitate clasei ResourceManager<sf::Texture>.

Clasa inamicilor, “Alien”, modelează inamicii din formație și folosește membri statici într-un mod foarte util: viteza și direcția de deplasare sunt comune pentru toate instanțele, iar când un alien atinge marginea ecranului, metoda statică changeDirection() schimbă direcția pentru toți în același timp. Textura este acum gestionată prin ResourceManager<sf::Texture>, ceea ce înseamnă că dacă există 50 de alienii de tipul 1, textura alien_1.png se încarcă o singură dată din disc și este reutilizată de toate instanțeleâ, ceea ce reprezintă o îmbunătățire importantă față de proiectul 2, unde fiecare obiect Alien își încărca propria copie a texturii.

Una dintre cele mai importante modificări față de proiectul 2 este transformarea clasei Laser într-o clasă template, Laser<T>. Tipul generic T reprezintă tipul numeric al vitezei: atributul T speed este atributul de tip generic cerut de temă, iar constructorul Laser(sf::Vector2f pos, T speedVal) este funcția membră dependentă de T. În practică, clasa este folosită cu argumentul implicit, Laser<>, care corespunde lui Laser<float>. Nu există un impact vizibil din perspectiva jucătorului, dar arhitectura devine generalizată: dacă s-ar dori precizie mai mare pentru viteză, s-ar putea folosi Laser<double> fără nicio modificare a codului clasei. În plus, toți vectorii care rețineau lasere au fost actualizați corespunzător, atât în Spaceship, cât și în Game: std::vector<std::shared_ptr<Laser<>>> lasers; std::vector<std::shared_ptr<Laser<>>> alienLasers;.

Legată de Laser este și funcția liberă șablon isOut<T>(), definită în Entity.h în afara oricărei clase. Aceasta verifică dacă o valoare numerică a depășit o limită dată, ținând cont de direcția de mișcare. Este folosită în două locuri cu sens: în Laser<T>::doUpdate() pentru a dezactiva laserul când iese din ecran, și în PowerUp::doUpdate() pentru a elimina bonusul care a coborât sub ecran fără să fie colectat. Fără această funcție, același cod de verificare a limitelor ar fi trebuit duplicat în ambele clase, lucru pe care o funcție template îl rezolvă mai ușor.

În ceea ce privește design pattern-urile, proiectul implementează trei, fiecare cu un rol bine definit. Factory Method este implementat în clasa AlienFactory, prin metoda statică createAlien(float x, float y, int row), care decide ce tip de alien să creeze în funcție de rândul din formație : rândul 0 produce alienii de tipul 3, rândurile 1 și 2 tipul 2, iar rândurile 3 și 4 tipul 1. Față de proiectul 2, unde această logică era inline în Game::initAliens(), ea este acum izolată într-o clasă dedicată, astfel încât orice schimbare în regulile de distribuție a tipurilor de alienii nu afectează logica generală a jocului.

Al doilea pattern, Singleton, este implementat în ResourceManager<T>. Instanța unică este garantată prin constructorul privat și prin metoda statică getInstance(), care folosește un static local. Clasa stochează resursele deja încărcate într-un std::map<std::string, T> indexat după un nume simbolic. La primul apel load(), resursa este încărcată din disc; la apelurile ulterioare cu aceeași cheie, metoda detectează că resursa există deja și nu mai accesează discul. Dacă încărcarea eșuează sau dacă se cere o resursă inexistentă, se aruncă ResourceException.

Al treilea pattern, Prototype, a fost extins față de proiectul 2. Acolo, clone() exista doar în Spaceship, fără să fie declarată în Entity. Acum, clone() const este pur virtuală în clasa de bază, ceea ce înseamnă că nicio clasă derivată nu poate exista fără să o implementeze. Utilizarea cu sens real se găsește în Spaceship::activateClone(): atunci când jucătorul colectează un ClonePowerUp, metoda clone() este apelată pe navă prin polimorfism, rezultatul este downcast-at cu dynamic_pointer_cast<Spaceship>, și se obține o copie funcțională a navei care se poziționează la cincizeci de pixeli distanță și trage independent. Faptul că clone() este virtuală permite ca această logică să fie scrisă generic în Entity, indiferent de tipul concret al obiectului.

Gestionarea excepțiilor, prezentă și în proiectul 2, este mai bine utilizată în proiectul 3 datorită centralizării în ResourceManager. Ierarhia proprie: GameException ca bază, cu ResourceException, GameplayException și ConfigurationException ca derivate, asigură că erorile de resurse au un singur punct de origine și pot fi capturate selectiv în main(). Blocul try-catch capturează excepțiile în ordinea corectă, de la cel mai specific la cel mai general, returnând coduri de eroare distincte pentru fiecare categorie de problemă.

Față de proiectul 2, în proiectul 3 au fost adăugate mai multe elemente noi. Clasa Laser a fost transformată în clasă template Laser<T>, cu atribut și funcție membră dependente de tipul generic, iar funcția liberă template isOut<T>() a fost introdusă și folosită în două clase diferite. A fost adăugată și clasa ResourceManager<T>, un Singleton template care se ocupă de încărcarea și stocarea resurselor grafice, astfel încât aceeași textură să nu fie încărcată de mai multe ori din disc. AlienFactory separă logica de creare a alienilor de restul codului din Game, prin pattern-ul Factory Method. În plus, metoda clone() a fost mutată în clasa de bază Entity ca metodă pur virtuală, ceea ce înseamnă că toate clasele derivate sunt obligate să o implementeze, extinzând astfel pattern-ul Prototype la nivelul întregii ierarhii.

În concluzie, proiectul acoperă cerințele temei și implementează un joc funcțional cu o structura bine organizată. Polimorfismul și pointerii de bază fac codul ușor de extins, astfel încât dacă s-ar adăuga un nou tip de entitate sau un nou power-up, modificările în codul existent ar fi minime, ceea ce era și scopul principal al întregii structuri.

## 1. Enunțul Problemei

### Ce se dă:
Se cere implementarea unui joc **Space Invaders** folosind principiile Programării Orientate pe Obiecte. Jocul conține mai multe entități cu comportamente distincte care interacționează între ele: o navă spațială controlată de utilizator, alienii inamici care se deplasează și trag proiectile, laserele trase de ambele tabere, și obiecte cu efecte speciale (power-ups). Toate entitățile sunt caracterizate prin poziție pe ecran și comportament specific. Entitățile trebuie să se poată actualiza și desena pe ecran, iar interacțiunile (coliziunile) trebuie gestionate corespunzător. Jocul are logică de victorie (eliminarea tuturor inamicilor) și logică de înfrângere (lovirea navei sau trecerea inamicilor de ea).

### Ce se cere (Proiect 3 - cerințe adăugate față de Proiect 2):
Față de cerințele anterioare, în proiectul 3 se adaugă: utilizarea **claselor șablon (template)** cu cel puțin un atribut și o funcție membră dependentă de tipul generic; utilizarea **funcțiilor libere șablon** (template); implementarea unui **Design Pattern creațional** (Factory Method); implementarea unui **Design Pattern structural sau comportamental** (Prototype și Singleton); și utilizarea unui **Design Pattern structural suplimentar** (prin clasa șablon `ResourceManager<T>` ca Singleton). Toate elementele noi trebuie să fie utilizate cu sens real în contextul jocului, nu artificial.

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

- **DESIGN PATTERN - Prototype (metodă pură nouă):**
  - `clone() const` - metodă virtuală pură care forțează toate derivatele să implementeze clonarea poliorfă

**Justificare:** Clasa de bază permite gestionarea uniformă a tuturor entităților prin pointeri de bază. Metodele statice urmăresc starea globală a sistemului fără a folosi variabile globale. Metoda `clone()` face parte din pattern-ul Prototype aplicat la nivelul întregii ierarhii.

---

#### **Clasa Derivată 1: `Spaceship`**
Reprezintă nava spațială controlată de jucător.

**Atribute specifice:**
- `sf::Texture image` și `sf::Sprite sprite` - reprezentarea grafică
- `std::vector<std::shared_ptr<Laser<>>> lasers` - vectorul laserelor trase de navă (**acum Laser este clasă șablon**)
- `sf::Clock fireClock` - cronometru pentru controlul ratei de tragere
- `float fireInterval`, `bool speedBoostActive`, `bool cloneActive` - stări power-up
- `std::shared_ptr<Spaceship> shipClone` - pointer la nava secundară din power-up Clone

**Metode specifice:**
- `MoveLeft()`, `MoveRight()` - deplasare controlată de utilizator
- `fire()` - trage lasere (instanțiază `Laser<>` cu argumentul implicit)
- `activateSpeedBoost()`, `activateClone()` - activează power-up-uri
- `updatePowerUps()` - gestionează durata power-up-urilor

**Implementări virtuale:**
- `doUpdate()`, `doDraw()`, `getBounds()` - override-uri corespunzătoare
- `clone() const` - returnează `std::make_shared<Spaceship>(*this)` (Prototype)

**Pattern copy-and-swap:**
- Constructor de copiere, operator= și funcție swap

**Justificare:** Nava poate fi clonată (prin copy constructor și `clone()`), iar power-up-urile necesită gestionare de stări și timpi specifici. Utilizează `ResourceManager<sf::Texture>` pentru încărcarea texturii.

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
- `clone() const` - returnează `std::make_shared<Alien>(*this)` (Prototype)

**Justificare:** Alienii trebuie să se mute sincron (de aceea `static` pentru viteză și direcție). Utilizează acum `ResourceManager<sf::Texture>` în loc să încarce direct textura, evitând reîncărcări inutile din disc.

---

#### **Clasa Derivată 3: `Laser<T>`** *(acum clasă șablon)*
Reprezintă proiectilele trase de navă sau alienii.

**Atribut șablon:**
- `T speed` - viteza deplasării pe verticală (**atribut de tip T — cerință clasă template**)

**Alte atribute specifice:**
- `bool active` - stare dacă laserul este activ
- `sf::RectangleShape shape` - geometria vizuală

**Funcție membră dependentă de T:**
- `Laser(sf::Vector2f pos, T speedVal)` - constructorul primește viteza ca parametru de tip T

**Implementări virtuale:**
- `doUpdate()` - folosește `static_cast<float>(speed)` și funcția liberă șablon `isOut<T>`
- `doDraw()`, `getBounds()`
- `clone() const` - returnează `std::make_shared<Laser<T>>(*this)` (Prototype)

**Utilizare:**
```cpp
// Cu argumentul implicit (float)
std::make_shared<Laser<>>(position, -10.f);  // lasere navă (viteza negativă = sus)
std::make_shared<Laser<>>(position, 5.f);    // lasere alien (viteza pozitivă = jos)
```

**Justificare:** Transformarea în clasă șablon permite utilizarea diferitelor tipuri numerice pentru viteză (float, double etc.). În practică se folosește `float` (argumentul implicit), dar arhitectura este generalizată.

---

#### **Clasă de Bază Secundară: `PowerUp`**
O clasă derivată din `Entity` care introduce un alt nivel de ierarhie.

**Atribute:**
- `sf::RectangleShape shape` - reprezentare vizuală
- `bool active` - stare

**Metode virtuale pure:**
- `applyEffect(Spaceship&)` - efect specific al fiecărui bonus

**Implementări virtuale:**
- `doUpdate()` - folosește funcția liberă șablon `isOut()`
- `doDraw()`, `getBounds()`

---

#### **Clase Derivate din PowerUp:**
1. **`SpeedPowerUp`** - accelerează rata de tragere
   - `applyEffect()` - apelează `activateSpeedBoost()` pe navă
   - `clone() const` - returnează `std::make_shared<SpeedPowerUp>(*this)` (Prototype)

2. **`ClonePowerUp`** - creează o navă secundară care trage și ea
   - `applyEffect()` - apelează `activateClone()` pe navă
   - `clone() const` - returnează `std::make_shared<ClonePowerUp>(*this)` (Prototype)

**Justificare:** Ierarhia multiplă de niveluri (Entity → PowerUp → SpeedPowerUp/ClonePowerUp) demonstrează polimorfismul în adâncime. Ambele derivate implementează `clone()` conform pattern-ului Prototype.

---

#### **Clasa Șablon: `ResourceManager<T>`** *(nouă în Proiect 3)*
Clasă șablon care implementează simultan **Singleton** și un manager de resurse generic.

**Atribute:**
- `std::map<std::string, T> resources` - dicționar de resurse indexate după nume

**Design Pattern Singleton:**
- Constructor privat, fără constructor de copiere, fără operator=
- `static ResourceManager& getInstance()` - returnează instanța unică prin static local

**Funcții membre dependente de T:**
- `void load(const std::string& name, const std::string& filepath)` - încarcă resursa de tip T din fișier
- `T& get(const std::string& name)` - returnează resursa stocată

**Gestionarea erorilor:**
- Dacă `loadFromFile()` eșuează, aruncă `ResourceException`
- Dacă se cere o resursă neîncărcată, aruncă `ResourceException`

**Utilizare:**
```cpp
auto& manager = ResourceManager::getInstance();
manager.load("spaceship", "Graphics/spaceship.png");
sf::Texture& tex = manager.get("spaceship");
```

**Justificare:** Evită reîncărcarea acelorași texturi de pe disc la fiecare instanțiere de `Alien`. Toți alienii de același tip partajează textura deja încărcată.

---

#### **Clasa: `AlienFactory`** *(nouă în Proiect 3)*
Implementează **Design Pattern Factory Method** pentru crearea alienilor.

**Metodă statică:**
```cpp
static std::shared_ptr createAlien(float x, float y, int row);
```

**Logica de creare:**
- Rând 0 → `alienType = 3` (cel mai puternic, grafic diferit)
- Rânduri 1–2 → `alienType = 2`
- Rânduri 3–4 → `alienType = 1`

**Utilizare în `Game::initAliens()`:**
```cpp
allEntities.push_back(AlienFactory::createAlien(x, y, row));
```

**Justificare:** Centralizează logica de configurare a tipului de alien în funcție de poziția sa în formație, izolând-o față de restul logicii de joc.

---

### 2.2 Funcție Liberă Șablon (`isOut<T>`)

Definită în `Entity.h`, în afara oricărei clase:

```cpp
template 
bool isOut(T currentY, T limitY, bool movingDown = true) {
    if (movingDown) return currentY > limitY;
    return currentY < limitY;
}
```

**Utilizări cu sens:**
- În `Laser<T>::doUpdate()` - verifică dacă laserul a ieșit din ecran (sus sau jos)
- În `PowerUp::doUpdate()` - verifică dacă power-up-ul a coborât sub ecran

**Justificare:** Funcția este utilă pentru orice tip numeric (float, double, int) și evită duplicarea codului de verificare a limitelor de ecran.

---

### 2.3 Design Patterns Implementate

#### **1. Prototype**
Metodă `clone() const` declarată pur virtuală în `Entity` și implementată în toate derivatele:

| Clasă | Implementare |
|-------|-------------|
| `Spaceship` | `std::make_shared<Spaceship>(*this)` |
| `Alien` | `std::make_shared<Alien>(*this)` |
| `Laser<T>` | `std::make_shared<Laser<T>>(*this)` |
| `SpeedPowerUp` | `std::make_shared<SpeedPowerUp>(*this)` |
| `ClonePowerUp` | `std::make_shared<ClonePowerUp>(*this)` |

**Utilizare reală:** Power-up-ul Clone apelează `this->clone()` pe navă pentru a crea nava secundară:
```cpp
auto duplicated = this->clone();
shipClone = std::dynamic_pointer_cast(duplicated);
```

#### **2. Singleton**
Implementat în `ResourceManager<T>`:
```cpp
static ResourceManager& getInstance() {
    static ResourceManager instance; // Inițializare thread-safe în C++11+
    return instance;
}
```

#### **3. Factory Method**
Implementat în `AlienFactory::createAlien()` - returnează `shared_ptr<Alien>` configurat în funcție de parametrul `row`.

#### **4. Non-Virtual Interface (NVI)**
Metodele publice `update()` și `draw()` din `Entity` apelează `doUpdate()` și `doDraw()` private — pattern deja prezent din Proiect 2, menținut și extins.

---

### 2.4 Gestionarea Excepțiilor

Implementată o ierarhie proprie de excepții derivată din `std::runtime_error`:

```cpp
class GameException : public std::runtime_error        // Bază proprie
class ResourceException : public GameException         // Erori de resurse
class GameplayException : public GameException         // Erori logică joc
class ConfigurationException : public GameException    // Erori configurație
```

**Utilizări:**
- `ResourceException` - aruncată în `ResourceManager<T>::load()` și `get()` dacă imaginile nu se încarcă sau nu există
- `ConfigurationException` - pentru erori de configurare (extensibil)
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

### 2.5 Utilizare Polimorfism și Dynamic Cast

#### **Pointeri de Bază cu Virtuale:**
În clasa `Game`, se gestionează o colecție:
```cpp
std::vector<std::shared_ptr> allEntities;
```

Toate derivatele sunt stocate ca `shared_ptr<Entity>`, iar metodele virtuale sunt apelate prin acești pointeri:
```cpp
for (auto& entity : allEntities) {
    entity->update();     // Apel virtual
    entity->draw(window); // Apel virtual
}
```

#### **Dynamic Cast cu Sens:**
```cpp
auto alienTarget = std::dynamic_pointer_cast(entity);
if (alienTarget) { /* coliziuni, mișcare */ }

auto pUp = std::dynamic_pointer_cast(entity);
if (pUp) { pUp->applyEffect(spaceship); }

// Prototype + dynamic_cast pentru nava clonată
auto duplicated = this->clone();
shipClone = std::dynamic_pointer_cast(duplicated);
```

---

### 2.6 Pattern Copy-and-Swap

Implementat pentru `Spaceship`:
```cpp
class Spaceship : public Entity {
    Spaceship(const Spaceship& other);           // Constructor de copiere
    Spaceship& operator=(Spaceship other);       // Operator= cu copy-and-swap
    friend void swap(Spaceship&, Spaceship&) noexcept;
};
```

**Implementare swap:**
```cpp
void swap(Spaceship& first, Spaceship& second) noexcept {
    using std::swap;
    swap(first.position, second.position);
    swap(first.image, second.image);
    first.sprite.setTexture(first.image, true);
    second.sprite.setTexture(second.image, true);
}
```

---

### 2.7 Utilizare std::shared_ptr

Aplicat extensiv:
- `std::vector<std::shared_ptr<Laser<>>> lasers` - lasere din navă (clasă șablon)
- `std::vector<std::shared_ptr<Entity>> allEntities` - toate entitățile
- `std::vector<std::shared_ptr<Laser<>>> alienLasers` - lasere inamice (clasă șablon)
- `std::shared_ptr<Spaceship> shipClone` - nava secundară

**Avantaje:**
- Management automat al memoriei
- Evitarea memory leaks
- Ștergere automată când nu mai există referințe

---

### 2.8 Date și Funcții Statice

1. **`Entity::entityCount`** (static member) - urmărește numărul total de entități vii
2. **`Entity::getEntityCount()`** (static method) - returnează numărul curent de entități
3. **`Alien::speed`** (static member) - viteza comună pentru toți alienii
4. **`Alien::direction`** (static member) - direcție comună (1 dreapta, -1 stânga)
5. **`Alien::changeDirection()`** (static method) - inversează direcția pentru toți alienii odată
6. **`ResourceManager<T>::getInstance()`** (static method) - returnează instanța Singleton

**Justificare:** Datele statice sincronizează comportamentul entităților de același tip fără a utiliza variabile globale.

---

### 2.9 Const-Correctness

Utilizat în mai multe locuri:
```cpp
bool isAlive() const { return alive; }
bool isActive() const { return active; }
sf::FloatRect getBounds() const override;
sf::Vector2f getCenter() const;
std::shared_ptr clone() const;
static int getEntityCount();
virtual void print(std::ostream&) const;
```

---

### 2.10 Operator <<

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
- Alienii se mută sincron pe orizontală; coboară odată ce ating marginea
- Implementat cu date statice în clasa `Alien`

---

## 3. Conformitate cu Cerințele

| Cerință | Status | Detalii |
|---------|--------|---------|
| 1a. Split header/source | ✅ | Toate clasele au `.h` și `.cpp` separate |
| 1b. Clase mici în aceeași pereche | ✅ | `PowerUp.h/cpp` conține PowerUp și derivatele; `AlienFactory.h` only-header |
| 2a. Ierarhie (1 bază + 3+ derivate) | ✅ | Entity + Spaceship, Alien, Laser\<T\>, PowerUp (4 derivate) |
| 2b. Bază proprie | ✅ | Entity nu este derivată din clasă predefinită |
| 2c. Virtuale și NVI | ✅ | Virtuale pure `doUpdate()`, `doDraw()`, `getBounds()`, `clone()` cu NVI |
| 2d. Constructor bază în derivate | ✅ | Toți constructorii derivatelor apelează Entity() |
| 2e. Pointer bază + downcast | ✅ | `vector<shared_ptr<Entity>>` cu `dynamic_pointer_cast` |
| 2e.1 Copy constructor/swap | ✅ | Implementat pentru Spaceship |
| 2e.2 dynamic_cast | ✅ | Utilizat în `Game::Update()`, `CheckForCollisions()`, `activateClone()` |
| 2e.3 std::string și vector | ✅ | Fără char* și vectori manuali |
| 2e.4 shared_ptr | ✅ | Utilizat pentru toate entitățile |
| 3a. Ierarhie excepții | ✅ | GameException, ResourceException, GameplayException, ConfigurationException |
| 3b. Throw/catch | ✅ | Throw în ResourceManager, catch în main |
| 4. Date și funcții statice | ✅ | Entity::entityCount, Alien::speed, Alien::direction, ResourceManager::getInstance() |
| 5. Const | ✅ | Multiple metode const |
| 6. Funcții nivel înalt | ✅ | Fire, MoveLeft, MoveRight, activateClone, applyEffect etc. |
| 7. Operator << | ✅ | Implementat cu metodă virtuală print() |
| 8. Fără citiri tastatură | ✅ | Doar control real-time cu sf::Keyboard |
| 9. README | ✅ | Acest document |
| 11. Fără globale/cod fără sens | ✅ | Doar static members în clase |
| **P3: Clasă șablon cu atribut T** | ✅ | `Laser<T>` cu `T speed`; `ResourceManager<T>` cu `map<string, T>` |
| **P3: Funcție membră dependentă de T** | ✅ | Constructor `Laser(pos, T speedVal)`, `load()` și `get()` în ResourceManager |
| **P3: Funcție liberă șablon** | ✅ | `isOut<T>()` în Entity.h, utilizată în Laser și PowerUp |
| **P3: Design Pattern Factory** | ✅ | `AlienFactory::createAlien()` |
| **P3: Design Pattern Prototype** | ✅ | `clone()` pur virtual în Entity, implementat în toate derivatele |
| **P3: Design Pattern Singleton** | ✅ | `ResourceManager<T>::getInstance()` |

---

## 4. Ce s-a Adăugat față de Proiect 2

### 4.1 Clasa Șablon `Laser<T>`
**Înainte (P2):** `Laser` era o clasă obișnuită cu `float speed`.  
**Acum (P3):** `Laser<T>` este o **clasă template** cu `T speed` ca atribut generic și constructorul `Laser(sf::Vector2f, T)` ca funcție membră dependentă de T. Se instanțiază cu `Laser<>` (argumentul implicit `float`) în tot codul.

Toate vectorii care rețineau `Laser` au fost actualizați:
```cpp
// Înainte
std::vector<std::shared_ptr> lasers;
// Acum
std::vector<std::shared_ptr<Laser<>>> lasers;
```

### 4.2 Funcție Liberă Șablon `isOut<T>`
**Înainte (P2):** Nu exista.  
**Acum (P3):** Funcție template definită în `Entity.h`, utilizată în `Laser<T>::doUpdate()` și `PowerUp::doUpdate()` pentru a verifica ieșirea din ecran.

### 4.3 Clasa Șablon `ResourceManager<T>` (Singleton)
**Înainte (P2):** `Spaceship` și `Alien` încărcau textura direct în constructor cu `loadFromFile()`.  
**Acum (P3):** Toți apelează `ResourceManager<sf::Texture>::getInstance()`, care:
- Reține texturile deja încărcate în `std::map`
- Evită reîncărcarea repetată a acelorași fișiere
- Aruncă `ResourceException` în caz de eșec
- Este un **Singleton** complet (constructor privat, fără copiere)

### 4.4 Design Pattern Factory Method — `AlienFactory`
**Înainte (P2):** Logica de creare a alienilor era inline în `Game::initAliens()`.  
**Acum (P3):** Mutată în clasa `AlienFactory` cu metoda statică `createAlien(x, y, row)` care selectează tipul corect de alien în funcție de rândul din formație.

### 4.5 Design Pattern Prototype — `clone()` în toată ierarhia
**Înainte (P2):** `clone()` exista doar în `Spaceship`, fără a fi declarată în `Entity`.  
**Acum (P3):** `clone() const` este declarată **pur virtuală în `Entity`**, forțând toate derivatele să o implementeze. Power-up-urile `SpeedPowerUp` și `ClonePowerUp` au acum și ele `clone()`. Metoda este utilizată real în `activateClone()` pentru duplicarea navei.

---

## 5. Compilare și Rulare

### Dependențe:
- SFML 3.0.2 (descărcat automat prin CMake FetchContent)
- C++17 sau mai nou
- CMake 3.28+

### Compilare:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Rulare:
```bash
./untitled
```

### Fișiere Necesare:
- `Graphics/spaceship.png`
- `Graphics/alien_1.png`, `alien_2.png`, `alien_3.png`

---

## 6. Bibliografie și Resurse

1. **SFML 3 Documentation**: https://www.sfml-dev.org/documentation/3.0.0/
2. **C++ Reference - Templates**: https://en.cppreference.com/w/cpp/language/templates
3. **C++ Reference - std::shared_ptr**: https://en.cppreference.com/w/cpp/memory/shared_ptr
4. **Modern C++ Design Patterns:**
   - Non-Virtual Interface (NVI)
   - Copy-and-Swap
   - Singleton
   - Factory Method
   - Prototype
5. **Exception Handling in C++**: https://en.cppreference.com/w/cpp/language/exceptions
6. **Dynamic Casting in C++**: https://en.cppreference.com/w/cpp/language/dynamic_cast
