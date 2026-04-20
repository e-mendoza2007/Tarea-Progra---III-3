#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <map>
#include <string>
#include <cctype>

using namespace std;

class Entity {
private:
    string nombre;
    int vida;
    int vidaMaxima;
    double posicionX, posicionY;  // Posición (x, y)
    int nivel;
    int elixir;  // Recurso


public:
    // Constructor
    Entity(const string& nombre, int vidaMaxima = 100, int inicioX = 0, int inicioY = 0)
        : nombre(nombre), vida(vidaMaxima), vidaMaxima(vidaMaxima), 
          posicionX(inicioX), posicionY(inicioY), nivel(1), elixir(50) {}


    // Getters para consultar estado
    string obtenerNombre() const { return nombre; }
    int obtenerVida() const { return vida; }
    int obtenerVidaMaxima() const { return vidaMaxima; }
    double obtenerPosicionX() const { return posicionX; }
    double obtenerPosicionY() const { return posicionY; }
    int obtenerNivel() const { return nivel; }
    int obtenerElixir() const { return elixir; }
    
    // Métodos públicos que modifican el estado
    
    void curar(int cantidad) {
        vida += cantidad;
        if (vida > vidaMaxima) vida = vidaMaxima;
        cout << "  -> " << nombre << " recupera " << cantidad << " de vida. Ahora tiene " << vida << " de vida.\n";
    }
    
    void recibirDanio(int cantidad) {
        vida -= cantidad;
        if (vida < 0) vida = 0;
        cout << "  -> " << nombre << " recibe " << cantidad << " de daño. Ahora tiene " << vida << " de vida.\n";
    }
    
    void mover(double deltaX, double deltaY) {
        posicionX += deltaX;
        posicionY += deltaY;
        cout << "  -> " << nombre << " se mueve a (" << posicionX << ", " << posicionY << ").\n";
    }
    
    void subirNivel() {
        nivel++;
        vidaMaxima += 20;
        vida = vidaMaxima;
        elixir += 10;
        cout << "  -> " << nombre << " sube al nivel " << nivel << "! Vida máxima: " << vidaMaxima << ", Elixir: " << elixir << "\n";
    }
    
    void restaurarElixir(int cantidad) {
        elixir += cantidad;
        if (elixir > 100) elixir = 100;
        cout << "  -> " << nombre << " recupera " << cantidad << " de elixir. Ahora tiene " << elixir << " de elixir.\n";
    }
    
    void gastarElixir(int cantidad) {
        if (elixir >= cantidad) {
            elixir -= cantidad;
            cout << "  -> " << nombre << " gasta " << cantidad << " de elixir. Elixir restante: " << elixir << "\n";
        } else {
            cout << "  -> Elixir insuficiente! Necesita " << cantidad << ", tiene " << elixir << "\n";
        }
    }
    
    // Mostrar estado completo
    void mostrarEstado() const {
        cout << "\n=== Estado de: " << nombre << " ===\n";
        cout << "Vida: " << vida << "/" << vidaMaxima << "\n";
        cout << "Posicion: (" << posicionX << ", " << posicionY << ")\n";
        cout << "Nivel: " << nivel << "\n";
        cout << "Elixir: " << elixir << "/100\n";
        cout << "================================\n";
    }
    
    // Para historial (formatea el estado actual)
    string obtenerEstadoComoString() const {
        return "Vida:" + to_string(vida) + "/" + to_string(vidaMaxima) +
               " Pos:(" + to_string(posicionX) + "," + to_string(posicionY) +
               ") Nivel:" + to_string(nivel) + " Elixir:" + to_string(elixir);
    }
};

// Definicion de conamdo

using Comando = function<void(const list<string>&)>;

// Centro de comandos

class ComandCenter {
private:
    map<string, Comando> comandos;
    list<string> historial;
    Entity& entity;                    

public:
    ComandCenter(Entity& ent) : entity(ent) {}
    
    void registrarComando(const string& nombre, Comando cmd) {
        comandos[nombre] = cmd;
        cout << "[DEBUG] Comando registrado: " << nombre << "\n";
    }
    
    void executeCommand(const string& cmd, const list<string>& args) {
        map<string, Comando>::iterator it = comandos.find(cmd);
        
        if (it == comandos.end()) {
            cout << "Comando desconocido: " << cmd << "\n";
            return;
        }
        
        // Registrar estado antes
        string estadoAntes = entity.obtenerEstadoComoString();
        
        // Ejecutar comando
        cout << "\n>> Ejecutando: " << cmd;
        for (list<string>::const_iterator argIt = args.begin(); argIt != args.end(); ++argIt) {
            cout << " " << *argIt;
        }
        cout << "\n";
        
        it->second(args);
        
        // Registrar estado después en el historial
        string estadoDespues = entity.obtenerEstadoComoString();
        historial.push_back(cmd + " | Antes: " + estadoAntes + " | Después: " + estadoDespues);
    }
    
    // Eliminar comando
    void remove_command(const string& cmd) {
        map<string, Comando>::iterator it = comandos.find(cmd);
        if (it != comandos.end()) {
            comandos.erase(it);
            cout << "Comando eliminado: " << cmd << "\n";
        } else {
            cout << "No se puede eliminar, comando no existe: " << cmd << "\n";
        }
    }
    
    // Registrar macro comando
    void macrocomandos(const string& name, const list<pair<string, list<string>>>& steps) {
        // Creamos un comando compuesto que ejecute todos los pasos
        Comando macro = [this, steps](const list<string>& args) {
            // Recorrer pasos usando iteradores explícitos
            for (list<pair<string, list<string>>>::const_iterator it = steps.begin();
                 it != steps.end(); ++it) {
                const string& cmdName = it->first;
                const list<string>& cmdArgs = it->second;
                
                cout << "  [Macro paso: " << cmdName << "]\n";
                executeCommand(cmdName, cmdArgs);
            }
        };
        
        registrarComando(name, macro);
        cout << "Macro comando registrado: " << name << "\n";
    }
    
    // Ejecutar macro
    void executeMacro(const string& name) {
        list<string> args_vacios;
        executeCommand(name, args_vacios);
    }
    
    // Mostrar historial
    void mostrarHistorial() const {
        cout << "\n=== HISTORIAL DE EJECUCIÓN ===\n";
        for (list<string>::const_iterator it = historial.begin(); it != historial.end(); ++it) {
            cout << *it << "\n";
        }
     cout << "================================\n\n";
    }
};


// Comando como funcion libre
void comandoCurar(Entity& entity, const list<string>& args) {

    if (args.size() != 1) {
        cout << "  Error: curar requiere 1 argumento (cantidad)\n";
        return;
    }
    try {
        int cantidad = stoi(args.front());
        if (cantidad < 0) {
            cout << "  Error: la cantidad no puede ser negativa\n";
            return;
        }
        entity.curar(cantidad);
    } catch (const exception& e) {
        cout << "  Error: argumento invalido, debe ser un número\n";
    }
}

void ComandoSubirNivel(Entity& entity, const list<string>& args) {
    if (args.size() != 0) {
        cout << "Este comando no requiere de argumentos\n";
        return;
    }
    entity.subirNivel();
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
    
    void operator()(const list<string>& args) {
        if (contadorUsos >= limiteUsos) {
            cout << "  Error: Comando danio alcanzo su límite de " << limiteUsos << " usos\n";
            return;
        }
        
        if (args.size() != 1) {
            cout << "  Error: danio requiere 1 argumento (cantidad)\n";
            return;
        }
        
        try {
            int cantidad = stoi(args.front());
            if (cantidad < 0) {
                cout << "  Error: la cantidad no puede ser negativa\n";
                return;
            }
            contadorUsos++;
            cout << "  [Danio usado " << contadorUsos << "/" << limiteUsos << " veces]\n";
            entity.recibirDanio(cantidad);
        } catch (const exception& e) {
            cout << "  Error: argumento inválido, debe ser un número\n";
        }
    }
    
    int obtenerContadorUsos() const { return contadorUsos; }
};

class Comands_restaurar_exi {

    Entity& entity;
    public:
    Comands_restaurar_exi(Entity& ent) : entity(ent) {}

    void operator()(const list<string>& args) {
        if (args.size() != 1) {
            cout << "Error: Restaurar exilir solo necesita un argumento\n";
            return;
        }
        try {

            int cantidad = stoi(args.front());
            if (cantidad < 0) {
                cout << "Error: no se aceptan numeros negativos\n";
                return;
            }
            entity.restaurarElixir(cantidad);
        }catch (const exception& e) {
            cout << "Error: El argumento debe ser un numero\n";
        }
    }

};

class Comands_gastar_exi {

    Entity& entity;
public:
    Comands_gastar_exi(Entity& ent) : entity(ent) {}

    void operator()(const list<string>& args) {
        if (args.size() != 1) {
            cout << "Error: Restaurar exilir solo necesita un argumento\n";
            return;
        }
        try {

            int cantidad = stoi(args.front());
            if (cantidad < 0) {
                cout << "Error: no se aceptan numeros negativos\n";
                return;
            }
            entity.restaurarElixir(cantidad);
        }catch (const exception& e) {
            cout << "Error: El argumento debe ser un numero\n";
        }
    }

};


int main () {
    Entity entity("Jugador 1",150,10,20);
    ComandCenter center(entity);
    ComandoDanio danio(entity);
    Comands_gastar_exi gastar_Exilir(entity);
    Comands_restaurar_exi restaurar_Exilir(entity);

    // Comando lambda
    auto mover = [&](const list<string> &args) {
        if (args.size() != 2) {
            cout << "  Error: El comando necesita dos argumentos para su implementacion(posicion x , y)\n";
            return;
        }
        try {
            double posX = stod(args.front());
            double posY = stod(args.back());
            entity.mover(posX, posY);
        } catch (const exception &e) {
            cout << "Los valores deben ser numeros\n";
        }
    };
    auto status = [&](const list<string> &args) {
        if (args.size() != 0) {
            cout << "Est comando no necesita argumentos\n";
            return;
        }

        entity.mostrarEstado();

    };

    // usado de lambda como conexión para la función libre
    auto curar = [&](const list<string> &args) {
        comandoCurar(entity,args);
    };
    auto Subir_Nivel = [&](const list<string> &args) {
        ComandoSubirNivel(entity,args);
    };



    center.registrarComando("move",mover);
    center.registrarComando("heal",curar);
    center.registrarComando("damage",danio);
    center.registrarComando("status",status);
    center.registrarComando("restaurar_exilir",restaurar_Exilir);
    center.registrarComando("gastar_exilir",gastar_Exilir);
    center.registrarComando("subir_nivel",Subir_Nivel);

    center.executeCommand("move", list<string>{"10", "20"});
    center.executeCommand("status", list<string>{});
    center.executeCommand("heal", list<string>{"10"});
    center.executeCommand("status", list<string>{});
    center.executeCommand("damage", list<string>{"10"});
    center.executeCommand("status", list<string>{});
    center.executeCommand("gastar_exilir",list<string>{"70"});
    center.executeCommand("status", list<string>{});
    center.executeCommand("restaurar_exilir",list<string>{"50"});



    list<pair<string, list<string>>> steps({{{"heal"},{"10"}},{{"status"},{}}});
    center.macrocomandos("heal_status", steps);
    list<pair<string, list<string>>> steps2({{{"damage"},{"40"}},{{"status"},{}}});
    center.macrocomandos("damage_status", steps2);
    list<pair<string, list<string>>> steps3({{{"move"},{"10", "30"}},{{"damage"},{"3"}}});
    center.macrocomandos("move_damage", steps3);

    center.executeMacro("heal_status");
    center.executeMacro("damage_status");
    center.executeMacro("move_damage");

    center.executeCommand("status", list<string>{});

    center.executeCommand("subir_nivel", list<string>{});

    center.executeCommand("status", list<string>{});

}