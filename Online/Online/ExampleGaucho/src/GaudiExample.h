#ifndef EXAMPLEGAUCHO_GAUDIEXAMPLE_H
#define EXAMPLEGAUCHO_GAUDIEXAMPLE_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/RndmGenerators.h"
// boost
#include "boost/lexical_cast.hpp"
#include<time.h>

/** @class GaudiExample
    Trivial Algorithm for tutorial purposes
    
    @author J. Helder Lopes, 27/07/2006
    @author J. Helder Lopes, 26/12/2006: Extended to test general c-structure and Profile1D monitoring
    @author J. Helder Lopes, 18/03/2008: Test publishing MonObjects
    
*/

// Forward declarations
class MonStatEntity;
class MonVectorI;
class MonVectorD;

class GaudiExample : public GaudiHistoAlg {
public:
  /// Constructor of this form must be provided
  GaudiExample(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  
  long counter1;
  int  counter2;
  bool aboveRef;
  double efficiency1;
  float  efficiency2;
  
  std::pair<double,double> position;
  
  // it does't work because they are not defined in IMonitorSvc
  std::pair<int,int>  index;
  std::pair<double,int>  ix;
  
  char* oldStatus;
  std::string status;
  
  Rndm::Numbers random1;
  Rndm::Numbers random2;
  Rndm::Numbers random3;
  Rndm::Numbers random4;
  
  IHistogramSvc* m_histosvc;
  
  IHistogram1D*   effHisto;
  
  time_t time_old,time_new;
  
  // new MonObjects into rates test
  
  MonStatEntity statEntity;
  
  MonVectorI monVectorInt;
  std::vector<int> m_vectInt;
  
  MonVectorD monVectorDouble;
  std::vector<double> m_vectDouble;
  
  // counters into rates test
  double count_rate_1;
  double count_rate_2;
    
  int nbinEntries;
  double sumOfWeights;
  double sumOfSquaredWeights;
  
  int sleepTime;
};

#endif    // EXAMPLEGAUCHO_GAUDIEXAMPLE_H
