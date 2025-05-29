// Robloy NES - Port inicial (versión texto, login, recarga por código)
// Compilar con cc65/ca65/ld65 o NESLib
// Autor: YanisMinesitoo

// NOTA: Este es un esqueleto básico en C para cc65/NESLib
// El juego real debe expandirse con lógica, gráficos y sonido NES

#include <neslib.h>
#include <string.h>

#define MAX_USERS 8
#define MAX_PROMOS 32
#define MAX_CODE_LEN 12
#define MAX_DESC_LEN 32

// Estructuras
struct Usuario {
  char nombre[13];
  char pass[7];
};
struct Promocode {
  char code[MAX_CODE_LEN];
  char desc[MAX_DESC_LEN];
};

// Datos fijos
struct Usuario usuarios[MAX_USERS] = {
  {"Axelo907", "FGH2YH"},
  {"byYanis", "02YYH2"},
  {"xXBacon_Jih", "62HXY2"},
  {"pioelcapooo", "H24H26"},
  {"liamsamu4", "XDYH2D"},
  {"arti", "ARX20D"},
  {"tutifruti_61", "X2432I"},
};
unsigned char num_usuarios = 7;

struct Promocode promocodes[MAX_PROMOS] = {
  {"A10F25G7XD", "+10 ROBUX"},
  {"FXD2789H25", "+25 ROBUX"},
  {"C2R2JOD3RN", "+40 ROBUX"},
  {"HDX421CM", "+200 MONEDAS"},
  {"AL20URXD", "+400 MONEDAS"},
  {"3X20MXYZ", "+4500 MONEDAS Y +500 ROBUX"},
  {"AXDHD121", "+4500 ROBUX"}
};
unsigned char num_promos = 7;

unsigned int saldo_monedas = 1000;
unsigned int saldo_robux = 0;
char last_username[13] = "";
char last_password[7] = "";
unsigned char tiene_premium = 0;
unsigned long ticks_juego = 0;
unsigned char avatar_sel = 0;

// Prototipos
void draw_menu(void);
void input_password(void);
void input_username(void);
void show_user_home(const char* username);
void show_robux_menu(void);
void redeem_promocode(void);
void show_shop_menu(void);
void show_minigame(void);
void show_changer(void);
void show_robux_shop(void);

// Utilidad: compara cadenas (NES no tiene strcmp)
unsigned char str_eq(const char* a, const char* b) {
  while (*a && *b) { if (*a != *b) return 0; a++; b++; }
  return (*a == *b);
}

// Utilidad: limpia pantalla
void clear_screen(void) {
  vram_adr(NAMETABLE_A);
  vram_fill(' ', 32*30);
}

void main(void) {
  ppu_on_all();
  draw_menu();
  while(1) { ppu_wait_nmi(); }
}

void draw_menu(void) {
  clear_screen();
  vram_adr(NTADR_A(10,2)); vram_write("Robloy NES", 11);
  vram_adr(NTADR_A(4,6)); vram_write("A: Login", 9);
  vram_adr(NTADR_A(4,8)); vram_write("B: Nueva cuenta", 16);
  while(1) {
    char pad = pad_poll(0);
    if (pad & PAD_A) { input_password(); break; }
    if (pad & PAD_B) { input_username(); break; }
    ppu_wait_nmi();
  }
}

void input_password(void) {
  clear_screen();
  vram_adr(NTADR_A(2,4)); vram_write("Introduce pass (6 chars):", 26);
  char input[7] = "";
  unsigned char pos = 0;
  while (pos < 6) {
    char c = get_char();
    if (c >= 'A' && c <= 'Z') { input[pos++] = c; vram_adr(NTADR_A(2+pos,6)); vram_put(c); }
    if (c == 0x08 && pos > 0) { pos--; vram_adr(NTADR_A(2+pos+1,6)); vram_put(' '); }
  }
  input[6] = 0;
  unsigned char found = 0;
  char found_username[13] = "";
  for (unsigned char i = 0; i < num_usuarios; i++) {
    if (str_eq(input, usuarios[i].pass)) {
      found = 1;
      for (unsigned char j = 0; j < 13; j++) found_username[j] = usuarios[i].nombre[j];
      break;
    }
  }
  if (!found && last_password[0] && str_eq(input, last_password)) {
    found = 1;
    for (unsigned char j = 0; j < 13; j++) found_username[j] = last_username[j];
  }
  if (found) {
    show_user_home(found_username);
  } else {
    clear_screen();
    vram_adr(NTADR_A(4,10)); vram_write("Pass incorrecta", 15);
    ppu_wait_nmi(); ppu_wait_nmi();
    draw_menu();
  }
}

void input_username(void) {
  clear_screen();
  vram_adr(NTADR_A(2,4)); vram_write("Nombre (max 12):", 17);
  char username[13] = "";
  unsigned char pos = 0;
  while (pos < 12) {
    char c = get_char();
    if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) { username[pos++] = c; vram_adr(NTADR_A(2+pos,6)); vram_put(c); }
    if (c == 0x08 && pos > 0) { pos--; vram_adr(NTADR_A(2+pos+1,6)); vram_put(' '); }
  }
  username[12] = 0;
  char password[7] = "ABCDEF";
  for (unsigned char i = 0; i < 6; i++) password[i] = 'A' + (i*3+pos)%26;
  password[6] = 0;
  for (unsigned char j = 0; j < 13; j++) last_username[j] = username[j];
  for (unsigned char j = 0; j < 7; j++) last_password[j] = password[j];
  clear_screen();
  vram_adr(NTADR_A(2,8)); vram_write("Usuario:", 8); vram_write(username, pos);
  vram_adr(NTADR_A(2,10)); vram_write("Pass:", 5); vram_write(password, 6);
  vram_adr(NTADR_A(2,12)); vram_write("Guarda tu pass!", 16);
  ppu_wait_nmi(); ppu_wait_nmi();
  draw_menu();
}

void show_user_home(const char* username) {
  clear_screen();
  vram_adr(NTADR_A(2,4)); vram_write("Bienvenido:", 11); vram_write(username, 12);
  char buf[32];
  sprintf(buf, "Monedas: %u", saldo_monedas); vram_adr(NTADR_A(2,6)); vram_write(buf, 16);
  sprintf(buf, "Robux: %u", saldo_robux); vram_adr(NTADR_A(2,7)); vram_write(buf, 12);
  // Mostrar avatar
  vram_adr(NTADR_A(2,8));
  const char* avatares[] = {"[C] Clasico", "[O] Oro", "[S] Sombrero", "[P] Capa", "[E] Espada", "[G] Gold (PREM)"};
  unsigned char max_avatar = tiene_premium ? 6 : 5;
  vram_write("Avatar: ",8); vram_write(avatares[avatar_sel],16);
  if (tiene_premium) {
    vram_adr(NTADR_A(2,9)); vram_write("[P] Usuario PREMIUM!",20);
    vram_adr(NTADR_A(2,10)); vram_write("-10% en tienda, +430R/hora",27);
  }
  vram_adr(NTADR_A(2,12)); vram_write("A: Tienda  B: Robux  START: Minijuego", 36);
  vram_adr(NTADR_A(2,13)); vram_write("C: Cambiador  SELECT: Volver", 28);
  while(1) {
    char pad = pad_poll(0);
    if (pad & PAD_A) { show_shop_menu(); break; }
    if (pad & PAD_B) { show_robux_menu(); break; }
    if (pad & PAD_START) { show_minigame(); break; }
    if (pad & PAD_SELECT) { draw_menu(); break; }
    if (pad & PAD_C) { show_changer(); break; }
    // Bonificación premium: cada ~3600 ticks (simula 1h)
    if (tiene_premium && (++ticks_juego)%3600==0) {
      saldo_robux += 430;
      vram_adr(NTADR_A(2,15)); vram_write("+430 Robux por ser PREMIUM!",27);
      ppu_wait_nmi();
    }
    ppu_wait_nmi();
  }
}

// Tienda básica: muestra ítems y permite comprar si hay saldo
void show_shop_menu(void) {
  clear_screen();
  vram_adr(NTADR_A(2,2)); vram_write("Tienda NES", 10);
  unsigned char option = 0;
  const char* items[] = {"Sombrero", "Zapas", "SombreroOro", "ZapasOro", "Capa", "Espada", "PREMIUM"};
  unsigned int precios_base[] = {0, 0, 50, 50, 100, 150, 25300};
  unsigned char num_items = 7;
  while(1) {
    for (unsigned char i = 0; i < num_items; i++) {
      vram_adr(NTADR_A(4,4+i));
      char buf[32];
      unsigned int precio = precios_base[i];
      if (i<6 && tiene_premium && precio>0) precio = precio - (precio/10); // -10% descuento
      if (i==6 && tiene_premium) sprintf(buf, "%c %s - COMPRADO", (option==i)?'>':' ', items[i]);
      else sprintf(buf, "%c %s - %u %s", (option==i)?'>':' ', items[i], precio, (i==6)?"M":"R$");
      vram_write(buf, 22);
    }
    vram_adr(NTADR_A(2,13)); vram_write("A: Comprar  B: Volver", 21);
    char pad = pad_poll(0);
    if (pad & PAD_DOWN && option < num_items-1) option++;
    if (pad & PAD_UP && option > 0) option--;
    if (pad & PAD_A) {
      unsigned int precio = precios_base[option];
      if (option<6 && tiene_premium && precio>0) precio = precio - (precio/10);
      if (option==6) {
        if (tiene_premium) {
          clear_screen();
          vram_adr(NTADR_A(2,15)); vram_write("Ya tienes Premium!",19);
        } else if (saldo_monedas >= 25300) {
          saldo_monedas -= 25300;
          tiene_premium = 1;
          clear_screen();
          vram_adr(NTADR_A(2,15)); vram_write("Ahora eres PREMIUM!",19);
        } else {
          clear_screen();
          vram_adr(NTADR_A(2,15)); vram_write("No tienes monedas",17);
        }
        ppu_wait_nmi(); ppu_wait_nmi();
        break;
      } else if (saldo_robux >= precio) {
        saldo_robux -= precio;
        clear_screen();
        vram_adr(NTADR_A(2,15)); vram_write("¡Compra exitosa!",16);
        ppu_wait_nmi(); ppu_wait_nmi();
        break;
      } else {
        clear_screen();
        vram_adr(NTADR_A(2,15)); vram_write("No tienes Robux",16);
        ppu_wait_nmi(); ppu_wait_nmi();
        break;
      }
    }
    if (pad & PAD_B) { show_user_home(last_username); break; }
    ppu_wait_nmi();
  }
}

void show_robux_menu(void) {
  clear_screen();
  char buf[32];
  sprintf(buf, "Monedas: %u", saldo_monedas); vram_adr(NTADR_A(2,4)); vram_write(buf, 16);
  sprintf(buf, "Robux: %u", saldo_robux); vram_adr(NTADR_A(2,5)); vram_write(buf, 12);
  vram_adr(NTADR_A(2,8)); vram_write("A: Recargar por codigo", 23);
  vram_adr(NTADR_A(2,9)); vram_write("B: Volver", 9);
  vram_adr(NTADR_A(2,10)); vram_write("C: Comprar Robux (monedas)",28);
  while(1) {
    char pad = pad_poll(0);
    if (pad & PAD_A) { redeem_promocode(); break; }
    if (pad & PAD_B) { draw_menu(); break; }
    if (pad & PAD_C) { show_robux_shop(); break; }
    ppu_wait_nmi();
  }
}

void redeem_promocode(void) {
  clear_screen();
  vram_adr(NTADR_A(2,4)); vram_write("Introduce codigo:", 18);
  char code[MAX_CODE_LEN] = "";
  unsigned char pos = 0;
  while (pos < MAX_CODE_LEN-1) {
    char c = get_char();
    if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) { code[pos++] = c; vram_adr(NTADR_A(2+pos,6)); vram_put(c); }
    if (c == 0x08 && pos > 0) { pos--; vram_adr(NTADR_A(2+pos+1,6)); vram_put(' '); }
  }
  code[MAX_CODE_LEN-1] = 0;
  unsigned char found = 0;
  for (unsigned char i = 0; i < num_promos; i++) {
    if (str_eq(code, promocodes[i].code)) {
      found = 1;
      // Procesar recompensa básica (solo monedas/robux)
      if (promocodes[i].desc[1] == '4') { saldo_monedas += 4500; saldo_robux += 500; }
      else if (promocodes[i].desc[6] == 'R') saldo_robux += 10 * (promocodes[i].desc[1]-'0');
      else if (promocodes[i].desc[6] == 'M') saldo_monedas += 200 * (promocodes[i].desc[1]-'0');
      clear_screen();
      vram_adr(NTADR_A(2,8)); vram_write("Codigo canjeado!", 16);
      break;
    }
  }
  if (!found) {
    clear_screen();
    vram_adr(NTADR_A(2,8)); vram_write("Codigo invalido", 15);
  }
  ppu_wait_nmi(); ppu_wait_nmi();
  show_robux_menu();
}

// Minijuego pseudo-3D: Corredor NES con rivales y hachas
void show_minigame(void) {
  clear_screen();
  vram_adr(NTADR_A(8,4)); vram_write("MINIJUEGO 3D!",13);
  vram_adr(NTADR_A(4,6)); vram_write("Esquiva rivales y hachas!",26);
  vram_adr(NTADR_A(4,8)); vram_write("Izq/Der: mover  B: salir",25);
  ppu_wait_nmi();
  ppu_wait_nmi();
  unsigned char player = 2; // 0-4 (5 carriles)
  unsigned char tick = 0;
  unsigned char rivals[15]; // 15 filas, 255 = sin rival, 0-4 = carril
  unsigned char hachas[15]; // 255 = sin hacha, 0-4 = carril
  for (unsigned char i=0;i<15;i++) { rivals[i]=255; hachas[i]=255; }
  unsigned int score = 0;
  while(1) {
    // Mover obstáculos hacia el jugador
    for (unsigned char i=14;i>0;i--) { rivals[i]=rivals[i-1]; hachas[i]=hachas[i-1]; }
    // Nuevo rival aleatorio al fondo
    if ((tick&3)==0 && (rand()%3)==0) rivals[0]=(rand()%5); else rivals[0]=255;
    // Nueva hacha aleatoria al fondo
    if ((tick&1)==0 && (rand()%4)==0) hachas[0]=(rand()%5); else hachas[0]=255;
    // Dibuja pista
    clear_screen();
    vram_adr(NTADR_A(8,2)); vram_write("Pista:",6);
    for (unsigned char z=0;z<15;z++) {
      vram_adr(NTADR_A(8,4+z));
      for (unsigned char x=0;x<5;x++) {
        if (rivals[z]==x && hachas[z]==x) vram_put('!'); // Colisión doble
        else if (rivals[z]==x) vram_put('R');
        else if (hachas[z]==x) vram_put('*'); // Hacha dorada
        else if (z==14 && x==player) vram_put('P');
        else vram_put('.');
      }
    }
    char buf[24];
    sprintf(buf,"Puntos: %u",score); vram_adr(NTADR_A(8,20)); vram_write(buf,12);
    vram_adr(NTADR_A(8,22)); vram_write("B: salir",8);
    // Input
    char pad = pad_poll(0);
    if (pad & PAD_LEFT && player>0) player--;
    if (pad & PAD_RIGHT && player<4) player++;
    if (pad & PAD_B) break;
    // Colisión
    if (rivals[14]==player || hachas[14]==player) {
      vram_adr(NTADR_A(8,12)); vram_write("GAME OVER!",10);
      ppu_wait_nmi(); ppu_wait_nmi();
      break;
    }
    score++;
    tick++;
    ppu_wait_nmi();
  }
  clear_screen();
  vram_adr(NTADR_A(8,10)); vram_write("Fin! Puntos:",13);
  char buf[12];
  sprintf(buf,"%u",score); vram_adr(NTADR_A(8,12)); vram_write(buf,6);
  ppu_wait_nmi(); ppu_wait_nmi();
  show_user_home(last_username);
}

// Cambiador de avatar: permite elegir entre varios estilos
void show_changer(void) {
  clear_screen();
  vram_adr(NTADR_A(8,4)); vram_write("CAMBIADOR DE AVATAR",19);
  const char* avatares[] = {"[C] Clasico", "[O] Oro", "[S] Sombrero", "[P] Capa", "[E] Espada", "[G] Gold (PREM)"};
  unsigned char max_avatar = tiene_premium ? 6 : 5;
  unsigned char sel = avatar_sel;
  while(1) {
    for (unsigned char i=0;i<max_avatar;i++) {
      vram_adr(NTADR_A(8,6+i));
      char buf[24];
      sprintf(buf, "%c %s", (sel==i)?'>':' ', avatares[i]);
      vram_write(buf, 18);
    }
    vram_adr(NTADR_A(8,14)); vram_write("A: Elegir  B: Volver",20);
    char pad = pad_poll(0);
    if (pad & PAD_DOWN && sel < max_avatar-1) sel++;
    if (pad & PAD_UP && sel > 0) sel--;
    if (pad & PAD_A) { avatar_sel = sel; break; }
    if (pad & PAD_B) break;
    ppu_wait_nmi();
  }
  show_user_home(last_username);
}

// Teclado virtual NES: permite seleccionar letras/números/borrar/OK con el joypad
char get_char(void) {
  // Definición del teclado: 6x7 grid (A-Z, 0-9, Borrar, OK)
  const char keys[6][7] = {
    {'A','B','C','D','E','F','G'},
    {'H','I','J','K','L','M','N'},
    {'O','P','Q','R','S','T','U'},
    {'V','W','X','Y','Z','0','1'},
    {'2','3','4','5','6','7','8'},
    {'9',0x08,' ',' ',' ',' ','>'} // 0x08=Borrar, '>'=OK
  };
  unsigned char row = 0, col = 0;
  while (1) {
    // Dibuja el teclado en pantalla
    clear_screen();
    vram_adr(NTADR_A(2,4)); vram_write("Teclado:",8);
    for (unsigned char r=0; r<6; r++) {
      vram_adr(NTADR_A(4,6+r));
      for (unsigned char c=0; c<7; c++) {
        char ch = keys[r][c];
        if (r==row && c==col) vram_put('[');
        else vram_put(' ');
        if (ch==0x08) vram_write("DEL",3);
        else if (ch=='>') vram_write("OK",2);
        else if (ch!=' ') vram_put(ch);
        else vram_put(' ');
        if (r==row && c==col) vram_put(']');
        else vram_put(' ');
      }
    }
    vram_adr(NTADR_A(2,14)); vram_write("A: Elegir  B: OK  DEL: Borrar",27);
    // Espera input
    char pad = 0, last_pad = 0;
    while (1) {
      pad = pad_poll(0);
      if (pad != last_pad && pad) break;
      last_pad = pad;
      ppu_wait_nmi();
    }
    if (pad & PAD_UP && row > 0) row--;
    if (pad & PAD_DOWN && row < 5) row++;
    if (pad & PAD_LEFT && col > 0) col--;
    if (pad & PAD_RIGHT && col < 6) col++;
    if (pad & PAD_A) {
      char sel = keys[row][col];
      if (sel == '>') return 0; // OK
      if (sel == 0x08) return 0x08; // DEL
      if (sel != ' ') return sel;
    }
    if (pad & PAD_B) return 0; // Alternativamente, B también puede ser OK
  }
}

void show_robux_shop(void) {
  clear_screen();
  vram_adr(NTADR_A(6,3)); vram_write("PAQUETES DE ROBUX",17);
  const unsigned int robux_packs[] = {100, 500, 2000, 5000, 6000, 7800, 8000, 9000, 10000, 25300};
  const unsigned int coin_prices[] = {1200, 5000, 18000, 32000, 38000, 48000, 50000, 56000, 60000, 150000};
  const char* estrellas[] = {"", "*", "**", "***", "***", "***", "***", "***", "***", "********"};
  unsigned char num_packs = 10;
  unsigned char sel = 0;
  while(1) {
    for (unsigned char i=0;i<num_packs;i++) {
      vram_adr(NTADR_A(6,5+i));
      unsigned int precio = coin_prices[i];
      if (tiene_premium) precio = precio - (precio/10);
      char buf[40];
      sprintf(buf, "%c %u Robux - %u monedas %s", (sel==i)?'>':' ', robux_packs[i], precio, estrellas[i]);
      vram_write(buf, 31);
    }
    vram_adr(NTADR_A(6,16)); vram_write("A: Comprar  B: Volver",21);
    char pad = pad_poll(0);
    if (pad & PAD_DOWN && sel < num_packs-1) sel++;
    if (pad & PAD_UP && sel > 0) sel--;
    if (pad & PAD_A) {
      unsigned int precio = coin_prices[sel];
      if (tiene_premium) precio = precio - (precio/10);
      if (saldo_monedas >= precio) {
        saldo_monedas -= precio;
        saldo_robux += robux_packs[sel];
        clear_screen();
        vram_adr(NTADR_A(6,18)); vram_write("Compra exitosa!",15);
      } else {
        clear_screen();
        vram_adr(NTADR_A(6,18)); vram_write("No tienes monedas",17);
      }
      ppu_wait_nmi(); ppu_wait_nmi();
      break;
    }
    if (pad & PAD_B) { show_robux_menu(); break; }
    ppu_wait_nmi();
  }
}
