# CacheLite - High Performance Multithreaded In-Memory KV Store

CacheLite is an ultra-fast, multithreaded, persistent key-value store built in C++17 from the ground up for raw performance and reliability.

## Features
* TCP server with a dedicated thread-per-connection architecture
* Mutex-protected thread-safe Key-Value store operations
* Lazy + active TTL expiration with a dedicated background sweep thread
* File stream (fstream) AOF persistence spanning across restarts
* Thread-safe global logger
* Multithreaded stress-test client

## Architecture
* **Winsock2 TCP Server**: Manages the socket lifecycle and spawns independent connection threads.
* **Client Handler**: Isolates the socket communication layer per connected client.
* **Command Parser**: Tokenizes raw byte streams into actionable C++ arguments.
* **KV Engine**: The thread-safe core map structure backing the `std::unordered_map` with a `std::mutex`.
* **TTL Manager**: Background daemon aggressively sweeping expired keys independent of GET calls.
* **Persistence Manager**: Loads and saves the cache state via AOF-style `dump.cache` formatting.
* **Logger**: Synchronized thread-safe console logging module.

## Supported Commands

| Command | Syntax | Behavior |
| --- | --- | --- |
| **SET** | `SET <key> <value>` | Stores or overwrites a key with a value, stripping any existing TTL. Returns `OK`. |
| **GET** | `GET <key>` | Returns the value. If missing or expired, returns `(nil)`. |
| **DEL** | `DEL <key>` | Removes the key from the store. Returns `1` if successful, `0` if not found. |
| **EXISTS**| `EXISTS <key>` | Returns `1` if the key exists and isn't expired, else `0`. |
| **EXPIRE**| `EXPIRE <key> <seconds>` | Sets a Time-To-Live in seconds. Returns `1` on success, `0` if the key does not exist. |

## Build Instructions
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Performance
The CacheLite engine sustains massive concurrency limits, easily achieving **148,458 ops/sec** across 10 concurrent clients performing 20,000 total operations with absolutely **0 errors**.

## What this project intentionally excludes
To maintain ultra-low latency and explicit focus on the absolute core mechanics of a caching engine, this project intentionally excludes the RESP protocol, thread pooling, LRU eviction, clustering, Pub/Sub, and transactions.

## Tech Stack
* C++17
* STL (`unordered_map`, `thread`, `mutex`, `chrono`, `fstream`)
* Winsock2
* CMake
* MinGW
