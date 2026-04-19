#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <functional>
#include <map>
#include <string>
#include <cctype>

class Entity {
private:
    std::string nombre;
    int vida;
    int vidaMaxima;
    int posicionX, posicionY;  // Posición (x, y)
    int nivel;
    int elixir;  // Recurso

public:
    // Constructor
    Entity(const std::string& nombre, int vidaMaxima = 100, int inicioX = 0, int inicioY = 0) 
        : nombre(nombre), vida(vidaMaxima), vidaMaxima(vidaMaxima), 
          posicionX(inicioX), posicionY(inicioY), nivel(1), elixir(50) {}
    
    // Getters para consultar estado
    std::string obtenerNombre() const { return nombre; }
    int obtenerVida() const { return vida; }
    int obtenerVidaMaxima() const { return vidaMaxima; }
    int obtenerPosicionX() const { return posicionX; }
    int obtenerPosicionY() const { return posicionY; }
    int obtenerNivel() const { return nivel; }
    int obtenerElixir() const { return elixir; }
    
    // Métodos públicos que modifican el estado
    
    void curar(int cantidad) {
        vida += cantidad;
        if (vida > vidaMaxima) vida = vidaMaxima;
        std::cout << "  -> " << nombre << " recupera " << cantidad << " de vida. Ahora tiene " << vida << " de vida.\n";
    }
    
    void recibirDanio(int cantidad) {
        vida -= cantidad;
        if (vida < 0) vida = 0;
        std::cout << "  -> " << nombre << " recibe " << cantidad << " de daño. Ahora tiene " << vida << " de vida.\n";
    }
    
    void mover(int deltaX, int deltaY) {
        posicionX += deltaX;
        posicionY += deltaY;
        std::cout << "  -> " << nombre << " se mueve a (" << posicionX << ", " << posicionY << ").\n";
    }
    
    void subirNivel() {
        nivel++;
        vidaMaxima += 20;
        vida = vidaMaxima;
        elixir += 10;
        std::cout << "  -> " << nombre << " sube al nivel " << nivel << "! Vida máxima: " << vidaMaxima << ", Elixir: " << elixir << "\n";
    }
    
    void restaurarElixir(int cantidad) {
        elixir += cantidad;
        if (elixir > 100) elixir = 100;
        std::cout << "  -> " << nombre << " recupera " << cantidad << " de elixir. Ahora tiene " << elixir << " de elixir.\n";
    }
    
    void gastarElixir(int cantidad) {
        if (elixir >= cantidad) {
            elixir -= cantidad;
            std::cout << "  -> " << nombre << " gasta " << cantidad << " de elixir. Elixir restante: " << elixir << "\n";
        } else {
            std::cout << "  -> Elixir insuficiente! Necesita " << cantidad << ", tiene " << elixir << "\n";
        }
    }
    
    // Mostrar estado completo
    void mostrarEstado() const {
        std::cout << "\n=== Estado de: " << nombre << " ===\n";
        std::cout << "Vida: " << vida << "/" << vidaMaxima << "\n";
        std::cout << "Posición: (" << posicionX << ", " << posicionY << ")\n";
        std::cout << "Nivel: " << nivel << "\n";
        std::cout << "Elixir: " << elixir << "/100\n";
        std::cout << "================================\n\n";
    }
    
    // Para historial (formatea el estado actual)
    std::string obtenerEstadoComoString() const {
        return "Vida:" + std::to_string(vida) + "/" + std::to_string(vidaMaxima) + 
               " Pos:(" + std::to_string(posicionX) + "," + std::to_string(posicionY) + 
               ") Nivel:" + std::to_string(nivel) + " Elixir:" + std::to_string(elixir);
    }
};

// Definicion de conamdo

using Comando = std::function<void(const std::list<std::string>&)>;

// Centro de comandos

class ComandCenter {
private:
    std::map<std::string, Comando> comandos;
    std::list<std::string> historial;  
    Entity& entity;                    

public:
    ComandCenter(Entity& ent) : entity(ent) {}
    
    void registrarComando(const std::string& nombre, Comando cmd) {
        comandos[nombre] = cmd;
        std::cout << "[DEBUG] Comando registrado: " << nombre << "\n";
    }
    
    void executeCommand(const std::string& cmd, const std::list<std::string>& args) {
        std::map<std::string, Comando>::iterator it = comandos.find(cmd);
        
        if (it == comandos.end()) {
            std::cout << "Comando desconocido: " << cmd << "\n";
            return;
        }
        
        // Registrar estado antes
        std::string estadoAntes = entity.obtenerEstadoComoString();
        
        // Ejecutar comando
        std::cout << "\n>> Ejecutando: " << cmd;
        for (std::list<std::string>::const_iterator argIt = args.begin(); argIt != args.end(); ++argIt) {
            std::cout << " " << *argIt;
        }
        std::cout << "\n";
        
        it->second(args);
        
        // Registrar estado después en el historial
        std::string estadoDespues = entity.obtenerEstadoComoString();
        historial.push_back(cmd + " | Antes: " + estadoAntes + " | Después: " + estadoDespues);
    }
    
    // Eliminar comando
    void remove_command(const std::string& cmd) {
        std::map<std::string, Comando>::iterator it = comandos.find(cmd);
        if (it != comandos.end()) {
            comandos.erase(it);
            std::cout << "Comando eliminado: " << cmd << "\n";
        } else {
            std::cout << "No se puede eliminar, comando no existe: " << cmd << "\n";
        }
    }
    
    // Registrar macro comando
    void macrocomandos(const std::string& name, const std::list<std::pair<std::string, std::list<std::string>>>& steps) {
        // Creamos un comando compuesto que ejecute todos los pasos
        Comando macro = [this, steps](const std::list<std::string>& args) {
            // Recorrer pasos usando iteradores explícitos
            for (std::list<std::pair<std::string, std::list<std::string>>>::const_iterator it = steps.begin(); 
                 it != steps.end(); ++it) {
                const std::string& cmdName = it->first;
                const std::list<std::string>& cmdArgs = it->second;
                
                std::cout << "  [Macro paso: " << cmdName << "]\n";
                executeCommand(cmdName, cmdArgs);
            }
        };
        
        registrarComando(name, macro);
        std::cout << "Macro comando registrado: " << name << "\n";
    }
    
    // Ejecutar macro
    void executeMacro(const std::string& name) {
        std::list<std::string> args_vacios;
        executeCommand(name, args_vacios);
    }
    
    // Mostrar historial
    void mostrarHistorial() const {
        std::cout << "\n=== HISTORIAL DE EJECUCIÓN ===\n";
        for (std::list<std::string>::const_iterator it = historial.begin(); it != historial.end(); ++it) {
            std::cout << *it << "\n";
        }
        std::cout << "================================\n\n";
    }
};


// Comando como funcion libre
void comandoCurar(Entity& entity, const std::list<std::string>& args) {
    if (args.size() != 1) {
        std::cout << "  Error: curar requiere 1 argumento (cantidad)\n";
        return;
    }
    try {
        int cantidad = std::stoi(args.front());
        if (cantidad < 0) {
            std::cout << "  Error: la cantidad no puede ser negativa\n";
            return;
        }
        entity.curar(cantidad);
    } catch (const std::exception& e) {
        std::cout << "  Error: argumento invalido, debe ser un número\n";
    }
}

// Comando como functor
class ComandoDanio {
private:
    Entity& entity;
    int contadorUsos;      
    int limiteUsos;        
    
public:
    ComandoDanio(Entity& ent, int limiteMaximo = 10) 
        : entity(ent), contadorUsos(0), limiteUsos(limiteMaximo) {}
    
    void operator()(const std::list<std::string>& args) {
        if (contadorUsos >= limiteUsos) {
            std::cout << "  Error: Comando danio alcanzó su límite de " << limiteUsos << " usos\n";
            return;
        }
        
        if (args.size() != 1) {
            std::cout << "  Error: danio requiere 1 argumento (cantidad)\n";
            return;
        }
        
        try {
            int cantidad = std::stoi(args.front());
            if (cantidad < 0) {
                std::cout << "  Error: la cantidad no puede ser negativa\n";
                return;
            }
            contadorUsos++;
            std::cout << "  [Danio usado " << contadorUsos << "/" << limiteUsos << " veces]\n";
            entity.recibirDanio(cantidad);
        } catch (const std::exception& e) {
            std::cout << "  Error: argumento inválido, debe ser un número\n";
        }
    }
    
    int obtenerContadorUsos() const { return contadorUsos; }
};

int main () {

    
}