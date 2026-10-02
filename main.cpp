#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Actividades{
        private:
       string Nombre_Actividad;
       int Id_Actividad;
       int Tiempo;
       string Dependencias;

       public: 
       Actividades(string Nombre_Actividad,int Id_Actividad,int Tiempo,string Dependencias) {
       this -> Nombre_Actividad = Nombre_Actividad;
       this -> Id_Actividad = Id_Actividad;
       this -> Tiempo = Tiempo;
       this -> Dependencias = Dependencias;
       }

       void SetNombre_Actividad (string NewNombre_Actividad) {
       Nombre_Actividad = NewNombre_Actividad;
       }
       void SetId_Actividad (int NewId_Actividad) {
       Id_Actividad = NewId_Actividad;
       }
       void SetTiempo (int NewTiempo) {
       Tiempo = NewTiempo;
       }
       void SetDependencias (string NewDependencias) {
       Dependencias = NewDependencias;
       }

       string GetNombre_Actividad() {
       return Nombre_Actividad;
       }
       int GetId_Actividad() {
       return Id_Actividad;
       }
       int GetTiempo() {
       return Tiempo;
       }
       string GetDependencias() {
       return Dependencias;
       }

       void PrintWeas() {
       cout << "Nombre_Actividad : " <<Nombre_Actividad;
       cout << "Id_Actividad : " <<Id_Actividad;
       cout << "Tiempo : " <<Tiempo;
       cout << "Dependencias : " <<Dependencias;
       }};

int Random() { // Esto para los milisegundos si no aparecen
       return rand() % (5000 - 100 + 1) + 100;
}

void LeerArchivo(string nombre_archivo, vector<Actividades> &actividades) {
        
        while (getline (planes, nombre_actividad)) {
    stringstream ss(linea);

    string id;
    string nombre;
    string tiempo;
    string dependencias;

    getline(ss, id, ':');
    getline(ss, nombre, ':');
    getline(ss, tiempo, ':');
    getline(ss, dependencias, ':');

    cout << "ID: " << id << endl; //Estos pa ver si se esta leyendo bien el archivo, lo quitamos al final
    cout << "Nombre: " << nombre << endl;
    cout << "Tiempo: " << tiempo << endl;
    cout << "Dependencias: " << dependencias << endl;

    cout << "ID (int): " << id_int << endl;
    cout << "Tiempo (int): " << tiempo_int << endl;

    Actividades actividad(nombre, id_int, tiempo_int, dependencias);
    actividades.push_back(actividad);

        }
    }

int main(){
    ifstream planes("plan.txt");
    if(!planes.is_open()){
        cout << "No se pudo abrir el plan manito, seguro lo escribiste bien?" << endl;
        return 1;
    }

    string nombre_actividad;
    while(getline(planes, nombre_actividad)){
        cout << "Nombre de la actividad: " << nombre_actividad << endl;
    }


    planes.close();
    return 0;
}