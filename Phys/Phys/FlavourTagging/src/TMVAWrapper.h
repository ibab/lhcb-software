#ifndef TMVAWRAPPER_H 
#define TMVAWRAPPER_H 1

// Include files
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

//#define SKIP_TMVA
//#define SKIP_CHARMBDT

/** @class TMVAWrapper TMVAWrapper.h
 *  
 *  A wrapper for the Read<Method> classes in TMVAClassiification
 *  Each charm mode needs its own wrapper; they will inherit from this base class
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class TMVAWrapper {
public: 
  virtual double GetMvaValue(std::vector<double> const &) { return 0; };
  virtual ~TMVAWrapper() { }
};
#endif // TMVAWRAPPER_H
