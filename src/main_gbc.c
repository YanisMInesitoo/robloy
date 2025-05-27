// main_gbc.c - Versión Robloy para Game Boy Color (GBC) usando GBDK-2020
#include <gb/gb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_ITEMS 10
#define MAX_PROMOCODES 32
#define MAX_PROMO_LEN 16
#define MAX_PROMO_DESC 48

// Estructuras
typedef struct {
    char nombre[16];
    UINT16 precio_robux;
    UINT8 gratis;
    UINT8 comprado;
    UINT8 equipado;
} ItemTienda;

typedef struct {
    char code[MAX_PROMO_LEN];
    char desc[MAX_PROMO_DESC];
} Promocode;

typedef struct {
    char nombre[13];
    char pass[7];
} UsuarioRobloy;

// Datos fijos
ItemTienda items[NUM_ITEMS] = {
    {"Sombrero", 0, 1, 0, 0},
    {"Zapas", 0, 1, 0, 0},
    {"SombreroOro", 50, 0, 0, 0},
    {"ZapasOro", 50, 0, 0, 0},
    {"Capa", 100, 0, 0, 0},
    {"Espada", 150, 0, 0, 0},
    {"DomDiamante", 250, 0, 0, 0},
    {"DomEsmeralda", 300, 0, 0, 0},
    {"DomOro", 350, 0, 0, 0},
    {"DomFuego", 400, 0, 0, 0}
};

Promocode promocodes[] = {
    {"A10F25G7XD", "+10 ROBUX"},
    {"FXD2789H25", "+25 ROBUX"},
    {"C2R2JOD3RN", "+40 ROBUX"},
    {"XYC4603RX0", "+55 ROBUX"},
    {"CHAMBA8XRX", "+70 ROBUX"},
    {"CAPITOROXD", "+80 ROBUX"},
    {"XD4XBROO42", "+200 ROBUX"},
    {"WEBOXXXX12", "SOMBRERO +200 MONEDAS"},
    {"X201FGH123", "ZAPAS +5 ROBUX"},
    {"12FGH129HD", "ZAPAS DE ORO"},
    {"XDXDGH12DH", "SOMBRERO DE ORO"},
    {"BRORBRORXD", "+300 MONEDAS"},
    {"SOXXCHAMBA", "+500 MONEDAS"},
    {"H9T0XD810D", "+1000 MONEDAS"},
    {"H9T0XD8102", "+2000 MONEDAS"},
    {"H9T0XD810H", "+3500 MONEDAS"},
    {"HDXD1024XD", "+5000 MONEDAS"},
    {"CAPA123456", "CAPA CONSEGUIDA"},
    {"ESPADA2025", "ESPADA CONSEGUIDA"},
    {"MASCARA777", "MASCARA CONSEGUIDA"},
    {"DOMDIAMANTE", "DOMINUS DIAMANTE CONSEGUIDO"},
    {"DOMESMERALD", "DOMINUS ESMERALDA CONSEGUIDO"},
    {"DOMORO2025", "DOMINUS ORO CONSEGUIDO"},
    {"DOMFUEGO99", "DOMINUS FUEGO CONSEGUIDO"},
    // Códigos de recarga nuevos
    {"HDX421CM", "+200 MONEDAS"},
    {"AL20URXD", "+400 MONEDAS"},
    {"H21XMSL2", "+900 MONEDAS"},
    {"GUL12456G", "+1500 MONEDAS"},
    {"SLGL24SM", "+2000 MONEDAS"},
    {"3X20MXYZ", "+4500 MONEDAS Y +500 ROBUX"},
    {"DRDC1MRM", "+6400 MONEDAS Y +1000 ROBUX"},
    {"ALA1XDXB", "+8400 MONEDAS Y +1200 ROBUX"},
    {"HLD2ADXR", "+9600 MONEDAS Y +2000 ROBUX"},
    {"AXDHD121", "+4500 ROBUX"}
};
UINT8 num_promocodes = sizeof(promocodes)/sizeof(promocodes[0]);

UsuarioRobloy usuarios_fijos[] = {
    {"Axelo907", "FGH2YH"},
    {"byYanis", "02YYH2"},
    {"xXBacon_Jih", "62HXY2"},
    {"pioelcapooo", "H24H26"},
    {"liamsamu4", "XDYH2D"},
    {"arti", "ARX20D"},
    {"tutifruti_61", "X2432I"},
};
UINT8 num_usuarios_fijos = sizeof(usuarios_fijos)/sizeof(usuarios_fijos[0]);

// Variables de juego
UINT16 saldo_monedas = 1000;
UINT16 saldo_robux = 0;
UINT8 es_premium = 0;
char last_username[13] = "";
char last_password[7] = "";

// Prototipos de funciones (deben ir antes de main y de cualquier uso)
void draw_menu(void);
void input_password(void);
void input_username(void);
void show_user_home(const char* username);
void show_robux_gbc(const char* username);
void generar_password(char* out);

// Reemplazo simple de strstr para GBDK (solo busca needle en haystack, retorna puntero o NULL)
char* strstr(const char* haystack, const char* needle) {
    const char* h;
    const char* n;
    if (!*needle) return (char*)haystack;
    while (*haystack) {
        h = haystack;
        n = needle;
        while (*h && *n && *h == *n) { h++; n++; }
        if (!*n) return (char*)haystack;
        haystack++;
    }
    return 0;
}

// Función auxiliar para generar una contraseña simple (pseudoaleatoria)
void generar_password(char* out) {
    static unsigned char seed = 0;
    char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (unsigned char i = 0; i < 6; i++) {
        out[i] = charset[(seed + i * 7) % 36];
    }
    out[6] = '\0';
    seed++;
}

void main() {
    DISPLAY_ON;
    SHOW_BKG;
    draw_menu();
}

void draw_menu() {
    printf("\n   Robloy GBC\n\n");
    printf("A: Iniciar sesión\nB: Crear cuenta\n");
    UINT8 key = waitpad(J_A | J_B);
    if (key & J_A) input_password();
    else if (key & J_B) input_username();
}

// Para evitar warnings de gets, usar fgets y limpiar salto de línea
void input_password() {
    char input[7] = "";
    printf("\nIntroduce contraseña (6 chars):\n");
    gets(input);
    UINT8 found = 0;
    char found_username[13] = "";
    for (UINT8 i = 0; i < num_usuarios_fijos; i++) {
        if (strcmp(input, usuarios_fijos[i].pass) == 0) {
            found = 1;
            strcpy(found_username, usuarios_fijos[i].nombre);
            break;
        }
    }
    if (!found && last_password[0] && strcmp(input, last_password) == 0) {
        found = 1;
        strcpy(found_username, last_username);
    }
    if (found) {
        show_user_home(found_username);
    } else {
        printf("\nContraseña incorrecta\n");
        waitpad(J_A|J_B|J_START);
        draw_menu();
    }
}

void input_username() {
    char username[13] = "";
    printf("\nNombre (max 12):\n");
    gets(username);
    char password[7];
    generar_password(password);
    strcpy(last_username, username);
    strcpy(last_password, password);
    printf("\nUsuario: %s\nPass: %s\nGuárdala!\n", username, password);
    waitpad(J_A|J_B|J_START);
    draw_menu();
}

void show_user_home(const char* username) {
    printf("\nBienvenido, %s\n", username);
    printf("Monedas: %u  Robux: %u\n", saldo_monedas, saldo_robux);
    printf("A: Tienda  B: Robux  SELECT: Volver\n");
    UINT8 key = waitpad(J_A | J_B | J_SELECT);
    if (key & J_A) {
        printf("\nTienda no implementada\n");
        waitpad(J_A|J_B|J_START);
        show_user_home(username);
    } else if (key & J_B) {
        show_robux_gbc(username);
        show_user_home(username);
    } else {
        draw_menu();
    }
}

void show_robux_gbc(const char* username) {
    (void)username;
    UINT8 salir = 0;
    while (!salir) {
        printf("\n--- Robux ---\n");
        printf("Monedas: %u  Robux: %u\n", saldo_monedas, saldo_robux);
        printf("A: Recargar por código  B: Volver\n");
        UINT8 key = waitpad(J_A | J_B);
        if (key & J_A) {
            char code[MAX_PROMO_LEN] = "";
            printf("\nIntroduce código promocional:\n");
            gets(code);
            UINT8 found = 0;
            for (UINT8 i = 0; i < num_promocodes; i++) {
                if (strcmp(code, promocodes[i].code) == 0) {
                    found = 1;
                    // Procesar recompensa
                    if (strstr(promocodes[i].desc, "Y")) {
                        // Recompensa combinada (monedas y robux)
                        UINT16 monedas = 0, robux = 0;
                        // Solo soporta un formato fijo: +XXXX MONEDAS Y +YYYY ROBUX
                        char* p = promocodes[i].desc;
                        monedas = (UINT16)((p[1]-'0')*1000 + (p[2]-'0')*100 + (p[3]-'0')*10 + (p[4]-'0'));
                        robux = (UINT16)((p[18]-'0')*1000 + (p[19]-'0')*100 + (p[20]-'0')*10 + (p[21]-'0'));
                        saldo_monedas += monedas;
                        saldo_robux += robux;
                        printf("\n¡Recarga exitosa! +%u Monedas y +%u Robux\n", monedas, robux);
                    } else if (strstr(promocodes[i].desc, "ROBUX")) {
                        UINT16 cantidad = 0;
                        // +XXXX ROBUX
                        char* p = promocodes[i].desc;
                        cantidad = (UINT16)((p[1]-'0')*1000 + (p[2]-'0')*100 + (p[3]-'0')*10 + (p[4]-'0'));
                        saldo_robux += cantidad;
                        printf("\n¡Recarga exitosa! +%u Robux\n", cantidad);
                    } else if (strstr(promocodes[i].desc, "MONEDAS")) {
                        UINT16 cantidad = 0;
                        // +XXXX MONEDAS
                        char* p = promocodes[i].desc;
                        cantidad = (UINT16)((p[1]-'0')*1000 + (p[2]-'0')*100 + (p[3]-'0')*10 + (p[4]-'0'));
                        saldo_monedas += cantidad;
                        printf("\n¡Recarga exitosa! +%u Monedas\n", cantidad);
                    } else if (strstr(promocodes[i].desc, "SOMBRERO DE ORO")) {
                        items[2].comprado = 1;
                        printf("\n¡SOMBRERO DE ORO conseguido!\n");
                    } else if (strstr(promocodes[i].desc, "ZAPAS DE ORO")) {
                        items[3].comprado = 1;
                        printf("\n¡ZAPAS DE ORO conseguidas!\n");
                    } else if (strstr(promocodes[i].desc, "SOMBRERO")) {
                        items[0].comprado = 1;
                        printf("\n¡SOMBRERO conseguido!\n");
                    } else if (strstr(promocodes[i].desc, "ZAPAS")) {
                        items[1].comprado = 1;
                        printf("\n¡ZAPAS conseguidas!\n");
                    } else if (strstr(promocodes[i].desc, "CAPA")) {
                        items[4].comprado = 1;
                        printf("\n¡CAPA conseguida!\n");
                    } else if (strstr(promocodes[i].desc, "ESPADA")) {
                        items[5].comprado = 1;
                        printf("\n¡ESPADA conseguida!\n");
                    } else if (strstr(promocodes[i].desc, "DOMINUS DIAMANTE")) {
                        items[6].comprado = 1;
                        printf("\n¡DOMINUS DIAMANTE conseguido!\n");
                    } else if (strstr(promocodes[i].desc, "DOMINUS ESMERALDA")) {
                        items[7].comprado = 1;
                        printf("\n¡DOMINUS ESMERALDA conseguido!\n");
                    } else if (strstr(promocodes[i].desc, "DOMINUS ORO")) {
                        items[8].comprado = 1;
                        printf("\n¡DOMINUS ORO conseguido!\n");
                    } else if (strstr(promocodes[i].desc, "DOMINUS FUEGO")) {
                        items[9].comprado = 1;
                        printf("\n¡DOMINUS FUEGO conseguido!\n");
                    } else {
                        printf("\n¡Código canjeado! %s\n", promocodes[i].desc);
                    }
                    break;
                }
            }
            if (!found) {
                printf("\nCódigo inválido o ya usado.\n");
            }
            printf("\nPulsa A para volver al menú Robux\n");
            waitpad(J_A);
        } else if (key & J_B) {
            salir = 1;
        }
    }
}
