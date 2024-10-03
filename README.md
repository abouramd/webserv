# Webserv

## Introduction

Webserv is a custom HTTP server implementation in C++98. This project aims to deepen understanding of how web servers and the HTTP protocol work by building a server from scratch.

## Features

- HTTP/1.1 compliant
- Supports GET, POST, and DELETE methods
- Configurable via a configuration file
- Serves static websites
- Handles file uploads
- CGI support
- Non-blocking I/O with select()
- Error handling with custom error pages

## Requirements

- C++98 compatible compiler
- Make

## Building the Project

To build the project, run:

```
make
```

This will compile the source files and generate the `webserv` executable.

## Usage

Run the server with:

```
./webserv [configuration_file]
```

If no configuration file is specified, the server will use a default configuration (./config/default.conf).

## Configuration

The server can be configured using a configuration file. The configuration file allows you to:

- Set port and host for each server
- Configure server names
- Set up default error pages
- Limit client body size
- Define routes with specific rules
- Configure directory listing
- Set up CGI execution

Refer to the project documentation for detailed configuration options.

## Bonus Features

- Cookie and session management
- Multiple CGI support

## Contributing
This is a team project developed as part of our curriculum. The main contributors are:

- [Abdelhay Bouramdane](https://github.com/abouramd).
- [Rachid Amhouch](https://github.com/rachidamhouch).
- [Lahbib Semlali](https://github.com/lahbibsemlali).

## Disclaimer

This project is part of the curriculum at 1337 and is intended for educational purposes.
