pragma Singleton

import QtQuick
import Omanta.Runtime

// Central text-scale helper. Every `font.pixelSize` in qml/ goes through
// Fonts.px() so omanta follows the desktop's apparent text size —
// `gsettings get org.gnome.desktop.interface text-scaling-factor`, which
// `omarchy display text size` drives in lockstep with the shell's
// [font] base-size (12px == 1.0).
//
// Theme.textScale arrives via the portal (org.gnome.desktop.interface /
// text-scaling-factor, same mechanism omacalc uses) and updates live, so
// bindings re-flow without a restart.
QtObject {
    readonly property real scale: Theme.textScale

    function px(base) {
        return Math.max(1, Math.round(base * scale));
    }
}
