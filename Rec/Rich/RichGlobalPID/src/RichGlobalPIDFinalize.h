// $Id: RichGlobalPIDFinalize.h,v 1.1.1.1 2003-06-30 16:10:54 jonesc Exp $
#ifndef RICHRECALGS_RICHGLOBALPIDFINALIZE_H
#define RICHRECALGS_RICHGLOBALPIDFINALIZE_H 1

// Base class
#include "RichGlobalPIDAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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

};

#endif // RICHRECALGS_RICHGLOBALPIDFINALIZE_H
