# Chronista 📜

Chronista is a project developed as part of the college assignment for the course
CK0117 - Database Management Systems. This project focuses on the implementation of the
conservative 2-Phase Locking (2V2PL) protocol for concurrency control.

## Project Overview

The primary objective of Chronista is to accept any given schedule
(a set of transactions and their operations) as input and ensure the correct synchronization of
the operations within that schedule.

## Features

- **Conservative 2-Phase Locking Protocol**: Implements the 2V2PL protocol for effective concurrency control.
- **Transaction Schedule Processing**: Accepts and processes various transaction schedules.
- **Synchronization Assurance**: Ensures correct synchronization of operations to prevent data anomalies.

## Usage

You can run the command line interface provided by Chronista using the following syntax:

```
chronista_cli [option] [input|filename]
```

### Options

- `-f [filename]` : Specify a file containing the transactions to be processed.
- `-i` : Accept transactions from the standard input (STDIN). You can pipe data directly into the command.
- `-h` : Display help information about the available options and usage.


### Examples

1. Using a file:
```bash
chronista-cli -f transactions.txt
```
2. Using standard input:
```bash
echo "transaction data" | chronista-cli -i
```
3. Displaying help:
```bash
chronista-cli -h
```

## Build, Run and Test

### Requirements

To build and run the project, ensure you have the following installed:

- **Compiler**: `clang++`, `gcc` or `cl`
- **Build System**: `CMake` and `make`
- **Testing Framework**: `ctest` (optional)

### Building the Project

First generate all configuration and build files with:

```bash
cmake .
```

Then, to build all executables and libraries, run the following command in the project directory:

```bash
make
```

or, if want only the Chronista library run:

```bash
make Chronista
```

### Running the Executable

To run the Chronista command-line interface (CLI), use one of the following commands:

```bash
make run
```

or

```bash
make chronista_cli && ./bin/chronista_cli
```

### Testing the Project

To compile and run the tests, follow these steps:

1. Compile the test binary: `make chronista_tests`
1. Run the tests using one of the following commands:
   - Using `ctest`: `ctest`
   - Using `make`: `make test`
   - Directly executing the test binary to view the native [doctest](https://github.com/doctest/doctest) output: `./bin/chronista_tests`

## License

This project is licensed under the [GNU General Public License v3.0 (GPL-3.0)](https://www.gnu.org/licenses/gpl-3.0.en.html).
