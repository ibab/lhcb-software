// $Id: RichGlobalPIDFinalize.h,v 1.3 2003-08-06 10:02:10 jonrob Exp $
#ifndef RICHRECALGS_RICHGLOBALPIDFINALIZE_H
#define RICHRECALGS_RICHGLOBALPIDFINALIZE_H 1

// Base class
#include "RichGlobalPIDAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// GSL
#include "gsl/gsl_sf_erf.h"

// Event
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"

/** @class RichGlobalPIDFinalize RichGlobalPIDFinalize.h
 *
 *  Finalises the PID results for the Global PID algorithm
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

class RichGlobalPIDFinalize : public RichGlobalPIDAlgBase {

public:

  /// Standard constructor
  RichGlobalPIDFinalize( const std::string& name,
                         ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDFinalize();   ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // data
  
  /// Location of processing status object in TES
  std::string m_procStatLocation;
  
};

#endif // RICHRECALGS_RICHGLOBALPIDFINALIZE_H
