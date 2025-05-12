#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <limits>
#include <fstream>

using namespace std;

// ANSI Colors
#define RESET     "\033[0m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define CYAN      "\033[36m"
#define BOLD      "\033[1m"

class Carta {
public:
    string valor;
    string palo;

    Carta(string v, string p) : valor(v), palo(p) {}

    int obtenerValor(int sumaActual = 0) const {
        if (valor == "A") return (sumaActual + 11 <= 21) ? 11 : 1;
        if (valor == "K" || valor == "Q" || valor == "J") return 10;
        return stoi(valor);
    }

    string comoTexto() const {
        return valor + " de " + palo;
    }
};

class Mazo {
private:
    vector<Carta> cartas;
    size_t actual = 0;

public:
    Mazo() {
        vector<string> palos = {"Corazones", "Diamantes", "Tréboles", "Picas"};
        vector<string> valores = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
        for (auto& p : palos)
            for (auto& v : valores)
                cartas.emplace_back(v, p);
        mezclar();
    }

    void mezclar() {
        shuffle(cartas.begin(), cartas.end(), default_random_engine(time(nullptr)));
        actual = 0;
    }

    Carta repartir() {
        if (actual >= cartas.size()) {
            mezclar();
        }
        return cartas[actual++];
    }

    void reiniciar() {
        actual = 0;
        mezclar();
    }
};

class Jugador {
protected:
    vector<Carta> mano;
    int puntos = 0;

public:
    string nombre;
    bool activo = true;

    Jugador(string n) : nombre(n) {}

    virtual void jugar(Mazo& mazo) = 0;

    void recibirCarta(const Carta& carta) {
        mano.push_back(carta);
        cout << CYAN << nombre << RESET << " recibe: " << carta.comoTexto() << endl;
    }

    int calcularMano() const {
        int total = 0;
        for (const auto& carta : mano) {
            total += carta.obtenerValor(total);
        }
        return total;
    }

    bool tieneBlackjack() const {
        return mano.size() == 2 && calcularMano() == 21;
    }

    void limpiarMano() {
        mano.clear();
    }

    void mostrar(bool ocultarPrimera = false) const {
        cout << YELLOW << nombre << RESET << " tiene: ";
        for (size_t i = 0; i < mano.size(); ++i) {
            if (i == 0 && ocultarPrimera)
                cout << "[CARTA OCULTA] ";
            else
                cout << mano[i].comoTexto() << " ";
        }
        cout << "→ Total: " << (ocultarPrimera ? "??" : to_string(calcularMano())) << endl;
    }

    void agregarPunto() {
        puntos++;
    }

    int getPuntos() const {
        return puntos;
    }
};

class JugadorBot : public Jugador {
public:
    JugadorBot(string n) : Jugador(n) {}

    void jugar(Mazo& mazo) override {
        while (activo) {
            int total = calcularMano();
            if (total <= 11) {
                recibirCarta(mazo.repartir());
            } else if (total >= 12 && total <= 18) {
                if (rand() % 2) recibirCarta(mazo.repartir());
                else break;
            } else break;

            if (calcularMano() > 21) {
                cout << RED << nombre << " se pasó de 21!" << RESET << endl;
                activo = false;
                break;
            }
        }
    }
};

class Crupier : public Jugador {
public:
    Crupier(string n) : Jugador(n) {}

    void jugar(Mazo& mazo) override {
        mostrar(true);
        cout << GREEN << "Tu turno, Crupier. ¿Pedir carta? (s/n): " << RESET;
        string opcion;
        while (cin >> opcion && opcion == "s") {
            recibirCarta(mazo.repartir());
            mostrar(true);
            if (calcularMano() >= 21) break;
            cout << GREEN << "¿Pedir otra carta? (s/n): " << RESET;
        }

        int total = calcularMano();
        if (total > 21) {
            cout << RED << "¡Te pasaste de 21!\n" << RESET;
            activo = false;
        } else {
            cout << GREEN << "Te plantaste con " << total << ".\n" << RESET;
        }
    }
};

class Juego {
private:
    Mazo mazo;
    Crupier crupier;
    vector<JugadorBot> bots;
    int rondas;

public:
    Juego(int r) : crupier("Crupier"), rondas(r) {
        bots.emplace_back("Bot1");
        bots.emplace_back("Bot2");
        bots.emplace_back("Bot3");
        bots.emplace_back("Bot4");
    }

    void jugar() {
        for (int ronda = 1; ronda <= rondas; ++ronda) {
            cout << BOLD << "\n=== RONDA " << ronda << " ===\n" << RESET;
            iniciarRonda();
            turnoBots();
            turnoCrupier();
            evaluarRonda();
        }

        mostrarGanadores();
    }

    void iniciarRonda() {
        mazo.reiniciar();
        crupier.limpiarMano();
        for (auto& b : bots) {
            b.limpiarMano();
            b.activo = true;
        }

        for (int i = 0; i < 2; ++i) {
            for (auto& b : bots) b.recibirCarta(mazo.repartir());
            crupier.recibirCarta(mazo.repartir());
        }

        for (auto& b : bots) b.mostrar();
        crupier.mostrar(true);
    }

    void turnoBots() {
        for (auto& b : bots) {
            pid_t pid = fork();
            if (pid == 0) {
                b.jugar(mazo);
                exit(0);
            } else {
                wait(NULL);
            }
        }
    }

    void turnoCrupier() {
        crupier.jugar(mazo);
    }

    void evaluarRonda() {
        int c_total = crupier.calcularMano();
        int victoriasBots = 0;

        for (auto& b : bots) {
            int b_total = b.calcularMano();
            if (b_total > 21) continue;

            if (b.tieneBlackjack() && !crupier.tieneBlackjack()) {
                b.agregarPunto();
                victoriasBots++;
                cout << GREEN << b.nombre << " gana con BLACKJACK!\n" << RESET;
            } else if (c_total > 21 || b_total > c_total) {
                b.agregarPunto();
                victoriasBots++;
                cout << GREEN << b.nombre << " gana con " << b_total << "\n" << RESET;
            } else if (b_total == c_total) {
                cout << YELLOW << b.nombre << " empata.\n" << RESET;
            } else {
                cout << RED << b.nombre << " pierde.\n" << RESET;
            }
        }

        if (c_total > 21) {
            cout << RED << "Crupier pierde por pasarse.\n" << RESET;
        } else if (victoriasBots == 0) {
            crupier.agregarPunto();
            crupier.agregarPunto();
            cout << GREEN << "Crupier gana a todos (+2 puntos).\n" << RESET;
        } else if (victoriasBots <= 2) {
            crupier.agregarPunto();
            cout << GREEN << "Crupier gana mayoría (+1 punto).\n" << RESET;
        } else {
            cout << RED << "Crupier no gana esta ronda.\n" << RESET;
        }
    }

    void mostrarGanadores() {
        ofstream archivo("estadisticas.txt");
        archivo << "=== PUNTUACIONES FINALES ===\n";

        cout << BOLD << "\n=== PUNTUACIONES FINALES ===\n" << RESET;
        cout << crupier.nombre << ": " << crupier.getPuntos() << " puntos\n";
        archivo << crupier.nombre << ": " << crupier.getPuntos() << " puntos\n";

        int maxPuntaje = crupier.getPuntos();
        vector<string> ganadores;
        if (maxPuntaje > 0) ganadores.push_back("Crupier");

        for (auto& b : bots) {
            cout << b.nombre << ": " << b.getPuntos() << " puntos\n";
            archivo << b.nombre << ": " << b.getPuntos() << " puntos\n";
            if (b.getPuntos() > maxPuntaje) {
                ganadores = {b.nombre};
                maxPuntaje = b.getPuntos();
            } else if (b.getPuntos() == maxPuntaje && maxPuntaje != 0) {
                ganadores.push_back(b.nombre);
            }
        }

        cout << "\nGanador(es): ";
        archivo << "\nGanador(es): ";
        for (auto& g : ganadores) {
            cout << BOLD << g << " ";
            archivo << g << " ";
        }
        cout << RESET << endl;
        archivo << endl;
        archivo.close();
    }
};

int main() {
    srand(time(nullptr));
    int rondas;

    cout << BOLD << "Bienvenido a Blackjack Antártico ❄️\n" << RESET;

    while (true) {
        cout << "Ingrese número de rondas: ";
        if (cin >> rondas && rondas > 0) break;
        cout << RED << "Entrada inválida. Ingrese un número entero mayor a 0.\n" << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    Juego partida(rondas);
    partida.jugar();
    return 0;
}

