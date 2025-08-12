# C++ Web Server

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![42 School](https://img.shields.io/badge/42%20Project-Webserv-black?style=for-the-badge&logo=42)
![1337 School](https://img.shields.io/badge/1337%20Project-Webserv-black?style=for-the-badge&logo=1337)
![http1.1](https://img.shields.io/badge/HTTP%2F1.1-FF0000?style=for-the-badge&logo=http&logoColor=white)
![RFC](https://img.shields.io/badge/RFC-0000FF?style=for-the-badge&logo=rfc&logoColor=white)
![epoll](https://img.shields.io/badge/epoll-00FF00?style=for-the-badge&logo=linux&logoColor=black)

## Overview

This project is a from-scratch implementation of a C++ HTTP/1.1 web server. It is designed to be robust, non-blocking, and capable of handling multiple client connections simultaneously using advanced I/O multiplexing. The server is configured via a detailed file, allowing it to host multiple websites (virtual hosts) on different ports, handle various HTTP methods, and serve static or dynamic content through CGI scripts.

***

## Platform Specificity: Linux Only

**This web server is designed to run exclusively on Linux.**

This is a deliberate design choice based on the use of the `epoll` API for handling I/O operations. While other mechanisms like `select` and `poll` are more portable, `epoll` offers significant performance advantages that are critical for a high-performance server.

## 📋 Table of Contents
- [Core Concept: `epoll` vs. `select`/`poll`](#core-concept-epoll-vs-selectpoll)
- [Architecture Diagram](#architecture-diagram)
- [Key Features](#key-features)
- [HTTP/1.1 and RFC Compliance](#http11-and-rfc-compliance)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [License](#license)
- [Author](#author)
- [Acknowledgments](#acknowledgments)

***

## Core Concept: `epoll` vs. `select`/`poll`

To handle many clients at once without creating a thread for each one, servers use I/O multiplexing. `select`, `poll`, and `epoll` are all APIs for this, but they work very differently.

### The Problem with `select` and `poll`

`select` and `poll` are level-triggered and operate in a way that is inefficient at scale. Every time you call them, you must pass the *entire list* of file descriptors (sockets) you are monitoring. The kernel then iterates through this entire list to check which ones are ready for I/O.

This becomes a major bottleneck as the number of connections grows. The complexity is **O(n)**, where `n` is the number of connections.

```
// The Inefficient "Ask Everyone" Method (select/poll)

Your App: "Hey Kernel, out of these 10,000 sockets, is anyone ready?"
   |
   v
Kernel:   (Iterates through all 10,000 sockets)
          "Okay, after checking all of them, sockets 3, 42, and 1000 are ready."
   |
   v
Your App: (Processes the 3 ready sockets)
   |
   v
Your App: "Okay, how about now? Out of these 10,000 sockets..." (repeats)
```

### The `epoll` Advantage

`epoll` is an edge-triggered mechanism. You create a single `epoll` instance in the kernel and register the file descriptors you are interested in *once*. The kernel then watches these for you. When a socket becomes ready, the kernel adds it to a "ready list." When you call `epoll_wait`, the kernel simply gives you that list of already-known ready sockets.

The complexity is **O(1)**, because the cost doesn't change with the number of connections being watched. This makes it vastly more scalable and performant.

```
// The Efficient "Tell Me When Ready" Method (epoll)

Your App: "Hey Kernel, create an epoll instance for me."
   |
   v
Your App: "Watch these 10,000 sockets and just let me know if any become ready."
   |
   v
Kernel:   (Monitors sockets internally. Sockets 3, 42, and 1000 become ready.)
          (Kernel adds them to a private "ready list".)
   |
   v
Your App: "Hey Kernel, is anyone on the ready list?"
   |
   v
Kernel:   "Yep, here are sockets 3, 42, and 1000." (Returns immediately)
```

This is why `epoll` is the standard for high-performance networking on Linux.

***

## Architecture Diagram

The server operates in a non-blocking event loop, reacting to I/O events managed by `epoll`.

```
+---------------------+
|   epoll_create()    | Creates the epoll instance
+---------------------+
          |
          v
+---------------------+
|  Bind Server Sockets| Listens on ports from config file
| (epoll_ctl ADD)     |
+---------------------+
          |
          v
+---------------------+
|   Main Event Loop   | Waits for events
|  (epoll_wait)       |
+---------------------+
          |
+-------------------------------------------------+
|                         |                         |
v                         v                         v
[ New Connection ]      [ Client Sent Data ]      [ Client Ready to Recv ]
  - accept()            - read() request          - write() response
  - Add client to       - Parse HTTP request      - If write is partial,
    epoll instance      - Match to server block     keep writing later
  - (non-blocking)      - Handle request          - Close connection if done
                        (static file or CGI)
```

***

## Key Features

* **HTTP/1.1 Compliant:** Adheres to the standards set by RFCs, implementing required methods and headers.
* **Configuration File:** A single `.conf` file to define multiple virtual servers, ports, error pages, and location-specific rules.
* **Non-Blocking I/O:** Uses `epoll` to manage thousands of simultaneous connections efficiently.
* **Virtual Servers:** Can host multiple websites on different ports from the same server process, thanks to the mandatory `Host` header in HTTP/1.1.
* **Static File Serving:** Serves static files (HTML, CSS, images) from a specified root directory.
* **CGI Execution:** Supports Common Gateway Interface (CGI) for dynamic content generation (e.g., with Python or PHP scripts).
* **Custom Error Pages:** Allows users to define custom pages for HTTP errors.
* **Request & Response Parsing:** Robust parsing of HTTP headers and construction of valid responses.

***

## HTTP/1.1 and RFC Compliance

This server implements version 1.1 of the Hypertext Transfer Protocol. The standards for HTTP/1.1 are defined in a series of documents called **RFCs (Request for Comments)**. While the original specification was **RFC 2616**, it has since been updated and split into a series of more specific documents (RFC 7230-7235).

Key aspects of HTTP/1.1 compliance in this server include:

* **Mandatory `Host` Header:** All HTTP/1.1 requests must include a `Host` header. This is what allows for **virtual hosting**, where a single server (with one IP address) can serve content for multiple domains. Our server uses this header to match incoming requests to the correct server block defined in the configuration file.
* **Implemented Methods:** The server correctly handles the most common HTTP methods:
    * `GET`: To request a representation of a specified resource.
    * `POST`: To submit data to be processed to a specified resource (e.g., submitting a form or uploading a file).
    * `DELETE`: To delete a specified resource.
* **Persistent Connections:** While not fully implemented with `keep-alive`, the server's architecture is designed for handling persistent connections where multiple requests can be sent over the same TCP connection.
* **Status Codes:** The server provides accurate status codes to indicate the result of a request (e.g., `200 OK`, `404 Not Found`, `500 Internal Server Error`).

***

## Project Structure

The project is organized into distinct layers, each responsible for a specific part of the server's functionality.

```
Web-Server/
├── Makefile
├── main.cpp
├── headers/
├── config_layer/
├── networking_layer/
├── parsing_layer/
├── response_layer/
├── utils/
├── configuration_test/
└── test_files/
```

* `Makefile`: Compiles the project into a `webserv` executable.
* `main.cpp`: The entry point of the program, responsible for initializing and running the main server loop.
* `headers/`: Contains all the C++ header files (`.hpp`).
* `config_layer/`: Handles parsing and validation of the server configuration file.
* `networking_layer/`: Manages sockets, ports, and the `epoll` instance for handling connections.
* `parsing_layer/`: Responsible for parsing incoming HTTP requests from clients.
* `response_layer/`: Constructs and sends HTTP responses back to the client.
* `utils/`: Contains miscellaneous utility functions used across the project.
* `configuration_test/` & `test_files/`: Directories for testing purposes.

***

## Getting Started

### Prerequisites

* A C++ compiler (e.g., `g++` or `clang`)
* `make` utility
* A **Linux** operating system

### Installation & Compilation

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/achahid19/Web-Server.git](https://github.com/achahid19/Web-Server.git)
    cd Web-Server
    ```

2.  **Compile the project:**
    ```bash
    make
    ```

3.  **Run the server:**
    You must provide a configuration file as an argument.
    ```bash
    ./webserv conf/default.conf
    ```
    The server will now be running and listening on the ports defined in your configuration.

***

## License

This project is academic work for the 42 Network and is not intended for commercial use or redistribution.

## Author
**© Anas Chahid ksabi **@KsDev**** - [achahid19](https://github.com/achahid19)

## Acknowledgements
This project is part of the curriculum at **1337 Coding School**, a member of the **42 Network**. It is a deep dive into low-level networking, system calls, and the mechanics of the HTTP protocol.

[![School](https://img.shields.io/badge/Notice-1337%20School-blue.svg)](https://1337.ma/en/)
[![School](https://img.shields.io/badge/Notice-42%20School-blue.svg)](https://42.fr/en/homepage/)
