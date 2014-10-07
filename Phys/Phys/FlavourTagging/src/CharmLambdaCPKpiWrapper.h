#ifndef CHARMLAMBDACPKPIWRAPPER_H 
#define CHARMLAMBDACPKPIWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyLambdaCPKpiSpace {
  class ReadBDT;
  class PurityTable;
}

/** @class CharmLambdaCPKpiWrapper CharmLambdaCPKpiWrapper.h
 *  
 *  Wrapper for D0 -> Kpi classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmLambdaCPKpiWrapper : public TMVAWrapper {
 public: 
  CharmLambdaCPKpiWrapper(std::vector<std::string> &);
  ~CharmLambdaCPKpiWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyLambdaCPKpiSpace::ReadBDT* mcreader;
  MyLambdaCPKpiSpace::PurityTable* purtable;

};
#endif // CHARMLAMBDACPKPIWRAPPER_H
