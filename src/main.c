// main.c - Entrada principal de Robloy para GBA
#include <gba_console.h>
#include <gba_video.h>
#include <gba_input.h>
#include <gba_systemcalls.h> // Cambiado de <gba_system.h> a <gba_systemcalls.h>
#include <gba_interrupt.h> // Añadido para irqInit, irqEnable y IRQ_VBLANK
#include <stdio.h>
#include <stdlib.h> // Para rand()
#include <string.h>

// Prototipo para evitar declaración implícita
typedef unsigned short u16;
void consoleClearRegion(int left, int top, int width, int height);

// Implementación mínima de consoleClearRegion para GBA (borra la pantalla de texto)
void consoleClearRegion(int left, int top, int width, int height) {
    for (int y = top; y < top + height; y++) {
        for (int x = left; x < left + width; x++) {
            iprintf("\x1b[%d;%dH ", y, x);
        }
    }
}

// --- Estructuras y defines ---
#define NUM_ITEMS 10
#define MAX_PROMOCODES 32
#define MAX_PROMO_LEN 16
#define MAX_PROMO_DESC 48
#define MAX_USUARIOS_ESPECIALES 16

typedef struct {
    const char* nombre;
    int precio_robux;
    int gratis;
    int comprado;
    int equipado;
} ItemTienda;

typedef struct {
    char code[MAX_PROMO_LEN];
    char desc[MAX_PROMO_DESC];
} Promocode;

typedef struct {
    char nombre[20];
    char pass[16];
} UsuarioEspecial;

// --- Variables globales ---
char last_username[13] = "";
char last_password[7] = "";
int saldo_monedas = 1000;
int saldo_robux = 0;
int es_premium = 0; // 0: normal, 1: premium
int tiene_sombrero = 0, tiene_zapas = 0, tiene_sombrero_oro = 0, tiene_zapas_oro = 0;
int tiene_capa = 0, tiene_espada = 0, tiene_mascara = 0;
ItemTienda items[NUM_ITEMS] = {
    {"Sombrero", 0, 1, 0, 0},
    {"Zapas", 0, 1, 0, 0},
    {"Sombrero de oro", 50, 0, 0, 0},
    {"Zapas de oro", 50, 0, 0, 0},
    {"Capa", 100, 0, 0, 0},
    {"Espada", 150, 0, 0, 0},
    {"Dominus Diamante", 250, 0, 0, 0},
    {"Dominus Esmeralda", 300, 0, 0, 0},
    {"Dominus Oro", 350, 0, 0, 0},
    {"Dominus Fuego", 400, 0, 0, 0}
};
Promocode promocodes[MAX_PROMOCODES];
int num_promocodes = 0;
UsuarioEspecial usuarios_especiales[MAX_USUARIOS_ESPECIALES];
int num_usuarios_especiales = 0;

// --- Prototipos de funciones principales ---
void input_password();
void input_username();
void show_user_home(const char* username);
void show_settings(const char* username);
void show_shop(const char* username);
void show_robux(const char* username);
void show_games(const char* username);
void juego_rivales_hachas(const char* username);
void juego_obby_coins(const char* username);
void save_progress();
void draw_menu();
void draw_keyboard(char input[7], int cursor_row, int cursor_col);

// --- Definición de teclado virtual para login ---
#define KEYBOARD_ROWS 3
#define KEYBOARD_COLS 10
const char keyboard[KEYBOARD_ROWS][KEYBOARD_COLS+1] = {
    "ABCDEF1234",
    "GHIJKL5678",
    "MNOPQR90ZX"
};

// --- Funciones de carga vacías para evitar crash en hardware/emulador ---
void load_account() {}
void load_promocodes() {}
void load_usuarios_especiales() {}
void load_progress() {}

// --- Funciones de carga ---
void load_account();
void load_progress();
void load_promocodes();
void load_usuarios_especiales();

#define consoleClear() consoleClearRegion(0,0,30,32)

int main(void) {
    // Inicializa la consola de texto
    irqInit();
    irqEnable(IRQ_VBLANK);
    consoleDemoInit();

    load_account();
    load_progress();
    load_promocodes(); // Cargar promocodes desde archivo
    load_usuarios_especiales(); // Cargar usuarios especiales desde archivo

    draw_menu();

    while (1) {
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDown();
        if (keys & KEY_A) {
            // Acción para Iniciar sesión
            input_password();
        } else if (keys & KEY_B) {
            // Acción para Crear cuenta
            input_username();
        }
    }
}

void input_password() {
    char input[7] = "";
    int pos = 0;
    int cursor_row = 0, cursor_col = 0;
    int done = 0;
    while (!done) {
        draw_keyboard(input, cursor_row, cursor_col);
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDownRepeat();
        if (keys & KEY_UP && cursor_row > 0) cursor_row--;
        if (keys & KEY_DOWN && cursor_row < KEYBOARD_ROWS-1) cursor_row++;
        if (keys & KEY_LEFT && cursor_col > 0) cursor_col--;
        if (keys & KEY_RIGHT && cursor_col < KEYBOARD_COLS-1) cursor_col++;
        if ((keys & KEY_A) && pos < 6) {
            input[pos++] = keyboard[cursor_row][cursor_col];
            input[pos] = '\0';
        }
        if ((keys & KEY_B) && pos > 0) {
            pos--;
            input[pos] = '\0';
        }
        if (pos == 6) done = 1;
    }
    consoleClear();
    iprintf("\x1b[10;8HContraseña: %s", input);
    // Leer archivo de cuenta y validar contraseña
    FILE* f = fopen("account.txt", "r");
    char file_username[20] = "";
    char file_password[16] = "";
    int found = 0;
    if (f) {
        fgets(file_username, sizeof(file_username), f);
        fgets(file_password, sizeof(file_password), f);
        // Eliminar salto de línea
        file_password[strcspn(file_password, "\r\n")] = 0;
        fclose(f);
        if (strcmp(input, file_password) == 0) {
            found = 1;
        }
    }
    if (found) {
        show_user_home(file_username);
    } else {
        iprintf("\x1b[12;8HContraseña incorrecta");
        VBlankIntrWait();
        for (int i = 0; i < 120; i++) VBlankIntrWait();
        draw_menu();
    }
}

void save_account(const char* username, const char* password) {
    // Guardar en account.txt (última cuenta usada)
    FILE* f = fopen("account.txt", "w");
    if (f) {
        fprintf(f, "%s\n%s\n", username, password);
        fclose(f);
    }
    // Añadir a usuarios.txt (todas las cuentas)
    FILE* fu = fopen("usuarios.txt", "a");
    if (fu) {
        fprintf(fu, "%s:%s\n", username, password);
        fclose(fu);
    }
}

void input_username() {
    char username[13] = "";
    int pos = 0;
    int cursor_row = 0, cursor_col = 0;
    int done = 0;
    // Teclado solo con letras
    const int NAME_ROWS = 3;
    const int NAME_COLS = 9;
    const char name_keyboard[3][10] = {
        "ABCDEFGHI",
        "JKLMNOPQR",
        "STUVWXYZ "
    };
    while (!done) {
        // Dibuja teclado para nombre
        consoleClear();
        iprintf("\x1b[2;8HEscoje tu nombre:");
        iprintf("\x1b[4;10H[%s]", username);
        for (int r = 0; r < NAME_ROWS; r++) {
            for (int c = 0; c < NAME_COLS; c++) {
                if (r == cursor_row && c == cursor_col) {
                    iprintf("\x1b[%d;%dH[%c]", 6+r, 10+c*2, name_keyboard[r][c]);
                } else {
                    iprintf("\x1b[%d;%dH %c ", 6+r, 10+c*2, name_keyboard[r][c]);
                }
            }
        }
        iprintf("\x1b[12;5HCruceta: mover  A: elegir  B: borrar");
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDownRepeat();
        if (keys & KEY_UP && cursor_row > 0) cursor_row--;
        if (keys & KEY_DOWN && cursor_row < NAME_ROWS-1) cursor_row++;
        if (keys & KEY_LEFT && cursor_col > 0) cursor_col--;
        if (keys & KEY_RIGHT && cursor_col < NAME_COLS-1) cursor_col++;
        if ((keys & KEY_A) && pos < 12) {
            username[pos++] = name_keyboard[cursor_row][cursor_col];
            username[pos] = '\0';
        }
        if ((keys & KEY_B) && pos > 0) {
            pos--;
            username[pos] = '\0';
        }
        if (pos == 12) done = 1;
    }
    // Generar contraseña aleatoria de 6 caracteres (letras y números)
    char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char password[7];
    for (int i = 0; i < 6; i++) {
        password[i] = charset[rand() % 36];
    }
    password[6] = '\0';
    // Guardar usuario y contraseña globalmente
    strcpy(last_username, username);
    strcpy(last_password, password);
    // Guardar en la tarjeta de memoria
    save_account(username, password);
    consoleClear();
    iprintf("\x1b[8;6HNombre creado: %s", username);
    iprintf("\x1b[10;6HTu contraseña es: %s", password);
    iprintf("\x1b[12;2H¡Guárdala para iniciar sesión!");
    VBlankIntrWait();
    for (int i = 0; i < 240; i++) VBlankIntrWait(); // Espera ~4 segundos
    draw_menu();
}

void draw_keyboard(char input[7], int cursor_row, int cursor_col) {
    consoleClear();
    iprintf("\x1b[2;8HIntroduce contraseña:");
    iprintf("\x1b[4;10H[%s]", input);
    for (int r = 0; r < KEYBOARD_ROWS; r++) {
        for (int c = 0; c < KEYBOARD_COLS; c++) {
            if (r == cursor_row && c == cursor_col) {
                iprintf("\x1b[%d;%dH[%c]", 6+r, 10+c*2, keyboard[r][c]);
            } else {
                iprintf("\x1b[%d;%dH %c ", 6+r, 10+c*2, keyboard[r][c]);
            }
        }
    }
    iprintf("\x1b[12;5HUsa la cruceta para mover, A para elegir, B borrar");
}

// Dibuja un cursor en la pantalla (excepto en los teclados)
void draw_cursor(int x, int y) {
    iprintf("\x1b[%d;%dH>", y, x);
}

void draw_menu() {
    // Limpia la pantalla
    consoleClear();
    // Título centrado (aproximadamente en la parte superior)
    iprintf("\x1b[10;12HRobloy");
    // Botón Iniciar sesión (parte inferior)
    iprintf("\x1b[18;10H[ Iniciar sesión ]");
    // Botón Crear cuenta (debajo)
    iprintf("\x1b[19;11H[ Crear cuenta ]");
}

void show_user_home(const char* username) {
    int option = 0;
    const int options_x[5] = {32, 37, 43, 24, 28};
    const int options_y[5] = {0, 0, 0, 0, 0};
    while (1) {
        consoleClear();
        iprintf("\x1b[0;24HInicio");
        // Mostrar nombre de usuario con icono premium si corresponde
        if (es_premium) {
            iprintf("\x1b[1;24H%s \x1b[33m★\x1b[37m", username); // Estrella dorada como icono premium
        } else {
            iprintf("\x1b[1;24H%s", username);
        }
        if (es_premium) iprintf("\x1b[1;36H[PREMIUM]");
        // Dibujo de Robux (amarillo/dorado, usando caracteres ASCII)
        iprintf("\x1b[0;32H\x1b[33m  ▄▄▄  \x1b[37m");
        iprintf("\x1b[1;32H\x1b[33m ▀   ▀ \x1b[37m");
        iprintf("\x1b[2;32H\x1b[33m| R$ |");
        iprintf("\x1b[3;32H ▀▀▀  \x1b[37m");
        // Icono de configuración (mitad tuerca, mitad bolsa)
        iprintf("\x1b[0;37H\x1b[36m(⚙️/🛍️)\x1b[37m");
        // Icono de tienda (mitad bolsa, mitad tuerca)
        iprintf("\x1b[0;43H\x1b[36m(🛍️/⚙️)\x1b[37m");
        iprintf("\x1b[0;28H[JUEGOS]");
        iprintf("\x1b[2;12H¡Bienvenido, %s!", username);
        if (es_premium) {
            iprintf("\x1b[3;12H¡Ventajas premium activas!");
            iprintf("\x1b[4;12H- Ítem exclusivo: Corona Premium");
            iprintf("\x1b[5;12H- +10%% Robux en compras");
        }
        iprintf("\x1b[4;2HPremios/Ítems: ");
        int y = 5;
        for (int i = 0; i < NUM_ITEMS; i++) {
            if (items[i].comprado) {
                iprintf("\x1b[%d;4H%s", y++, items[i].nombre);
                if (items[i].equipado) iprintf(" (Equipado)");
            }
        }
        if (es_premium) {
            iprintf("\x1b[%d;4HCorona Premium (Exclusivo)", y++);
        }
        if (tiene_mascara) { iprintf("\x1b[%d;4HMáscara", y++); }
        iprintf("\x1b[22;2HUsa la cruceta y A para elegir. Start: Menú");
        draw_cursor(options_x[option], options_y[option]);
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDownRepeat();
        if (keys & KEY_LEFT && option > 0) option--;
        if (keys & KEY_RIGHT && option < 4) option++;
        if (keys & KEY_A) {
            if (option == 0) show_robux(username);
            else if (option == 1) show_settings(username);
            else if (option == 2) show_shop(username);
            else if (option == 3) { draw_menu(); return; }
            else if (option == 4) show_games(username);
        }
        if (keys & KEY_START) {
            save_progress();
            draw_menu();
            return;
        }
        // Guardar progreso con SELECT + START
        if ((keys & KEY_START) && (keys & KEY_SELECT)) {
            save_progress();
            iprintf("\x1b[20;2HProgreso guardado!");
            VBlankIntrWait();
            for (int i = 0; i < 60; i++) VBlankIntrWait();
        }
    }
}

void show_settings(const char* username) {
    int option = 0;
    while (1) {
        consoleClear();
        iprintf("\x1b[2;10H--- Configuración ---");
        iprintf("\x1b[4;8H1. Cerrar sesión");
        draw_cursor(7, 4); // Cursor al lado de "Cerrar sesión"
        iprintf("\x1b[22;2HA para cerrar sesión, B para volver");
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDown();
        if (keys & KEY_A) {
            save_progress();
            draw_menu();
            return;
        }
        if (keys & KEY_B) {
            show_user_home(username);
            return;
        }
    }
}

void show_shop(const char* username) {
    int option = 0;
    int salir = 0;
    while (!salir) {
        consoleClear();
        iprintf("\x1b[2;10H--- Tienda ---");
        iprintf("\x1b[3;4HMonedas: %d   Robux: %d", saldo_monedas, saldo_robux);
        for (int i = 0; i < NUM_ITEMS; i++) {
            int y = 5 + i;
            iprintf("\x1b[%d;6H%s", y, items[i].nombre);
            if (items[i].gratis) {
                iprintf(" (Gratis)");
            } else {
                iprintf(" (%d R$)", items[i].precio_robux);
            }
            if (items[i].comprado) {
                if (items[i].equipado) iprintf(" [Equipado]");
                else iprintf(" [Comprado]");
            }
            if (option == i) draw_cursor(4, y);
        }
        iprintf("\x1b[20;2HA: Comprar/Equipar  B: Volver  Arriba/Abajo: Mover");
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDownRepeat();
        if ((keys & KEY_DOWN) && option < NUM_ITEMS-1) option++;
        if ((keys & KEY_UP) && option > 0) option--;
        if (keys & KEY_B) {
            salir = 1;
        }
        if (keys & KEY_A) {
            if (!items[option].comprado) {
                if (items[option].gratis || saldo_robux >= items[option].precio_robux) {
                    if (!items[option].gratis) saldo_robux -= items[option].precio_robux;
                    items[option].comprado = 1;
                    // Sincronizar variables antiguas
                    if (option == 0) tiene_sombrero = 1;
                    if (option == 1) tiene_zapas = 1;
                    if (option == 2) tiene_sombrero_oro = 1;
                    if (option == 3) tiene_zapas_oro = 1;
                    if (option == 4) tiene_capa = 1;
                    if (option == 5) tiene_espada = 1;
                    // Equipar automáticamente al comprar
                    for (int j = 0; j < NUM_ITEMS; j++) items[j].equipado = 0;
                    items[option].equipado = 1;
                } else {
                    iprintf("\x1b[22;2HNo tienes suficientes Robux!");
                    VBlankIntrWait();
                    for (int i = 0; i < 60; i++) VBlankIntrWait();
                }
            } else {
                // Equipar/des-equipar
                if (!items[option].equipado) {
                    for (int j = 0; j < NUM_ITEMS; j++) items[j].equipado = 0;
                    items[option].equipado = 1;
                } else {
                    items[option].equipado = 0;
                }
            }
            save_progress();
        }
    }
    show_user_home(username);
}

void show_robux(const char* username) {
    int option = 0;
    const int num_paquetes = 5;
    const int robux_packs[5] = {50, 200, 1000, 10000, 25300};
    const int precios[5] = {100, 350, 1500, 11000, 25000}; // Precios en monedas
    int salir = 0;
    int premium_option = num_paquetes; // Opción extra para premium
    int premium_precio = 3200; // Premium ahora cuesta 3200 monedas
    while (!salir) {
        consoleClear();
        iprintf("\x1b[2;10H--- Comprar Robux ---");
        iprintf("\x1b[4;6HTu saldo: %d monedas | %d R$", saldo_monedas, saldo_robux);
        for (int i = 0; i < num_paquetes; i++) {
            iprintf("\x1b[%d;8H%d R$ - %d monedas", 6+i*2, robux_packs[i], precios[i]);
            if (option == i) draw_cursor(6, 6+i*2);
        }
        // Opción de premium
        if (es_premium) {
            iprintf("\x1b[%d;8H[PREMIUM ACTIVADO]", 6+num_paquetes*2);
        } else {
            iprintf("\x1b[%d;8HComprar Premium - %d monedas", 6+num_paquetes*2, premium_precio);
            if (option == premium_option) draw_cursor(6, 6+num_paquetes*2);
        }
        iprintf("\x1b[14;4HA: Comprar  B: Volver  Arriba/Abajo: Mover");
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDownRepeat();
        int max_option = es_premium ? num_paquetes-1 : num_paquetes;
        if ((keys & KEY_DOWN) && option < max_option) option++;
        if ((keys & KEY_UP) && option > 0) option--;
        if (keys & KEY_B) {
            salir = 1;
        }
        if (keys & KEY_A) {
            if (option < num_paquetes) {
                if (saldo_monedas >= precios[option]) {
                    saldo_monedas -= precios[option];
                    saldo_robux += robux_packs[option];
                    save_progress();
                    iprintf("\x1b[16;4H¡Compra exitosa! +%d R$", robux_packs[option]);
                } else {
                    iprintf("\x1b[16;4HNo tienes suficientes monedas");
                }
            } else if (option == premium_option && !es_premium) {
                if (saldo_monedas >= premium_precio) {
                    saldo_monedas -= premium_precio;
                    es_premium = 1;
                    save_progress();
                    iprintf("\x1b[16;4H¡Ahora eres usuario PREMIUM!");
                } else {
                    iprintf("\x1b[16;4HNo tienes suficientes monedas para premium");
                }
            }
            VBlankIntrWait();
            for (int i = 0; i < 60; i++) VBlankIntrWait();
        }
    }
    show_user_home(username);
}

void redeem_promocode(const char* username) {
    char code[11] = "";
    int pos = 0;
    int cursor_row = 0, cursor_col = 0;
    int done = 0;
    // Teclado para promocode: letras y números
    const int CODE_ROWS = 4;
    const int CODE_COLS = 9;
    const char code_keyboard[4][10] = {
        "ABCDEF123",
        "GHIJKL456",
        "MNOPQR789",
        "STUVWXYZ0"
    };
    while (!done) {
        consoleClear();
        iprintf("\x1b[2;8HIntroduce promocode:");
        iprintf("\x1b[4;10H[%s]", code);
        for (int r = 0; r < CODE_ROWS; r++) {
            for (int c = 0; c < CODE_COLS; c++) {
                if (r == cursor_row && c == cursor_col) {
                    iprintf("\x1b[%d;%dH[%c]", 6+r, 10+c*2, code_keyboard[r][c]);
                } else {
                    iprintf("\x1b[%d;%dH %c ", 6+r, 10+c*2, code_keyboard[r][c]);
                }
            }
        }
        iprintf("\x1b[12;5HCruceta: mover  A: elegir  B: borrar");
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDownRepeat();
        if (keys & KEY_UP && cursor_row > 0) cursor_row--;
        if (keys & KEY_DOWN && cursor_row < CODE_ROWS-1) cursor_row++;
        if (keys & KEY_LEFT && cursor_col > 0) cursor_col--;
        if (keys & KEY_RIGHT && cursor_col < CODE_COLS-1) cursor_col++;
        if ((keys & KEY_A) && pos < 10) {
            code[pos++] = code_keyboard[cursor_row][cursor_col];
            code[pos] = '\0';
        }
        if ((keys & KEY_B) && pos > 0) {
            pos--;
            code[pos] = '\0';
        }
        if (pos == 10) done = 1;
    }
    // Validar promocode desde el archivo cargado
    int encontrado = 0;
    char mensaje[64] = "";
    for (int i = 0; i < num_promocodes; i++) {
        if (strcmp(code, promocodes[i].code) == 0) {
            encontrado = 1;
            // Procesar recompensa según la descripción
            if (strstr(promocodes[i].desc, "ROBUX")) {
                int cantidad = 0;
                sscanf(promocodes[i].desc, "+%d ROBUX", &cantidad);
                saldo_robux += cantidad;
                snprintf(mensaje, sizeof(mensaje), "+%d ROBUX!", cantidad);
            } else if (strstr(promocodes[i].desc, "MONEDAS")) {
                int cantidad = 0;
                sscanf(promocodes[i].desc, "+%d MONEDAS", &cantidad);
                saldo_monedas += cantidad;
                snprintf(mensaje, sizeof(mensaje), "+%d MONEDAS!", cantidad);
                if (strstr(promocodes[i].desc, "SOMBRERO")) { tiene_sombrero = 1; items[0].comprado = 1; }
            } else if (strstr(promocodes[i].desc, "SOMBRERO DE ORO")) { tiene_sombrero_oro = 1; items[2].comprado = 1; strcpy(mensaje, "¡SOMBRERO DE ORO!"); }
            else if (strstr(promocodes[i].desc, "ZAPAS DE ORO")) { tiene_zapas_oro = 1; items[3].comprado = 1; strcpy(mensaje, "¡ZAPAS DE ORO!"); }
            else if (strstr(promocodes[i].desc, "ZAPAS")) { tiene_zapas = 1; items[1].comprado = 1; if (strstr(promocodes[i].desc, "+")) { int r=0; sscanf(promocodes[i].desc, "ZAPAS +%d ROBUX", &r); saldo_robux+=r; snprintf(mensaje, sizeof(mensaje), "¡ZAPAS +%d ROBUX!", r); } else strcpy(mensaje, "¡ZAPAS CONSEGUIDAS!"); }
            else if (strstr(promocodes[i].desc, "CAPA")) { tiene_capa = 1; items[4].comprado = 1; strcpy(mensaje, "¡Capa conseguida!"); }
            else if (strstr(promocodes[i].desc, "ESPADA")) { tiene_espada = 1; items[5].comprado = 1; strcpy(mensaje, "¡Espada conseguida!"); }
            else if (strstr(promocodes[i].desc, "MÁSCARA")) { tiene_mascara = 1; strcpy(mensaje, "¡Máscara conseguida!"); }
            else if (strstr(promocodes[i].desc, "DOMINUS DIAMANTE")) { items[6].comprado = 1; strcpy(mensaje, "¡Dominus Diamante conseguido!"); }
            else if (strstr(promocodes[i].desc, "DOMINUS ESMERALDA")) { items[7].comprado = 1; strcpy(mensaje, "¡Dominus Esmeralda conseguido!"); }
            else if (strstr(promocodes[i].desc, "DOMINUS ORO")) { items[8].comprado = 1; strcpy(mensaje, "¡Dominus Oro conseguido!"); }
            else if (strstr(promocodes[i].desc, "DOMINUS FUEGO")) { items[9].comprado = 1; strcpy(mensaje, "¡Dominus Fuego conseguido!"); }
            else { strncpy(mensaje, promocodes[i].desc, sizeof(mensaje)-1); mensaje[sizeof(mensaje)-1] = '\0'; }
            break;
        }
    }
    if (!encontrado) {
        strcpy(mensaje, "Promocode inválido");
    }
    consoleClear();
    iprintf("\x1b[10;6H%s", mensaje);
    iprintf("\x1b[12;2HPulsa cualquier botón para volver");
    // Sincronizar premios especiales con los ítems al canjear promocodes
    if (tiene_sombrero) { items[0].comprado = 1; }
    if (tiene_zapas) { items[1].comprado = 1; }
    if (tiene_sombrero_oro) { items[2].comprado = 1; }
    if (tiene_zapas_oro) { items[3].comprado = 1; }
    if (tiene_capa) { items[4].comprado = 1; }
    if (tiene_espada) { items[5].comprado = 1; }
    save_progress();
    VBlankIntrWait();
    while (1) {
        scanKeys();
        if (keysDown()) break;
        VBlankIntrWait();
    }
}

void show_games(const char* username) {
    int option = 0;
    // Declarar los juegos y variables dentro del bucle principal para evitar errores de ámbito
    while (1) {
        const char* juegos[] = {
            "Obby de lava",
            "Escape del Noob",
            "Simulador de Robux",
            "Tycoon de Casas",
            "Aventura en la Jungla",
            "RIVALES HACHAS",
            "OBBY COINS"
        };
        int num_juegos = sizeof(juegos)/sizeof(juegos[0]);
        int salir = 0;
        while (!salir) {
            consoleClear();
            iprintf("\x1b[2;10H--- Juegos ---");
            for (int i = 0; i < num_juegos; i++) {
                iprintf("\x1b[%d;8H%s", 5+i, juegos[i]);
                if (option == i) draw_cursor(6, 5+i);
            }
            iprintf("\x1b[20;2HA: Jugar  B: Volver  Arriba/Abajo: Mover");
            VBlankIntrWait();
            scanKeys();
            u16 keys = keysDownRepeat();
            if ((keys & KEY_DOWN) && option < num_juegos-1) option++;
            if ((keys & KEY_UP) && option > 0) option--;
            if (keys & KEY_B) salir = 1;
            if (keys & KEY_A) {
                if (option == num_juegos-2) {
                    juego_rivales_hachas(username);
                } else if (option == num_juegos-1) {
                    juego_obby_coins(username);
                } else {
                    consoleClear();
                    iprintf("\x1b[10;6HCargando: %s...", juegos[option]);
                    iprintf("\x1b[12;2HPulsa cualquier botón para volver");
                    VBlankIntrWait();
                    while (1) { scanKeys(); if (keysDown()) break; VBlankIntrWait(); }
                }
            }
        }
        show_user_home(username);
        break;
    }
}

void juego_rivales_hachas(const char* username) {
    int vida_jugador = 20;
    int vida_bot = 20;
    int fuerza = 0;
    int turno = 1; // 1: jugador, 0: bot
    int salir = 0;
    char mensaje[32] = "";
    while (!salir) {
        consoleClear();
        iprintf("\x1b[2;10H--- Rivales Hachas ---");
        iprintf("\x1b[4;6HTu vida: %d", vida_jugador);
        iprintf("\x1b[5;6HVida bot: %d", vida_bot);
        iprintf("\x1b[7;6HFuerza extra: %d", fuerza);
        if (mensaje[0]) iprintf("\x1b[9;4H%s", mensaje);
        if (vida_jugador <= 0 || vida_bot <= 0) {
            iprintf("\x1b[12;4H%s", vida_jugador > 0 ? "¡Ganaste!" : "Perdiste...");
            iprintf("\x1b[14;2HPulsa cualquier botón para salir");
            VBlankIntrWait();
            while (1) { scanKeys(); if (keysDown()) break; VBlankIntrWait(); }
            break;
        }
        if (turno) {
            iprintf("\x1b[16;2HL1: Recargar fuerza  R1: Atacar  B: Salir");
            VBlankIntrWait();
            scanKeys();
            u16 keys = keysDown();
            if (keys & KEY_L) {
                fuerza += 2;
                snprintf(mensaje, sizeof(mensaje), "¡Fuerza +2!");
                turno = 0;
            } else if (keys & KEY_R) {
                int dmg = 1 + fuerza;
                vida_bot -= dmg;
                snprintf(mensaje, sizeof(mensaje), "¡Atacas con %d de daño!", dmg);
                fuerza = 0;
                turno = 0;
            } else if (keys & KEY_B) {
                salir = 1;
            }
        } else {
            int bot_accion = rand() % 2;
            if (bot_accion == 0) {
                int dmg = 1 + (rand() % 2); // Bot ataca con 1 o 2
                vida_jugador -= dmg;
                snprintf(mensaje, sizeof(mensaje), "Bot ataca con %d!", dmg);
            } else {
                snprintf(mensaje, sizeof(mensaje), "Bot se prepara...");
            }
            turno = 1;
            VBlankIntrWait();
            for (int i = 0; i < 60; i++) VBlankIntrWait();
        }
    }
    show_games(username);
}

void juego_obby_coins(const char* username) {
    int nivel = 1;
    int salir = 0;
    while (!salir) {
        consoleClear();
        iprintf("\x1b[2;10H--- OBBY COINS ---");
        iprintf("\x1b[4;6HNivel actual: %d / 70", nivel);
        iprintf("\x1b[6;4HCompleta el parkour para ganar monedas!");
        if (nivel < 70) {
            iprintf("\x1b[10;4HPulsa B para saltar al siguiente nivel");
            iprintf("\x1b[12;4HPulsa A para salir");
            VBlankIntrWait();
            scanKeys();
            u16 keys = keysDown();
            if (keys & KEY_B) nivel++;
            if (keys & KEY_A) salir = 1;
        } else {
            iprintf("\x1b[10;4H¡Felicidades! Completaste los 70 niveles!");
            iprintf("\x1b[12;4H¡Ganaste 6500 monedas!");
            saldo_monedas += 6500;
            save_progress();
            iprintf("\x1b[14;2HPulsa cualquier botón para salir");
            VBlankIntrWait();
            while (1) { scanKeys(); if (keysDown()) break; VBlankIntrWait(); }
            salir = 1;
        }
    }
    show_games(username);
}

// --- Guardado/carga de progreso con premium ---
void save_progress() {
    FILE* f = fopen("progress.txt", "w");
    if (f) {
        fprintf(f, "%d %d %d\n", saldo_monedas, saldo_robux, es_premium);
        for (int i = 0; i < NUM_ITEMS; i++) fprintf(f, "%d %d ", items[i].comprado, items[i].equipado);
        fprintf(f, "\n");
        fclose(f);
    }
}

void load_progress() {
    FILE* f = fopen("progress.txt", "r");
    if (f) {
        fscanf(f, "%d %d %d", &saldo_monedas, &saldo_robux, &es_premium);
        for (int i = 0; i < NUM_ITEMS; i++) fscanf(f, "%d %d ", &items[i].comprado, &items[i].equipado);
        fclose(f);
    }
}
