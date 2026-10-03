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
       vector<int> Dependencias;

       public: 
       Actividades(string Nombre_Actividad,int Id_Actividad,int Tiempo, vector<int> Dependencias) {
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
       void SetDependencias (vector<int> NewDependencias) {
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
       vector<int> GetDependencias() {
       return Dependencias;
       }

       void PrintWeas() {
       cout << " Nombre_Actividad : " <<Nombre_Actividad << endl;
       cout << " Id_Actividad : " <<Id_Actividad << endl;
       cout << " Tiempo : " <<Tiempo << endl;
       cout << " Dependencias : " ;
       for (int id : Dependencias) {
           cout << id << " ";
       }
       cout << endl;
       }};

int Random() { // Esto para los milisegundos si no aparecen
       return rand() % (5000 - 100 + 1) + 100;
}

string quitarespaciosinador(string wea){
    size_t inicio = wea.find_first_not_of(" \t\r\n");

    if(inicio ==string::npos){
        return "";
    }

    size_t fin = wea.find_last_not_of(" \t\r\n");
    return wea.substr(inicio, fin - inicio + 1);
}



vector<int> desglosardependenciasinador(string dependencias) {

    vector<int> resultao;

    if (dependencias.length() == 0) {
        return resultao;
    }

    stringstream ss(dependencias);
    string dependencia2;

    while (getline(ss, dependencia2, ',')) {
        dependencia2 = quitarespaciosinador(dependencia2);
        if (!dependencia2.empty()) {
            resultao.push_back(stoi(dependencia2));
        }
    }
    return resultao;
}

void LeerArchivo(ifstream &archivo, vector<Actividades> &actividades) {

    string linea;

    while (getline(archivo, linea)) {

        if(quitarespaciosinador(linea).empty()){
            continue;
        }

        stringstream ss(linea);

        string id;
        string nombre;
        string tiempo;
        string dependencias;

        getline(ss, id, ':');
        getline(ss, nombre, ':');
        getline(ss, tiempo, ':');
        getline(ss, dependencias, ':');

        id = quitarespaciosinador(id);
        nombre = quitarespaciosinador(nombre);
        tiempo = quitarespaciosinador(tiempo);
        dependencias = quitarespaciosinador(dependencias);

        int id_actividad = stoi(id); //si no pongo el stoi me tira error, así que mejor lo dejo xD
        int tiempo_actividad = 0;

        if (tiempo.empty()) {
        tiempo_actividad = Random();
        }else{
        tiempo_actividad = stoi(tiempo); //ah coño, claro, el texto es string, makes sense

        // Convertir dependencias de string a vector<int> 
        vector<int> dependencias_actividad = desglosardependenciasinador(dependencias); 
        // Crear actividad 
        Actividades actividad( nombre, id_actividad, tiempo_actividad, dependencias_actividad ); 
        // Guardarla en el vector 
        actividades.push_back(actividad);
    }


        }
    }




int main(){
    ifstream planes("plan.txt");
    if(!planes.is_open()){
        cout << "No se pudo abrir el plan manito, seguro lo escribiste bien?" << endl;
        return 1;
    }

    vector<Actividades> actividades;
    LeerArchivo(planes, actividades);
    planes.close();

    for(Actividades &actividad : actividades){
        cout << endl << "------UwU------" << endl;   
        actividad.PrintWeas();
        
    }



    return 0;
}