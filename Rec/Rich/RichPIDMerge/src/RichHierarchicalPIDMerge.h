
/** @file RichHierarchicalPIDMerge.h
 *
 *  Header file for RICH algorithm : RichHierarchicalPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichHierarchicalPIDMerge.h,v 1.1 2004-08-19 14:04:36 jonrob Exp $
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

/** @class RichHierarchicalPIDMerge RichHierarchicalPIDMerge.h RichRecAlgs/RichHierarchicalPIDMerge.h
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
 *  @todo Rewrite this algorithm in a more generic way
 */

class RichHierarchicalPIDMerge : public RichAlgBase {

public:

  /// Standard constructor
  RichHierarchicalPIDMerge( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichHierarchicalPIDMerge( ); ///< Destructor

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

  /// Flag to turn on the selection of Ring PID results if the corresponding global/local
  /// only has RICH2 information
  bool m_ringSel;

};

#endif // RICHPIDMERGE_RICHPIDSIMPLEMERGE_H
