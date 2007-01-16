#ifndef EXAMPLEGAUCHO_GAUDIEXAMPLE_H
#define EXAMPLEGAUCHO_GAUDIEXAMPLE_H 1

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include<time.h>

/** @class GaudiExample
    Trivial Algorithm for tutorial purposes
    
    @author J. Helder Lopes, 27/07/2006
    @author J. Helder Lopes, 26/12/2006: Extended to test general c-structure and Profile1D monitoring

*/

// Forward declarations
class IHistogram1D;
struct event;

class GaudiExample : public Algorithm {
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
  char* oldStatus;
  std::string status;

  IHistogramSvc* m_histosvc;

  IHistogram1D*   my1Dhisto1;
  IHistogram1D*   my1Dhisto2;

  IHistogram2D*   my2Dhisto;

  IProfile1D* my1Dhprof;

  event * myEvent;

  Rndm::Numbers random1;
  Rndm::Numbers random2;
  Rndm::Numbers random3;

  time_t time_old,time_new;

  int nbinEntries;
  double sumOfWeights;
  double sumOfSquaredWeights;

};

// Toy structure to test passing genral structur to DIM/Gaucho
struct event{
int evt;
double px;
double py;
char status[9];
};

#endif    // EXAMPLEGAUCHO_GAUDIEXAMPLE_H
