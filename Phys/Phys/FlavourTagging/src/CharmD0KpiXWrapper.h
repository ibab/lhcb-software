#ifndef CHARMD0KPIXWRAPPER_H 
#define CHARMD0KPIXWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyD0KpiXSpace {
  class ReadBDT;
  class PurityTable;
}

/** @class CharmD0KpiXWrapper CharmD0KpiXWrapper.h
 *  
 *  Wrapper for D0 -> KpiX classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmD0KpiXWrapper : public TMVAWrapper {
 public: 
  CharmD0KpiXWrapper(std::vector<std::string> &);
  ~CharmD0KpiXWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyD0KpiXSpace::ReadBDT* mcreader;
  MyD0KpiXSpace::PurityTable* purtable;
};
#endif // CHARMD0KPIXWRAPPER_H
