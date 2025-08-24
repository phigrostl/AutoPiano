# How To Use

## Usage Guide
This project allows you to run commands directly from the command line with various parameters. Below is a guide on how to use these commands and their respective arguments.

### Command Syntax

#### Cmdline
```bash
.\AutoPiano.exe [-f/-file (string)] [-d/-dw]
```

#### Help
```bash
.\AutoPiano.exe -h/--help
```

#### Version
```bash
.\AutoPiano.exe --version
```

#### GUI
```bash
.\AutoPiano.exe
```

### Where

#### `-f, --file`
This name of the labeled arguments following this flag.

#### `-d, --dw`
Don't show the window.
There are notes in the Cmdline.

### Use

#### Play and Pause
Press `Space` to play or pause the MIDI file.

The program is pause at initialization and you need to press `Space` to start playing.

#### jump (Don't use -d/-dw)
When playing, press the `left mouse button` above the window to jump to the current position of the mouse pointer.