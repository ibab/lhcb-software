// $Id: RichGlobalPIDInitialize.h,v 1.1.1.1 2003-06-30 16:10:54 jonesc Exp $
#ifndef RICHRECALGS_RICHGLOBALPIDINITIALIZE_H
#define RICHRECALGS_RICHGLOBALPIDINITIALIZE_H 1

// base class
#include "RichGlobalPIDAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"

/** @class RichGlobalPIDInitialize RichGlobalPIDInitialize.h
 *
 *  Initialises the working objects for the Global PID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

class RichGlobalPIDInitialize : public RichGlobalPIDAlgBase {

public:

  /// Standard constructor
  RichGlobalPIDInitialize( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDInitialize();   ///< Destructor

  virtual StatusCode initialize();      ///< Algorithm initialization
  virtual StatusCode execute   ();      ///< Algorithm execution
  virtual StatusCode finalize  ();      ///< Algorithm finalization

};

#endif // RICHRECALGS_RICHGLOBALPIDINITIALIZE_H
