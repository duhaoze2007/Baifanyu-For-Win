#!/bin/bash
# BaifanYu (白饭鱼) — Windows build script.
#
#   bash build.sh            -> BaifanYu.exe (release, single self-contained file)
#   bash build.sh debug      -> the same, with -Og and no stripping
#
# Needs a MinGW-w64 toolchain on PATH (g++, windres).  MSYS2's ucrt64 is what
# this was developed against:  pacman -S mingw-w64-ucrt-x86_64-gcc
#
# Everything the app needs — artwork, duck samples, icon, manifest — is compiled
# into the executable with windres, so the result is one standalone .exe.
set -e
cd "$(dirname "$0")"
HERE="$(pwd -W 2>/dev/null || pwd)"

CONFIG="${1:-release}"

GXX="$(command -v g++ || true)"
WINDRES="$(command -v windres || true)"
if [ -z "$GXX" ] || [ -z "$WINDRES" ]; then
    echo "!! g++ / windres not found on PATH." >&2
    echo "   Install a MinGW-w64 toolchain, e.g. on MSYS2:" >&2
    echo "     pacman -S mingw-w64-ucrt-x86_64-gcc" >&2
    exit 1
fi

case "$HERE" in
    *[!\ -~]*) ASCII=no ;;
    *) ASCII=yes ;;
esac

# windres and ld shell their helper processes out with ANSI command lines, so
# they cannot cope with a non-ASCII path (a Chinese user name, for instance;
# gcc itself is fine).  When the project lives under such a path we mirror it
# into an ASCII directory, build there, and copy the executable back.
if [ "$ASCII" = yes ]; then
    BUILD="$HERE"
else
    BUILD=""
    for candidate in "${BAIFANYU_STAGE:-}" "/c/ProgramData/BaifanYu-build" \
                     "/c/msys64/tmp/BaifanYu-build" "/c/baifanyu-build"; do
        [ -n "$candidate" ] || continue
        case "$candidate" in
            *[!\ -~]*) continue ;;
        esac
        if mkdir -p "$candidate" 2>/dev/null; then
            BUILD="$candidate"
            break
        fi
    done
    if [ -z "$BUILD" ]; then
        echo "!! The project path contains non-ASCII characters and no ASCII" >&2
        echo "   staging directory could be created.  Set BAIFANYU_STAGE to one." >&2
        exit 1
    fi
    echo "==> Path is not ASCII; building in $BUILD"
    cp -ru "$HERE/src" "$HERE/assets" "$BUILD/"
fi

cd "$BUILD"

echo "==> Compiling resources (artwork, sounds, icon, manifest)..."
"$WINDRES" src/app.rc -o app.res.o -I src

CXXFLAGS="-std=c++17 -municode -mwindows -DUNICODE -D_UNICODE -Wall -Wno-unused-parameter"
if [ "$CONFIG" = debug ]; then
    CXXFLAGS="$CXXFLAGS -Og -g"
    echo "==> Building (debug)..."
else
    CXXFLAGS="$CXXFLAGS -O2"
    echo "==> Building (release)..."
fi

# Compile first, then link: joining both steps into one g++ call makes ld
# stumble over the 13 MB resource object, and separate objects keep rebuilds
# incremental.
mkdir -p build/obj
for f in src/*.cpp; do
    b="$(basename "$f" .cpp)"
    "$GXX" $CXXFLAGS -c "$f" -o "build/obj/$b.o"
done
echo "    compiled $(ls build/obj/*.o | wc -l) objects"

LINKFLAGS="-municode -mwindows -static -static-libgcc -static-libstdc++"
[ "$CONFIG" = release ] && LINKFLAGS="$LINKFLAGS -s"
LIBS="-lgdiplus -lole32 -luser32 -lgdi32 -lshell32 -lcomctl32 -lwinmm -lwtsapi32 -limm32 -ladvapi32"

echo "==> Linking..."
"$GXX" $LINKFLAGS build/obj/*.o app.res.o -o BaifanYu.exe \
    -Wl,--nxcompat -Wl,--dynamicbase $LIBS

if [ "$BUILD" != "$HERE" ]; then
    if ! cp "BaifanYu.exe" "$HERE/BaifanYu.exe" 2>/dev/null; then
        echo "!! Could not overwrite $HERE/BaifanYu.exe" >&2
        echo "   She is probably still running — quit her from the notification-area" >&2
        echo "   menu (or stop the process) and build again." >&2
        exit 1
    fi
fi

echo "==> Done: $HERE/BaifanYu.exe"
ls -la "$HERE/BaifanYu.exe" | awk '{printf "    size: %.1f MB\n", $5/1048576}'
echo "    Run it — her face appears in the notification area and she floats on"
echo "    your desktop.  Right-click her or the tray icon for skin / settings."
