#ifndef SettingsLog_h
#define SettingsLog_h 1

#include <iostream>
#include <fstream>

//Structure to output to two streams at the same time. Used for outputting to terminal and writing to file at the same time
struct SettingsLog
{
    SettingsLog(std::ostream& out1, std::ostream& out2) : out1_(out1), out2_(out2){}
    std::ostream& out1_;
    std::ostream& out2_;
};

template <typename T> SettingsLog& operator << (SettingsLog& h, T const& t)
{
   h.out1_ << t;
   h.out2_ << t;
   return h;
}

inline SettingsLog& operator << (SettingsLog& h, std::ostream&(*f)(std::ostream&))
{
   h.out1_ << f;
   h.out2_ << f;
   return h;
}

#endif