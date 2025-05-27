# Robloy GBC 🎮✨

Robloy es un juego hecho en C inspirado en Roblox para Game Boy Color, usando GBDK-2020. 🕹️

**Versión actual:** 1.002  
**Desarrollo:** 1.002 (recarga por código, standalone)

> 📢 ¡Suscríbete a los canales de YouTube!  
> 🔔 @SoyJhosfra  
> 🔔 @SoyJhosfraPRIME

---

💾 **Cartucho físico (GBC):** 37,99 €
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
- 🇮🇱 ILS: ₪150

---

💎 **Premium:** 3,200 monedas (compra dentro del juego)

---

## 🚀 Características principales
- 🏠 Menú principal con el título "Robloy" centrado.
- 🔑 Iniciar sesión y crear cuenta (usuarios fijos y usuario temporal).
- 👥 Soporte para múltiples cuentas fijas.
- 💾 Guardado/carga de progreso (pendiente de implementar en SRAM GBC).
- 🏆 Premios, ítems y recompensas exclusivas.
- 🛒 Tienda con compra/equipamiento de ítems.
- 💸 Sistema de monedas y Robux, paquetes y promocodes.
- ⭐ Usuario premium: compra con 3,200 monedas, ventajas exclusivas, icono especial.
- 🎮 Menú de juegos y minijuegos en modo texto (en desarrollo).
- 🏅 Premios especiales y recompensas por logros.
- 🛡️ Seguridad: validación de login y usuarios.
- 🎨 Interfaz con iconos y mejoras visuales (limitado por hardware).
- 🏆 Sistema de logros y premios secretos.
- 🏁 Guardado rápido (próximamente).
- 🏵️ Ítems exclusivos para usuarios premium.
- 🏦 Bonos de monedas y Robux por eventos y promocodes.
- 🏦 Recarga de Robux/monedas y regalos secretos con códigos desde el menú de Robux (pulsa A).
- 🏹 Más minijuegos y desafíos en desarrollo.
- 🏆 Ranking de mejores jugadores (próximamente).
- 🧑‍💻 Código fuente organizado y fácil de modificar.

## 🌐 ¿Se puede jugar online?
No, actualmente **no se puede jugar online de ninguna forma** en Robloy GBC. Todo el progreso, minijuegos y funciones son completamente offline.

## 🛠️ Compilación
1. 📦 Instala GBDK-2020: https://gbdk.dev/
2. 💻 Abre la terminal en la carpeta del proyecto.
3. 🏗️ Ejecuta:
   ```
   make -f Makefile.gbc
   ```
   O simplemente ejecuta `build_gbc.bat` en Windows.
4. El archivo generado será `robloy_gbc.gb`

## 📁 Estructura del proyecto
- 📂 src/: Código fuente en C.
- 🏗️ build/: Archivos generados.
- Makefile.gbc: Compilación para GBC.
- build_gbc.bat: Script automático para compilar en Windows.

## 💡 Créditos y agradecimientos
- 👤 Desarrollador: YanisMinesitoo
- 🛠️ Basado en GBDK-2020 y herramientas libres
- 🙏 Gracias a la comunidad homebrew GBC

## 📅 Última actualización
27 de mayo de 2025

## 📄 Licencia
GPL 3.0

## Cambios en la versión 1.002 (GBC)
- Añadida función de recarga: ahora puedes introducir códigos de recarga desde el menú de Robux pulsando A.
- El sistema de promocodes es accesible directamente desde el menú de Robux.
- Lógica de cuentas, tienda, monedas, Robux y promocodes fijos en binario.
- Menú y flujo principal en modo texto.
- Guardado/carga de progreso pendiente de implementar en SRAM GBC.
- Máxima compatibilidad con hardware real y emuladores.
- Esta es la versión recomendada para todos los dispositivos GBC.

---

¡Disfruta Robloy en tu Game Boy Color! 🎮
