/*
Function to print a symbol to the end of the terminal
*/

#ifndef PrintLines_h
#define PrintLines_h 1

class SettingsLog;
class ostream;

//void PrintToEndOfTerminal(std::ostream& stream, char symbol);

void PrintToEndOfTerminal(SettingsLog stream, char symbol);

void PrintToEndOfTerminal(char symbol);
#endif
