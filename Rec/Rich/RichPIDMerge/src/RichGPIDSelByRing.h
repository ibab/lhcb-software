
/** @file RichGPIDSelByRing.h
 *
 *  Header file for RICH algorithm : RichGPIDSelByRing
 *
 *  CVS Log :-
 *  $Id: RichGPIDSelByRing.h,v 1.1 2004-08-19 14:04:35 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */

#ifndef RICHPIDMERGE_RICHPIDSIMPLEMERGE_H
#define RICHPIDMERGE_RICHPIDSIMPLEMERGE_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"

// event model
#include "Event/ProcStatus.h"
#include "Event/RichPID.h"
#include "Event/RichGlobalPID.h"
#include "Event/RichLocalPID.h"
#include "Event/RichRingRefitPID.h"

/** @class RichGPIDSelByRing RichGPIDSelByRing.h RichRecAlgs/RichGPIDSelByRing.h
 *
 *  Debug algorithm. Selects only Global results if there is an associated 
 *  Ring PID available for the same track. Used to compare the performance
 *  two algorithms.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 *
 *  @todo Rewrite this algorithm in a more generic way
 */

class RichGPIDSelByRing : public RichAlgBase {

public:

  /// Standard constructor
  RichGPIDSelByRing( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichGPIDSelByRing( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private:

  /// Location of processing status object in TES
  std::string m_procStatLocation;

  /// Output location in TES for RichPIDs
  std::string m_richPIDLocation;

  /// Input location in TES for RichGlobalPIDs
  std::string m_richGlobalPIDLocation;

  /// Input location in TES for RichRingRefitPIDs
  std::string m_richRingFitPIDLocation;

};

#endif // RICHPIDMERGE_RICHPIDSIMPLEMERGE_H
