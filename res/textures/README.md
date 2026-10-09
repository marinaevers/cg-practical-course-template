# Texturen

| Datei | Verwendung | Hinweise |
|---|---|---|
| `skybox/posx.png` … `skybox/negz.png` | Skybox (Aufgabe 5) | Je eine Seite der Cubemap: `posx` → `GL_TEXTURE_CUBE_MAP_POSITIVE_X`, `negx` → `GL_TEXTURE_CUBE_MAP_NEGATIVE_X`, analog für y und z. Die Bilder folgen der OpenGL-Konvention für Cubemaps und werden **ohne** vertikales Spiegeln geladen. 1024×1024 px, RGB. |
| `tent_canvas.png` | Zeltdach, Grundtextur (Aufgabe 8) | 16 rot-weiße Streifen entlang u (einmal um den Kegel herum), v entlang der Mantellinie. Nahtlos in u. |
| `light_bulbs.png` | Zeltdach, Lauflichtkette (Aufgabe 8) | 16 Glühbirnen in einer Reihe, RGBA mit transparentem Hintergrund (nicht vormultipliziert). Nahtlos in u, damit die Texturkoordinaten zeitabhängig verschoben werden können. |
| `uv_checker.png` | Debugging | Zum Prüfen der Texturkoordinaten der Grundkörper. |

Die Texturen wurden prozedural erzeugt und dürfen frei verwendet werden.
