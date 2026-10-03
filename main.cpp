#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

enum class Estado {
    Calmao,
    Dandole,
    Finiquitao,
    Aborto
};

class Actividades{
        private:
       string Nombre_Actividad;
       int Id_Actividad;
       int Tiempo;
       Estado Estado_Actividad;
       vector<int> Dependencias;

       public: 
       Actividades(string Nombre_Actividad,int Id_Actividad,int Tiempo, vector<int> Dependencias) {
       this -> Nombre_Actividad = Nombre_Actividad;
       this -> Id_Actividad = Id_Actividad;
       this -> Tiempo = Tiempo;
       this -> Dependencias = Dependencias;
       this -> Estado_Actividad = Estado::Calmao;
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
       void SetEstado_Actividad (Estado NewEstado_Actividad) {
       Estado_Actividad = NewEstado_Actividad;
       }

       void SetDependencias (vector<int> NewDependencias) {
       Dependencias = NewDependencias;
       }

       string GetNombre_Actividad() {
       return Nombre_Actividad;
       }
       int GetId_Actividad() const { //si no pongo ese const todo se va ala mierda
       return Id_Actividad;
       }
       int GetTiempo() {
       return Tiempo;
       }
       vector<int> GetDependencias() {
       return Dependencias;
       }

       Estado GetEstado_Actividad() {
         return Estado_Actividad;
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
        }

        // Convertir dependencias de string a vector<int>
        vector<int> dependencias_actividad = desglosardependenciasinador(dependencias);

        // Crear actividad
        Actividades actividad(nombre, id_actividad, tiempo_actividad, dependencias_actividad);

        // Guardarla en el vector
        actividades.push_back(actividad);
    }
}


int BuscarActividadPorId(const vector<Actividades> &actividades, int id) {

    for (int i = 0; i < (int)actividades.size(); ++i) {
        if (actividades[i].GetId_Actividad() == id) {
            return i; // Encontrada
        }
    }
    return -1; // No encontrada
}

bool Estalisteilor(Actividades &actividad, vector<Actividades> &actividades) {
    
    if (actividad.GetDependencias().empty()) {
        return true; // No tiene dependencias, por lo tanto está listo
    }

    for (int id : actividad.GetDependencias()) {

        int posicion = BuscarActividadPorId(actividades, id);
        if (posicion == -1) {
            return false; // Dependencia no encontrada
        }
        if(actividades[posicion].GetEstado_Actividad() != Estado::Finiquitao) {
            return false; // Dependencia no finalizada
        }
    }
    return true; // Todas las dependencias encontradas
}

void ExpansionDeDominio(Actividades &actividad) {
    pid_t pid = fork();
    if (pid == -1) {
        cout << "Error al crear el proceso de la actividad " << actividad.GetNombre_Actividad() << endl;
        return;
    }

    if (pid == 0) {
    actividad.SetEstado_Actividad(Estado::Dandole);
    cout << "se expandió la Actividad " << actividad.GetNombre_Actividad() << " (ID: " << actividad.GetId_Actividad() << ") con teempo de ejecución: " << actividad.GetTiempo() << " ms" << endl;
    usleep(actividad.GetTiempo() * 1000); //

    cout << "Actividad " << actividad.GetNombre_Actividad() << " (ID: " << actividad.GetId_Actividad() << ") ha sido finiquitadisima (en el hijo!)." << endl;
    actividad.SetEstado_Actividad(Estado::Finiquitao);
    _exit(0); // nigerun dayoo
    }else{
        actividad.SetEstado_Actividad(Estado::Dandole);
        waitpid(pid, nullptr, 0);
        actividad.SetEstado_Actividad(Estado::Finiquitao);
        cout << "Actividad " << actividad.GetNombre_Actividad() << " (ID: " << actividad.GetId_Actividad() << ") ha sido finiquitadisima (en el padre!)." << endl;

    }
}

void VacioInfinito(vector<Actividades> &actividades){
    bool quedanactividades = true;

    while(quedanactividades){
        quedanactividades = false;
        for(Actividades &actividad : actividades){

            if(actividad.GetEstado_Actividad() == Estado::Calmao && Estalisteilor(actividad, actividades)){
                ExpansionDeDominio(actividad);
                quedanactividades = true;
            }
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

    cout << endl;

cout << "===== PRUEBA FORK v: 1.0 =====" << endl;

pid_t pid = fork();

if (pid == -1) {
    cout << "Error al crear el proceso hijo." << endl;
    return 1;
}

if (pid == 0) {

    cout << "Soy el proceso hijo." << endl;
    cout << "Mi PID es: " << getpid() << endl;

    return 0;

} else {

    cout << "Soy el proceso padre." << endl;
    cout << "Mi PID es: " << getpid() << endl;
    cout << "El PID de mi hijo es: " << pid << endl;

    waitpid(pid, nullptr, 0);

    cout << "El proceso hijo ha terminao." << endl;
}

cout << endl;
cout << "===== EJECUTANDO ACTIVIDAD =====" << endl;

VacioInfinito(actividades);


    return 0;
}