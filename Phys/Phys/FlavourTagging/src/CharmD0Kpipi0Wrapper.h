#ifndef CHARMD0KPIPI0WRAPPER_H 
#define CHARMD0KPIPI0WRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyD0Kpipi0Space {
  class ReadBDT;
}

/** @class CharmD0Kpipi0Wrapper CharmD0Kpipi0Wrapper.h
 *  
 *  Wrapper for D0 -> Kpipi0 classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmD0Kpipi0Wrapper : public TMVAWrapper {
 public: 
  CharmD0Kpipi0Wrapper(std::vector<std::string> &);
  ~CharmD0Kpipi0Wrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyD0Kpipi0Space::ReadBDT* mcreader;

};
#endif // CHARMD0KPIPI0WRAPPER_H
