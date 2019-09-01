# neo2-llkh

*alternativer Treiber mittels Low-Level Keyboard Hook*

*Read this in [English](README.en.md)*

Dieser Treiber unterstützt folgende Tastaturlayouts:
* [Neo2](http://www.neo-layout.org)
* [Aus der Neo-Welt](http://www.adnw.de)
* [AdNWzjßf](http://adnw.de/index.php?n=Main.AdNWzj%c3%9ff)
* [KOY](http://adnw.de/index.php?n=Main.SeitlicheNachbaranschl%c3%a4ge)
* KOU (mein persönliches Layout auf Basis von KOY)

## Selbst kompilieren
Um diesen Treiber aus den Quellen zu installieren, klone dieses Projekt (`git clone https://github.com/MaxGyver83/neo2-llkh.git`) oder lade es als zip herunter und entpacke es. Führe dann `make` im `src`-Ordner aus. Dafür müssen make und gcc installiert sein. Wenn du diese Programme noch nicht hast, könntest du z.B. [MinGW](https://sourceforge.net/projects/mingw/) installieren.

## Verwendung
Starte einfach die selbst kompilierte `neo-llkh.exe` aus dem `src`-Ordner. Möglicherweise funktioniert auch die mitgelieferte `neo-llkh.exe` aus dem `bin`-Ordner (kompiliert unter Windows 10). Standardmäßig wird das Neo2-Layout geladen. Wenn du ein anderers Layout verwenden möchtest, starte das Programm aus der Kommandozeile (`cmd.exe`) mit einem alternativen Layout als Parameter, also zum Beispiel:

`neo-llkh.exe adnw`

Folgende Layout-Parameter werden erkannt: `adnw`, `adnwzjf` (=AdNWzjßf), `koy`, `kou`. Alternativ kannst du auch eine der bat-Dateien im `bin`-Ordner per Doppelklick starten.

Wenn du symmetrische Level3-Modifier verwenden möchtest (also den rechten Modifier auf der ä- statt auf der #-Taste), füge eine 1 als zweiten Parameter hinzu. Beispiel:

`neo-llkh.exe koy 1`

Über das Icon in der Taskleiste kannst du den Treiber wieder beenden.

## Funktionsumfang
Es werden nur die Ebenen 1-4 unterstützt. Das Einrasten von Ebene 4 (beide Level4-Modifier gleichzeitig) wird nicht unterstützt. Das Einrasten von Ebene 2 (beide Shift-Tasten gleichzeitig) wird unterstützt, muss aber explizit aktiviert werden. Dafür muss der Treiber mit dem Parameter 1 an dritter Stelle gestartet werden:

`neo-llkh.exe neo 0 1`

Folgende tote Tasten funktionieren: ``^ ` ´ ̧ ̌ ~ ° ̇  `` (teilweise muss die tote Taste vor, teilweise nach dem Buchstaben gedrückt werden)

Diese Tasten sollten eigentlich tote Tasten sein, geben aber direkt die entsprechenden Symbole aus: `̷  ¨ ˝`

Außerdem gibt es einen Modus, in dem bei Shortcuts immer das QWERTZ-Layout gilt. Das heißt, immer wenn Strg, die linke Alt-Taste oder eine Windows-Taste gedrückt ist, wird unabhängig vom eingestellten Layout QWERTZ verwendet. Somit können Strg-c, Strg-v, Strg-s usw. einfach mit der linken Hand betätigt werden. Um diesen Modus zu aktivieren, muss eine `1` als vierter Parameter übergeben werden:

`neo-llkh.exe adnw 0 0 1`
