// *********************************************************
//
// StringConvert.h
//
// By J. Blouw, MPI, Heidelberg 2005.
// (Johan.Blouw@mpi-hd.mpg.de)
// For the Beetle chip tests
// 
// A simple c++ convert function, primarily developed
// for use on the powerPC, with gcc-2.95 (which does not
// know about templates...). 
//
// *********************************************************
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#ifndef CONVERT_HH
#define CONVERT_HH

using namespace std;

class BadStringConversion : public runtime_error {
 public:
  BadStringConversion(const string& s) : runtime_error(s.c_str()) {;};
};

inline string stringify(double x) {
#ifdef LynxOS
  ostrstream o;
#else
  ostringstream o;
#endif
  if (!(o << x))
    throw BadStringConversion("stringify(double)");
  return o.str();
}

inline string stringify(int x) {
#ifdef LynxOS
  ostrstream o;
#else
  ostringstream o;
#endif
  if ( !(o << x) )
    throw BadStringConversion("stringify(int)");
  return o.str();
}

inline string stringify( char *c ) {
#ifdef LynxOS
  ostrstream o;
#else
  ostringstream o;
#endif
  if ( ! (o << c) )
    throw BadStringConversion("stringify(char *c)");
  return o.str();
}

inline string stringify( size_t t ) {
#ifdef LynxOS
  ostrstream o;
#else
  ostringstream o;
#endif
  if ( ! (o << t) )
    throw BadStringConversion("stringify(size_t)");
  return o.str();
}


inline string stringify( bool b ) {
#ifdef LynxOS
  ostrstream o;
#else
  ostringstream o;
#endif
  if ( ! (o << b) )
    throw BadStringConversion("stringify(bool)");
  return o.str();
}
#endif
