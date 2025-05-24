// main.c - Entrada principal de Robloy para GBA
#include <gba_console.h>
#include <gba_video.h>
#include <gba_input.h>
#include <gba_system.h>
#include <stdio.h>
#include <stdlib.h> // Para rand()

void draw_menu();
void draw_keyboard(char input[7], int cursor_row, int cursor_col);

#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 9
const char keyboard[KEYBOARD_ROWS][KEYBOARD_COLS+1] = {
    "ABCDEF123",
    "GHIJKL456",
    "MNOPQR789",
    "STUVWXYZ0"
};

void input_password();
void input_username();

int main(void) {
    // Inicializa la consola de texto
    irqInit();
    irqEnable(IRQ_VBLANK);
    consoleDemoInit();

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
    // Aquí puedes validar la contraseña
    // Validar contraseña (ejemplo: "ABC123")
    if (strcmp(input, "ABC123") == 0) {
        iprintf("\x1b[10;8HContraseña correcta!");
    } else {
        iprintf("\x1b[10;8HContraseña incorrecta, noob");
    }
    VBlankIntrWait();
    for (int i = 0; i < 120; i++) VBlankIntrWait(); // Espera ~2 segundos
    draw_menu();
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
