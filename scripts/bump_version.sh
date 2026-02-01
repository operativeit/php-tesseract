#!/usr/bin/env bash
set -euo pipefail

BUMP="${1:-}"
if [[ -z "$BUMP" ]]; then
  echo "Uso: $0 {major|minor|patch|X.Y.Z}"
  exit 1
fi

# Carpeta actual tipo tesseract-1.0.0 (si existe)
CUR_DIR="$(ls -d tesseract-* 2>/dev/null | sort -V | tail -n 1 || true)"
CUR_VER="0.0.0"
if [[ -n "$CUR_DIR" ]]; then
  CUR_VER="${CUR_DIR#tesseract-}"
fi

# Calcula NEXT_VER
if [[ "$BUMP" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
  NEXT_VER="$BUMP"
else
  IFS='.' read -r MA MI PA <<< "$CUR_VER"
  case "$BUMP" in
    major) MA=$((MA+1)); MI=0; PA=0;;
    minor) MI=$((MI+1)); PA=0;;
    patch) PA=$((PA+1));;
    *) echo "BUMP inválido: $BUMP"; exit 1;;
  esac
  NEXT_VER="$MA.$MI.$PA"
fi

echo "Versión: $CUR_VER -> $NEXT_VER"

# Actualiza SOLO <version><release>…</release>
perl -0777 -i -pe "s|(<version>\\s*<release>)[^<]+(</release>\\s*</version>)|\\1$NEXT_VER\\2|s" package.xml

# Si hay carpeta versionada, renómbrala
if [[ -n "${CUR_DIR:-}" && -d "$CUR_DIR" ]]; then
  git mv "tesseract-$CUR_VER" "tesseract-$NEXT_VER"
  # Actualiza README por si menciona la carpeta
  sed -i "s/tesseract-$CUR_VER/tesseract-$NEXT_VER/g" README.md 2>/dev/null || true
fi

echo "$NEXT_VER" > .version
