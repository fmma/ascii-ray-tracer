#include <windows.h>

void ResetCursor() {
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) 
    {
        MessageBox(NULL, TEXT("GetConsoleScreenBufferInfo"), 
            TEXT("Console Error"), MB_OK); 
        return;
    }
	
    csbiInfo.dwCursorPosition.X = 0;
    csbiInfo.dwCursorPosition.Y -= HEIGHT; 

	if (! SetConsoleCursorPosition(hStdout, 
        csbiInfo.dwCursorPosition)) 
    {
        MessageBox(NULL, TEXT("SetConsoleCursorPosition"), 
            TEXT("Console Error"), MB_OK); 
        return;
    }
}
