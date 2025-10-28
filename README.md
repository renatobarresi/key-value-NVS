# Resilient Map

A simple, resilient key-value map implementation in C, designed for embedded systems. It persists data to a storage layer, which is mocked to use the host's filesystem for demonstration and testing purposes.

## Features

-   **Key-Value Storage**: Store data using string-based keys.
-   **Data Types**: Supports string and `uint32_t` value types.
-   **Persistence**: Entries are saved to a storage backend.
-   **Resilience**: Newer entries with the same key automatically overwrite older ones upon initialization.

## Folder Structure

```
.
├── cmake/                # CMake helper scripts (CPM.cmake)
├── source/
│   ├── app/              # Business logic
│   │   ├── inc/
│   │   └── src/
│   └── hardware/         # Hardware abstraction layer
│       └── mx25_mock/    # Mock for the flash driver
├── test/
|   |── mx25_flash_mock/  # File simulating flash is created here
│   └── unit_test/        # Unit tests
├── CMakeLists.txt        # Main CMake build script
└── README.md             # This file
```

## How to Build and Run

### Prerequisites
-   CMake (version 3.28+)
-   A C/C++ compiler (e.g., GCC)
-   Make (or another build system generator)

### Build Instructions

```bash
# Create a build directory
mkdir build && cd build

# Configure the project
cmake ..

# Build the application and unit tests
make
```

This will create two executables inside the `build/` directory:
-   `resilientMap`: The main application.
-   `test/unit_test/unitTests`: The suite of unit tests.

### Running the Application

```bash
./build/resilientMap
```

The application will prompt you to enter key-value pairs. When you are done, type `END` for the key. The contents of the map will then be printed to the console.

### Running Unit Tests

```bash
./build/test/unit_test/unitTests
```
