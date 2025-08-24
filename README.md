# AutoPiano
## Use

#### [How to use the program](https://github.com/phigrostl/AutoPiano/docs/HowToUse.md)

## Build
### 1. **Clone the repository**

```bash
git clone https://github.com/phigrostl/AutoPiano.git
```
```bash
cd AutoPiano
```

### 2. **Install dependencies**

The project relies on the following libraries:
- [EasyX](https://easyx.cn/) (GUI library)
- [MidiFile](https://github.com/craigsapp/midifile) (MIDI file parser)
- [TCLAP](https://github.com/mirror/tclap) (Command line argument parser)

### 3. **Build the project**
#### After installing the dependencies, you can build the project using the following command:
```bash
g++ -std=c++11 -o AutoPiano AutoPiano.cpp -leasyx
```
