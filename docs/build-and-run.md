# Build and Run Guide

## 1. Overview

This repository contains 14 distinct C++ programs across Chapters 01–05.

Each program has its own `main()` function and is intended to be compiled and executed independently.

The repository does not currently use a central CMake build configuration. Each source file can be compiled directly using GNU G++ with the C++17 standard.

This guide covers three development environments:

1. Native Ubuntu Linux.
2. Native Windows using Visual Studio Code and a Windows C++ compiler.
3. Windows using Visual Studio Code with the Windows Subsystem for Linux (WSL).

The examples use the terminal for compilation and execution. Visual Studio Code can be used as the editor and terminal environment,
but it does not provide a C++ compiler by itself.

## 2. Project Requirements and Assumptions

### 2.1 Software requirements

| Requirement | Purpose |
|---|---|
| C++17-compatible compiler | Compile the source programs |
| Visual Studio Code (optional) | Source-code editing and integrated terminal |
| C/C++ extension for VS Code (optional) | IntelliSense, code navigation, and debugging support |
| Terminal | Run compilation and execution commands |

The project does not require:

- A database.
- External libraries.
- A network connection during program execution.
- A Python installation.
- CMake or another build system.

The source programs use standard C++ facilities and are intended to be compiled independently.

### 2.2 Compiler options

The recommended compilation options are:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude source.cpp -o program
```

| Option | Purpose |
|---|---|
| `-std=c++17` | Compile using the C++17 language standard |
| `-Wall` | Enable commonly useful compiler warnings |
| `-Wextra` | Enable additional warnings |
| `-pedantic` | Request diagnostics for non-standard language extensions |
| `-Iinclude` | Add the repository's `include/` directory to the header search path |
| `-o program` | Specify the output executable name |

These options are used consistently throughout the guide.

## 3. Repository Structure

The source programs are organized as follows:

```text
include/
└── project_banner.hpp

chapters/
├── 01-reactive-agent/
│   └── main.cpp
│
├── 02-environment-simulation/
│   └── main.cpp
│
├── 03-bfs-open-loop-agent/
│   └── main.cpp
│
├── 04-closed-loop-agent/
│   ├── main.cpp
│   └── experiments/
│       ├── already-at-goal/
│       │   └── main.cpp
│       ├── closed-loop-recovery/
│       │   └── main.cpp
│       ├── fault-injection-replanning/
│       │   └── main.cpp
│       ├── mismatch-handling/
│       │   └── main.cpp
│       └── planner-failure/
│           └── main.cpp
│
└── 05-dijkstra/
    ├── main.cpp
    └── experiments/
        ├── stale-priority-queue/
        │   └── stale_priority_queue_experiment.cpp
        ├── standalone-new-grid/
        │   └── dijkstra_standalone_run_new_grid.cpp
        ├── unreachable-goal/
        │   └── unreachable_goal_experiment.cpp
        └── zero-cost-cycle/
            └── zero_cost_experiment.cpp
```

Each source file is an independent program. Do not compile all source files together into one executable because they contain separate entry points.

### Shared project header

The repository provides a shared header at `include/project_banner.hpp`. It defines `printProjectBanner()`, which displays the project
identification and collaboration attribution when a program starts.

All 14 independent programs include this header and call `printProjectBanner()` at the beginning of `main()`. Because the header is stored
outside the chapter directories, compilation commands run from the repository root must include `-Iinclude`.
This option adds the repository's `include/` directory to the compiler's header search path.

## 4. Environment A — Native Ubuntu Linux

These instructions apply to a normal Ubuntu installation, whether Ubuntu is installed directly on a computer or used inside a virtual machine.

### 4.1 Install G++

Open the Ubuntu terminal:

```bash
sudo apt update
sudo apt install build-essential
```

This installs G++ and the standard development tools required for compiling C++ programs.

Verify the installation:

```bash
g++ --version
```

### 4.2 Open the repository

Navigate to the directory containing the repository:

```bash
cd /path/to/cpp-agent-systems-lab
```

Replace the example path with the actual location of your cloned repository.

Confirm that you are at the repository root:

```bash
pwd
ls
```

The `chapters` directory should be visible.

### 4.3 Compile and run a chapter

For example, Chapter 01:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/01-reactive-agent/main.cpp -o /tmp/ch01-reactive-agent
/tmp/ch01-reactive-agent
```

The executable is placed under `/tmp` so that generated binaries do not clutter the repository.

The same process applies to the other chapter implementations and experiments.

## 5. Environment B — Windows with Visual Studio Code

These instructions apply to a normal Windows installation without requiring WSL.

### 5.1 Install Visual Studio Code

Download and install Visual Studio Code:

https://code.visualstudio.com/

### 5.2 Install a Windows C++ compiler

Visual Studio Code is an editor and development environment. It does not include a C++ compiler by default.

One option is to use **MSYS2 with MinGW-w64 GCC**.

Download MSYS2:

https://www.msys2.org/

Install MSYS2 using its installation instructions.

Open the **MSYS2 UCRT64** terminal and install the compiler and development tools:

```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-gcc
```

This installs the GCC toolchain for the UCRT64 environment.

### 5.3 Add the compiler to the Windows PATH

The default UCRT64 compiler directory is:

```text
C:\msys64\ucrt64\bin
```

Add this directory to the Windows user or system `Path` environment variable.

After updating PATH, close and reopen PowerShell or the VS Code terminal.

Verify the installation:

```powershell
g++ --version
```

If Windows cannot find `g++`, check the PATH configuration and confirm that the compiler exists in the directory above.

### 5.4 Install the C/C++ extension

In Visual Studio Code:

1. Open the Extensions panel.
2. Search for `C/C++`.
3. Install the Microsoft C/C++ extension.

The extension provides IntelliSense, code navigation, and debugging support. It does not replace the compiler installation.

### 5.5 Open the repository in VS Code

Clone the repository using Git or open an existing local copy.

For example, from PowerShell:

```powershell
git clone git@github.com:Ayush-Vats0/cpp-agent-systems-lab.git
cd cpp-agent-systems-lab
code .
```

Use the repository's actual GitHub URL if its name or remote URL has not yet been changed.

Alternatively, use **File → Open Folder** in VS Code.

### 5.6 Compile and run using the integrated terminal

Open the integrated terminal:

`Terminal → New Terminal`

Select PowerShell if necessary.

The following example compiles and runs Chapter 01:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/01-reactive-agent/main.cpp -o "$env:TEMP\ch01-reactive-agent.exe"

& "$env:TEMP\ch01-reactive-agent.exe"
```

The executable is stored in the Windows temporary directory rather than in the repository.

### 5.7 PowerShell helper for compiling and running programs

To make repeated builds easier, define this function in the PowerShell terminal:

```powershell
function Build-Run {
    param(
        [string]$Source,
        [string]$Name,
        [int]$ExpectedExitCode = 0
    )

    $Output = Join-Path $env:TEMP "$Name.exe"

    g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude $Source -o $Output

    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed: $Source"
    }

    & $Output
    $ActualExitCode = $LASTEXITCODE

    if ($ActualExitCode -ne $ExpectedExitCode) {
        Write-Error "Unexpected exit code. Expected $ExpectedExitCode, received $ActualExitCode."
    }
}
```

Example:

```powershell
Build-Run "chapters/01-reactive-agent/main.cpp" "ch01-reactive-agent"
```

This function:

1. Compiles the specified source file.
2. Places the executable in the Windows temporary directory.
3. Runs the executable.
4. Checks its exit status against the expected value.

The function exists only in the current PowerShell session. It can be defined again whenever a new terminal session is opened.

## 6. Environment C — Windows with VS Code and WSL

This option uses Ubuntu inside WSL while keeping Visual Studio Code on Windows.

It is particularly useful when working with Linux-oriented development workflows.

### 6.1 Install WSL and Ubuntu

If WSL is not already installed, open PowerShell as Administrator:

```powershell
wsl --install
```

Follow the Windows prompts and complete the Ubuntu initial setup.

For additional information:

https://learn.microsoft.com/windows/wsl/

### 6.2 Install the Linux development tools

Open the Ubuntu WSL terminal:

```bash
sudo apt update
sudo apt install build-essential
```

Verify G++:

```bash
g++ --version
```

### 6.3 Install the VS Code WSL extension

In Windows Visual Studio Code:

1. Open Extensions.
2. Search for `WSL`.
3. Install the Microsoft WSL extension.

### 6.4 Open the repository inside WSL

Navigate to the repository from the Ubuntu terminal.

For example, if the repository is located on the Windows C: drive:

```bash
cd /mnt/c/Users/YourWindowsUser/Dev/cpp-agent-systems-lab
```

Replace the example path with the actual location.

Open the directory in VS Code:

```bash
code .
```

VS Code will open the project using its WSL development environment.

### 6.5 Compile and run

Use the same Linux compilation commands described in Section 4.

For example:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/05-dijkstra/main.cpp -o /tmp/ch05-dijkstra

/tmp/ch05-dijkstra
```

In this arrangement:

- VS Code runs on Windows.
- The compiler runs inside Ubuntu WSL.
- The executable is a Linux binary.
- The program is executed inside WSL.

A Linux executable produced inside WSL should not be treated as a native Windows `.exe`.

## 7. Compile and Run Individual Chapters

The following commands are common to native Ubuntu and WSL Ubuntu.

Run them from the repository root.

### Chapter 01 — Reactive Agent

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/01-reactive-agent/main.cpp -o /tmp/ch01-reactive-agent
/tmp/ch01-reactive-agent
```

### Chapter 02 — Environment Simulation

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/02-environment-simulation/main.cpp -o /tmp/ch02-environment-simulation
/tmp/ch02-environment-simulation
```

### Chapter 03 — BFS Open-Loop Agent

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/03-bfs-open-loop-agent/main.cpp -o /tmp/ch03-bfs-open-loop-agent
/tmp/ch03-bfs-open-loop-agent
```

### Chapter 04 — Closed-Loop Agent

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/04-closed-loop-agent/main.cpp -o /tmp/ch04-closed-loop-agent
/tmp/ch04-closed-loop-agent
```

### Chapter 05 — Dijkstra

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/05-dijkstra/main.cpp -o /tmp/ch05-dijkstra
/tmp/ch05-dijkstra
```

## 8. Compile and Run Chapter 04 Experiments

Each experiment is an independent program.

### 8.1 Already at Goal

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/04-closed-loop-agent/experiments/already-at-goal/main.cpp -o /tmp/ch04-already-at-goal
/tmp/ch04-already-at-goal
```

### 8.2 Closed-Loop Recovery

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/04-closed-loop-agent/experiments/closed-loop-recovery/main.cpp -o /tmp/ch04-closed-loop-recovery
/tmp/ch04-closed-loop-recovery
```

### 8.3 Fault Injection and Replanning

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/04-closed-loop-agent/experiments/fault-injection-replanning/main.cpp -o /tmp/ch04-fault-injection-replanning
/tmp/ch04-fault-injection-replanning
```

### 8.4 Mismatch Handling

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/04-closed-loop-agent/experiments/mismatch-handling/main.cpp -o /tmp/ch04-mismatch-handling
/tmp/ch04-mismatch-handling
```

### 8.5 Planner Failure

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/04-closed-loop-agent/experiments/planner-failure/main.cpp -o /tmp/ch04-planner-failure
/tmp/ch04-planner-failure
```

## 9. Compile and Run Chapter 05 Experiments

### 9.1 Stale Priority Queue Experiment

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/05-dijkstra/experiments/stale-priority-queue/stale_priority_queue_experiment.cpp -o /tmp/ch05-stale-priority-queue
/tmp/ch05-stale-priority-queue
```

### 9.2 Standalone Dijkstra Experiment

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/05-dijkstra/experiments/standalone-new-grid/dijkstra_standalone_run_new_grid.cpp -o /tmp/ch05-standalone-dijkstra
/tmp/ch05-standalone-dijkstra
```

### 9.3 Unreachable Goal Experiment

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/05-dijkstra/experiments/unreachable-goal/unreachable_goal_experiment.cpp -o /tmp/ch05-unreachable-goal
/tmp/ch05-unreachable-goal
```

**Expected behavior:** This experiment deliberately constructs an unreachable goal.

Dijkstra exhausts its priority queue, reports that the goal cannot be reached, and returns an empty path. The program intentionally exits with status `1`.

This non-zero exit status is expected for this experiment. It should not be interpreted as a compilation failure or an unexpected crash.

### 9.4 Zero-Cost Cycle Experiment

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/05-dijkstra/experiments/zero-cost-cycle/zero_cost_experiment.cpp -o /tmp/ch05-zero-cost-cycle
/tmp/ch05-zero-cost-cycle
```

## 10. Running Chapter 05 Experiments on Native Windows

The commands in Sections 7–9 use Linux executable paths and `/tmp`.

For native Windows, use the PowerShell `Build-Run` function from Section 5.7.

Examples:

```powershell
Build-Run "chapters/05-dijkstra/main.cpp" "ch05-dijkstra"

Build-Run "chapters/05-dijkstra/experiments/stale-priority-queue/stale_priority_queue_experiment.cpp" "ch05-stale-priority-queue"

Build-Run "chapters/05-dijkstra/experiments/standalone-new-grid/dijkstra_standalone_run_new_grid.cpp" "ch05-standalone-dijkstra"

Build-Run "chapters/05-dijkstra/experiments/unreachable-goal/unreachable_goal_experiment.cpp" "ch05-unreachable-goal" 1

Build-Run "chapters/05-dijkstra/experiments/zero-cost-cycle/zero_cost_experiment.cpp" "ch05-zero-cost-cycle"
```

The unreachable-goal experiment is supplied with an expected exit code of `1`.

The same function can be used for the Chapter 01–04 programs by supplying their respective source paths and executable names.

## 11. Executable Output Locations

The examples use different output locations depending on the environment.

| Environment | Output location |
|---|---|
| Native Ubuntu | `/tmp` |
| WSL Ubuntu | `/tmp` inside the WSL distribution |
| Native Windows | Windows temporary directory, referenced by `$env:TEMP` |

This keeps generated executables separate from source files and reduces the risk of accidentally committing binaries to Git.

The repository's `.gitignore` also contains a rule for the executable name `agent`.

## 12. Compilation and Runtime Troubleshooting

### 12.1 Compiler not found

On Ubuntu or WSL, if the terminal reports:

```text
g++: command not found
```

Install the development tools:

```bash
sudo apt update
sudo apt install build-essential
```

On Windows, if PowerShell reports that `g++` is not recognized, check that:

- MSYS2 and its UCRT64 GCC package are installed.
- `C:\msys64\ucrt64\bin` is present in PATH.
- The terminal was reopened after modifying PATH.

### 12.2 Source file not found

If compilation reports that a source file cannot be found:

1. Confirm that the terminal is at the repository root.
2. Check the directory structure.
3. Verify the source filename and its capitalization.

On Ubuntu or WSL:

```bash
pwd
find chapters -type f -name "*.cpp"
```

On PowerShell:

```powershell
Get-Location
Get-ChildItem chapters -Recurse -Filter *.cpp
```

Linux paths are case-sensitive.

### 12.3 Compilation warnings

The recommended compiler options enable warnings.

Review warnings rather than automatically ignoring them. A warning does not always indicate a runtime defect, but it can reveal issues worth investigating.

### 12.4 Non-zero exit status

A non-zero exit status is not automatically evidence of a defect.

The unreachable-goal experiment intentionally exits with status `1` after reporting that no route exists.

Other programs should be assessed against their documented behavior and runtime output.

### 12.5 VS Code cannot find the compiler

Check the compiler from the integrated terminal first:

```bash
g++ --version
```

or, in PowerShell:

```powershell
g++ --version
```

If the command fails in the terminal, configure the compiler installation and PATH before troubleshooting VS Code IntelliSense.

The C/C++ extension's IntelliSense configuration and the actual compiler installation are separate concerns.

## 13. Verification Notes

The 14 distinct source programs, including the shared project banner integration, were compiled individually using:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude
```

The verification results were:

| Verification item | Result |
|---|---|
| Distinct source programs | 14 |
| Successful compilations | 14 |
| Successful exit-0 runs | 13 |
| Expected non-zero run | 1 |
| Unexpected runtime failures | 0 |
| Tracked executable binaries | None |

The unreachable-goal experiment accounts for the expected non-zero exit status.

These results describe the verification performed during repository preparation. They do not imply that every possible input, environment configuration,
compiler implementation, or future modification has been tested.

The documented Windows setup provides an additional supported build route; the verification counts above should not be interpreted as
evidence that all 14 programs have already been tested on every operating system.

### Platform Verification Limitation

The source programs were compiled and executed successfully in a Linux environment using GNU G++ with C++17.

**Native Windows compilation and execution have not been tested or verified.**

The Windows instructions provided in this guide describe a proposed build setup using MSYS2 UCRT64 GCC and Visual Studio Code.
They are provided as guidance, not as evidence of successful Windows testing.

Consequently:

- Linux (Ubuntu/WSL): Source compilation and runtime behavior verified.
- Native Windows: Build instructions provided, but compilation and runtime behavior remain unverified.
- Windows-specific compiler or runtime differences have not been evaluated.

The repository should not be described as cross-platform verified until the programs have been independently compiled and tested on native Windows.

## 14. Conclusion

The repository uses a straightforward compilation model:

- Each chapter implementation is compiled independently.
- Each experiment is compiled independently.
- C++17 is used throughout.
- Compiler warnings are enabled.
- Ubuntu and WSL use GNU G++.
- Native Windows can use MSYS2 MinGW-w64 GCC.
- VS Code can be used as the editor and integrated terminal.
- Build outputs are kept outside the repository.
- Expected failure behavior is distinguished from unexpected runtime failure.

This approach keeps the individual implementations easy to inspect, compile, and reproduce without introducing a build-system dependency that the current
project does not require.
