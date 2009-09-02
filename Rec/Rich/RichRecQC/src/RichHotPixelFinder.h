
//-----------------------------------------------------------------------------
/** @file RichHotPixelFinder.h
 *
 *  Header file for tool : Rich::DAQ::HotPixelFinder
 *
 *  CVS Log :-
 *  $Id: RichHotPixelFinder.h,v 1.1 2009-09-02 16:24:36 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RichHotPixelFinder_H
#define RICHRECQC_RichHotPixelFinder_H 1

// STL
#include <sstream>

// RichKernel
#include "RichKernel/RichAlgBase.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/RichHashMap.h"

// RichDet
#include "RichDet/DeRichSystem.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class HotPixelFinder RichHotPixelFinder.h
     *
     *  Simple monitor to look for hot HPD pixels
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2008-10-14
     */
    //-----------------------------------------------------------------------------
    class HotPixelFinder : public Rich::AlgBase
    {

    public:

      /// Standard constructor
      HotPixelFinder( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~HotPixelFinder( ); ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialization
      virtual StatusCode execute();    ///< Algorithm execution
      virtual StatusCode finalize();   ///< Algorithm finalization

    private:

      /// Process an individual TAE event
      StatusCode processTAEEvent( const std::string & taeEvent );

    private:

      /// Hit count map
      typedef Rich::HashMap<LHCb::RichSmartID,unsigned int> HitCountMap;

    private:

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_RichSys;

      ///< The TAE location(s) to monitor
      std::vector<std::string> m_taeEvents;

      /// Number of events to look for hot pixels over
      unsigned int m_nEventsForAverage;

      /// Count hits per channel in current event sample
      HitCountMap m_hitCount;

      /// Count of number of events since last search for hot pixels
      unsigned int m_nEvts;

      /// pixel event occupancy to consider a pixel as hot
      double m_hotOcc;

    };

  }
}

#endif // RICHRECQC_RichHotPixelFinder_H
