
//------------------------------------------------------------------------------------
/** @file RichGlobalPIDInitialize.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDInitialize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDInitialize.h,v 1.4 2006-12-19 09:06:20 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//------------------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H
#define RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H 1

// base class
#include "RichGlobalPIDAlgBase.h"

// Event
#include "Event/RichRecStatus.h"

//------------------------------------------------------------------------------------
/** @class RichGlobalPIDInitialize RichGlobalPIDInitialize.h
 *
 *  Initialises the working objects for the Global PID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//------------------------------------------------------------------------------------

class RichGlobalPIDInitialize : public RichGlobalPIDAlgBase {

public:

  /// Standard constructor
  RichGlobalPIDInitialize( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDInitialize();   ///< Destructor

  virtual StatusCode initialize();      // Algorithm initialization
  virtual StatusCode execute   ();      // Algorithm execution
  virtual StatusCode finalize  ();      // Algorithm finalization

};

#endif // RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H
