
//-----------------------------------------------------------------------------
/** @file RichRawDataSize.h
 *
 *  Header file for tool : Rich::DAQ::RawDataSize
 *
 *  CVS Log :-
 *  $Id: RichRawDataSize.h,v 1.3 2009-07-29 12:35:06 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RichRawDataSize_H
#define RICHRECQC_RichRawDataSize_H 1

// STL
#include <sstream>

// RichKernel
#include "RichKernel/RichHistoAlgBase.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/RichMap.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Event model
#include "Event/RawEvent.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class RawDataSize RichRawDataSize.h
     *
     *  Simple monitor to monitor the raw data sizes for the RICH
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2008-10-14
     */
    //-----------------------------------------------------------------------------
    class RawDataSize : public Rich::HistoAlgBase
    {

    public:

      /// Standard constructor
      RawDataSize( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~RawDataSize( ); ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialization
      virtual StatusCode execute();    ///< Algorithm execution

    protected:

      /// Pre-Book all (non-MC) histograms
      virtual StatusCode prebookHistograms();

    private:

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_RichSys;

      ///< The TAE location(s) to monitor
      std::vector<std::string> m_taeEvents;

      /// Flag to turn on filling of the individual HPD plots
      bool m_hpdPlots;

    };

  }
}

#endif // RICHRECQC_RichRawDataSize_H
