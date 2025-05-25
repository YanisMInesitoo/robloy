import React from "react";
import { createRoot } from "react-dom/client";
import "./style.css";

const App = () => (
  <div className="container">
    <header>
      <h1>Robloy <span className="pixel">🎮✨</span></h1>
      <p className="subtitle">¡El Roblox retro para Game Boy Advance!</p>
      <p className="dominio">
        🌐 <b>
          <a href="https://robloy.org" target="_blank" rel="noopener noreferrer" style={{ color: "#00ffe7", textDecoration: "underline" }}>
            robloy.org
          </a>
        </b> (próximamente)
      </p>
    </header>
    <nav>
      <a href="#descarga">Descarga</a>
      <a href="#instrucciones">Instrucciones</a>
      <a href="#caracteristicas">Características</a>
      <a href="#creditos">Créditos</a>
    </nav>
    <main>
      <section id="descarga">
        <h2>📥 Descarga</h2>
        <p>
          <a className="btn" href="../robloy.gba" download>⬇️ Descargar ROM (.gba)</a>
        </p>
        <p>
          <a className="btn" href="../README.md" download>📖 Manual y detalles</a>
        </p>
      </section>
      <section id="instrucciones">
        <h2>🕹️ Cómo jugar en tu GBA</h2>
        <ol>
          <li>💾 Copia <b>robloy.gba</b> a la raíz de tu microSD (<b>EASYROMS</b> o similar).</li>
          <li>📝 Añade <b>usuarios.txt</b> y <b>promocodes.txt</b> si quieres cuentas y promocodes.</li>
          <li>🔌 Inserta la microSD en tu flashcart y ¡a jugar! 🚀</li>
        </ol>
        <p className="tip">💡 El progreso se guarda automáticamente en la SRAM. ¡No necesitas archivos extra! 🧠</p>
      </section>
      <section id="caracteristicas">
        <h2>✨ Características</h2>
        <ul>
          <li>🏠 Menú principal y sistema de cuentas</li>
          <li>🛒 Tienda, monedas, Robux y premios</li>
          <li>🎮 Minijuegos: ⚔️ Rivales Hachas y 🪙 Obby Coins</li>
          <li>💎 Premium y 🎁 promocodes</li>
          <li>🕹️ Compatible con hardware real y flashcarts humildes</li>
        </ul>
      </section>
      <section id="creditos">
        <h2>👾 Créditos</h2>
        <ul>
          <li>👤 Desarrollador: YanisMinesitoo</li>
          <li>🛠️ Basado en devkitPro</li>
          <li>🙏 Gracias a la comunidad homebrew GBA</li>
        </ul>
      </section>
    </main>
    <footer>
      <p>Robloy GBA &copy; 2025</p>
    </footer>
  </div>
);

const root = createRoot(document.getElementById("root")!);
root.render(<App />);
