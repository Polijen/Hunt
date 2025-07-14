Treasure Hunt Game System
A UNIX C project simulating a digital treasure hunt system with multi-process management, file-based state, and inter-process communication via pipes and signals.

Table of Contents
Project Overview

# Features

Directory Structure

Build Instructions

# Usage

Phase 1: File System Operations

Phase 2: Process & Signal Management

Phase 3: Pipes, Redirects & External Integration

Commands

Design Notes

Contributing

License

# Project Overview
This project implements a digital treasure hunt game system in C, designed for UNIX environments. It allows users to create, manage, and participate in treasure hunts, with all data stored in files. The system uses processes and signals for monitoring and interaction, and pipes for inter-process communication.

# Features
File-based treasure and hunt management

Interactive command-line interface

Multi-process architecture with signals

Anonymous pipes for communication

External scoring tool integration

Operation logging and symlink creation

# Directory Structure
.
├── p              # treasure_manager executable
├── o              # monitor executable
├── treasure_hub   # main interactive hub executable
├── score_calc     # external score calculator
├── Hunts/         # directory containing all hunt subdirectories
│   └── <hunt_id>/
│       ├── Treasures.txt
│       └── logged_hunt
├── logged_hunt-<hunt_id> # symlink to each hunt's log file
├── README.md
├── (source .c files)
Build Instructions
Clone the repository and enter the directory.

Compile all components:

gcc -o p paste.txt
gcc -o o monitor.c
gcc -o treasure_hub treasure_hub.c
gcc -o score_calc score_calc.c
Ensure the executables are in your working directory.

# Usage
Phase 1: File System Operations
Manage treasure hunts and treasures using the p program:

Add a treasure:

./p add <hunt_id>
List treasures:

./p list <hunt_id>
View a treasure:

./p view <hunt_id> <treasure_id>
Remove a treasure:

./p remove_treasure <hunt_id> <treasure_id>
Remove a hunt:

./p remove_hunt <hunt_id>
All operations are logged in each hunt's logged_hunt file, and a symlink logged_hunt-<hunt_id> is created in the root.

Phase 2: Process & Signal Management
Use the interactive hub to manage and monitor hunts:

Start the monitor:

./treasure_hub
 start_monitor
List all hunts and treasure counts:

 list_hunts
List treasures in a hunt:

 list_treasures
View a specific treasure:

 view_treasure
Stop the monitor:

 stop_monitor
Exit the program:

 exit
The hub and monitor communicate via signals and pipes; the monitor executes commands and sends results back to the hub.

Phase 3: Pipes, Redirects & External Integration
Monitor output is sent back to the hub via a pipe.

Calculate user scores for each hunt:

 calculate_score
This command launches a separate process for each hunt, running score_calc to sum up treasure values per user.

# Commands
Command	Description
start_monitor	Launch the monitor process
list_hunts	List all hunts and treasure counts
list_treasures	List all treasures for a given hunt
view_treasure	View details of a specific treasure
calculate_score	Calculate and display user scores for each hunt
stop_monitor	Stop the monitor process
exit	Exit the interactive hub
Design Notes
All data is stored in the Hunts/ directory.

Each hunt is a subdirectory containing treasures and logs.

All inter-process communication uses anonymous pipes and signals (via sigaction()).

No temporary files are used for command passing.

External scoring is handled by a separate program (score_calc).

Contributing
Contributions, bug reports, and feature requests are welcome! Please fork the repository and submit a pull request.

License
This project is for educational purposes. Please contact the author for reuse or distribution permissions.
