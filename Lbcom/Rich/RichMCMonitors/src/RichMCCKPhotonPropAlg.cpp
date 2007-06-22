
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonPropAlg.cpp
 *
 * Header file for monitor algorithm RichMCCKPhotonPropAlg
 *
 * CVS Log :-
 * $Id: RichMCCKPhotonPropAlg.cpp,v 1.1 2007-06-22 12:45:54 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichMCCKPhotonPropAlg.h"

// units namespace
using namespace Rich::MC;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCCKPhotonPropAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCCKPhotonPropAlg::MCCKPhotonPropAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : Rich::HistoAlgBase ( name , pSvcLocator      ),
    m_mcTruth          ( NULL                    )
{
}

//=============================================================================
// Destructor
//=============================================================================
MCCKPhotonPropAlg::~MCCKPhotonPropAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCCKPhotonPropAlg::initialize()
{
  // must be done first
  const StatusCode sc = RichHistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // add custom initialisation stuff here

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCCKPhotonPropAlg::execute()
{
  // do we have any photons (i.e. is extended RICH MC truth available) ? 
  // If not just return
  if ( !exist<LHCb::MCRichOpticalPhotons>(LHCb::MCRichOpticalPhotonLocation::Default) )
    return StatusCode::SUCCESS;
    
  // Load the photons that are to be analysed
  const LHCb::MCRichOpticalPhotons * photons 
    = get<LHCb::MCRichOpticalPhotons> ( LHCb::MCRichOpticalPhotonLocation::Default );

  // Rich Histo ID utility class
  const RichHistoID hid;

  // loop over the photons
  for ( LHCb::MCRichOpticalPhotons::const_iterator iP = photons->begin();
        iP != photons->end(); ++iP )
  {

    // Which rad ?
    const Rich::RadiatorType rad = (*iP)->mcRichHit()->radiator();

    // photon energy
    const float energy     = (*iP)->energyAtProduction() / Gaudi::Units::electronvolt;
    // wavelength
    const float wavelength = 
      ( energy>0 ? Gaudi::Units::c_light*Gaudi::Units::h_Planck/(*iP)->energyAtProduction() : 0 )
      / Gaudi::Units::nanometer;

    // plots
    plot1D( energy, hid(rad,"photonEnergy"), "CK Photon Energy (eV)", 
            0, 10, 100 );
    plot1D( wavelength, hid(rad,"photonWavelength"), "CK Photon Wavelength (nm)", 
            0, 800, 100 );

  } // loop over photons

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCCKPhotonPropAlg::finalize()
{
  // must be called after all other actions
  return RichHistoAlgBase::finalize();
}

//=============================================================================
