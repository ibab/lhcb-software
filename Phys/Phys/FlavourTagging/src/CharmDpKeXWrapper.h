#ifndef CHARMDPKEXWRAPPER_H 
#define CHARMDPKEXWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyDpKeXSpace {
  class ReadBDT;
}

/** @class CharmDpKeXWrapper CharmDpKeXWrapper.h
 *  
 *  Wrapper for Dp -> KeX classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmDpKeXWrapper : public TMVAWrapper {
 public: 
  CharmDpKeXWrapper(std::vector<std::string> &);
  ~CharmDpKeXWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyDpKeXSpace::ReadBDT* mcreader;

};
#endif // CHARMDPKEXWRAPPER_H
