// $Id: BrunelDigiMUON.h,v 1.2 2001-06-25 08:24:57 cattaneb Exp $
#ifndef BRUNEL_DIGI_MUON_H
#define BRUNEL_DIGI_MUON_H


// Include Files
#include "GaudiKernel/Algorithm.h"


/** @class BrunelDigiMUON BrunelDigiMUON.h
 *
 * Steering for SICBDST MUON digitisation in Brunel
 *
 * @author: Marco Cattaneo
 */

class BrunelDigiMUON : public Algorithm    {

public:

  /// Constructor.
  BrunelDigiMUON( const std::string& name, ISvcLocator *svcloc );

  /// Destructor.
  virtual ~BrunelDigiMUON();

  /// Initialise the algorithm
  virtual StatusCode initialize();

  /// Execute the algorithm
  virtual StatusCode execute();

  /// Finalise the algorithm
  virtual StatusCode finalize();

private:
  bool m_addBkg; ///< Flags whether to add muon background hits 
};

#endif // BRUNEL_DIGI_MUON_H
