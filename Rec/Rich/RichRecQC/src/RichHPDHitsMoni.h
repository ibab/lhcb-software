
//-----------------------------------------------------------------------------
/** @file RichHPDHitsMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::HPDHitsMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichHPDHitsMoni_H
#define RICHRECMONITOR_RichHPDHitsMoni_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

namespace Rich
{
  namespace Rec
  {
    //-----------------------------------------------------------------------------
    /** @class HPDHitsMoni RichHPDHitsMoni.h
     *
     *  Simple monitor for the hits in each PD or PD column
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/04/2002
     */
    //-----------------------------------------------------------------------------

    class HPDHitsMoni : public Rich::Rec::HistoAlgBase
    {

    public:

      /// Standard constructor
      HPDHitsMoni( const std::string& name,
                   ISvcLocator* pSvcLocator );

      virtual ~HPDHitsMoni( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    protected:

      /// Pre-Book all (non-MC) histograms
      virtual StatusCode prebookHistograms();

    private: // definitions

      /// HPD count map
      typedef Rich::Map<LHCb::RichSmartID,unsigned int> HPDCountMap;

    private:

      /// Create an empty map of all active HPDs and data size
      void initMap( HPDCountMap & hpdMap );

    private: // data

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_richSys;

      /// Flag to turn on the creation of individual PD # hit plots
      bool m_pdNumHits;

      /// Flag to turn on the creation of individual PD hit maps
      bool m_pdHitMaps;

    };

  }
}

#endif // RICHRECMONITOR_RichHPDHitsMoni_H
