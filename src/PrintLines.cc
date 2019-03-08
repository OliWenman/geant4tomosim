#include "PrintLines.hh"

//Used to find size of the window
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

//Used to print to the right stream
#include <iostream>
#include "SettingsLog.hh"

//Used to know what G4cout is
#include "globals.hh"

/*void PrintLines::PrintToEndOfTerminal(std::ostream& stream, char symbol)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    G4cout << "\n";
    for (int i = 0; i < w.ws_col ; i++){stream << symbol;}
    G4cout << G4endl;
}*/

void PrintToEndOfTerminal(SettingsLog stream, char symbol)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    G4cout << "\n";
    for (int i = 0; i < w.ws_col ; i++){stream << symbol;}
    G4cout << G4endl;
}

void PrintToEndOfTerminal(char symbol)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    G4cout << "\n";
    for (int i = 0; i < w.ws_col ; i++){G4cout << symbol;}
    G4cout << G4endl;
}
