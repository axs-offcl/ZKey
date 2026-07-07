# ZKey - Antivirus False Positive Reports

## Why ZKey Gets Flagged

ZKey uses `SetWindowsHookEx` for input overlay — the same API used by keyloggers. This causes heuristic-based AV detection. The app is open source, safe, and does not log keystrokes.

---

## Microsoft Defender (Required)

**Submit here**: https://www.microsoft.com/en-us/wdsi/filesubmission

**Email**: `samples@samples.mcas.microsoft.com`

**Subject**: False Positive Report - ZKey Overlay (SetWindowsHookEx)

```
Hello Microsoft Security Team,

I am the developer of ZKey, an open-source keyboard and mouse input overlay 
for streamers and gamers. The application uses SetWindowsHookEx to capture 
keyboard and mouse events for real-time overlay display.

This is a legitimate use of the Windows API — the same API is used by many 
overlay tools (OBS, Discord, game overlays). ZKey does NOT log, store, or 
transmit keystrokes.

Repository: https://github.com/axs-offcl/ZKey
Source code: https://github.com/axs-offcl/ZKey/tree/main/src

Please whitelist ZKey.exe to prevent false positive detections.

Thank you.
```

---

## Bitdefender

**Submit here**: https://www.bitdefender.com/business/support/

**Email**: `viruslaboratory@bitdefender.com`

```
Subject: False Positive - ZKey Overlay Application

Dear Bitdefender Team,

ZKey is an open-source keyboard/mouse overlay for Windows. It uses 
SetWindowsHookEx for input capture (same as OBS, Discord overlays).

Please analyze and whitelist:
- File: ZKey.exe
- SHA256: [get from VirusTotal after build]
- Repository: https://github.com/axs-offcl/ZKey

The application does not log keystrokes — it only highlights pressed keys 
on an overlay for streaming purposes.
```

---

## Norton (Symantec)

**Submit here**: https://submit.nortonsafety.com/

```
Subject: False Positive Report - ZKey Overlay

ZKey is an open-source keyboard overlay application that uses Windows 
input hooks for real-time key highlighting. This is a legitimate use 
of SetWindowsHookEx, similar to OBS and Discord game overlays.

The application does not collect or transmit any user data.
Repository: https://github.com/axs-offcl/ZKey

Please whitelist this application.
```

---

## Kaspersky

**Submit here**: https://opentip.kaspersky.com/

```
False Positive Report - ZKey Overlay

ZKey uses SetWindowsHookEx for keyboard/mouse overlay display. This is 
a standard Windows API used by legitimate overlay applications. ZKey is 
open source and does not log keystrokes.

Repository: https://github.com/axs-offcl/ZKey
```

---

## McAfee

**Submit here**: https://secure.mcafee.com/appsubmission/

```
False Positive - ZKey Overlay

ZKey is an open-source keyboard/mouse overlay for streamers. It uses 
SetWindowsHookEx for input capture (same API as OBS, Discord).

Does NOT log or transmit keystrokes.
Repository: https://github.com/axs-offcl/ZKey

Please whitelist ZKey.exe.
```

---

## How to Get SHA256 Hash

After building ZKey.exe:

```powershell
Get-FileHash .\build\Release\ZKey.exe -Algorithm SHA256
```

Or upload to https://www.virustotal.com to get the hash and check detections.

---

## After Getting Code Signing

Once you have a code signing certificate:
1. Sign ZKey.exe with `signtool`
2. Re-submit to all AV vendors — signed binaries get whitelisted faster
3. The signature proves your identity and builds trust
