# Installation (Kurzfassung für Entwickler)

Drei Wege, je nach Ziel:

| Ziel | Weg |
|---|---|
| Nur benutzen (fertige Anlage) | Docker, siehe [ANLEITUNG.md](ANLEITUNG.md) |
| Am Code arbeiten | Nativ bauen, siehe unten |
| Eigenes Image bauen | `docker compose build`, siehe [ANLEITUNG.md](ANLEITUNG.md) §4 |
| Image ohne Docker Hub verteilen | Datei-Transfer oder eigene Registry, siehe [ANLEITUNG.md](ANLEITUNG.md) §4 „Image selbst hosten" |

> Hinweis: Das Docker-Hub-Image `janallm/looschen-datenbereitstellung` ist
> Stand Juli 2026 und enthält die aktuellen Fixes **nicht** — bis zum nächsten
> Push besser selbst bauen.

## Quellcode holen

```bash
git clone https://github.com/JanAllm/looschen-datenbereitstellung.git
cd looschen-datenbereitstellung
```

Keine Submodule nötig — open62541 liegt als Quelle unter `third_party/`.

## Nativ bauen (Linux)

Voraussetzungen: CMake ≥ 3.18, GCC oder Clang mit C++20, OpenCV (4 **oder** 5),
OpenSSL, Python 3 (nur für die open62541-Codegenerierung). GoogleTest ist
optional und wird sonst automatisch geladen.

```bash
# Arch/CachyOS
sudo pacman -S --needed cmake gcc opencv openssl gtest

# Ubuntu/Debian
sudo apt install build-essential cmake libopencv-dev libssl-dev python3
```

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

Der erste Configure-Lauf braucht Internet (CMake FetchContent lädt cpp-httplib,
SQLite und nlohmann/json).

### Tests

```bash
cd build && ctest --output-on-failure
```

`ctest -R FullFlow` fährt den kompletten SPS-Dialog gegen einen eingebetteten
OPC-UA-Testserver — sinnvoll vor jedem Deploy.

### Starten

Die Anwendung erwartet die Weboberflächen-Dateien neben der Executable und die
Settings-Datenbank unter `SETTINGS_DB` (der Default `/app/data/settings.db`
ist der Container-Pfad):

```bash
cp -r src/webVisu build/bin/
cp cert.pem key.pem build/bin/webVisu/   # für HTTPS; ohne sie schlägt TLS fehl
mkdir -p build/bin/data
cd build/bin && SETTINGS_DB=./data/settings.db ./MyCppExecutable
```

Weboberfläche: `https://localhost:5000` (selbstsigniertes Zertifikat, Warnung
im Browser bestätigen). Erstkonfiguration — SPS-Endpunkt, Node-Indizes,
Projektordner, `LenDataArray` passend zum SPS-Array — siehe
[ANLEITUNG.md](ANLEITUNG.md) §5.

## Nativ bauen (Windows)

Wie Linux, zusätzlich: MSVC (x64, C++20) und OpenCV wird unter
`D:/opencv/build` erwartet (siehe `CMakeLists.txt`). CMake kopiert nach dem
Build DLLs, `webVisu/` und Konfiguration automatisch neben die EXE.

## Raspberry Pi / Anlage

Die Anlage läuft als Docker-Container auf einem Pi (ARM64). Ein x86-Image
funktioniert dort nicht — auf dem Pi selbst bauen oder per
`docker buildx build --platform linux/arm64` cross-bauen.
