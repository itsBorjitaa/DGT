#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "usuario.h"
#include "accidente.h"
#include "multa.h"
#include "vehiculo.h"
#include "database.h"
#include "logger.h"

#define MAX_INPUT 100

// variable global para almacenar el usuario actual
extern char usuarioActual[50];

void menuInicial() {
    int opcion;
    do {
        printf("\n--- DGT ---\n");
        printf("1. Iniciar sesion\n");
        printf("2. Registrarse\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                menuInicioSesion();
                break;
            case 2:
                menuRegistro();
                break;
            case 0:
                printf("Saliendo del sistema...\n");
                registrarAccion(usuarioActual, "Saliendo del sistema.");
                // Limpiar el archivo de log (borrar contenido)
                FILE *logFile = fopen("log.txt", "w"); 
                if (logFile == NULL) {
                    printf("Error al abrir el archivo de log para limpiar.\n");
                } else {
                    fclose(logFile); 
                }
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
        strcpy(usuarioActual, usuario);
        registrarAccion(usuarioActual, "Inicio de sesion exitoso");

        if (strcmp(rol, "usuario") == 0) {
            menuUsuario();
        } else if (strcmp(rol, "admin") == 0) {
            menuAdministrador();
        }
    } else {
        printf("Credenciales incorrectas.\n");
        registrarAccion(usuario, "Intento fallido de inicio de sesion");
    }
}


void menuRegistro() {
    char usuario[MAX_INPUT], contrasena[MAX_INPUT];
    char nombre[MAX_INPUT], apellidos[MAX_INPUT], dni[MAX_INPUT];
    char email[MAX_INPUT], telefono[MAX_INPUT];
    char rol[10];

    printf("\n--- Registro de Usuario ---\n");
    
    printf("Usuario: ");
    scanf("%s", usuario);
    
    if (existeUsuario(usuario)) {
        printf("El nombre de usuario ya existe. Por favor, elija otro.\n");
        registrarAccion("Sistema", "Intento fallido de registro: usuario ya existente");
        return;
    }
    
    printf("Contrasena: ");
    scanf("%s", contrasena);
    
    int n;
    do {
        printf("Seleccione Rol:\n");
        printf("0. Seleccionar Admin\n");
        printf("1. Seleccionar Usuario\n");
        printf("Introduzca la opcion: ");
        scanf("%d", &n);

        if (n == 0) {
            strcpy(rol, "admin"); 
        } else if (n == 1) {
            strcpy(rol, "usuario");
        } else {
            printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (n != 0 && n != 1);
    
    getchar();

    printf("Nombre: ");
    fgets(nombre, MAX_INPUT, stdin);
    nombre[strcspn(nombre, "\n")] = 0;
    
    printf("Apellidos: ");
    fgets(apellidos, MAX_INPUT, stdin);
    apellidos[strcspn(apellidos, "\n")] = 0;
    
    printf("DNI: ");
    scanf("%s", dni);
    
    getchar(); 
    
    printf("Email: ");
    scanf("%s", email);
    
    printf("Telefono: ");
    scanf("%s", telefono);

    if (registrarUsuario(usuario, contrasena, rol, nombre, apellidos, dni, email, telefono)) {
        printf("Usuario registrado correctamente.\n");
        registrarAccion(usuario, "Se registro correctamente en el sistema");
    } else {
        printf("Error al registrar el usuario.\n");
        registrarAccion(usuario, "Error al registrarse en el sistema");
    }
}


void menuUsuario() {
    int opcion;
    do {
        printf("\n--- Vista de Usuario ---\n");
        printf("1. Consultar datos personales\n");
        printf("2. Consultar datos de vehiculos\n");
        printf("3. Modificar informacion de vehiculo\n");
        printf("4. Aniadir nuevo vehiculo\n");
        printf("5. Consultar mis multas\n");
        printf("6. Pagar multa\n");
        printf("7. Exportar datos\n");
        printf("8. Registrar un accidente\n");
        printf("9. Consultar mis accidentes\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:  do {
                consultarDatosUsuario(); 
                printf("\nPresione 9 para volver al menu: ");
                scanf("%d", &opcion);
                getchar(); 
            } while (opcion != 9); 
            break;
            case 2: do {
                consultarDatosVehiculos();
                printf("\nPresione 9 para volver al menu: ");
                scanf("%d", &opcion);
                getchar(); 
            } while (opcion != 9); 
            break;
            case 3: modificarInformacionVehiculo(); break;
            case 4: agregarNuevoVehiculo(); break;
            case 5:  do {
                consultarMultasUsuario();
                printf("\nPresione 9 para volver al menu: ");
                scanf("%d", &opcion);
                getchar(); 
            } while (opcion != 9);
            break;
            case 6: pagarMultaUsuario(); break;
            case 7: guardarDatosUsuarioEnTXT(); break;
            case 8: {
                char fecha[20], descripcion[200];
                printf("Fecha del accidente (YYYY-MM-DD): ");
                scanf("%s", fecha);
                getchar();
                printf("Descripcion del accidente: ");
                fgets(descripcion, sizeof(descripcion), stdin);
                descripcion[strcspn(descripcion, "\n")] = 0;
                registrarAccidente(usuarioActual, fecha, descripcion);
                break;
            }
            case 9: consultarAccidentesUsuario(usuarioActual); break;
            case 0: printf("Saliendo de la vista de usuario...\n"); 
            registrarAccion(usuarioActual, "Usuario salio del menu");
            break;
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
        printf("3. Consultar todas las multas\n");
        printf("4. Aniadir multa\n");
        printf("5. Cambiar estado de multa\n");
        printf("6. Consultar datos de un accidente\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: consultarTodosLosUsuarios(); break;
            case 2: consultarTodosLosVehiculos(); break;
            case 3: consultarMultasAdmin(); break;
            case 4: agregarMultaAdmin(); break;
            case 5: cambiarEstadoMulta(); break;
            case 6: {
                int idAccidente;
                printf("Ingrese el ID del accidente a consultar: ");
                scanf("%d", &idAccidente);
                consultarDatosAccidente(idAccidente);
                break;
            }
            case 0: printf("Saliendo de la vista de administrador...\n"); break;
            default: printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (opcion != 0);
}