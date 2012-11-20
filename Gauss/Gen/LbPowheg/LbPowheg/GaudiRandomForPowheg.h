#ifndef LBPOWHEG_GAUDIRANDOMFORPOWHEG_H
#define LBPOWHEG_GAUDIRANDOMFORPOWHEG_H 

// Include.
#include "Basics.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class GaudiRandomForPythia8 GaudiRandomForPythia8.h
    LbPythia8/GaudiRandomForPythia8.h
 *  Random generator class to interface Gaudi Random Generator to Powheg.
 *
 *  @author Philip Ilten
 *  @date   2007-07-31
 */

using namespace Rndm;

class GaudiRandomForPowheg : public Pythia8::RndmEngine {

public:

  /// Constructor.
  GaudiRandomForPowheg( IRndmGenSvc *i, StatusCode &sc);

  /// Destructor.
  virtual ~GaudiRandomForPowheg();

  /// Random number.
  virtual double flat();
 
private: 

  /// Random number generator.
  Numbers m_gaudiGenerator; 
};
#endif // LBPOWHEG_GAUDIRANDOMFORPOWHEG_H
