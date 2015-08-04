
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonPropAlg.cpp
 *
 * Header file for monitor algorithm RichMCCKPhotonPropAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

// local
#include "RichMCCKPhotonPropAlg.h"

// units namespace
using namespace Rich::MC;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCCKPhotonPropAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCCKPhotonPropAlg::MCCKPhotonPropAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : HistoAlgBase ( name , pSvcLocator ),
    m_refIndex   ( NULL               ) { }

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
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // tools
  acquireTool( "RichRefractiveIndex", m_refIndex );

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
  const Rich::HistoID hid;

  //            Radiator          Aerogel  Rich1Gas  Rich2Gas
  const double minPhotEn[]    = { 1.5,     1.5,      1.5     };
  const double maxPhotEn[]    = { 4.5,     7,        7.5     };
  const double minRefInd[]    = { 1.0280,  1.00128,  1.00039 };
  const double maxRefInd[]    = { 1.0312,  1.00150,  1.00046 };

  // loop over the photons
  for ( LHCb::MCRichOpticalPhotons::const_iterator iP = photons->begin();
        iP != photons->end(); ++iP )
  {

    // Which rad ?
    const Rich::RadiatorType rad = (*iP)->mcRichHit()->radiator();
    // skip non-primary radiators
    if ( rad != Rich::Aerogel  && 
         rad != Rich::Rich1Gas && 
         rad != Rich::Rich2Gas ) continue;

    // photon energy
    const double energy     = (*iP)->energyAtProduction() / Gaudi::Units::electronvolt;
    // wavelength
    const double wavelength =
      ( energy>0 ? Gaudi::Units::c_light*Gaudi::Units::h_Planck/(*iP)->energyAtProduction() : 0 )
      / Gaudi::Units::nanometer;
    // ref index for this photon
    const double refInd     = m_refIndex->refractiveIndex( rad, energy );

    if ( msgLevel(MSG::DEBUG) )
      debug() << rad << " photon : energy=" << energy << " refIndex=" << refInd 
              << " wavelength=" << wavelength
              << endmsg;

    // plots
    plot1D( energy, hid(rad,"photonEnergy"), "CK Photon Energy (eV)",
            minPhotEn[rad], maxPhotEn[rad], 100 );
    plot1D( wavelength, hid(rad,"photonWavelength"), "CK Photon Wavelength (nm)",
            0, 800, 100 );
    plot1D( refInd, hid(rad,"refIndex"), "Refractive index of observed photons",
            minRefInd[rad], maxRefInd[rad], 100 );

  } // loop over photons

  return StatusCode::SUCCESS;
}

//=============================================================================
