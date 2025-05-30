#include <stdio.h>
#include <time.h>
#include <string.h>
#include "logger.h"

// Función para registrar acciones del usuario en un archivo de log
void registrarAccion(const char *usuario, const char *accion) {
    char filename[100];
    snprintf(filename, sizeof(filename), "log.txt", usuario);  

    FILE *logFile = fopen(filename, "a");
    if (logFile == NULL) {
        printf("Error al abrir el archivo de log.\n");
        return;
    }

    time_t t;
    time(&t);
    struct tm *tm_info = localtime(&t);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(logFile, "[%s] Usuario: %s - Accion: %s\n", timestamp, usuario, accion);
    fclose(logFile);
}
