// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelSICBChck.h,v 1.1 2001-06-06 16:54:10 cattaneb Exp $
#ifndef BRUNELSICB_BRUNELSICBCHCK_H 
#define BRUNELSICB_BRUNELSICBCHCK_H 1
// Include files
#include "GaudiKernel/Algorithm.h"

/** @class BrunelSICBChck BrunelSICBChck.h BrunelSicb/BrunelSICBChck.h
 *  Interface algorithm to SICB DST checking code.
 *
 *  @author Marco Cattaneo
 *  @date   01/06/2001
 */
class BrunelSICBChck : public Algorithm {
public:
  /// Standard constructor
  BrunelSICBChck( const std::string& name, ISvcLocator* pSvcLocator );

  /// Standard destructor
  virtual ~BrunelSICBChck( ) { }

  /// Extend the default behaviour
  StatusCode BrunelSICBChck::initialize();
  StatusCode BrunelSICBChck::execute();
  StatusCode BrunelSICBChck::finalize();
protected:

private:

};
#endif // BRUNELSICB_BRUNELSICBCHCK_H
