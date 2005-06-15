// $Id: ParentAlg.h,v 1.1.1.1 2005-06-15 16:19:21 cattanem Exp $
#ifndef GAUDIEXAMPLE_PARENTALG_H
#define GAUDIEXAMPLE_PARENTALG_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include<time.h>

/** @class ParentAlg
    Trivial Algorithm for tutorial purposes
    
    @author nobody
*/

// Forward declarations
class IHistogram1D;
//class HistogramSvc;

class ParentAlg : public Algorithm {
public:
  /// Constructor of this form must be provided
  ParentAlg(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  Algorithm* m_subalg1;
  Algorithm* m_subalg2;
  IMonitorSvc* m_publishsvc;
  //  IHistogramSvc* histoSvc;
  
  long counter1;
  int  counter2, counter3, counter4;
  bool aboveRef;
  float frac1;
  double frac2;
  std::pair<double,double> position;
  char* status;
  std::string oldStatus;
  double VELOeff;
  IHistogramSvc* m_histosvc;
  IHistogram1D*   myhisto;
  IHistogram1D*   myhisto2;
  IHistogram1D*   myhisto3;
  IHistogram1D*   myhisto4;
  IHistogram1D*   myhisto5;

  IHistogram2D*   my2Dhisto;

  Rndm::Numbers random;

  time_t time_old,time_new;

};

#endif    // GAUDIEXAMPLE_PARENTALG_H
