
/** @file RichGlobalPIDFinalize.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDFinalize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDFinalize.h,v 1.4 2004-07-27 10:56:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

#ifndef RICHGLOBALPID_RICHGLOBALPIDFINALIZE_H
#define RICHGLOBALPID_RICHGLOBALPIDFINALIZE_H 1

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
 *  Finalises the RichPID objects for the Global PID algorithm
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

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  /// Location of processing status object in TES
  std::string m_procStatLocation;

};

#endif // RICHGLOBALPID_RICHGLOBALPIDFINALIZE_H
