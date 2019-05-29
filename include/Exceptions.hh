#ifndef Exceptions_h
#define Exceptions_h 1 

#include <iostream>
#include <exception>

struct InvalidFlag : public std::exception
{
    const char * what () const throw() {return "Invalid flag";}
};

struct InvalidUnits : public std::exception
{
    const char * what () const throw() {return "Invalid units";}
};

struct InvalidArray : public std::exception
{
    const char * what () const throw() {return "Invalid array";}
};

struct InvalidLinspace : public std::exception
{
    const char * what () const throw() {return "Invalid linspace function";}
};

struct materialAlreadyExists : public std::exception
{
    const char * what () const throw() {return "The material already exists";}
};

struct parameterNotFound : public std::exception
{
    const char * what () const throw() {return "unable to find parameter";}
};

struct parameterIsNegative : public std::exception
{
    const char * what () const throw() {return "parameter cannot be negative";}
};

#endif
