# syntax=docker/dockerfile:1
# =============================================================================
# Stage 1: Build
# =============================================================================
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Der Dienst selbst ist Python-frei (Webserver/Einstellungen/Bild nativ in C++:
# cpp-httplib + SQLite + OpenSSL). python3 wird hier NUR zum Bauen benötigt:
# open62541 erzeugt seinen Code (Nodeset/Amalgamation) mit Python-Skripten.
# Im Runtime-Image ist kein Python mehr enthalten.
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config \
    ca-certificates \
    python3 \
    libssl-dev \
    libopencv-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

# open62541 wird im Projekt-Build mitgebaut (siehe CMakeLists). Tests aus,
# damit im Image kein GoogleTest heruntergeladen wird.
RUN cmake -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_TESTS=OFF \
 && cmake --build build --target MyCppExecutable

# =============================================================================
# Stage 2: Runtime
# =============================================================================
FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Nur noch OpenCV- und OpenSSL-Runtime - kein Python, kein Flask.
# Hinweis: libopencv-dev ist bewusst gewählt (zuverlässig, zieht alle
# benötigten Runtime-Libs). Später auf die spezifischen libopencv-*-Runtime-
# Pakete verschlankbar, sobald die exakten Ubuntu-24.04-Namen verifiziert sind.
RUN apt-get update && apt-get install -y --no-install-recommends \
    libopencv-dev \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Artefakte aus dem Build-Stage: Programm + HTML-Templates.
COPY --from=builder /src/build/MyCppExecutable      /app/MyCppExecutable
COPY --from=builder /src/src/webVisu/templates      /app/webVisu/templates

# Laufzeit-Konfiguration. RESTART_ON_SAVE entfällt: Einstellungen werden zur
# Laufzeit übernommen (OPC-Thread verbindet bei Bedarf selbst neu).
ENV APP_DIR=/app \
    SETTINGS_DB=/app/data/settings.db

# Mount-Punkte: Projektordner (fest) und persistente Settings-DB.
RUN mkdir -p /data/projects /app/data
VOLUME ["/app/data"]

EXPOSE 5000

# Dienst direkt starten - kein Python-Entrypoint mehr nötig.
CMD ["/app/MyCppExecutable"]
