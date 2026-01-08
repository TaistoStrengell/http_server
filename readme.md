# Simple HTTP Server

A simple multithreaded HTTP server implemented in C.

## Features
* **Threadpool**: Uses a fixed number of threads to handle incoming requests.
* **HTTP Parsing**: Parses the method, path, and version from HTTP requests.
* **File Serving**: Serves files directly from the disk using the `sendfile` system call.
* **IPv6 Support**: The server listens for both IPv4 and IPv6 traffic.

## Makefile Commands
* `make`: Compiles the program and creates the `server` binary.
* `make clean`: Removes object files and the binary.

## Usage
1. Compile the program: `make`
2. Ensure the file `test_file` exists in the root directory.
3. Start the server: `./server`
4. Test in another terminal: `curl -v http://localhost:8080/test_file`