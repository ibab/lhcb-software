// $Id: RichPIDSimpleMerge.h,v 1.4 2004-07-20 12:45:29 jonesc Exp $
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
 *  Prepares RichPIDs using results from all PID algorithms
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */

class RichPIDSimpleMerge : public RichAlgBase {

public:

  /// Standard constructor
  RichPIDSimpleMerge( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichPIDSimpleMerge( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

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
