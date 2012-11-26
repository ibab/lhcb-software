
//-----------------------------------------------------------------------------
/** @file RichIsolatedTrackTool.cpp
 *
 *  Implementation file for tool : Rich::Rec::IsolatedTrackTool
 *
 *  @author Susan Haines  Susan.Carol.Haines@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/02/2008
 */
//-----------------------------------------------------------------------------

// local
#include "RichIsolatedTrackTool.h"

// Namespaces
using namespace LHCb;
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

//Declare tool factory
DECLARE_TOOL_FACTORY( IsolatedTrackTool )

//=============================================================================
// Standard constructor
//=============================================================================

IsolatedTrackTool::IsolatedTrackTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : Rich::Rec::ToolBase ( type, name, parent ),
    m_ckAngle        ( NULL ),
    m_geomEffic      ( NULL ),
    m_ckThetaMax     ( Rich::NRadiatorTypes, 0 ),
    m_sepGMax        ( Rich::NRadiatorTypes, 0 ),
    m_scale          ( Rich::NRadiatorTypes, 0 )
{
  // Define interface for this tool
  declareInterface<IIsolatedTrack>(this);

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#define  init_vect(A, B, C) {A, B, C}
#else
  using namespace boost::assign;
#define  init_vect(A, B, C) list_of(A)(B)(C)
#endif
  //Default cut values.  Can change in options file.

  //                                                                       Aero    C4F10   CF4
  declareProperty( "SizeMomCut", m_sizemomcut                 = init_vect( 5     , 10    , 20      ));
  declareProperty( "SizeGeoCut", m_sizegeocut                 = init_vect( 0.3   , 0.3   , 0.3     )); // geometric efficiency
  declareProperty( "SizeSepCut", m_sizesepcut                 = init_vect( 260   , 150   , 260     )); // track hit point separation
  declareProperty( "SizeXposnCut", m_sizeXexit                = init_vect( 0     , 0     , 100     )); // RICH X discontinuity
  declareProperty( "SizeYposnCut", m_sizeYexit                = init_vect( 50    , 50    , 0       )); // RICH Y discontinuity
  declareProperty( "SizePhotonAssocCut", m_sizephotonassoccut = init_vect( 0.8   , 0.8   , 0.8     )); // min fraction not assoc with another track
  declareProperty( "SizeRingWidth", m_sizeringwidth           = init_vect( 0.01  , 0.01  , 0.00944 )); // theta band width
  declareProperty( "SizePhiCut",    m_sizephicut              = init_vect( 0.2125, 0.2125, 0.2125  )); // max fraction of photons in one phi region
  declareProperty( "SizeHitRegionCut", m_sizehitregioncut     = init_vect( 0.8   , 0.8   , 0.8     )); // min fraction of photons lying in theta band
  declareProperty( "MaxTrackROI", m_maxROI                    = init_vect( 390   , 86    , 200     )); // Pixel hit regions
  declareProperty( "CKthetaMax", m_ckThetaMax                 = init_vect( 0.24  , 0.052 , 0.03    ));
  declareProperty( "SepGMax", m_sepGMax                       = init_vect( 342   , 75    , 130     ));
  declareProperty( "NPhiRegions", m_nPhiRegions               = init_vect( 8     , 8     , 8       )); // Number of phi regions
  declareProperty( "MinSegPhotons", m_minSegPhotons           = init_vect( 2     , 2     , 2       ));
  declareProperty( "AbortEarly", m_abortEarly = true );
#undef init_vect
  setProperty( "UseEfficiencyRowFormat", true );
}

//=============================================================================
// Initialisation
//=============================================================================

StatusCode IsolatedTrackTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichGeomEff",        m_geomEffic );
  acquireTool( "RichRecGeometry",    m_geomTool  );
  acquireTool( "RichCherenkovAngle", m_ckAngle   );

  // loop over radiators
  for ( int radi = 0; radi < Rich::NRadiatorTypes; ++radi )
  {
    // numbers for hit pixel calculations
    m_scale[radi] = (m_ckThetaMax[radi]/m_sepGMax[radi]);
  }

  return sc;
}

//=============================================================================

bool IsolatedTrackTool::isIsolated( const LHCb::RichRecTrack * track,
                                    const Rich::ParticleIDType pid ) const
{
  if ( !track ) return false;
  for ( LHCb::RichRecTrack::Segments::const_iterator iS = track->richRecSegments().begin();
        iS != track->richRecSegments().end(); ++iS )
  {
    if ( isIsolated(*iS,pid) ) return true;
  }
  return false;
}

bool IsolatedTrackTool::isIsolated( const LHCb::RichRecTrack * track ) const
{
  if ( !track ) return false;
  for ( LHCb::RichRecTrack::Segments::const_iterator iS = track->richRecSegments().begin();
        iS != track->richRecSegments().end(); ++iS )
  {
    if ( isIsolated(*iS) ) return true;
  }
  return false;
}

bool IsolatedTrackTool::isIsolated( const LHCb::Track * track,
                                    const Rich::ParticleIDType pid ) const
{
  const LHCb::RichRecTrack * rtrack = ( track ? richTracks()->object(track->key()) : NULL );
  return ( rtrack && isIsolated(rtrack,pid) );
}

bool IsolatedTrackTool::isIsolated( const LHCb::Track * track ) const
{
  const LHCb::RichRecTrack * rtrack = ( track ? richTracks()->object(track->key()) : NULL );
  return ( rtrack && 
           ( isIsolated( rtrack, Rich::Electron ) ||
             isIsolated( rtrack, Rich::Muon     ) ||
             isIsolated( rtrack, Rich::Pion     ) ||
             isIsolated( rtrack, Rich::Kaon     ) ||
             isIsolated( rtrack, Rich::Proton   )  ) 
           );
}

//=============================================================================

bool IsolatedTrackTool::isIsolated( const LHCb::RichRecSegment * segment ) const
{
  return ( isIsolated( segment, Rich::Electron ) ||
           isIsolated( segment, Rich::Muon     ) ||
           isIsolated( segment, Rich::Pion     ) ||
           isIsolated( segment, Rich::Kaon     ) ||
           isIsolated( segment, Rich::Proton   ) );
}

bool IsolatedTrackTool::isIsolated( const LHCb::RichRecSegment * segment,
                                    const Rich::ParticleIDType pid ) const
{
  if ( !segment ) return false;
  if ( Rich::BelowThreshold == pid ) return false;

  LHCb::RichRecSegment * nonc_seg = const_cast<LHCb::RichRecSegment*>(segment);

  bool trackIsIsolated(true);

  // the RICH and radiator type
  const Rich::RadiatorType rad  = segment->trackSegment().radiator();
  const Rich::DetectorType rich = segment->trackSegment().rich();
  if ( rad == Rich::Aerogel /* || rad == Rich::Rich1Gas */ ) 
  { 
    //Warning("Not yet implemented for "+Rich::text(rad),StatusCode::SUCCESS,1).ignore(); 
    return false;
  }

  // Get a reference to the Geometrical track object.
  const RichTrackSegment & trackSeg = segment->trackSegment();

  // Radiator exit point and momentum
  const Gaudi::XYZPoint & exitPoint = trackSeg.exitPoint();
  const Gaudi::XYZVector & exitMom  = trackSeg.exitMomentum();

  // Exit cuts
  trackIsIsolated &= testCut( "RICH panel X boundary", rad, fabs(exitPoint.x()) > m_sizeXexit[rad] );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;
  trackIsIsolated &= testCut( "RICH panel Y boundary", rad, fabs(exitPoint.y()) > m_sizeYexit[rad] );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  // Cuts on exit momentum
  const double exitP = std::sqrt( exitMom.Mag2() );
  trackIsIsolated &= testCut( "Min Track exit momentum", rad, exitP > m_sizemomcut[rad]*Gaudi::Units::GeV );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  // geom. eff. cut
  const double geomEff = m_geomEffic->geomEfficiency( nonc_seg, pid );
  trackIsIsolated &= testCut( "HPD geometrical efficiency", rad, geomEff > m_sizegeocut[rad] );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  // Track hit point (if it were a photon) on detector plane, in local coords (i.e. centre of Ch ring)
  const Gaudi::XYZPoint & tkPtnLocal = segment->pdPanelHitPointLocal();

  // Loop over all segments to compare ring centre for the chosen segment with all others
  bool OK(true);
  for ( RichRecSegments::const_iterator iSeg2 = richSegments()->begin();
        iSeg2 != richSegments()->end(); ++iSeg2 )
  {
    const RichRecSegment * segment2 = *iSeg2;
    
    if ( segment == segment2 ) continue; // check not comparing the segment with itself

    // The radiator type.  Must be same as main segment
    if ( segment2->trackSegment().radiator() != rad ) { continue; }

    // Track hit point (Ch ring centre)
    const Gaudi::XYZPoint & tkPtnLocal1 = segment2->pdPanelHitPointLocal();

    // Compare outer loop segment entry to all other segments and find separation
    const double Xdifsq      = std::pow( fabs(tkPtnLocal.x()-tkPtnLocal1.x()), 2 );
    const double Ydifsq      = std::pow( fabs(tkPtnLocal.y()-tkPtnLocal1.y()), 2 );
    const double centreXYdif = std::sqrt(Xdifsq+Ydifsq);

    // If ring centre too close to any other, return false
    OK = centreXYdif > m_sizesepcut[rad];
    if (!OK) break;
    
  } // Segment comparison loop end

  trackIsIsolated &= testCut( "track CK ring centre proximity", rad, OK );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  // initialise all to zero for each segment
  unsigned int hittotal(0), hitsoutside(0), hitsinside(0);

  // Container of pixels for this rich
  IPixelCreator::PixelRange range = pixelCreator()->range(rich); 

  // Use PID average Ch theta for segment
  const double avtheta = m_ckAngle->avgCherenkovTheta( nonc_seg, pid );

  // Define ring around average Ch theta
  const double ringmin = avtheta-m_sizeringwidth[rad];
  const double ringmax = avtheta+m_sizeringwidth[rad];

  //Loop over all hit pixels
  for ( IPixelCreator::PixelRange::const_iterator iPixel = range.begin();
        iPixel != range.end(); ++iPixel )
  {
    RichRecPixel * pixel = *iPixel; // hit pixel

    const double sepsq = m_geomTool->trackPixelHitSep2(segment,pixel);//square of sep of pixel from track centre
    const double sep   = std::sqrt(sepsq);

    // Check hit pixel lies within reasonable distance of track centre
    if ( sep < m_maxROI[rad] )
    {
      ++hittotal; // Counting total hits near this segment

      //Estimated Ch theta for this pixel
      const double ckThetaEsti = sep*m_scale[rad];

      // Does hit lie outside ring region?
      if      ( ckThetaEsti > ringmax ) { ++hitsoutside; }
      else if ( ckThetaEsti < ringmin ) { ++hitsinside;  }

    } // boundary check
  } // loop over pixels

  if (hittotal==0) { hittotal = 1; } // To prevent dividing by zero
  const double frachitsout = (double)(hitsinside+hitsoutside) / (double)hittotal;
  trackIsIsolated &= testCut( "Hit fraction outside CK ring", rad, (1-frachitsout) > m_sizehitregioncut[rad] );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  // Photon association, position and total photon counters, initialise zero for each segment before looping over photons
  unsigned int photonassoc(0), photontotal(0);

  // Tally vector for number of photons in each phi region
  std::vector<int> regTally(m_nPhiRegions[rad],0);

  // Loop over photons, calculations
  for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
        iPhot != segment->richRecPhotons().end(); ++iPhot )
  {

    LHCb::RichRecPhoton * photon = *iPhot;

    ++photontotal; //counting total photons associated with this segment

    // Which CK phi region
    unsigned int region = (unsigned int)( m_nPhiRegions[rad] * 
                                          photon->geomPhoton().CherenkovPhi() / (M_PI*2.0) );
    if ( region >= m_nPhiRegions[rad] ) region = 0; // Protect against index being out of range
    
    // count number in each region
    ++regTally[region];

    // No. of photons associated with this pixel (should be=1 if photon assigned to one track only)
    const unsigned int Nphotons = photon->richRecPixel()->richRecPhotons().size();

    // If this photon is associated with more than one track...
    if ( Nphotons > 1 ) { ++photonassoc; }
    
  } // photon loop end

  // Test how many photons associated with two tracks for this segment

  if ( photontotal==0 ) { photontotal = 1; } // to prevent dividing by zero
  
  // Minimum number of associated photons
  trackIsIsolated &= testCut( "min # associated photons", rad, photontotal >= m_minSegPhotons[rad] );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  const double fracphotonsassoc = (double)photonassoc/(double)photontotal;
  trackIsIsolated &= testCut( "photon multiple association", rad, 
                              (1.0-fracphotonsassoc) > m_sizephotonassoccut[rad] ); 
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  // How many lie in each phi region?
  bool photonregionindicator = true;
  for ( std::vector<int>::const_iterator iRegion = regTally.begin();
        iRegion != regTally.end(); ++iRegion )
  {
    if ( (double)(*iRegion)/(double)photontotal > m_sizephicut[rad] )
    {
      photonregionindicator = false;
    }
  }
  trackIsIsolated &= testCut( "photon phi regions", rad, photonregionindicator );

  // count final accept/reject
  if ( !m_abortEarly )
  {
    counter(Rich::text(rad)+" : Isolated Tracks") += trackIsIsolated ? 1 : 0 ;
  }

  if ( trackIsIsolated ) debug() << "Found isolated " << rad << " segment" << endmsg;
  
  // return final status
  return trackIsIsolated;
}


//=============================================================================
