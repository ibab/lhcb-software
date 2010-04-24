
//-----------------------------------------------------------------------------
/** @file RichHPDHitsMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::HPDHitsMoni
 *
 *  CVS Log :-
 *  $Id: RichHPDHitsMoni.h,v 1.6 2009-08-05 23:14:59 jonrob Exp $
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
     *  Simple monitor for the hits in each HPD
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

    private: // data

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_richSys;

    };

  }
}

#endif // RICHRECMONITOR_RichHPDHitsMoni_H
