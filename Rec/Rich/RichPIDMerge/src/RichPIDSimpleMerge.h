
/** @file RichPIDSimpleMerge.h
 *
 *  Header file for RICH algorithm : RichPIDSimpleMerge
 *
 *  CVS Log :-
 *  $Id: RichPIDSimpleMerge.h,v 1.5 2004-07-26 17:56:24 jonrob Exp $
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

/** @class RichPIDSimpleMerge RichPIDSimpleMerge.h RichRecAlgs/RichPIDSimpleMerge.h
 *
 *  Prepares RichPIDs using results from all individual PID algorithms.
 *
 *  The PID results are used with the follow order of precedence :-
 *
 *    1. For a given, if configured to do so, the Ring Refit PID result is used. 
 *       By default, this feature is turned OFF.
 * 
 *    2. The global PID result for each track is used if available and if a Ring 
 *       Refit result has not been used.
 *
 *    3. Finally, if no PID result is used and if a local PID result exists, it is used.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 *
 *  @todo Rewrite this algorithm in a m more generic way
 */

class RichPIDSimpleMerge : public RichAlgBase {

public:

  /// Standard constructor
  RichPIDSimpleMerge( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichPIDSimpleMerge( ); ///< Destructor

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

  /// Input location in TES for RichLocalPIDs
  std::string m_richLocalPIDLocation;

  /// Input location in TES for RichRingRefitPIDs
  std::string m_richRingFitPIDLocation;

  /// Flag to turn on/off use of Ring Refit PID results
  bool m_useRingPIDs;

  /// Flag to turn on/off use of Local PID results
  bool m_useLocalPIDs;
  
  /// Flag to turn on/off use of Ring Global PID results
  bool m_useGlobalPIDs;

};

#endif // RICHPIDMERGE_RICHPIDSIMPLEMERGE_H
