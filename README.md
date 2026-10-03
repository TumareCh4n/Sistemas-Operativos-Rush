# Planificador Dieciochero (Tarea 1 SO)

Lee un plan de actividades (DAG), las ejecuta con `fork` respetando dependencias y un máximo de K procesos a la vez. Si llega Ctrl+C, llega la Seremi y se cae todo.

## Compilar y correr

```
g++ -Wall -Wextra -std=c++17 main.cpp -o planificador
./planificador plan.txt K
```

## Formato del plan

```
ID : nombre : tiempo_ms : dep1, dep2
```

Si no hay tiempo → se asigna uno random entre 100 y 5000 ms.

---

## Qué hace cada cosa

### Estado (enum)
- `Calmao` → todavía no parte
- `Dandole` → ejecutándose
- `Finiquitao` → terminó bien
- `Aborto` → se cayó o la abortaron

### Clase `Actividades`
- Constructor → guarda nombre, id, tiempo, dependencias → estado inicial `Calmao`
- `SetNombre_Actividad / SetId_Actividad / SetTiempo / SetEstado_Actividad / SetDependencias` → cambian el campo correspondiente
- `GetNombre_Actividad / GetId_Actividad / GetTiempo / GetDependencias / GetEstado_Actividad` → devuelven el campo correspondiente
- `PrintWeas` → imprime nombre, id, tiempo y dependencias (para debug)

### Lectura del archivo
- `Random` → devuelve un número entre 100 y 5000 → tiempo cuando el plan no trae uno
- `quitarespaciosinador` → recibe un string → le saca espacios, tabs y saltos de línea de los extremos
- `desglosardependenciasinador` → recibe "1, 2" → separa por comas → devuelve `vector<int>` {1, 2}
- `LeerArchivo` → lee línea por línea → ignora líneas vacías → separa por `:` → arma cada `Actividades` → la mete al vector

### Búsquedas y dependencias
- `BuscarActividadPorId` → recorre el vector → devuelve la posición de la actividad o -1 si no existe
- `ritualinverso` → recibe un id → devuelve los ids de las actividades que dependen de él (los dependientes)
- `Estalisteilor` → revisa las dependencias de una actividad → true solo si todas existen y están `Finiquitao`

### Structs
- `ListaProcesosActivos` → pid + id de la actividad + pipe de lectura hacia el padre
- `PipesActividades` → id origen → id destino + extremos de lectura y escritura del pipe

### Pipes entre actividades
- `CrearPipes` → por cada arista origen → dependiente crea un pipe → devuelve todos los `PipesActividades`
- `BuscarPipesDeOrigen` → filtra los pipes por los que sale una actividad (para escribir)
- `BuscarPipesDeDestino` → filtra los pipes por los que llega a una actividad (para leer)
- `BuscarProcesoActivo` → busca un pid en los procesos activos → devuelve la posición o -1

### Ejecución de una actividad
- `ExpansionDeDominio` → crea un pipe hacia el padre → `fork`
  - **hijo** → restaura SIGINT por defecto → lee el mensaje de cada dependencia → imprime que se expandió → `usleep` por el tiempo → escribe "TERMINADA" al padre y a sus dependientes → `_exit(0)`
  - **padre** → guarda el pipe de lectura → marca la actividad `Dandole` → devuelve el pid

### La Seremi (Ctrl+C)
- `seremiLlego` → bandera global que se levanta cuando llega SIGINT
- `ManejadorSeremi` → handler de SIGINT → solo levanta la bandera, nada más

### Fallas
- `PurgaDeLaRama` → recibe una actividad fallida → busca sus dependientes → los marca `Aborto` → repite con los dependientes de esos → hasta que se acaba la rama

### El loop principal
- `VacioInfinito` → repite hasta que no quede nada por hacer:
  1. si llegó la Seremi → mata los procesos activos (`SIGTERM`) → los espera → marca todo lo pendiente como `Aborto` → termina
  2. recorre las actividades → lanza las que estén `Calmao` y listas → sin pasarse de K procesos
  3. si no hay procesos corriendo → se acabó, sale
  4. `waitpid` → espera que termine alguno
  5. terminó bien → lee su mensaje del pipe → `Finiquitao`
  6. terminó mal → `Aborto` → `PurgaDeLaRama`
  7. al salir → lo que quedó `Calmao` (ciclo o dependencia inexistente) → `Aborto`

### main
- valida argumentos (archivo y K > 0) → `srand` → `LeerArchivo` → `CrearPipes` → instala el handler de SIGINT → `VacioInfinito` → cierra los pipes → imprime el resumen (finalizadas / abortadas / pendientes)
- retorna 130 si fue por Ctrl+C, 0 si no