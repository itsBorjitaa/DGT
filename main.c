#include <stdio.h>
#include <string.h>
#include "database.h"
#include "menu.h"
#include <time.h>

// para compilar todo: gcc main.c database.c usuario.c menu.c sqlite3.c -o main
// para ejecutar .exe: ./main.exe
// usuarios de prueba:
//      NOMBRE      CONTRASENA
//      admin       admin123
//      prueba      prueba123

#define MAX_USUARIO 50
#define MAX_CONTRASENA 50

typedef struct {
    char usuario[MAX_USUARIO];
    char contrasena[MAX_CONTRASENA];
    char rol[10];
} Usuario;

sqlite3 *db;
char usuarioActual[MAX_USUARIO]; // variable global para almacenar el usuario actual

int main() {
    inicializarDB();
    
    menuInicial();

    sqlite3_close(db);
    return 0;
}