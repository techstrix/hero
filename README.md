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

For the nested X server used in the example below, also install:

```sh
sudo apt install xserver-xephyr xterm
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

The recommended way to try it is inside [Xephyr](https://www.freedesktop.org/wiki/Software/Xephyr/),
a nested X server that runs in a window on your existing desktop.

Start Xephyr on display `:1`:

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
`DISPLAY`. The same trick works for other toolkit apps that prefer Wayland.

### Logging

`hero` logs through glog. To send logs to stderr instead of a file under `/tmp`:

```sh
GLOG_logtostderr=1 DISPLAY=:1 ./hero
```
