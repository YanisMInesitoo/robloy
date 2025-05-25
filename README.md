# Robloy 🎮✨

Robloy es un juego hecho en C inspirado en Roblox para Game Boy Advance. 🕹️

**Versión actual:** 1.0  
**Desarrollo:** 1.002 preview P2 (versión de prueba, no oficial)

> 📢 ¡Suscríbete a los canales de YouTube!  
> 🔔 @SoyJhosfra  
> 🔔 @SoyJhosfraPRIME

---

💾 **Cartucho físico:** 37,99 €
- 💵 USD: $41.50
- 💷 GBP: £32.00
- 💴 JPY: ¥6,500
- 🇭🇰 HKD: HK$325
- 🇨🇦 CAD: $57.00
- 🇨🇳 CNY: ¥300
- 🇷🇺 RUB: ₽3,800
- 🇺🇦 UAH: ₴1,700
- 🇲🇽 MXN: $770
- 🇦🇷 ARS: $38,000
- 🇻🇪 VES: Bs.1,500
- 🇨🇴 COP: $160,000
- 🇵🇸 ILS: ₪150

---

💎 **Premium:** 3,200 monedas (compra dentro del juego)

---

## 🚀 Características principales
- 🏠 Menú principal con el título "Robloy" centrado.
- 🔑 Iniciar sesión y crear cuenta con teclado virtual.
- 👥 Soporte para múltiples cuentas y usuarios especiales.
- 💾 Guardado/carga de progreso, monedas, Robux y premios.
- 🏆 Premios, ítems y recompensas exclusivas.
- 🛒 Tienda con compra/equipamiento de ítems.
- 💸 Sistema de monedas y Robux, paquetes y promocodes.
- ⭐ Usuario premium: compra con 3,200 monedas, ventajas exclusivas, icono especial.
- 🎮 Menú de juegos con minijuegos jugables:
  - ⚔️ RIVALES HACHAS: combate por turnos.
  - 🪙 OBBY COINS: 70 niveles, recompensa al completar.
- 🏅 Premios especiales y recompensas por logros.
- 🛡️ Seguridad: validación de login y usuarios.
- 🎨 Interfaz con iconos y mejoras visuales.
- 🏆 Sistema de logros y premios secretos.
- 🏁 Guardado rápido con SELECT+START.
- 🏵️ Ítems exclusivos para usuarios premium.
- 🏦 Bonos de monedas y Robux por eventos y promocodes.
- 🏹 Más minijuegos y desafíos en desarrollo.
- 🏆 Ranking de mejores jugadores (próximamente).
- 🧑‍💻 Código fuente organizado y fácil de modificar.

## 🌐 ¿Se puede jugar online?
No, actualmente **no se puede jugar online de ninguna forma** en Robloy (versión 1.002 o 1.0). El modo online se intentará implementar en la versión 1.003. Por ahora, todo el progreso, minijuegos y funciones son completamente offline.

## 🛠️ Compilación
1. 📦 Instala devkitPro: https://devkitpro.org/
2. 💻 Abre la terminal MSYS2 de devkitPro.
3. 📂 Navega a la carpeta del proyecto:
   ```sh
   cd /c/Users/Yanis/Documents/robloy
   ```
4. 🏗️ Ejecuta:
   ```sh
   make
   ```

## 📁 Estructura del proyecto
- 📂 src/: Código fuente en C.
- 🖼️ gfx/: Gráficos (opcional).
- 🏗️ build/: Archivos generados.
- 📝 usuarios.txt: Usuarios y contraseñas.
- 🏷️ promocodes.txt: Promocodes y recompensas.

## 💡 Créditos y agradecimientos
- 👤 Desarrollador: YanisMinesitoo
- 🛠️ Basado en devkitPro y herramientas libres
- 🙏 Gracias a la comunidad homebrew GBA

## 📅 Última actualización
24 de mayo de 2025

## 📜 Licencia
GPL 3.0

## SPOIL33R
- En la 1.002 estaran los amigos

## Cambios en la versión 1.011

- Ahora el guardado de progreso (monedas, Robux, premium, ítems) se realiza en la SRAM de la consola, eliminando la necesidad de archivos externos para el progreso.
- Esta versión está adaptada para funcionar en más dispositivos humildes y flashcarts, incluyendo aquellos que solo soportan guardado SRAM (como la R36S y clones).
- Ya no es necesario el archivo `progress.txt` para guardar el avance.

> **Nota:** El sistema de cuentas y login sigue usando archivos, pero el progreso del usuario es totalmente compatible con hardware real y la mayoría de flashcarts.

## 🏅 Primer cartucho vendido
- El primer cartucho físico de Robloy fue vendido a un sev1llano. ¡Gracias por apoyar el desarrollo homebrew! 🎉