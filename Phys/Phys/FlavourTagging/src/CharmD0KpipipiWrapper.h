#ifndef CHARMD0KPIPIPIWRAPPER_H 
#define CHARMD0KPIPIPIWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyD0KpipipiSpace {
  class ReadBDT;
}

/** @class CharmD0KpipipiWrapper CharmD0KpipipiWrapper.h
 *  
 *  Wrapper for D0 -> Kpipipi classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmD0KpipipiWrapper : public TMVAWrapper {
 public: 
  CharmD0KpipipiWrapper(std::vector<std::string> &);
  ~CharmD0KpipipiWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyD0KpipipiSpace::ReadBDT* mcreader;

};
#endif // CHARMD0KPIPIPIWRAPPER_H
