#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <unordered_set>

#include <easyx.h>

#include <MIDIFile/MidiFile.h>
using namespace smf;

#include "Key.h"

#ifndef NOTE_H
#define NOTE_H

enum class NOTEType
{
    NOTE_ON = 1,
    NOTE_OFF = 0
};
class NOTE
{
public:
    int pitch = 0;
    float time = 0.0f;
    int color = 0; // HSL:h
    NOTEType type = NOTEType::NOTE_ON;

    NOTE() : pitch(0), time(0.0f), type(NOTEType::NOTE_ON), color(0) {}
    NOTE(int pitch, float time, NOTEType type, int color) : pitch(pitch), time(time), type(type), color(color) {}
};

class NOTEL
{
public:
    int pitch = 0;
    float stime = 0.0f;
    float etime = 0.0f;
    int color = 0;

    NOTEL() : pitch(0), stime(0.0f), etime(0.0f) {}
    NOTEL(int pitch, float stime, float etime, int color) : pitch(pitch), stime(stime), etime(etime), color(color) {}
};

bool operator==(const NOTE& a, const NOTE& b)
{
    return a.pitch == b.pitch && a.time == b.time && a.type == b.type;
}

void addNote(std::vector<NOTE>& notes, NOTE note)
{
    // 遍历notes向量中的每一个元素
    for (int i = 0; i < (int)notes.size(); i++)
    {
        // 如果note的时间小于notes向量中第i个元素的时间
        if (note.time < notes[i].time)
        {
            // 将note插入到notes向量中的第i个位置
            notes.insert(notes.begin() + i, note);
            // 返回
            return;
        }
    }
    // 如果note的时间大于notes向量中所有元素的时间，则将note添加到notes向量的末尾
    notes.push_back(note);
}

int pitchCorrection(int pitch)
{
    if (pitch < 36)
        return pitch % 12 + 36;
    if (pitch > 71)
        return pitch % 12 + 60;
    return pitch;
}

void removeDuplicates(std::vector<NOTE>& vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
        for (size_t j = i + 1; j < vec.size(); ++j)
            if (vec[i] == vec[j])
            {
                vec.erase(vec.begin() + j);
                --j;
            }
}

std::vector<NOTE> parseMidiFile(const std::string& filename)
{
    std::vector<NOTE> notes;
    smf::MidiFile midifile;
    midifile.read(filename);
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();
    for (int track = 0; track < midifile.getTrackCount(); track++)
        for (int event = 0; event < midifile.getEventCount(track); event++)
            if (midifile.getEvent(track, event).isNoteOn())
            {
                NOTE note;
                note.pitch = midifile.getEvent(track, event)[1];
                note.time = midifile[track][event].seconds;
                note.type = NOTEType::NOTE_ON;
                addNote(notes, note);
            }
            else if (midifile.getEvent(track, event).isNoteOff())
            {
                NOTE note;
                note.pitch = midifile.getEvent(track, event)[1];
                note.time = midifile[track][event].seconds;
                note.type = NOTEType::NOTE_OFF;
                addNote(notes, note);
            }
    removeDuplicates(notes);
    return notes;
}

std::vector<NOTE> parsePlayMidiFile(const std::string& filename)
{
    std::vector<NOTE> notes;
    smf::MidiFile midifile;
    midifile.read(filename);
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();
    for (int track = 0; track < midifile.getTrackCount(); track++)
        for (int event = 0; event < midifile.getEventCount(track); event++)
            if (midifile.getEvent(track, event).isNoteOn())
            {
                NOTE note;
                note.pitch = pitchCorrection(midifile.getEvent(track, event)[1]);
                note.time = midifile[track][event].seconds;
                note.type = NOTEType::NOTE_ON;
                note.color = track * 30;
                addNote(notes, note);
            }
            else if (midifile.getEvent(track, event).isNoteOff())
            {
                NOTE note;
                note.pitch = pitchCorrection(midifile.getEvent(track, event)[1]);
                note.time = midifile[track][event].seconds;
                note.type = NOTEType::NOTE_OFF;
                note.color = track * 30;
                addNote(notes, note);
            }
    return notes;
}

int getNotesStartIndex(std::vector<NOTE> notes, float ts)
{
    for (int i = 0; i < notes.size(); i++)
        if (notes[i].time >= ts)
            return i;
    return -1;
}
int getNotesEndIndex(std::vector<NOTE> notes, float te)
{
    for (int i = notes.size() - 1; i >= 0; i--)
        if (notes[i].time <= te)
            return i;
    return -1;
}

std::vector<NOTEL> linkNotes(std::vector<NOTE> notes, float ts = -1, float te = -1)
{
    float noteUD[36];
    for (int index = 0; index < 36; index++)
        noteUD[index] = -1;
    std::vector<NOTEL> notel;
    int rangeS = ts == -1 || te == -1 ? 0 : getNotesStartIndex(notes, ts - 2);
    int rangeE = ts == -1 || te == -1 ? notes.size() : getNotesEndIndex(notes, te + 2);
    for (int i = rangeS; i < rangeE; i++)
    {
        if (notes[i].type == NOTEType::NOTE_ON)
        {
            if (noteUD[pitchCorrection(notes[i].pitch) - 36] == -1)
                noteUD[pitchCorrection(notes[i].pitch) - 36] = notes[i].time;
            else
            {
                NOTEL note;
                note.pitch = notes[i].pitch;
                note.stime = noteUD[pitchCorrection(notes[i].pitch) - 36];
                note.etime = notes[i].time;
                note.color = notes[i].color;
                notel.push_back(note);
                noteUD[pitchCorrection(notes[i].pitch) - 36] = notes[i].time;
            }
        }
        else if (notes[i].type == NOTEType::NOTE_OFF && noteUD[pitchCorrection(notes[i].pitch) - 36] != -1)
        {
            NOTEL note;
            note.pitch = notes[i].pitch;
            note.stime = noteUD[pitchCorrection(notes[i].pitch) - 36];
            note.etime = notes[i].time;
            note.color = notes[i].color;
            notel.push_back(note);
            noteUD[pitchCorrection(notes[i].pitch) - 36] = -1;
        }
    }
    return notel;
}

std::vector<NOTE> timeNotes(std::vector<NOTEL> notel)
{
    std::vector<NOTE> notes;
    for (int i = 0; i < notel.size(); i++)
    {
        NOTE note1;
        NOTE note2;
        note1.pitch = notel[i].pitch;
        note1.time = notel[i].stime;
        note1.type = NOTEType::NOTE_ON;
        note2.pitch = notel[i].pitch;
        note2.time = notel[i].etime;
        note2.type = NOTEType::NOTE_OFF;
        addNote(notes, note1);
        addNote(notes, note2);
    }
    return notes;
}

int pitchToKey(int pitch)
{
    return keyList[pitch - 36];
}

std::pair<int, int> getNoteTime(std::vector<NOTE> notes, float ts, float te)
{
    int s, e;
    for (int i = 0; i < notes.size(); i++)
        if (notes[i].time >= ts)
        {
            s = i;
            break;
        }
    for (int i = notes.size() - 1; i >= 0; i--)
        if (notes[i].time <= te)
        {
            e = i;
            break;
        }
    return std::make_pair(s, e);
}

std::vector<NOTE> getVectorRange(const std::vector<NOTE>& notes, float s, float e)
{
    int si = 0, ei = 0;
    for (int i = 0; i < notes.size(); i++)
    {
        if (notes[i].time >= s)
        {
            si = i;
            break;
        }
    }
    for (int i = notes.size() - 1; i >= 0; i--)
    {
        if (notes[i].time <= e)
        {
            ei = i;
            break;
        }
    }
    return std::vector<NOTE>(notes.begin() + si, notes.begin() + ei + 1);
}

int getScreenX(int p)
{
    return (p - 36) * 10;
}

int getScreenY(float t, float mt)
{
    return (200 - t * 100 + mt * 100) > 200 ? 200 : (200 - t * 100 + mt * 100);
}

int _getScreenY(float t, float mt)
{
    return (200 - t * 100 + mt * 100);
}

bool whiteKey[12] = { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 };
bool downKey[36] = { 0 };
int KeyColor[36] = { 0 };
int whiteX[12] = { 0, -1, 15, -1, 35, 50, -1, 65, -1, 85, -1, 105 };
int whiteW[12] = { 15, -1, 20, -1, 15, 15, -1, 20, -1, 20, -1, 15 };

void waitForKeyPress()
{
    std::cout << "Please press the 'A' for start play the MIDI file." << std::endl;

    while (true)
    {
        if (GetAsyncKeyState('A') & 0x8000)
        {
            std::cout << "Start playing the MIDI file." << std::endl;
            break;
        }
    }
}

int getKeyX(int w, int k)
{
    return whiteX[k % 12] + w * 120;
}

int getKeyW(int k)
{
    return whiteW[k % 12];
}

void drawKeyboard()
{
    for (int w = 0; w < 3; w++)
        for (int k = 0; k < 12; k++)
        {
            float s = whiteKey[k % 12] ? 0.5 : 0.25;
            COLORREF color = HSLtoRGB(KeyColor[k + w * 12], 0.5, s);
            if (whiteKey[k % 12] && downKey[k + w * 12])
                setfillcolor(color);
            else if (whiteKey[k % 12])
                setfillcolor(0xFFFFFF);
            else if (downKey[k + w * 12])
                setfillcolor(color);
            else
                setfillcolor(0x000000);
            fillrectangle(w * 120 + k * 10, 200, w * 120 + k * 10 + 10, 235);
            setlinecolor(0x000000);
            line(w * 120 + k * 10, 200, w * 120 + k * 10, 235);
            line(w * 120 + k * 10 + 10, 200, w * 120 + k * 10 + 10, 235);
            line(w * 120 + k * 10, 200, w * 120 + k * 10 + 10, 200);
            line(w * 120 + k * 10, 235, w * 120 + k * 10 + 10, 235);

            if (whiteKey[k % 12])
            {
                if (downKey[k + w * 12])
                    setfillcolor(color);
                else
                    setfillcolor(0xFFFFFF);
                solidrectangle(getKeyX(w, k), 235, getKeyX(w, k) + getKeyW(k), 270);
                setlinecolor(0x000000);
                line(getKeyX(w, k), 235, getKeyX(w, k), 270);
                line(getKeyX(w, k) + getKeyW(k), 235, getKeyX(w, k) + getKeyW(k), 270);
            }
        }
}

void drawNotes(std::vector<NOTEL> notel, std::chrono::milliseconds duration)
{
    int i;
    for (i = 0; i < notel.size(); i++)
    {
        if (getScreenY(notel[i].etime, duration.count() / 1000.0f) < 200)
            break;
    }
    for (int j = i; j < notel.size(); j++)
    {
        if (getScreenY(notel[j].etime, duration.count() / 1000.0f) > 200 || getScreenY(notel[j].stime, duration.count() / 1000.0f) < 0)
        {
            continue;
        }
        bool isDown = _getScreenY(notel[j].stime, duration.count() / 1000.0f) > 200;
        float s;
        if (whiteKey[notel[j].pitch % 12] && !isDown)
            s = 0.25;
        else if (whiteKey[notel[j].pitch % 12])
            s = 0.5;
        else if (!isDown)
            s = 0.1;
        else
            s = 0.4;
        COLORREF color = HSLtoRGB(notel[j].color, 0.5, s);
        setfillcolor(color);
        int sy = getScreenY(notel[j].stime, duration.count() / 1000.0f);
        int ey = getScreenY(notel[j].etime, duration.count() / 1000.0f);
        if (ey > 200)
            ey = 200;
        if (sy < 36)
            sy = 36;
        if (ey < 36)
            ey = 36;
        if (sy > 200)
            sy = 200;
        fillrectangle(getScreenX(notel[j].pitch), ey, getScreenX(notel[j].pitch) + 10, sy);
        setlinecolor(0x000000);
    }
}

void drawFileNotes(std::vector<NOTEL> notel, int track, int noteNum, std::string& filename, bool isPlaying = false)
{
    if (!isPlaying)
        initgraph(360, 200, 1);
    int h = isPlaying ? 1 : 2;
    setlinestyle(PS_SOLID, 1);

    float eTime;
    if (notel.size() == 0)
        eTime = 1;
    else
        eTime = notel[notel.size() - 1].etime;
    for (int i = 0; i < notel.size(); i++)
    {
        COLORREF color = HSLtoRGB(notel[i].color, 0.5, 0.5);
        setlinecolor(color);
        line(notel[i].stime * 360 / eTime, (72 - notel[i].pitch) * h, notel[i].etime * 360 / eTime, (72 - notel[i].pitch) * h);
    }
    if (!isPlaying)
    {
        settextstyle(20, 0, _T("Consolas"));
        outtextxy(0, 100, (LPCTSTR)("Name: " + filename).c_str());
        outtextxy(0, 120, (LPCTSTR)("Track: " + std::to_string(track)).c_str());
        outtextxy(0, 140, (LPCTSTR)("Note Num: " + std::to_string(noteNum)).c_str());
        outtextxy(0, 160, (LPCTSTR)("Time: " + std::to_string(eTime) + " s").c_str());
        outtextxy(0, 180, (LPCTSTR)(L"Please input a to play this file"));
    }
}

void PlayNotes(std::vector<NOTE> notes, bool Ndraw)
{

    for (int i = 0; i < 36; i++) {
        downKey[i] = 0;
        KeyColor[i] = 0;
    }

    if (!Ndraw) {
        initgraph(360, 270, 1);
        setlinecolor(0x000000);
        settextstyle(20, 0, _T("Consolas"));
        setbkmode(1);
    }

    std::vector<NOTEL> notel = linkNotes(notes);
    int i = 0;

    HWND hwnd = GetForegroundWindow();

    auto start = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1000);

    bool isPlaying = false;
    bool spacePressed = false;

    std::string filename = "notes.txt";

    if (!Ndraw)
        drawFileNotes(notel, 0, 0, filename, true);

    ExMessage msg;
    int oldx = 0, oldy = 0;

    while (i < notes.size())
    {
        if (!Ndraw) {
            BeginBatchDraw();
            peekmessage(&msg);
            if (msg.message == WM_MOUSEMOVE)
            {
                oldx = msg.x;
                oldy = msg.y;
            }
            else
            {
                msg.x = oldx;
                msg.y = oldy;
            }

            if (msg.message == WM_LBUTTONDOWN)
            {
                if (msg.y < 40 && isPlaying)
                {
                    float dur = msg.x / 360.0f * notes[notes.size() - 1].time;
                    start = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds((int)(dur * 1000));
                    for (int j = 0; j < notes.size(); j++)
                    {
                        if (notes[j].time > dur)
                        {
                            i = j;
                            break;
                        }
                    }
                    for (int j = 0; j < 36; j++)
                    {
                        sendKeyMessageUp(pitchToKey(j + 36), hwnd);
                        KeyColor[j] = 0;
                        downKey[j] = 0;
                    }
                }
                else if (msg.y < 40)
                {
                    isPlaying = false;
                    float dur = msg.x / 360.0f * notes[notes.size() - 1].time;
                    start = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds((int)(dur * 1000));
                    for (int j = 0; j < notes.size(); j++)
                    {
                        if (notes[j].time > dur)
                        {
                            i = j;
                            break;
                        }
                    }
                    for (int j = 0; j < 36; j++)
                    {
                        sendKeyMessageUp(pitchToKey(j + 36), hwnd);
                        KeyColor[j] = 0;
                        downKey[j] = 0;
                    }
                }
            }
        }

        // 时间计算
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

        if (!Ndraw) {
            clearrectangle(0, 36, 360, 270);
            // 渲染键盘
            drawKeyboard();

            // 渲染音符
            drawNotes(notel, duration);

            // 渲染时间
            setlinestyle(PS_SOLID, 1);
            float t = duration.count() / 1000.0f / (float)notes[notes.size() - 1].time * 360.0;
            int c = (1 - fmod(t, 1)) * 255;
            setlinecolor(RGB(c, c, c));
            line(t - 1, 36, t - 1, 40);
            setlinecolor(0xffffff);
            line(t, 36, t, 40);
            setlinecolor(RGB(255 - c, 255 - c, 255 - c));
            line(t + 1, 36, t + 1, 40);

            outtextxy(0, 40, (LPCTSTR)(std::to_string((int)(duration.count() / 1000.0f / (float)notes[notes.size() - 1].time * 100.0f)) + "%").c_str());
            outtextxy(0, 60, (LPCTSTR)(std::to_string((int)(duration.count() / 1000.0f)) + "s/" + std::to_string((int)(notes[notes.size() - 1].time)) + "s").c_str());

            // 处理音符事件
            setlinestyle(PS_SOLID, 1);
        }
        while (i < notes.size() && duration.count() > notes[i].time * 1000)
        {

            if (notes[i].type == NOTEType::NOTE_ON)
            {
                sendKeyMessageDown(pitchToKey(notes[i].pitch), hwnd);
                downKey[notes[i].pitch - 36] = 1;
                KeyColor[notes[i].pitch - 36] = notes[i].color;
            }
            else if (notes[i].type == NOTEType::NOTE_OFF)
            {
                sendKeyMessageUp(pitchToKey(notes[i].pitch), hwnd);
                downKey[notes[i].pitch - 36] = 0;
            }
            if (Ndraw) {
                putchar('\r');
                for (int j = 0; j < 36; j++) {
                    putchar(downKey[j] ? '1' : ' ');
                }
                printf(" %d/%d", i, notes.size());
                printf(" %ds/%ds", (int)(duration.count() / 1000), (int)(notes[notes.size() - 1].time));
            }
            i++;
        }
        if (!Ndraw) {
            EndBatchDraw();
        }
        if (GetAsyncKeyState('F') & 0x8000)
        {
            for (int j = 0; j < 36; j++)
            {
                sendKeyMessageUp(pitchToKey(j + 36), hwnd);
            }
            break;
        }
        if (GetAsyncKeyState(' ') & 0x8000)
        {
            if (!spacePressed)
            {
                spacePressed = true;
                isPlaying = !isPlaying;
            }
        }
        else
            spacePressed = false;
        if (!isPlaying)
        {
            start = std::chrono::high_resolution_clock::now() - duration;
        }
    }
    if (!Ndraw)
        closegraph();
}

#endif // NOTE_H