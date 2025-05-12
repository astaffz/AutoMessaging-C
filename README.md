# Simple Message Spamming Script, written in C

A simple Windows console tool that automatically types a user input repeatedly into the active window. It uses `SendInput()` and listens for `Backspace` and `ESC` keys to stop or paus (Currently hardcoded in, no option to change the keys).
---

## 🧰 Features

- Types your message repeatedly with a user-defined delay.
- Optional repeat limit (`0 = infinite`).
- Press `ESC` to pause/resume sending.
- Press `Backspace` to stop and exit.
- Retries `SendInput()` up to 3 times if it fails.
- Thread-safe and memory-safe with retry handling for critical parts.
