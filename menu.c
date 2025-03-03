#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "usuario.h"
#include "database.h"

#define MAX_INPUT 100

// Variable global para almacenar el usuario actual
extern char usuarioActual[50];

void menuInicial() {
    int opcion;
    do {
        printf("\n--- Sistema de Gestion DGT ---\n");
        printf("1. Iniciar sesion\n");
        printf("2. Registrarse\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer

        switch (opcion) {
            case 1:
                menuInicioSesion();
                break;
            case 2:
                menuRegistro();
                break;
            case 0:
                printf("Saliendo del sistema...\n");
                break;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (opcion != 0);
}

void menuInicioSesion() {
    char usuario[MAX_INPUT], contrasena[MAX_INPUT], rol[10];

    printf("\n--- Inicio de Sesion ---\n");
    printf("Usuario: ");
    scanf("%s", usuario);
    printf("Contrasena: ");
    scanf("%s", contrasena);

    if (verificarCredenciales(usuario, contrasena, rol)) {
        // Guardar el usuario actual en la variable global
        strcpy(usuarioActual, usuario);
        
        if (strcmp(rol, "usuario") == 0) {
            menuUsuario();
        } else if (strcmp(rol, "admin") == 0) {
            menuAdministrador();
        }
    } else {
        printf("Credenciales incorrectas.\n");
    }
}

void menuRegistro() {
    char usuario[MAX_INPUT], contrasena[MAX_INPUT];
    char nombre[MAX_INPUT], apellidos[MAX_INPUT], dni[MAX_INPUT];
    char email[MAX_INPUT], telefono[MAX_INPUT];

    printf("\n--- Registro de Usuario ---\n");
    
    printf("Usuario: ");
    scanf("%s", usuario);
    
    if (existeUsuario(usuario)) {
        printf("El nombre de usuario ya existe. Por favor, elija otro.\n");
        return;
    }
    
    printf("Contrasena: ");
    scanf("%s", contrasena);
    
    getchar(); // limpiar buffer
    
    printf("Nombre: ");
    fgets(nombre, MAX_INPUT, stdin);
    nombre[strcspn(nombre, "\n")] = 0; // eliminar salto de linea
    
    printf("Apellidos: ");
    fgets(apellidos, MAX_INPUT, stdin);
    apellidos[strcspn(apellidos, "\n")] = 0;
    
    printf("DNI: ");
    scanf("%s", dni);
    
    getchar(); // limpiar buffer
    
    printf("Email: ");
    scanf("%s", email);
    
    printf("Telefono: ");
    scanf("%s", telefono);

    if (registrarUsuario(usuario, contrasena, nombre, apellidos, dni, email, telefono)) {
        printf("Usuario registrado correctamente.\n");
    } else {
        printf("Error al registrar el usuario.\n");
    }
}

void menuUsuario() {
    int opcion;
    do {
        printf("\n--- Vista de Usuario ---\n");
        printf("1. Consultar datos personales\n");
        printf("2. Consultar datos de vehiculos\n");
        printf("3. Modificar informacion de vehiculo\n");
        printf("4. Anadir nuevo vehiculo\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: consultarDatosUsuario(); break;
            case 2: consultarDatosVehiculos(); break;
            case 3: modificarInformacionVehiculo(); break;
            case 4: agregarNuevoVehiculo(); break;
            case 0: printf("Saliendo de la vista de usuario...\n"); break;
            default: printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (opcion != 0);
}

void menuAdministrador() {
    int opcion;
    do {
        printf("\n--- Vista de Administrador ---\n");
        printf("1. Consultar todos los usuarios\n");
        printf("2. Consultar todos los vehiculos\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: consultarTodosLosUsuarios(); break;
            case 2: consultarTodosLosVehiculos(); break;
            case 0: printf("Saliendo de la vista de administrador...\n"); break;
            default: printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (opcion != 0);
}