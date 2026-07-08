<p align="center">
  <img src="icon/zkey.ico" width="100" alt="ZKey Logo"/>
</p>

<h1 align="center">ZKey</h1>

<p align="center">
  <strong>Real-time Keyboard & Mouse Input Overlay for Windows</strong>
</p>

<p align="center">
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=BETA&secondaryLabel=V2.0&primaryBGColor=%237C3AED&primaryTextColor=%23FFFFFF&secondaryBGColor=%235B21B6&secondaryTextColor=%23FFFFFF&primaryFontWeight=900&secondaryFontWeight=700&primaryLetterSpacing=3&secondaryLetterSpacing=2" alt="Beta v2.0"/>
  &nbsp;
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=MADE&secondaryLabel=WITH+QT6&primaryBGColor=%2341CD52&primaryTextColor=%23FFFFFF&secondaryBGColor=%2334A843&secondaryTextColor=%23FFFFFF" alt="Made with Qt6"/>
  &nbsp;
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=C%2B%2B&secondaryLabel=20&primaryBGColor=%2300599C&primaryTextColor=%23FFFFFF&secondaryBGColor=%23004080&secondaryTextColor=%23FFFFFF" alt="C++20"/>
  &nbsp;
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=WINDOWS&secondaryLabel=10%2F11&primaryBGColor=%230078D4&primaryTextColor=%23FFFFFF&secondaryBGColor=%23005A9E&secondaryTextColor=%23FFFFFF" alt="Windows 10/11"/>
  &nbsp;
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=LICENSE&secondaryLabel=GPLv3&primaryBGColor=%23333333&primaryTextColor=%23FFFFFF&secondaryBGColor=%23555555&secondaryTextColor=%23FFFFFF" alt="GPLv3"/>
</p>

<p align="center">
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=OPEN&secondaryLabel=SOURCE&primaryBGColor=%23FF6B35&primaryTextColor=%23FFFFFF&secondaryBGColor=%23D4531A&secondaryTextColor=%23FFFFFF&primaryIcon=github&primaryIconColor=%23FFFFFF&primaryIconSize=16&primaryIconPosition=left" alt="Open Source"/>
  &nbsp;
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=PLUGIN&secondaryLabel=SYSTEM&primaryBGColor=%23EAB308&primaryTextColor=%23000000&secondaryBGColor=%23CA8A04&secondaryTextColor=%23FFFFFF" alt="Plugin System"/>
  &nbsp;
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=ZERO&secondaryLabel=COST&primaryBGColor=%2310B981&primaryTextColor=%23FFFFFF&secondaryBGColor=%23059669&secondaryTextColor=%23FFFFFF" alt="Zero Cost"/>
</p>

<br/>

## What is ZKey?

ZKey is a **real-time keyboard and mouse input overlay** designed for streamers, gamers, and content creators. It displays an always-on-top, semi-transparent virtual keyboard and mouse that highlights keys as you press them — perfect for tutorials, live streams, and screen recordings.

<br/>

## Demo

<p align="center">
  <em>Record your GIFs here using <a href="https://www.screentogif.com">ScreenToGif</a> and place them in <code>docs/</code></em>
</p>

<!-- Replace these placeholders with actual GIFs -->
<!-- ![Overlay Demo](docs/overlay_demo.gif) -->
<!-- ![Settings Panel](docs/settings_demo.gif) -->
<!-- ![Plugin System](docs/plugin_demo.gif) -->
<!-- ![Layout Editor](docs/editor_demo.gif) -->

<br/>

## Features

<table>
<tr>
<td width="50%" valign="top">

### Keyboard Overlay
- **5 keyboard layouts** — Whole Keyboard, No Numpad, Numbers, Compact, Ctrl Buttons
- **Custom layouts** — design your own with the built-in Layout Editor
- **Real-time highlighting** — pressed keys light up instantly
- **Per-key customization** — colors, font size, bold, corner radius, gradient, glow
- **Independent opacity** — separate sliders for keyboard and mouse
- **Auto-hide** — hides when a game window is in focus
- **KPS counter** — keys per second with Show/Reset toggles

</td>
<td width="50%" valign="top">

### Mouse Overlay
- **5 mouse buttons** — LMB, RMB, MMB, M4 (Forward), M5 (Back)
- **Per-button controls** — Show, Click#, and Reset toggles for each button
- **Click counter modes** — total count or rate counter with decay
- **Visual wheel indicator** — shows scroll direction
- **Configurable offset** — position relative to keyboard
- **Lock toggle** — prevent accidental movement

</td>
</tr>
</table>

<table>
<tr>
<td width="50%" valign="top">

### Plugin System
- **Lua scripting** — extend ZKey with custom plugins
- **Plugin Marketplace** — discover & install from GitHub (zero hosting)
- **Custom GUI** — plugins can add settings pages with sliders, checkboxes, dropdowns
- **Hot-reload** — reload plugins without restarting
- **Sandboxed** — safe execution with instruction limits

</td>
<td width="50%" valign="top">

### Layout Editor
- **Drag-and-drop** — design custom keyboard layouts on a grid
- **Multi-select** — marquee or Shift+click selection
- **Undo/Redo** — full undo stack for all operations
- **Import/Export** — save as `.zkeylayout` files
- **5 built-in presets** — start from existing layouts

</td>
</tr>
</table>

<table>
<tr>
<td width="50%" valign="top">

### Advanced Features
- **Multi-Overlay** — multiple independent overlay windows
- **Overlay Profiles** — save, load, export, and share configurations
- **Profile Marketplace** — download community profiles from GitHub
- **Export as Image** — capture overlays as PNG/JPEG/BMP
- **OSD Mode** — floating text overlay for pressed keys

</td>
<td width="50%" valign="top">

### Customization
- **8 color themes** — per-keyboard and per-mouse theme presets
- **Theme presets** — Green, Red, Purple, Cherry, Yellow, Dark, Light, Custom
- **Draggable overlay** — move anywhere on screen, lock in place
- **Pin toggle** — tilt animation when unpinned
- **Always-on-top** — stays visible over games and applications
- **Persistent config** — settings saved to `ZKey.json` automatically
- **System tray** — quick toggle, settings access, quit

</td>
</tr>
</table>

<br/>

## Download

<p align="center">
  <a href="https://github.com/yourusername/ZKey/releases">
    <img src="https://forthebadge.com/api/badges/generate?primaryLabel=DOWNLOAD&secondaryLabel=latest+release&primaryBGColor=%237C3AED&primaryTextColor=%23FFFFFF&secondaryBGColor=%235B21B6&secondaryTextColor=%23FFFFFF&primaryFontWeight=900&primaryLetterSpacing=3" alt="Download Latest Release"/>
  </a>
</p>

1. Download from the [Releases](https://github.com/yourusername/ZKey/releases) page
2. Extract `deploy.zip`
3. Run `ZKey.exe`

**Requirements:**
- Windows 10/11 64-bit
- [Microsoft Visual C++ Redistributable 2022](https://aka.ms/vs/17/release/vc_redist.x64.exe)

<br/>

## Quick Start

```
1. Launch ZKey.exe — overlay appears at default position
2. Press Ctrl+Shift+K — opens Settings window
3. General tab — choose keyboard layout, enable mouse overlay, configure per-button Show/Click#/Reset
4. Theme tab — pick keyboard and mouse color presets
5. Controls tab — adjust opacity, scale, pin, auto-hide
6. Drag the overlay to reposition it, click pin to lock
7. Right-click tray icon to toggle overlay or quit
```

<br/>

## Plugin Development

```lua
-- my_plugin.lua
function onLoad()
    zkey.log("Plugin loaded!")
end

function onKeyDown(key)
    zkey.log("Key pressed: " .. key)
end

-- Add a settings page
zkey.addSettingsPage({
    title = "My Plugin",
    icon = "\u{2699}",
    controls = {
        { type = "slider", id = "speed", label = "Speed", min = 1, max = 100, default = 50 },
        { type = "checkbox", id = "enabled", label = "Enabled", default = true }
    }
})
```

<p align="center">
  <a href="https://forthebadge.com/api/badges/generate?primaryLabel=READ+THE&secondaryLabel=DOCUMENTATION&primaryBGColor=%23333333&primaryTextColor=%23FFFFFF&secondaryBGColor=%23555555&secondaryTextColor=%23FFFFFF">
    <img src="https://forthebadge.com/api/badges/generate?primaryLabel=READ+THE&secondaryLabel=DOCUMENTATION&primaryBGColor=%23333333&primaryTextColor=%23FFFFFF&secondaryBGColor=%23555555&secondaryTextColor=%23FFFFFF" alt="Read the Documentation"/>
  </a>
</p>

<br/>

## Build from Source

### Prerequisites

| Tool | Version | Link |
|------|---------|------|
| Visual Studio | 2022 Build Tools | [Download](https://visualstudio.microsoft.com/visual-cpp-build-tools/) |
| Qt | 6.8.3 (msvc2022_64) | [Download](https://www.qt.io/download) |
| CMake | 3.22+ | [Download](https://cmake.org/download/) |

### Build

```bash
# Open "x64 Native Tools Command Prompt for VS 2022"
cd ZKey

# Configure
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.8.3/msvc2022_64

# Build
cmake --build build

# Deploy Qt DLLs
C:\Qt\6.8.3\msvc2022_64\bin\windeployqt build/ZKey.exe
```

<br/>

## Project Structure

```
ZKey/
├── src/                        # C++ source code
│   ├── main.cpp               # Entry point, tray icon, CLI
│   ├── overlaywidget.*        # Overlay rendering (keyboard + mouse)
│   ├── overlaymanager.*       # Multi-overlay management
│   ├── inputmanager.*         # Input hooks (keyboard + mouse)
│   ├── settingswindow.*       # Settings UI shell + sidebar
│   ├── settingspage_general.* # Keyboard/mouse enable, layouts, offsets
│   ├── settingspage_theme.*   # Color presets for keyboard and mouse
│   ├── settingspage_controls.* # Opacity, scale, pin, auto-hide
│   ├── settingspage_osd.*     # OSD text mode settings
│   ├── settingspage_plugins.* # Plugin marketplace
│   ├── settingspage_profiles.* # Profile management
│   ├── settingspage_settings.* # Hotkeys, updates, feedback
│   ├── layouteditorwindow.*   # Custom layout editor
│   ├── pluginmanager.*        # Plugin lifecycle management
│   ├── theme.*                # 8 built-in color themes
│   ├── toggleswitch.*         # Custom toggle switch widget
│   └── ...
├── plugins/                    # Lua plugins
│   ├── examples/              # Example plugins
│   └── plugins_config/        # Plugin configuration files
├── layouts/                   # Built-in keyboard layouts
├── icon/                      # Application icons
├── resources.qrc              # Qt resource file
├── CMakeLists.txt             # Build configuration
├── installer.iss              # Inno Setup installer script
└── README.md
```

<br/>

## CLI Usage

ZKey supports headless command-line operation for managing plugins and profiles:

```bash
# Plugin management
ZKey.exe --list-plugins
ZKey.exe --install-plugin <url>
ZKey.exe --enable-plugin <name>
ZKey.exe --disable-plugin <name>

# Profile management
ZKey.exe --list-profiles
ZKey.exe --save-profile "My Setup"
ZKey.exe --load-profile "My Setup"

# JSON output
ZKey.exe --list-plugins --json
```

<br/>

## Keyboard Layouts

| Layout | Description |
|--------|-------------|
| **Whole KB** | Full layout with function row, main keys, nav cluster, and numpad |
| **No Numpad** | Same as above without the numpad |
| **Numbers** | Single row of 0–9 plus Enter |
| **Compact** | 3×3 grid + bottom row (phone numpad style) |
| **Ctrl Buttons** | WASD cluster with Tab, Shift, Ctrl, Alt, Space |

<br/>

## Security & Privacy

ZKey uses Windows input hooks (`SetWindowsHookEx`) to capture keyboard and mouse events for the overlay. This is the **same API** used by keyloggers, which is why antivirus software may flag it.

**Why hooks are needed:**
- To detect key presses and highlight them on the overlay in real-time
- To detect mouse button clicks and scroll wheel events
- No other Windows API provides this functionality for a third-party overlay

**What ZKey does NOT do:**
- Does **not** log or store keystrokes
- Does **not** send data over the network (except plugin marketplace HTTPS requests)
- Does **not** persist keystroke data to disk
- Does **not** inject DLLs into other processes
- Does **not** capture passwords or sensitive input selectively
- Source code is fully auditable on GitHub

**If your antivirus flags ZKey:**
1. This is a **false positive** — ZKey is open source and safe
2. Add an exception for the ZKey folder in your antivirus settings
3. Submit the file to your antivirus vendor for whitelisting

<br/>

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Overlay not appearing | Check that `ZKey.json` exists. If corrupt, delete it and restart |
| No key highlights | Run ZKey as Administrator (some games require admin for hooks) |
| Antivirus warning | False positive — see [Security & Privacy](#security--privacy) above |
| High CPU | Should be <2% at idle. Check for conflicting software |

<br/>

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

<br/>

## License

Distributed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE) for details.

<br/>

<p align="center">
  <img src="https://forthebadge.com/api/badges/generate?primaryLabel=BUILT&secondaryLabel=WITH+LOVE&panels=3&primaryBGColor=%23EF4444&primaryTextColor=%23FFFFFF&secondaryBGColor=%23DC2626&secondaryTextColor=%23FFFFFF&tertiaryBGColor=%23B91C1C&tertiaryTextColor=%23FFFFFF&primaryIcon=github&primaryIconColor=%23FFFFFF&primaryIconSize=16" alt="Built with Love"/>
</p>
