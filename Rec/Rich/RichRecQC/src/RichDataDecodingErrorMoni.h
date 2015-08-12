
//-----------------------------------------------------------------------------
/** @file RichDataDecodingErrorMoni.h
 *
 *  Header file for algorithm class : Rich::DAQ::DataDecodingErrorMoni
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

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

// Event
#include "Event/ODIN.h"

// RichDet
#include "RichDet/DeRichSystem.h"

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

    private:

      /// Fill the plots for the given L1ID and error code
      void fillPlots( const Rich::DAQ::Level1CopyNumber& copyN,
                      const int errorCode,
                      const bool error,
                      AIDA::IProfile1D * h1D,
                      AIDA::IHistogram2D * h2D );

      /// Make plots for given L1 board
      StatusCode makePlots( const Rich::DAQ::IngressMap & inMap,
                            const int l1ID );

      /// Get histo labels
      const Rich::HistoAlgBase::BinLabels & labels();

      /// Get the 1D histogram
      AIDA::IProfile1D * getHisto( const int l1ID );

    private: // data

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_RichSys;

      /// Pointer to the ODIN data object
      const LHCb::ODIN * m_odin;

    };

  }
}

#endif // RICHRECQC_RichDataDecodingErrorMoni_H
