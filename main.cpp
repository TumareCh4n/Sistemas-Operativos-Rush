#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <ctime>
#include <csignal>
#include <cerrno>
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

        vector<int> dependencias_actividad = desglosardependenciasinador(dependencias);

        Actividades actividad(nombre, id_actividad, tiempo_actividad, dependencias_actividad);

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


vector<int> ritualinverso(vector<Actividades> &actividades, int idActividad){

    vector<int> dependientes;

    for(Actividades &actividad : actividades){

        vector<int> dependencias = actividad.GetDependencias();

        for(int dependencia : dependencias){

            if(dependencia == idActividad){
                dependientes.push_back(actividad.GetId_Actividad());
                break;
            }
        }
    }

    return dependientes;
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


struct ListaProcesosActivos{
    pid_t pid;
    int idActividad;
    int pipeLectura;
};


struct PipesActividades{
    int idOrigen;
    int idDestino;
    int pipeLectura;
    int pipeEscritura;
};




vector<PipesActividades> CrearPipes(vector<Actividades> &actividades){

    vector<PipesActividades> pipes;

    for(Actividades &actividad : actividades){

        vector<int> dependientes =
            ritualinverso(actividades, actividad.GetId_Actividad());

        for(int idDependiente : dependientes){

            int tuberia[2];

            if(pipe(tuberia) == -1){
                cout << "Error al crear pipe entre actividades "
                     << actividad.GetId_Actividad()
                     << " y "
                     << idDependiente
                     << endl;
                continue;
            }

            PipesActividades conexion;

            conexion.idOrigen = actividad.GetId_Actividad();
            conexion.idDestino = idDependiente;
            conexion.pipeLectura = tuberia[0];
            conexion.pipeEscritura = tuberia[1];

            pipes.push_back(conexion);
        }
    }

    return pipes;
}


vector<PipesActividades> BuscarPipesDeOrigen(
    vector<PipesActividades> &pipes,
    int idActividad
){

    vector<PipesActividades> pipesOrigen;

    for(PipesActividades &conexion : pipes){

        if(conexion.idOrigen == idActividad){
            pipesOrigen.push_back(conexion);
        }
    }

    return pipesOrigen;
}


vector<PipesActividades> BuscarPipesDeDestino(
    vector<PipesActividades> &pipes,
    int idActividad
){

    vector<PipesActividades> pipesDestino;

    for(PipesActividades &conexion : pipes){

        if(conexion.idDestino == idActividad){
            pipesDestino.push_back(conexion);
        }
    }

    return pipesDestino;
}


int BuscarProcesoActivo(vector<ListaProcesosActivos> &procesos, pid_t pid) {
    for (int i = 0; i < (int)procesos.size(); ++i) {
        if (procesos[i].pid == pid) {
            return i;
        }
    }

    return -1;
}




pid_t ExpansionDeDominio(
    Actividades &actividad,
    int &pipeLectura,
    vector<PipesActividades> &pipesEntrada,
    vector<PipesActividades> &pipesSalida
){

    int tuberia[2];

    if(pipe(tuberia) == -1){
        cout << "Error al crear el pipe de la actividad "
             << actividad.GetNombre_Actividad() << endl;
        return -1;
    }

    pid_t pid = fork();

    if(pid == -1){
        cout << "Error al crear el proceso de la actividad "
             << actividad.GetNombre_Actividad() << endl;
        return -1;
    }

    
if(pid == 0){


    signal(SIGINT, SIG_DFL);

    close(tuberia[0]);


    for(PipesActividades &conexion : pipesEntrada){

        char mensaje[100];

        read(
            conexion.pipeLectura,
            mensaje,
            sizeof(mensaje)
        );

        cout << "Actividad "
             << actividad.GetId_Actividad()
             << " recibió: "
             << mensaje
             << " desde actividad "
             << conexion.idOrigen
             << endl;
    }

    cout << "se expandió la Actividad "
         << actividad.GetNombre_Actividad()
         << " (ID: " << actividad.GetId_Actividad()
         << ") con teempo de ejecución: "
         << actividad.GetTiempo() << " ms" << endl;

    usleep(actividad.GetTiempo() * 1000);

    const char* mensaje = "TERMINADA";

    write(tuberia[1], mensaje, strlen(mensaje) + 1);

    for(PipesActividades &conexion : pipesSalida){

        write(
            conexion.pipeEscritura,
            mensaje,
            strlen(mensaje) + 1
        );
    }

    close(tuberia[1]);

    _exit(0);
}


    else {

        close(tuberia[1]);

        pipeLectura = tuberia[0];

        actividad.SetEstado_Actividad(Estado::Dandole);

        return pid;
    }
}




volatile sig_atomic_t seremiLlego = 0;

void ManejadorSeremi(int senal){
    (void)senal;
    seremiLlego = 1;
}


void PurgaDeLaRama(vector<Actividades> &actividades, int idFallida){

    vector<int> porRevisar;
    porRevisar.push_back(idFallida);

    while(!porRevisar.empty()){

        int idActual = porRevisar.back();
        porRevisar.pop_back();

        vector<int> dependientes = ritualinverso(actividades, idActual);

        for(int idDependiente : dependientes){

            int posicion = BuscarActividadPorId(actividades, idDependiente);

            if(posicion != -1 &&
               actividades[posicion].GetEstado_Actividad() == Estado::Calmao){

                actividades[posicion].SetEstado_Actividad(Estado::Aborto);

                cout << "Actividad "
                     << actividades[posicion].GetNombre_Actividad()
                     << " (ID: " << idDependiente
                     << ") abortada porque dependía de la actividad "
                     << idActual << endl;

                porRevisar.push_back(idDependiente);
            }
        }
    }
}


void VacioInfinito(vector<Actividades> &actividades, int K, vector<PipesActividades> &pipes){

    vector<ListaProcesosActivos> procesosActivos;

    while(true){

        if(seremiLlego){

            cout << endl << "!!! LLEGÓ LA SEREMI !!! Abortando todas las actividades..." << endl;

            for(ListaProcesosActivos &proceso : procesosActivos){

                kill(proceso.pid, SIGTERM);
                waitpid(proceso.pid, nullptr, 0);
                close(proceso.pipeLectura);

                int posicionActividad =
                    BuscarActividadPorId(actividades, proceso.idActividad);

                if(posicionActividad != -1){
                    actividades[posicionActividad].SetEstado_Actividad(Estado::Aborto);
                    cout << "Actividad "
                         << actividades[posicionActividad].GetNombre_Actividad()
                         << " (ID: " << proceso.idActividad
                         << ") abortada por la Seremi." << endl;
                }
            }
            procesosActivos.clear();

            for(Actividades &actividad : actividades){
                if(actividad.GetEstado_Actividad() == Estado::Calmao){
                    actividad.SetEstado_Actividad(Estado::Aborto);
                }
            }
            return;
        }

        for(Actividades &actividad : actividades){

            if(seremiLlego || (int)procesosActivos.size() >= K){
                break;
            }

            if(actividad.GetEstado_Actividad() == Estado::Calmao &&
               Estalisteilor(actividad, actividades)){

                int pipeLectura;

                vector<PipesActividades> pipesEntrada =
                    BuscarPipesDeDestino(pipes, actividad.GetId_Actividad());

                vector<PipesActividades> pipesSalida =
                    BuscarPipesDeOrigen(pipes, actividad.GetId_Actividad());

                pid_t pid = ExpansionDeDominio(
                    actividad,
                    pipeLectura,
                    pipesEntrada,
                    pipesSalida
                );

                if(pid != -1){
                    ListaProcesosActivos proceso;
                    proceso.pid = pid;
                    proceso.idActividad = actividad.GetId_Actividad();
                    proceso.pipeLectura = pipeLectura;
                    procesosActivos.push_back(proceso);
                }
                else{
                    actividad.SetEstado_Actividad(Estado::Aborto);
                    PurgaDeLaRama(actividades, actividad.GetId_Actividad());
                }
            }
        }

        if(seremiLlego){
            continue;
        }

        if(procesosActivos.empty()){
            break;
        }

        int status = 0;
        pid_t pidTerminado = waitpid(-1, &status, 0);

        if(pidTerminado == -1){
            if(errno == EINTR){
                continue;
            }
            break;
        }

        if(WIFSIGNALED(status) && WTERMSIG(status) == SIGINT){
            seremiLlego = 1;
        }
        if(seremiLlego){
            continue;
        }

        int posicion = BuscarProcesoActivo(procesosActivos, pidTerminado);

        if(posicion == -1){
            continue;
        }

        int idActividad = procesosActivos[posicion].idActividad;
        int pipeLectura = procesosActivos[posicion].pipeLectura;

        procesosActivos.erase(procesosActivos.begin() + posicion);

        bool salioBien = WIFEXITED(status) && WEXITSTATUS(status) == 0;

        char mensaje[100] = {0};
        ssize_t leidos = 0;

        if(salioBien){
            leidos = read(pipeLectura, mensaje, sizeof(mensaje) - 1);
        }
        close(pipeLectura);

        int posicionActividad = BuscarActividadPorId(actividades, idActividad);

        if(posicionActividad == -1){
            continue;
        }

        if(salioBien){

            if(leidos > 0){
                cout << "Mensaje recibido por pipe: " << mensaje << endl;
            }

            actividades[posicionActividad].SetEstado_Actividad(Estado::Finiquitao);

            cout << "Actividad "
                 << actividades[posicionActividad].GetNombre_Actividad()
                 << " (ID: " << idActividad
                 << ") ha sido finiquitadisima (en el padre!)."
                 << endl;
        }
        else{

            actividades[posicionActividad].SetEstado_Actividad(Estado::Aborto);

            cout << "Actividad "
                 << actividades[posicionActividad].GetNombre_Actividad()
                 << " (ID: " << idActividad
                 << ") se cayó (en el padre!), abortando su rama."
                 << endl;

            PurgaDeLaRama(actividades, idActividad);
        }
    }

    for(Actividades &actividad : actividades){
        if(actividad.GetEstado_Actividad() == Estado::Calmao){
            actividad.SetEstado_Actividad(Estado::Aborto);
            cout << "Actividad "
                 << actividad.GetNombre_Actividad()
                 << " (ID: " << actividad.GetId_Actividad()
                 << ") nunca pudo ejecutarse (dependencia inexistente o ciclo)." << endl;
        }
    }
}




int main(int argc, char **argv) {

    if(argc != 3){
        cout << "se supone que tienes que poner: ./planificador + el archivo + K" << endl;
        return 1;
    }

    ifstream planes(argv[1]);

    if(!planes.is_open()){
        cout << "No se pudo abrir el plan manito, seguro lo escribiste bien?" << endl;
        return 1;
    }

    int K = 0;

    try{
        K = stoi(argv[2]);
    }
    catch(const exception &e){
        cout << "K tiene que ser un número entero, no '" << argv[2] << "'" << endl;
        return 1;
    }

    if(K <= 0){
        cout << "K tiene que ser mayor que 0" << endl;
        return 1;
    }

    srand(time(nullptr));

    vector<Actividades> actividades;

    try{
        LeerArchivo(planes, actividades);
    }
    catch(const exception &e){
        cout << "El plan tiene una línea mal escrita (ID o tiempo no numérico)" << endl;
        planes.close();
        return 1;
    }
    planes.close();

    if(actividades.empty()){
        cout << "El plan está vacío, no hay nada que celebrar" << endl;
        return 1;
    }

    vector<PipesActividades> pipes = CrearPipes(actividades);

    struct sigaction accion;
    memset(&accion, 0, sizeof(accion));
    accion.sa_handler = ManejadorSeremi;
    sigemptyset(&accion.sa_mask);
    accion.sa_flags = 0;
    sigaction(SIGINT, &accion, nullptr);

    cout << "===== EJECUTANDO PLAN (" << actividades.size()
         << " actividades, K = " << K << ") =====" << endl;

    VacioInfinito(actividades, K, pipes);

    for(PipesActividades &conexion : pipes){
        close(conexion.pipeLectura);
        close(conexion.pipeEscritura);
    }

    int finalizadas = 0;
    int abortadas = 0;
    int pendientes = 0;

    for(Actividades &actividad : actividades){

        Estado estado = actividad.GetEstado_Actividad();

        if(estado == Estado::Finiquitao){
            finalizadas++;
        }
        else if(estado == Estado::Aborto){
            abortadas++;
        }
        else{
            pendientes++;
        }
    }

    cout << endl << "===== RESUMEN =====" << endl;
    cout << "Finalizadas: " << finalizadas << endl;
    cout << "Abortadas:   " << abortadas << endl;
    cout << "Pendientes:  " << pendientes << endl;

    if(seremiLlego){
        return 130;
    }

    return 0;
}