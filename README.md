# GehennaServer

An early C++ networking server for experimenting with authenticated client and game-server connections. It uses SFML TCP packets for communication and SQLite for key lookup. The implemented application workflow is a small authenticated message relay; gameplay and game-server packet processing remain unfinished.

## Implemented behavior

- Accepts TCP connections through an SFML socket selector.
- Reads a key string from the first packet and checks it using bound SQLite parameters.
- Separates ordinary clients and game servers by permission level, rejecting keys already connected in the corresponding group.
- Replies with `authenticated` or `denied`.
- Broadcasts a client's subsequent string messages to the other ordinary clients.
- Removes disconnected sockets from the selector and connection maps.
- Supports console input `1` to list clients and `test` to broadcast a test string.

The client admission check uses a capacity of 100 ordinary clients. This is a configured limit, not a measured concurrency guarantee.

## Source layout

| Path | Purpose |
| --- | --- |
| `src/Main.cpp` | Application entry point |
| `src/Server.cpp`, `src/Server.h` | Listener, authentication, connection maps, relay and console thread |
| `src/backend/DataHandler.cpp` | Directory and database initialization code |
| `src/backend/PacketHandler.*` | Empty packet-handler placeholder |
| `CMakeLists.txt` | C++17 executable and dependency configuration |

## Build prerequisites

- CMake 3.28 or newer, Git, and a C++17 compiler.
- Network access during configuration to fetch SFML from its `2.6.x` branch. This is a moving branch rather than a fixed dependency revision.
- SQLite development headers, a compiler-compatible link library, and the matching runtime DLL on Windows.

The current build file contains a machine-specific `SQLITE3_DIR`. Edit that setting in `CMakeLists.txt` to a local installation containing `include/sqlite3.h`, a link library under `lib`, and `sqlite3.dll` at its root. Passing a cache variable alone does not override the existing unconditional setting.

After adapting SQLite, the CMake build commands are:

```sh
cmake -S . -B build
cmake --build build --config Debug
```

Use a new build directory; the committed `cmake-build-debug` directory contains generated, machine-specific artifacts.

## Runtime requirements

Start the executable with the repository root as its working directory because the server opens `data/key.db` relative to the current directory. For a single-configuration Windows build, the expected command is:

```powershell
.\build\bin\GehennaServer.exe
```

Multi-configuration generators can place the executable in a configuration subdirectory such as `build/bin/Debug`.

Authentication requires a `KEYS` table with `HASH`, `USED`, and `PERMISSIONLEVEL` columns. Eligible rows have `USED = 0`; permission levels 1 and 2 identify clients and game servers respectively. A compatible client must use SFML packet serialization, sending a string key first and string messages afterward. No client or automated smoke test is included here.

Database provisioning is incomplete: `main` declares functions named `DataHandler` and `PacketHandler` instead of constructing their objects, so the initializer does not execute. Its table definition also omits `PERMISSIONLEVEL`. A fresh authentication database therefore requires corrected initialization or separate provisioning with synthetic development keys.

## Current limitations

Game-server packets are received but not processed. Player storage is commented out, and `PacketHandler` has no implementation. Authentication performs blocking reads without a timeout. The console thread accesses connection state without using the declared mutex, and shutdown/cleanup is unfinished. Transport encryption and key lifecycle management are absent. Treat this as a local development prototype.

There is no project test target or CI workflow in the repository. The console commands provide manual inspection only.

## Dependencies and license

Project-specific code lives under `src`; networking and database primitives are supplied by SFML and SQLite. Generated dependency artifacts are not original server implementation. No repository-level license is supplied; dependency licenses remain separate from permission to reuse this project's code.
