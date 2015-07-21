#ifndef KKPIPI_EFFICIENCY_HH
#define KKPIPI_EFFICIENCY_HH

#include <string>
#include "Mint/IGetDalitzEvent.h"
#include "Mint/DalitzEventAccess.h"

class TH1F;
class TFile;
class IDalitzEventAccess;

class KKpipiEfficiency : virtual public IGetDalitzEvent, public DalitzEventAccess{
  TH1F* _hKFast, *_hKSlow, *_hPiFast, *_hPiSlow;
  double _pKFast, _pKSlow, _pPiFast, _pPiSlow;

  void resetP();
  bool getP();
  static double oneEff(TH1F* h, double p);
  static TH1F* get_h(TFile* f, const std::string& hname);
  double simple();
  double eff();

public:
  KKpipiEfficiency(const std::string& fname="Ratios.root"
		   , IDalitzEventAccess* daddy=0);

  virtual double RealVal();
};


#endif
//
