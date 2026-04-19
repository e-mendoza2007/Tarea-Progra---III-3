#include <iostream>
#include <vector>
#include <algorithm>
#include<list>
#include<functional>
#include <map>
using namespace std;



template<typename T>
class ComandCenter;

template<typename T>
class Enity {

    friend class ComandCenter<T>;


};



using comand = function < void ( const  list < string>& args) > ;

template<typename T>
class ComandCenter {

    map<string, comand> comandos;

    public:

    void resgistrar_comand(const string& nombre, function<void(const list < string>&)> f) {
        comandos[nombre] = f;
    }
    void executeCommand(const string& cmd, T t) {

    }
    void remove_command(const string& cmd) {
        comandos.erase(cmd);
    }


    void macrocomandos(const string& name,list<pair<string,list<string>>>& steps){}

    void executeCommand(const string& cmd, list<pair<string,list<string>>> steps){}


};


int main() {


}






