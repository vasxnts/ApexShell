# ApexShell

ApexShell is a high-performance, lightweight custom Linux shell implemented in C++ from scratch. This project was developed to explore and demonstrate core operating system concepts, such as process management, inter-process communication (IPC) via pipelines, and environment handling.

## Key Features

I've implemented the core features that make a shell actually usable:

- **Process Mastery**: ApexShell handles external programs using `fork()` and `execvp()`, giving you full control over system processes.
- **Smart Pipelines**: You can chain commands together just like in Bash. Want to find something and count it? `ls -la | grep .cpp | wc -l` works like a charm.
- **I/O Redirection**: 
  - Read from files using `<`
  - Write to files using `>` (overwrite) or `>>` (append).
- **Environment Aware**: It understands your system. Try `echo $USER`. You can also set your own variables using the custom `export` command.
- **The Essentials (Built-ins)**: 
  - `cd`: Smooth navigation, including the pro-feature `cd -` to jump back to your previous directory instantly.
  - `pwd`, `echo`, `exit`: The bread and butter of any session.

## How to get it running

### What you'll need:
- A Linux environment (WSL2 works great too).
- `g++` compiler (C++17 support).
- `make` utility.

### Building the project:
Just pull the trigger in your terminal:
```bash
make
