# free_mdict_cpp

## Project Overview
free_mdict_cpp is an open-source MDX format dictionary file parsing tool that supports dictionary information display, keyword list output, and interactive word query functionality.

## Usage
### Basic Usage

```bash
./free_mdict [options] <dict_path> 
```

### View Dictionary Information

```bash
./free_mdict /path/to/dictionary.mdx
```

### List All Keywords

```bash
./free_mdict --keys /path/to/dictionary.mdx

### Interactive Query Mode
```bash
./free_mdict --search /path/to/dictionary.mdx
```

### Command Line Parameters

<dict_path>: Dictionary file path (required, provided as positional argument)

--keys: List all keywords

--search: Start interactive query mode

--help: Display help information

## Performance Features
- Uses binary search algorithm to optimize record block query efficiency
- Supports memory management, can release unused resources in small memory mode
- Provides dictionary initialization time statistics

## Development and Contribution
Welcome to submit Issues and Pull Requests to help improve this project.

## Dependency Installation and Compilation

### Dependencies
The project depends on the following libraries:

- CMake (version 3.10 or higher)
- zlib
- gflags (optional) only for Executable program not for lib

### Installing Dependencies on macOS
Using Homebrew to install dependencies:

```bash
brew install cmake zlib gflags
```

### Installing Dependencies on Ubuntu/Debian
Using apt to install dependencies:

```bash
sudo apt-get update
sudo apt-get install cmake libz-dev libgflags-dev
```

### Installing Dependencies on CentOS/RHEL
Using yum to install dependencies:

```bash
sudo yum install cmake zlib-devel gflags-devel
```
### Compiling the Project
1. Clone the repository

```bash
git clone git@github.com:cooper-zhzhang/free_mdict_cpp.git
cd free_mdict_cpp
```

2. Create build directory

```bash
mkdir build
cd build
```
3. Configure the project (uses Release mode by default)

```bash
cmake ..
```
4. Compile the project

```bash
make -j$(nproc)  # or use make -j$(sysctl -n hw.ncpu) (on macOS)
```
5. Install (optional)

```bash
make install
```
Installed files will be located at:
- Executable: /usr/local/bin/free_mdict
- Static library: /usr/local/lib/free_mdict/libfree_mdict.a
- Header files: /usr/local/include/free_mdict/mdict_extern.h

### Compilation Options
Customize compilation behavior through CMake options:

```bash
# Enable small memory mode
cmake -DSMALL_MEMORY=ON ..
# Note: In small memory mode, query speed may slow down due to frequent memory allocation and deallocation.
# Small memory mode does not support getting all keywords, only single keyword queries.

# Change build type (Debug/Release)
cmake -DBUILD_TYPE=Debug ..
```

## Important Notes
- Current version primarily supports MDX format dictionary files
- Currently only supports dictionary files of version 2.0
- Only supports UTF-8/UTF-16 encoded dictionary files
- Only supports zlib compressed dictionary files
- Please ensure the dictionary files used are in correct and complete format

## Static Library Usage
After installing the static library, the header file mdict_extern.h is located in the /usr/local/include/free_mdict/ directory.
When linking the static library, you need to add the -lfree_mdict -L/usr/local/lib/free_mdict options.