
/** @file RichHierarchicalPIDMerge.h
 *
 *  Header file for RICH algorithm : Rich::Rec::HierarchicalPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichHierarchicalPIDMerge.h,v 1.6 2009-06-11 14:21:10 jonrob Exp $
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

namespace Rich
{
  namespace Rec
  {

    /** @class HierarchicalPIDMerge RichHierarchicalPIDMerge.h
     *
     *  Prepares RichPIDs using results from all individual PID algorithms.
     *
     *  Currently, there is only one PID algorithm - RichGlobalPID...
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2002-07-10     *
     *  @todo Rewrite this algorithm in a more generic way
     *  @todo Review if it is needed at all
     */

    class HierarchicalPIDMerge : public Rich::AlgBase
    {

    public:

      /// Standard constructor
      HierarchicalPIDMerge( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~HierarchicalPIDMerge( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    private:

      /// Location of processing status object in TES
      std::string m_procStatLocation;

      /// Output location in TES for RichPIDs
      std::string m_richPIDLocation;

      /// Input location in TES for RichGlobalPIDs
      std::string m_richGlobalPIDLocation;

      /// Flag to turn on/off use of Global PID results
      bool m_useGlobalPIDs;

      /// Flag to turn on/off the fulling of summary information into the ProcStat object
      bool m_fillProcStat;

    };

  }
}

#endif // RICHPIDMERGE_RICHPIDSIMPLEMERGE_H
