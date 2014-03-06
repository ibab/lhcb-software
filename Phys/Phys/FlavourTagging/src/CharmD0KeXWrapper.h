#ifndef CHARMD0KEXWRAPPER_H 
#define CHARMD0KEXWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyD0KeXSpace {
  class ReadBDT;
}

/** @class CharmD0KeXWrapper CharmD0KeXWrapper.h
 *  
 *  Wrapper for D0 -> KeX classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmD0KeXWrapper : public TMVAWrapper {
public: 
  CharmD0KeXWrapper(std::vector<std::string> &);
  ~CharmD0KeXWrapper();
  double GetMvaValue(std::vector<double> const &);
  
 protected:

 private:
  MyD0KeXSpace::ReadBDT* mcreader;

};
#endif // CHARMD0KEXWRAPPER_H
