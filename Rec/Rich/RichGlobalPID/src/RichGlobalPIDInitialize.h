
/** @file RichGlobalPIDInitialize.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDInitialize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDInitialize.h,v 1.2 2004-07-27 10:56:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

#ifndef RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H
#define RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H 1

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

  virtual StatusCode initialize();      // Algorithm initialization
  virtual StatusCode execute   ();      // Algorithm execution
  virtual StatusCode finalize  ();      // Algorithm finalization

};

#endif // RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H
