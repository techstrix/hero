# hero

A small, minimal X11 window manager written in C++.

`hero` reparents every top-level client window into a simple frame, gives it a
colored border, and keeps the client's geometry in sync with its frame. It is a
learning-sized implementation — no window decorations, no workspaces, no
config file — but it is enough to start X11 applications on a raw X server.

## Requirements

- A C++20 compiler (`clang++` is used by default)
- Xlib development headers
- Google's glog logging library
- `make`

On Debian/Ubuntu:

```sh
sudo apt install clang make libx11-dev libgoogle-glog-dev
```

On Arch Linux:

```sh
sudo pacman -S clang make libx11 glog
```

For the nested X server and the client used by `start_hero.sh`, also install:

Debian/Ubuntu:

```sh
sudo apt install xserver-xephyr kitty
```

Arch Linux:

```sh
sudo pacman -S xorg-xephyr kitty
```

## Building

```sh
make
```

This produces the `hero` binary in the project directory. To remove the build
artifacts:

```sh
make clean
```

## Usage

`hero` connects to the X server specified by the `DISPLAY` environment
variable, so it must be started with `DISPLAY` pointing at a display that has no
other window manager running.

### Quick start with `start_hero.sh`

`start_hero.sh` wraps the common setups: it chooses a display, starts `hero`,
and launches a client for you.

Test it inside [Xephyr](https://www.freedesktop.org/wiki/Software/Xephyr/), a
nested X server that runs in a window on your existing desktop:

```sh
./start_hero.sh xephyr
```

This mode:

- starts (or reuses) Xephyr on display `:3` at `1366x768`,
- runs `hero` against `:3`,
- launches `kitty` on `:3` with `WAYLAND_DISPLAY=""`, so it stays inside
  Xephyr even when your desktop is Wayland.

Run it against your current desktop session instead:

```sh
./start_hero.sh
```

or explicitly:

```sh
./start_hero.sh real
```

Real mode uses whatever `DISPLAY` is already set, so make sure no other window
manager is running on it.

The script backgrounds both `hero` and the client. Close the Xephyr window (or
kill `hero`) when you are done; `hero` exits when the X server goes away.

Edit the last lines of `start_hero.sh` to launch different clients:

```sh
WAYLAND_DISPLAY="" DISPLAY="$TARGET_DISPLAY" kitty &
```

### Manual setup

You can also run the pieces yourself. Start Xephyr on display `:1`:

```sh
Xephyr :1 -screen 1280x720 -ac &
```

Then run `hero` on that display (this terminal will stay busy in the event loop):

```sh
DISPLAY=:1 ./hero
```

From another terminal, launch a client on the same display:

```sh
DISPLAY=:1 xterm &
```

The client should appear inside the Xephyr window, surrounded by a red frame.
When you are done, close the Xephyr window; `hero` exits when the X server goes
away.

#### Wayland-native clients

If you are running Xephyr on a Wayland session, some clients (e.g. `kitty`)
default to their Wayland backend and will open on your real desktop instead of
inside Xephyr, ignoring `DISPLAY`. To force them onto X11 and into Xephyr,
unset the Wayland display too:

```sh
WAYLAND_DISPLAY="" DISPLAY=:1 kitty
```

Passing the empty string is the important part: it tells the toolkit that no
Wayland compositor is available, so it falls back to X11 and connects to
`DISPLAY`. The same trick works for other toolkit apps that prefer Wayland, and
`start_hero.sh` already does this for the client it launches.

### Logging

`hero` logs through glog. To send logs to stderr instead of a file under `/tmp`:

```sh
GLOG_logtostderr=1 DISPLAY=:1 ./hero
```
Incase you would like to run this as your main wm (with caution) always have a background wm to fall back to follow these steps:
1. Create an ~/.xinitrc file and populate it with
```

#!/bin/sh

# Launch a fallback terminal in case Hero crashes
xterm &

# Start the Hero window manager
exec hero




```
Save this file 


### 1. Create a Hero session file
Create a file called:

hero.desktop

The usual location for an X11 window manager session is:

/usr/share/xsessions/hero.desktop

### 2. Put the following inside it:
```

[Desktop Entry]
Name=Hero
Comment=Hero Window Manager
Exec=sh -c 'xterm & exec hero'
TryExec=hero
Type=Application
DesktopNames=Hero

```

If hero isn't installed in a directory on your $PATH, replace both Exec and TryExec with its absolute executable path, for example:
```
Exec=/home/youruser/hero/build/hero
TryExec=/home/youruser/hero/build/hero
```
Make sure the executable is accessible to your user.

### 3. Select Hero from the login screen
1. Log out of your current desktop session.
2. At the login screen, look for the session selector (often a gear icon or menu).
3. Select Hero.
4. Log in.
