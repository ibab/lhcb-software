// $Id: RichPIDSimpleMerge.h,v 1.3 2004-06-18 11:18:50 jonesc Exp $
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

};

#endif // RICHPIDMERGE_RICHPIDSIMPLEMERGE_H
