// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelFinalisation.h,v 1.2 2001-06-06 16:54:10 cattaneb Exp $
#ifndef BRUNEL_FINALISATION__H
#define BRUNEL_FINALISATION__H 1

// Include files
#include "GaudiKernel/Algorithm.h"

/** @class BrunelFinalisation BrunelFinalisation.h
 * Generic Brunel finalisation, independent of phase
 *
 * @author  Marco Cattaneo
 * @date    15th February 2000
 */

class BrunelFinalisation : public Algorithm {
public:
  /// Construtor
  BrunelFinalisation( const std::string& name, ISvcLocator *svcloc );
  /// Destructor
  virtual ~BrunelFinalisation();

  /// Extend the default behaviour
  StatusCode BrunelFinalisation::initialize();
  StatusCode BrunelFinalisation::execute();
  StatusCode BrunelFinalisation::finalize();
private:
  bool m_dropBanks; ///< Flags whether to drop some SICB banks from DST
};

#endif    // BRUNEL_FINALISATION__H
