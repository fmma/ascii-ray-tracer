#include <windows.h>
#include <stdio.h>

#define N_KEYS (256)
#define MAX_EVENTS (16)

namespace console {

struct console {
    int nKeyEvents;
    KEY_EVENT_RECORD keyEvents[MAX_EVENTS];
    char pressedKeysBitSet[N_KEYS];
    HANDLE hStdin;
    HANDLE hStdout;
    int width;
    int height;
    LPSTR frameBuffer;

    console(int width, int height) : width(width), height(height) {
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        frameBuffer = new char[width * height];
        for(int r = 0; r < height; ++r) {
            for(int c = 0; c < width ; ++c) {
                draw(r, c, ' ');
            }
            draw(r, width, '\n');
        }
        draw(0, 0, '/');
        draw(height-1, 0, '\\');
        draw(0, width-1, '\\');
        draw(height-1, width-1, '/');
        for(int i = 1; i < height-1; ++i) {
            draw(i, 0, '|');
            draw(i, width-1, '|');
        }
        for(int i = 1; i < width-1; ++i) {
            draw(0, i, '=');
            draw(height-1, i, '=');
        }
        for(int i = 0; i < N_KEYS; ++i) {
            pressedKeysBitSet[i] = 0;
        }

        clear();
        resetCursor();
        hideCursor();
    }

    ~console() {
        delete [] frameBuffer;
        showCursor();
        SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        clear();
    }

    void hideCursor() {
        CONSOLE_CURSOR_INFO info;
        GetConsoleCursorInfo(hStdout, &info);
        info.bVisible = FALSE;
        SetConsoleCursorInfo(hStdout, &info);
    }

    void showCursor() {
        CONSOLE_CURSOR_INFO info;
        GetConsoleCursorInfo(hStdout, &info);
        info.bVisible = TRUE;
        SetConsoleCursorInfo(hStdout, &info);
    }
    
    void draw(int r, int c, char val) {
        frameBuffer[r * (width + 1) + c] = val;
    }

    void drawString(int r, int c, const char *val) {
        int n = strlen(val);
        for(int i = 0; i < n; ++i) {
            frameBuffer[r * (width + 1) + c + i] = val[i];
        }
    }

    void drawInt(int r, int c, int val) {
        char buf[50];
        sprintf(buf, "%d", val);
        drawString(r, c, buf);
    }

    void fillRect(int r, int c, int w, int h, char val) {
        for(int r0 = r; r0 < r + w; ++r0) {
            for(int c0 = c; c0 < c + h; ++c0) {
                draw(r0, c0, val);
            }
        }
    }

    void resetCursor() {
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
        {
            MessageBox(NULL, TEXT("GetConsoleScreenBufferInfo"),
                TEXT("Console Error"), MB_OK);
            return;
        }
        
        csbiInfo.dwCursorPosition.X = 0;
        if(csbiInfo.dwCursorPosition.Y >= height)
            csbiInfo.dwCursorPosition.Y -= height;
        else
            csbiInfo.dwCursorPosition.Y = 0;

        if (! SetConsoleCursorPosition(hStdout, csbiInfo.dwCursorPosition)) 
        {
            MessageBox(NULL, TEXT("SetConsoleCursorPosition"),
                TEXT("Console Error"), MB_OK);
            return;
        }
    }
    
    void repaint() {
        COORD c;
        c.X = 0;
        c.Y = 0;
        DWORD out;
        WriteConsoleOutputCharacter(hStdout, frameBuffer, (width + 1) * height, c, &out);
        // resetCursor();
        // printf("%s", frameBuffer);
    }
    
    void clear() {
        system("clear");
    }
    
    void handleKeys() {
        DWORD nEvents = 0;
        DWORD nEventsRead = 0;
        GetNumberOfConsoleInputEvents(hStdin, &nEvents);
        if(nEvents > MAX_EVENTS) {
            nEvents = MAX_EVENTS;
        }
        if(nEvents > 0){
            INPUT_RECORD eventBuffer[nEvents];
            ReadConsoleInput(hStdin, eventBuffer, nEvents, &nEventsRead);
            nKeyEvents = nEventsRead;
            for(DWORD i = 0; i < nEventsRead; ++i){
                keyEvents[i] = eventBuffer[i].Event.KeyEvent;
                WORD code = keyEvents[i].wVirtualKeyCode;
                pressedKeysBitSet[code] = keyEvents[i].bKeyDown;
            }
        }
    }
};

}