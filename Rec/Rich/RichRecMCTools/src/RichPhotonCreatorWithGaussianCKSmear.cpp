
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithGaussianCKSmear.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorWithGaussianCKSmear
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithGaussianCKSmear.cpp,v 1.2 2005-06-17 15:28:34 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

// local
#include "RichPhotonCreatorWithGaussianCKSmear.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonCreatorWithGaussianCKSmear>          s_factory ;
const        IToolFactory& RichPhotonCreatorWithGaussianCKSmearFactory = s_factory ;

// Standard constructor
RichPhotonCreatorWithGaussianCKSmear::
RichPhotonCreatorWithGaussianCKSmear( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichPhotonCreatorBase ( type, name, parent ),
    m_mcRecTool           ( 0 ),
    m_delPhotCr           ( 0 ),
    m_applySmearingToAll  ( true ),
    m_smearRad            ( Rich::NRadiatorTypes, true ),
    m_smearWid            ( Rich::NRadiatorTypes, 0 )
{

  // job options
  declareProperty( "RadiatorsToSmear",          m_smearRad           );
  declareProperty( "ApplySmearingToAllPhotons", m_applySmearingToAll );
  declareProperty( "SmearingWidths",            m_smearWid           );

}

StatusCode RichPhotonCreatorWithGaussianCKSmear::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPhotonCreatorBase::initialize();
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
      info() << "Applying a Gaussian smear of " << m_smearWid[iRad] << " mrad to "
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
    Warning( "Using CK theta smearing tool, but no radiators selected", StatusCode::SUCCESS );
  }

  return sc;
}

StatusCode RichPhotonCreatorWithGaussianCKSmear::finalize()
{
  // finalise random numbers
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    if ( m_smearRad[iRad] ) { m_rand[iRad].finalize(); }
  }

  // Execute base class method
  return RichPhotonCreatorBase::finalize();
}

RichRecPhoton *
RichPhotonCreatorWithGaussianCKSmear::buildPhoton( RichRecSegment * segment,
                                                   RichRecPixel * pixel,
                                                   const RichRecPhotonKey key ) const
{

  // First, create a photon using the delegated tool
  RichRecPhoton * newPhoton = m_delPhotCr->reconstructPhoton(segment,pixel);
  if ( !newPhoton ) return newPhoton; // if null, return

  // has this photon already been smeared ?
  if ( !m_photonDone[key] )
  {

    // This is a new photon , so add to reference map
    m_photonDone[key] = true;

    // Now, smear this photon

    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    if ( m_smearRad[rad] )
    {

      // See if there is a true cherenkov photon for this segment/pixel pair
      if ( !m_applySmearingToAll &&
           !richMCRecTool()->trueOpticalPhoton(segment,pixel) ) return newPhoton;

      // Smear the Cherenkov theta
      const double smear = (m_rand[rad])();
      const double newCKtheta = smear + newPhoton->geomPhoton().CherenkovTheta();
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << rad << " photon. Applying theta smearing " << smear
                  << ": theta = " << newPhoton->geomPhoton().CherenkovTheta()
                  << " -> " << newCKtheta << endreq;
      }
      newPhoton->geomPhoton().setCherenkovTheta( newCKtheta );

    }

  }

  // Return pointer to this photon
  return newPhoton;

}
