#ifndef Adder_Adder_H
#define Adder_Adder_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include<time.h>

/** @class Adder


*/

// Forward declarations
class IHistogram1D;
class IHistogram2D;
class IProfile1D;

class Adder : public Algorithm {
public:
  /// Constructor of this form must be provided
  Adder(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  //TH1 * sum;
  std::vector<TH1*> sum;
  std::vector<TH2*> sum2d;
  std::vector<TProfile*> sump;
  IHistogramSvc* histoSvc;

  IHistogram1D*  h;
  IHistogram2D*  h2d;
  IProfile1D* p;
  
  std::string m_nodename; 
  std::vector<std::string> m_histogramname; 
  int m_refreshtime;
  std::string m_dimclientdns;
  
  std::vector<int> j1D;
  std::vector<int> j2D;
  std::vector<int> jpD;
};

#endif    // Adder_Adder_H
