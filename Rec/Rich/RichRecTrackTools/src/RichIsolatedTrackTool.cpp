
//-----------------------------------------------------------------------------
/** @file RichIsolatedTrackTool.cpp
 *
 *  Implementation file for tool : Rich::Rec::IsolatedTrackTool
 *
 *  CVS Log :-
 *  $Id: RichIsolatedTrackTool.cpp,v 1.2 2008-05-14 10:01:37 jonrob Exp $
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
DECLARE_TOOL_FACTORY( IsolatedTrackTool );

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

  using namespace boost::assign;

  //Default cut values.  Can change in options file.

  //                                                                     Aero  C4F10  CF4
  declareProperty( "SizeMomCut", m_sizemomcut                 = list_of  (20)  (20)  (20)  );
  declareProperty( "SizeGeoCut", m_sizegeocut                 = list_of  (0.7) (0.7) (0.7) ); // geometric efficiency
  declareProperty( "SizeSepCut", m_sizesepcut                 = list_of  (260) (260) (260) ); // track hit point separation
  declareProperty( "SizeXposnCut", m_sizeXexit                = list_of  (0)   (0)   (100) ); // RICH X discontinuity
  declareProperty( "SizeYposnCut", m_sizeYexit                = list_of  (0)   (50)  (0)   ); // RICH Y discontinuity
  declareProperty( "SizePhotonAssocCut", m_sizephotonassoccut = list_of  (0.8) (0.8) (0.8) ); // min fraction not assoc with another track
  declareProperty( "SizeRingWidth", m_sizeringwidth           = list_of  (0.00944)(0.00944)(0.00944) );// theta band width
  declareProperty( "SizePhiCut",    m_sizephicut              = list_of  (0.2125) (0.2125) (0.2125)  );// max fraction of photons in one phi region
  declareProperty( "SizeHitRegionCut", m_sizehitregioncut     = list_of  (0.8) (0.8) (0.8) ); // min fraction of photons lying in theta band
  declareProperty( "MaxTrackROI", m_maxROI                    = list_of  (390) (86)  (200) ); // Pixel hit regions
  declareProperty( "CKthetaMax", m_ckThetaMax                 = list_of  (0.24)(0.052) (0.03) );
  declareProperty( "SepGMax", m_sepGMax                       = list_of  (342) (75) (130) );
  declareProperty( "NPhiRegions", m_nPhiRegions               = list_of  (8)   (8)  (8)   ); // Number of phi regions

  declareProperty( "AbortEarly", m_abortEarly = true );

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
  acquireTool( "RichGeomEff", m_geomEffic  );
  acquireTool( "RichRecGeometry", m_geomTool );
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  // loop over radiators
  for ( int radi = 0; radi < Rich::NRadiatorTypes; ++radi )
  {
    //numbers for hit pixel calculations
    m_scale[radi] = (m_ckThetaMax[radi]/m_sepGMax[radi]);
  }

  return sc;
}

StatusCode IsolatedTrackTool::finalize()
{
  // Summary printout
  const PoissonEffFunctor eff("%7.2f +-%5.2f");
  for ( CutEffMap::const_iterator iCut = m_summary.begin();
        iCut != m_summary.end(); ++iCut )
  {
    std::string desc = Rich::text(iCut->first.first)+" '"+iCut->first.second+"'";
    desc.resize(45, ' ');
    info() << "Cut " << desc << " accepted "
           <<  eff(iCut->second.passed,iCut->second.total()) 
           << " % of tracks" << endreq;
  }

  // must finalise base class
  return Rich::Rec::ToolBase::finalize();
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
  LHCb::RichRecSegment * nonc_seg = const_cast<LHCb::RichRecSegment*>(segment);
 
  bool trackIsIsolated(true);

  // the RICH and radiator type
  const Rich::RadiatorType rad  = segment->trackSegment().radiator();
  const Rich::DetectorType rich = segment->trackSegment().rich();
  if ( rad == Rich::Aerogel /* || rad == Rich::Rich1Gas */ ) 
  { Warning("Not yet implemented for "+Rich::text(rad),StatusCode::SUCCESS); return false; }

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

  //Cuts on exit momentum
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
  for ( RichRecSegments::const_iterator iSeg2 = richSegments()->begin();
        iSeg2 != richSegments()->end(); ++iSeg2 )
  {
    const RichRecSegment * segment2 = *iSeg2;

    if ( segment == segment2 ) continue;//check not comparing the segment with itself

    // The radiator type.  Must be same as main segment
    const Rich::RadiatorType rad1 = segment2->trackSegment().radiator();
    if ( rad1 != rad ) { continue; }

    //Track hit point (Ch ring centre)
    const Gaudi::XYZPoint & tkPtnLocal1 = segment2->pdPanelHitPointLocal();

    //compare outer loop segment entry to all other segments and find separation
    const double Xdifsq      = std::pow( fabs(tkPtnLocal.x()-tkPtnLocal1.x()), 2 );
    const double Ydifsq      = std::pow( fabs(tkPtnLocal.y()-tkPtnLocal1.y()), 2 );
    const double centreXYdif = std::sqrt(Xdifsq+Ydifsq);

    // If ring centre too close to any other, return false
    trackIsIsolated &= testCut( "track CK ring centre proximity", rad, centreXYdif > m_sizesepcut[rad] );
    if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;
    
  }//Segment comparison loop end

  int hittotal    = 0;
  int hitsoutside = 0;
  int hitsinside  = 0; //initialise all to zero for each segment

  IPixelCreator::PixelRange range = pixelCreator()->range(rich);//Container of pixels for this rich

  // Use PID average Ch theta for segment
  const double avtheta = m_ckAngle->avgCherenkovTheta( nonc_seg, pid );

  // Define ring around average Ch theta
  const double ringmin = avtheta-m_sizeringwidth[rad];
  const double ringmax = avtheta+m_sizeringwidth[rad];

  //Loop over all hit pixels
  for ( IPixelCreator::PixelRange::const_iterator iPixel = range.begin();
        iPixel != range.end(); ++iPixel )
  {
    RichRecPixel * pixel = *iPixel;//hit pixel

    const double sepsq = m_geomTool->trackPixelHitSep2(segment,pixel);//square of sep of pixel from track centre
    const double sep   = std::sqrt(sepsq);

    // Check hit pixel lies within reasonable distance of track centre
    if ( sep < m_maxROI[rad] )
    {
      ++hittotal; // Counting total hits near this segment

      //Estimated Ch theta for this pixel
      const double ckThetaEsti = sep*m_scale[rad];

      // Does hit lie outside ring region?
      if (ckThetaEsti>ringmax) { ++hitsoutside; }
      if (ckThetaEsti<ringmin) { ++hitsinside;  }

    } // boundary check
  }//loop over pixels

  if (hittotal==0) { hittotal = 1; } // To prevent dividing by zero
  const double frachitsout = (double)(hitsinside+hitsoutside) / (double)hittotal;
  trackIsIsolated &= testCut( "Hit fraction outside CK ring", rad, (1-frachitsout) > m_sizehitregioncut[rad] );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  //Photon association, position and total photon counters, initialise zero for each segment before looping over photons
  int photonassoc = 0;
  int photontotal = 0;

  // Tally vector for number of photons in each phi region
  std::vector<int> regTally(m_nPhiRegions[rad],0);

  //Loop over photons, calculations
  for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
        iPhot != segment->richRecPhotons().end(); ++iPhot )
  {

    LHCb::RichRecPhoton * photon = *iPhot;

    ++photontotal; //counting total photons associated with this segment

    // Which CK phi region
    const int region = (int)( m_nPhiRegions[rad] * photon->geomPhoton().CherenkovPhi() / (M_PI*2.0) );

    // count number in each region
    ++regTally[region];

    // No. of photons associated with this pixel (should be=1 if photon assigned to one track only)
    const unsigned int Nphotons = photon->richRecPixel()->richRecPhotons().size();

    // If this photon is associated with more than one track...
    if (Nphotons>1)
    {
      ++photonassoc;//counting photons associated with this track and at least one other track
    }
    
  }//photon loop end

  //Test how many photons associated with two tracks for this segment

  if ( photontotal==0 ) { photontotal = 1; }//to prevent dividing by zero
  
  // don't consider tracks with only one associated hit - optional
  trackIsIsolated &= testCut( "min # associated photons", rad, photontotal > 1 );
  if ( !trackIsIsolated && m_abortEarly ) return trackIsIsolated;

  const double fracphotonsassoc = (double)photonassoc/(double)photontotal;
  trackIsIsolated &= testCut( "photon multiple association", rad, 
                              (1-fracphotonsassoc) > m_sizephotonassoccut[rad] ); 
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

  if ( trackIsIsolated ) debug() << "Found isolated " << rad << " segment" << endreq;
  
  // return final status
  return trackIsIsolated;
}


//=============================================================================
