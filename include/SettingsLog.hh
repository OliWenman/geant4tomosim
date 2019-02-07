#ifndef SettingsLog_h
#define SettingsLog_h 1

#include <iostream>
#include <fstream>
#include "globals.hh"

//Structure to output to filestream and terminal at the same time
struct SettingsLog
{ 
    //By defualt streaming to terminal and file are on
    SettingsLog(std::ostream& fileStream) : fileStream_(fileStream) {terminalOn = true; fileOn = true;}
    std::ostream& fileStream_;
    bool terminalOn;
    bool fileOn;
};

template <typename T> SettingsLog& operator << (SettingsLog& log, T const& outputStream)
{
    if (log.fileOn){
        log.fileStream_ << outputStream;}
     
    if (log.terminalOn){
        G4cout << outputStream;}
    
    return log;
}

inline SettingsLog& operator << (SettingsLog& log, std::ostream&(*outputStream)(std::ostream&))
{
    if(log.fileOn){
        log.fileStream_ << outputStream;}
   
    if (log.terminalOn){
        G4cout << outputStream;}
        
    return log;
}

#endif
