
#include "NOTE.h"
#include <iostream>
#include <commdlg.h>
#include <tclap/CmdLine.h>

using namespace std;
using namespace smf;

int main(int argc, char* argv[])
{
    string fileName;
    vector<NOTE> notes;

    const char* info = "A program to play midi files \n \
        You nead to press the space to play the MIDI file. \n \
        You can press the space to pause and continue the MIDI file. \n \
        You can press the 'F' key to end the MIDI file. \n \
        You can enter the 'exit' to exit the program. \n \
        If you don't use the -d or --dw option, you can use the arrow keys to control the time of the music. \n \
        ";

    TCLAP::CmdLine cmd(info, ' ', "v0.1.7");
    TCLAP::ValueArg<string> fileNameArg("f", "file", "The name of the file to play", false, "", "string");
    TCLAP::SwitchArg dwArg("d", "dw", "Don't show the window", false);

    cmd.add(dwArg);
    cmd.add(fileNameArg);

    try {
        cmd.parse(argc, argv);
        if (fileNameArg.isSet()) {
            fileName = fileNameArg.getValue();
        }
    }
    catch (TCLAP::ArgException& e) {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    }

    do {
        if (!fileNameArg.isSet()) {
            cout << "Please enter the name of the file to play: ";
            cin >> fileName;
        }
        if (fileName == "exit")
            break;
        smf::MidiFile midifile;
        midifile.read(fileName);
        midifile.doTimeAnalysis();
        midifile.linkNotePairs();
        notes = parsePlayMidiFile(fileName);
        vector<NOTEL> notel = linkNotes(notes);
        int track = midifile.getTrackCount();
        int noteNum = notel.size();

        PlayNotes(notes, dwArg.isSet());

    } while (!dwArg.isSet());

    

    return 0;
}
