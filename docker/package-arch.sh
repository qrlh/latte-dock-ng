#!/usr/bin/env bash
# Create an Arch Linux pacman package for latte-dock-ng.
# Runs inside the Arch Docker container.
set -euo pipefail

VERSION="${1:?Usage: $0 <version>}"
OUTDIR="${2:-/out}"

cmake /src -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DVERSION="${VERSION}"
cmake --build . --parallel "$(nproc)"

DESTDIR=/staging cmake --install .

cat > /staging/.PKGINFO << EOF
pkgname = latte-dock-ng
pkgver = ${VERSION}-1
pkgdesc = Latte Dock NG - Plasma 6 desktop dock
url = https://github.com/ruizhi-lab/latte-dock-ng
arch = x86_64
license = GPL-2.0-or-later
depend = qt6-base
depend = qt6-declarative
depend = qt6-wayland
depend = libplasma
depend = plasma-activities
depend = plasma-activities-stats
depend = plasma-workspace
depend = kwayland
depend = kirigami
depend = knewstuff
depend = kcmutils
depend = karchive
depend = kconfig
depend = kcoreaddons
depend = kcrash
depend = kdbusaddons
depend = kdeclarative
depend = kglobalaccel
depend = kguiaddons
depend = ki18n
depend = kiconthemes
depend = kio
depend = kitemmodels
depend = knotifications
depend = kpackage
depend = ksvg
depend = kwindowsystem
depend = kxmlgui
depend = layer-shell-qt
depend = hicolor-icon-theme
EOF

cd /staging
tar -c -I 'zstd -19' -f "${OUTDIR}/latte-dock-ng-${VERSION}-1-x86_64.pkg.tar.zst" .PKGINFO usr/
