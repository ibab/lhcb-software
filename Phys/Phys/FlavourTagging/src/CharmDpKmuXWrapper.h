#ifndef CHARMDPKMUXWRAPPER_H 
#define CHARMDPKMUXWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyDpKmuXSpace {
  class ReadBDT;
}

/** @class CharmDpKmuXWrapper CharmDpKmuXWrapper.h
 *  
 *  Wrapper for Dp -> KmuX classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmDpKmuXWrapper : public TMVAWrapper {
 public: 
  CharmDpKmuXWrapper(std::vector<std::string> &);
  ~CharmDpKmuXWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyDpKmuXSpace::ReadBDT* mcreader;

};
#endif // CHARMDPKMUXWRAPPER_H
