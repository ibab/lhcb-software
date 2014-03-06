#ifndef CHARMD0KPIWRAPPER_H 
#define CHARMD0KPIWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyD0KpiSpace {
  class ReadBDT;
}

/** @class CharmD0KpiWrapper CharmD0KpiWrapper.h
 *  
 *  Wrapper for D0 -> Kpi classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmD0KpiWrapper : public TMVAWrapper {
 public: 
  CharmD0KpiWrapper(std::vector<std::string> &);
  ~CharmD0KpiWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyD0KpiSpace::ReadBDT* mcreader;

};
#endif // CHARMD0KPIWRAPPER_H
