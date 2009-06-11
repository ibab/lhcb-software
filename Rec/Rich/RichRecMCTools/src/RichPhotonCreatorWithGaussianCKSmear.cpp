//-------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithGaussianCKSmear.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorWithGaussianCKSmear
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithGaussianCKSmear.cpp,v 1.8 2009-06-11 12:41:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichPhotonCreatorWithGaussianCKSmear.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonCreatorWithGaussianCKSmear );

// Standard constructor
PhotonCreatorWithGaussianCKSmear::
PhotonCreatorWithGaussianCKSmear( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : PhotonCreatorBase ( type, name, parent ),
    m_mcRecTool           ( 0 ),
    m_delPhotCr           ( 0 ),
    m_applySmearingToAll  ( true ),
    m_smearRad            ( Rich::NRadiatorTypes, true ),
    m_smearWid            ( Rich::NRadiatorTypes, 0 ),
    m_smearCount          ( Rich::NRadiatorTypes, 0 )
{

  // job options
  declareProperty( "RadiatorsToSmear",          m_smearRad           );
  declareProperty( "ApplySmearingToAllPhotons", m_applySmearingToAll );
  declareProperty( "SmearingWidths",            m_smearWid           );

}

StatusCode PhotonCreatorWithGaussianCKSmear::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = PhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichDelegatedPhotonCreator", m_delPhotCr );

  // Initialize gaussian smearing
  IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  bool willSmear = false;
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    if ( m_smearRad[iRad] )
    {
      if ( !m_rand[iRad].initialize( randSvc, Rndm::Gauss(0,m_smearWid[iRad]) ) )
      {
        return Error( "Failed to initialize Gaussian smear for " +
                      Rich::text((Rich::RadiatorType)iRad) );
      }
      info() << "Applying a Gaussian smear of " << 1000*m_smearWid[iRad] << " mrad to "
             << Rich::text((Rich::RadiatorType)iRad) << " photons" << endreq;
      willSmear = true;
    }
  }
  if ( willSmear )
  {
    if ( m_applySmearingToAll )
    {
      info() << "Applying smearing to all photons in selected radiators" << endreq;
    }
    else
    {
      info() << "Applying smearing only to true Cherenkov photons" << endreq;
    }
  }
  else
  {
    Warning( "Using CK theta smearing tool, but no radiators selected for smearing",
             StatusCode::SUCCESS );
  }

  return sc;
}

StatusCode PhotonCreatorWithGaussianCKSmear::finalize()
{

  // printout smear count
  if ( nEvents() > 0 )
  {

    // statistical tool
    const StatDivFunctor occ("%10.2f +-%7.2f");

    // Print out final stats
    info() << "=================================================================" << endreq
           << "  Photon smearing summary : " << nEvents() << " events :-" << endreq
           << "    Aerogel   : "
           << occ(m_smearCount[Rich::Aerogel],nEvents()) << "  photons/event "
           << 1000*m_smearWid[Rich::Aerogel] << " mrad smear" << endreq
           << "    Rich1Gas     : "
           << occ(m_smearCount[Rich::Rich1Gas],nEvents())   << "  photons/event "
           << 1000*m_smearWid[Rich::Rich1Gas] << " mrad smear" << endreq
           << "    Rich2Gas       : "
           << occ(m_smearCount[Rich::Rich2Gas],nEvents())     << "  photons/event "
           << 1000*m_smearWid[Rich::Rich2Gas] << " mrad smear" << endreq
           << "=================================================================" << endreq;

  }

  // finalise random numbers
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    if ( m_smearRad[iRad] ) { m_rand[iRad].finalize(); }
  }

  // Execute base class method
  return PhotonCreatorBase::finalize();
}

LHCb::RichRecPhoton *
PhotonCreatorWithGaussianCKSmear::buildPhoton( LHCb::RichRecSegment * segment,
                                               LHCb::RichRecPixel * pixel,
                                               const RichRecPhotonKey key ) const
{

  // First, create a photon using the delegated tool
  LHCb::RichRecPhoton * newPhoton = m_delPhotCr->reconstructPhoton(segment,pixel);
  if ( !newPhoton ) return newPhoton; // if null, return

  // has this photon already been smeared ?
  if ( !m_photSmearDone[key] )
  {

    // This is a new photon , so add to reference map
    m_photSmearDone[key] = true;

    // Now, smear this photon
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    if ( m_smearRad[rad] )
    {

      // See if there is a true cherenkov photon for this segment/pixel pair
      if ( m_applySmearingToAll ||
           richMCRecTool()->trueOpticalPhoton(segment,pixel) )
      {

        // Smear the Cherenkov theta
        const double smear      = (m_rand[rad])();
        const double newCKtheta = smear + newPhoton->geomPhoton().CherenkovTheta();
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << rad << " photon. Applying theta smearing " << smear
                    << ": theta = " << newPhoton->geomPhoton().CherenkovTheta()
                    << " -> " << newCKtheta << endreq;
        }
        newPhoton->geomPhoton().setCherenkovTheta( static_cast<float>(newCKtheta) );

        // count smeared photons
        ++m_smearCount[rad];

      } // photon is to be smeared

    } // radiator is to be smeared

  } // photon allready smeared

  // Return pointer to this photon
  return newPhoton;

}

void PhotonCreatorWithGaussianCKSmear::InitNewEvent()
{
  // initialize base class
  RichPhotonCreatorBase::InitNewEvent();
  // local stuff
  m_photSmearDone.clear();
}
