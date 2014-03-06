#ifndef CHARMD0KMUXWRAPPER_H 
#define CHARMD0KMUXWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyD0KmuXSpace {
  class ReadBDT;
}

/** @class CharmD0KmuXWrapper CharmD0KmuXWrapper.h
 *  
 *  Wrapper for D0 -> KmuX classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmD0KmuXWrapper : public TMVAWrapper {
 public: 
  CharmD0KmuXWrapper(std::vector<std::string> &);
  ~CharmD0KmuXWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyD0KmuXSpace::ReadBDT* mcreader;

};
#endif // CHARMD0KMUXWRAPPER_H
