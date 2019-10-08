# neo2-llkh

*alternativer Treiber mittels Low-Level Keyboard Hook*

(*Read this in [English](README.en.md)*)

Dieser Treiber unterstützt folgende Tastaturlayouts:
* [Neo2](http://www.neo-layout.org)
* [Aus der Neo-Welt](http://www.adnw.de)
* [AdNWzjßf](http://adnw.de/index.php?n=Main.AdNWzj%c3%9ff)
* [bone](https://web.archive.org/web/20180721192908/http://wiki.neo-layout.org/wiki/Bone)
* [KOY](http://adnw.de/index.php?n=Main.SeitlicheNachbaranschl%c3%a4ge)
* [KOU](http://maximilian-schillinger.de/kou-layout.html) (mein persönliches Layout auf Basis von KOY)


## Funktionsumfang
* Alle 6 Ebenen (auf den Ebenen 5 und 6 funktionieren nicht alle Zeichen)
* Die meisten toten Tasten werden unterstützt: ``^ ` ´ ̧ ̌ ~ ° ̇  `` (teilweise muss die tote Taste vor, teilweise nach dem Buchstaben gedrückt werden)
* Level2-Lock: Ebene 2 kann eingerastet werden (beide Shift-Tasten gleichzeitig)
* Echtes CapsLock: Alle Buchstaben groß schreiben. Zahlen, Punkt und Komma bleiben unverändert. (Beide Shift-Tasten gleichzeitig)
* Level4-Lock: Ebene 4 kann eingerastet werden (beide Mod4-Tasten gleichzeitig)
* Der rechte Ebene3-Modifier kann auf die ä-Taste gelegt werden
* Wenn gewünscht, gilt für Shortcuts (mit Strg, Alt und/oder Win) das QWERTZ-Layout
* Die linke Strg- und Alt-Taste können getauscht werden
* Alternativ können die linke Strg-, Alt- und Windows-Taste getauscht werden
* Die CapsLock-Taste kann in eine zusätzliche Escape-Taste verwandelt werden (wenn sie alleine angeschlagen wird)

### Was nicht funktioniert

* Diese Tasten sollten eigentlich tote Tasten sein, geben aber direkt die entsprechenden Symbole aus: `̷  ¨ ˝`

## Selbst kompilieren
Um diesen Treiber aus den Quellen zu installieren, klone dieses Projekt (`git clone https://github.com/MaxGyver83/neo2-llkh.git`) oder lade es als zip herunter und entpacke es. Führe dann `make` im `src`-Ordner aus. Dafür müssen make und gcc installiert sein. Wenn du diese Programme noch nicht hast, könntest du z.B. [MinGW](https://sourceforge.net/projects/mingw/) installieren.

## Verwendung
Starte einfach die selbst kompilierte `neo-llkh.exe` aus dem `src`-Ordner. Möglicherweise funktioniert auch die mitgelieferte `neo-llkh.exe` aus dem `bin`-Ordner (kompiliert unter Windows 10). Standardmäßig wird das Neo2-Layout geladen.

Über das Icon (![appicon](src/appicon.ico)) in der Taskleiste kannst du den Treiber wieder beenden.

## Einstellungen

Alle Einstellungen werden in der mitgelieferten `settings.ini` gemacht. Es wird immer die ini-Datei aus dem gleichen Ordner wie die `neo-llkh.exe` verwendet. Um eine Funktion zu aktivieren, setze den entsprechenden Wert auf `1`. `0` bedeutet deaktiviert.

### Layout
Wenn du ein anderers Layout verwenden möchtest, ändere den `layout`-Eintrag in der `settings.ini`, zum Beispiel:

`layout=adnw`

Folgende Layout-Parameter werden erkannt: `neo`, `adnw`, `adnwzjf` (=AdNWzjßf), `bone`, `koy`, `kou`.

### Einrasten von Ebene 2
Das Einrasten von Ebene 2 (beide Shift-Tasten gleichzeitig) wird unterstützt, muss aber explizit aktiviert werden. Dafür muss der Wert von `shiftLockEnabled` in der `settings.ini` auf `1` gesetzt werden:

`shiftLockEnabled=1`

### Echtes CapsLock
CapsLock (beide Shift-Tasten gleichzeitig) wird unterstützt, muss aber explizit aktiviert werden. Dafür muss der Wert von `capsLockEnabled` in der `settings.ini` auf `1` gesetzt werden:

`capsLockEnabled=1`

### Einrasten von Ebene 4
Das Einrasten von Ebene 4 (beide Mod4-Tasten \[`<>` + `AltGr`\] gleichzeitig) wird unterstützt, muss aber explizit aktiviert werden. Dafür muss der Wert von `level4LockEnabled` in der `settings.ini` auf `1` gesetzt werden:

`level4LockEnabled=1`

Wenn Level2- und Level4-Lock aktiv sind, wirkt nur der Level4-Lock. 

### Symmetrische Level3-Modifier
Wenn du den rechten Modifier auf die `Ä`- statt auf die `#`-Taste legen willst, setze den Wert von `symmetricalLevel3Modifiers` auf `1`:

`symmetricalLevel3Modifiers=1`

### QWERTZ-Layout für Shortcuts
Außerdem gibt es einen Modus, in dem bei Shortcuts immer das QWERTZ-Layout gilt. Das heißt, immer wenn Strg, die linke Alt-Taste oder eine Windows-Taste gedrückt ist, wird unabhängig vom eingestellten Layout QWERTZ verwendet. Somit können Strg-c, Strg-v, Strg-s usw. einfach mit der linken Hand betätigt werden. Um diesen Modus zu aktivieren, muss `qwertzForShortcuts` in der `settings.ini` auf `1` gesetzt werden:

`qwertzForShortcuts=1`

### Linke Alt- und Strg-Taste vertauschen
Für Mac-Freunde, die es gewohnt sind, Shortcuts mit dem linken Daumen auszuführen.

`swapLeftCtrlAndLeftAlt=1`

### Linke Alt-, Strg- und Win-Taste vertauschen
Wie die vorige Option, allerdings wird hier zusätzlich die linke Windows-Taste versetzt.
Damit ergibt sich die Reihenfolge: Win, Alt, Ctrl (auf Standardtastaturen)

`swapLeftCtrlLeftAltAndLeftWin=1`

### Ebenen 5 und 6
Achtung: Experimentell! Die Ebenen 5 und 6 sind für die Zahlentasten noch nicht umgesetzt. Auch im Buchstabenfeld funktionieren nicht alle Tasten (obwohl ihnen Symbole zugewiesen wurden).

`supportLevels5and6=1`

### CapsLock-Taste als Escape verwenden
Für vim-Freunde: Wenn die CapsLock-Taste einzeln angeschlagen wird, sendet sie ein Escape. In Kombination mit anderen Tasten ist sie der linke Ebene3-Modifier.

`capsLockAsEscape=1`
