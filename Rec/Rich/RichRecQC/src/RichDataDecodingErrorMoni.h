
//-----------------------------------------------------------------------------
/** @file RichDataDecodingErrorMoni.h
 *
 *  Header file for algorithm class : Rich::DAQ::DataDecodingErrorMoni
 *
 *  CVS Log :-
 *  $Id: RichDataDecodingErrorMoni.h,v 1.6 2009-08-05 23:14:59 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RichDataDecodingErrorMoni_H
#define RICHRECQC_RichDataDecodingErrorMoni_H 1

// STD
#include <sstream>

// base class
#include "RichKernel/RichHistoAlgBase.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

// Event
#include "Event/ODIN.h"

namespace Rich
{
  namespace DAQ
  {
    //-----------------------------------------------------------------------------
    /** @class DataDecodingErrorMoni RichDataDecodingErrorMoni.h
     *
     *  Simple monitor for the hits in each HPD
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/04/2002
     */
    //-----------------------------------------------------------------------------

    class DataDecodingErrorMoni : public HistoAlgBase
    {

    public:

      /// Standard constructor
      DataDecodingErrorMoni( const std::string& name,
                             ISvcLocator* pSvcLocator );

      virtual ~DataDecodingErrorMoni( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    protected:
      
      /// Pre-Book all (non-MC) histograms
      virtual StatusCode prebookHistograms();
      
    private: // data

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder;

    };

  }
}

#endif // RICHRECQC_RichDataDecodingErrorMoni_H
