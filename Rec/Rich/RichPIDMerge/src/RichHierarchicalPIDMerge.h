
/** @file RichHierarchicalPIDMerge.h
 *
 *  Header file for RICH algorithm : Rich::Rec::HierarchicalPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichHierarchicalPIDMerge.h,v 1.4 2007-03-09 22:40:52 jonrob Exp $
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

namespace Rich
{
  namespace Rec
  {

    /** @class HierarchicalPIDMerge RichHierarchicalPIDMerge.h
     *
     *  Prepares RichPIDs using results from all individual PID algorithms.
     *
     *  The PID results are used with the follow order of precedence :-
     *
     *    1. The global PID result for each track is used if available and if a Ring
     *       Refit result has not been used.
     *
     *    2. Finally, if no PID result is used and if a local PID result exists, it is used.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2002-07-10
     *
     *  @todo Rewrite this algorithm in a more generic way
     */

    class HierarchicalPIDMerge : public Rich::AlgBase
    {

    public:

      /// Standard constructor
      HierarchicalPIDMerge( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~HierarchicalPIDMerge( ); ///< Destructor

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

      /// Flag to turn on/off use of Local PID results
      bool m_useLocalPIDs;

      /// Flag to turn on/off use of Global PID results
      bool m_useGlobalPIDs;

    };

  }
}

#endif // RICHPIDMERGE_RICHPIDSIMPLEMERGE_H
