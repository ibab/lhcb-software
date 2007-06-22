
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonPropAlg.h
 *
 * Header file for monitor algorithm Rich::MC::MCCKPhotonPropAlg
 *
 * CVS Log :-
 * $Id: RichMCCKPhotonPropAlg.h,v 1.1 2007-06-22 12:45:54 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCMONITORS_RichMCCKPhotonPropAlg_H
#define RICHMCMONITORS_RichMCCKPhotonPropAlg_H 1

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// base class
#include "RichKernel/RichHistoAlgBase.h"

// MCEvent
#include "Event/MCRichOpticalPhoton.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichStatDivFunctor.h"

// tool Interfaces
#include "RichKernel/IRichMCTruthTool.h"

// boost
#include "boost/assign/list_of.hpp"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @class MCCKPhotonPropAlg RichMCCKPhotonPropAlg.h
     *
     *  Monitor algorithm to study the properties of the RICH MC CK photons
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2006-11-03
     */
    //-----------------------------------------------------------------------------

    class MCCKPhotonPropAlg : public Rich::HistoAlgBase
    {

    public:

      /// Standard constructor
      MCCKPhotonPropAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~MCCKPhotonPropAlg( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalization

    private: // methods

      /// Access the MC Truth Tool on-demand
      inline const Rich::MC::IMCTruthTool * mcTruthTool() const
      {
        if ( !m_mcTruth ) { acquireTool( "RichMCTruthTool", m_mcTruth, 0, true ); }
        return m_mcTruth;
      }

    private: // data

      /// Pointer to MCtruth association tool
      mutable const Rich::MC::IMCTruthTool * m_mcTruth;

    };

  }
} // RICH namespace

#endif // RICHMCMONITORS_RichMCCKPhotonPropAlg_H
