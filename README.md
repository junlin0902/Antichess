# Project Antichess
## Description
Project Antichess is a Python-based implementation of the antichess variant of chess. The project's core functionality is encapsulated in the antichess.py script, which allows players to engage in antichess matches with different modes of play.

## Installation
No specific installation steps required if Python is already installed. Just clone or download the project.

## Usage
To run the game, navigate to the project directory and execute the following commands in the terminal:

For a default game:
```shell
    python3 antichess.py
```

For player versus environment (PvE) mode, specify either 'white' or 'black':
```shell
    python3 antichess.py white
```
or
```shell
    python3 antichess.py black
```

Note: If more than two arguments are given or if an argument other than "white" or "black" is provided, the program will exit. If "white" is not specified, the player will automatically be assigned to play as black.
