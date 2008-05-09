
//-----------------------------------------------------------------------------
/** @file RichIsolatedTrackTool.cpp
 *
 *  Implementation file for tool : Rich::Rec::IsolatedTrackTool
 *
 *  CVS Log :-
 *  $Id: RichIsolatedTrackTool.cpp,v 1.1 2008-05-09 13:58:18 jonrob Exp $
 *
 *  @author Susan Haines  Susan.Carol.Haines@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/02/2008
 */
//-----------------------------------------------------------------------------

// local
#include "RichIsolatedTrackTool.h"

// Gaudi
#include "GaudiKernel/ToolFactory.h"


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
  : Rich::Rec::HistoToolBase ( type, name, parent ),
    m_ckAngle           ( NULL ),
    m_geomEffic      (0),
    m_maxROI         ( Rich::NRadiatorTypes, 0 ),
    m_ckThetaMax     ( Rich::NRadiatorTypes, 0 ),
    m_sepGMax        ( Rich::NRadiatorTypes, 0 ),
    m_scale          ( Rich::NRadiatorTypes, 0 )
  {
  // Define interface for this tool
  declareInterface<IIsolatedTrack>(this);

  //Default cut values.  Can change in options file.
  declareProperty( "SizeMomCut", m_sizemomcut=20 );
  declareProperty( "SizeGeoCut", m_sizegeocut=0.7 );//geometric efficiency
  declareProperty( "SizeSepCut", m_sizesepcut = 260 );//track hit point separation
  declareProperty( "SizeXposnCut", m_sizeXexit=100);//RICH2 discontinuity
  declareProperty( "SizePhotonAssocCut", m_sizephotonassoccut=0.8);//min fraction not assoc with another track
  declareProperty( "SizeRingWidth", m_sizeringwidth=0.00944);//theta band width
  declareProperty( "SizePhiCut", m_sizephicut=0.2125);//max fraction of photons in one phi region
  declareProperty( "SizeHitRegionCut", m_sizehitregioncut=0.8);//min fraction of photons lying in theta band

  //Pixel hit regions
  m_maxROI[Rich::Aerogel]  = 390;
  m_maxROI[Rich::Rich1Gas] =  86;
  m_maxROI[Rich::Rich2Gas] = 200; //originally 165 - max sep of hit pixel from track centre to allow it to be considered
  declareProperty( "MaxTrackROI", m_maxROI );

  m_ckThetaMax[Rich::Aerogel]  = 0.24;
  m_ckThetaMax[Rich::Rich1Gas] = 0.0520;
  m_ckThetaMax[Rich::Rich2Gas] = 0.03;
  declareProperty( "CKthetaMax", m_ckThetaMax );

  m_sepGMax[Rich::Aerogel]   = 342;
  m_sepGMax[Rich::Rich1Gas]  = 75;
  m_sepGMax[Rich::Rich2Gas]  = 130;
  declareProperty( "SepGMax", m_sepGMax );

}



//=============================================================================
// Initialisation
//=============================================================================

StatusCode IsolatedTrackTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::HistoToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  // Get geometric efficiency tool
  acquireTool( "RichGeomEff", m_geomEffic  );
  //Get geometry tool
  acquireTool( "RichRecGeometry", m_geomTool );

  // loop over radiators
  for ( int radi = 0; radi < Rich::NRadiatorTypes; ++radi )
  {
      //numbers for hit pixel calculations
      m_scale[radi] = (m_ckThetaMax[radi]/m_sepGMax[radi]);
  }

  return sc;
}


//=============================================================================

bool IsolatedTrackTool::isIsolated( const LHCb::RichRecSegment * segment ) const
{
  return ( isIsolated( segment, Rich::Electron ) ||
           isIsolated( segment, Rich::Muon ) ||
           isIsolated( segment, Rich::Pion ) ||
           isIsolated( segment, Rich::Kaon ) ||
           isIsolated( segment, Rich::Proton ) );
}

bool IsolatedTrackTool::isIsolated( const LHCb::RichRecSegment * segment,
                                    const Rich::ParticleIDType pid ) const
{
  LHCb::RichRecSegment * nonc_seg = const_cast<LHCb::RichRecSegment*>(segment);

  // the radiator type.  Only RICH2 at the moment.
  const Rich::RadiatorType rad = segment->trackSegment().radiator();
  if ( rad != Rich::Rich2Gas ){
    return false;
  }

  // Get a reference to the Geometrical track object.
  const RichTrackSegment & trackSeg = segment->trackSegment();

  // Radiator exit point and momentum
  const Gaudi::XYZPoint & exitPoint = trackSeg.exitPoint();
  const Gaudi::XYZVector & exitMom   = trackSeg.exitMomentum();

  //Segment exit point
  double Xexit = exitPoint.x();
    
  //Exit cut
  double cutXexit = fabs(Xexit)-m_sizeXexit;

  if (cutXexit<0) {
    return false; //Track segment too close to central RICH2 HPD plane discontinuity
  }

  //Cuts on exit momentum
  double exitPx = exitMom.x();
  double exitPy = exitMom.y();
  double exitPz = exitMom.z();
  double exitPxsq = exitPx*exitPx;
  double exitPysq = exitPy*exitPy;
  double exitPzsq = exitPz*exitPz;
  double exitP = sqrt(exitPxsq+exitPysq+exitPzsq);

  double cutexitP = exitP-m_sizemomcut*Gaudi::Units::GeV;


  if (cutexitP<0){
    return false;//segment does not pass momentum cut
  }

  //Track hit point (if it were a photon) on detector plane, in local coords (i.e. centre of Ch ring)
  const Gaudi::XYZPoint & tkPtnLocal = segment->pdPanelHitPointLocal();
  double Xringcentre = tkPtnLocal.x();
  double Yringcentre = tkPtnLocal.y();


  //Loop over all segments to compare ring centre for the chosen segment with all others
  for ( RichRecSegments::const_iterator iSeg2 = richSegments()->begin();
        iSeg2 != richSegments()->end(); ++iSeg2 ) {
    const RichRecSegment * segment2 = *iSeg2;


    if ( segment == segment2 ) continue;//check not comparing the segment with itself

    // The radiator type.  Must also be RICH2.
    const Rich::RadiatorType rad1 = segment2->trackSegment().radiator();
    if ( rad1 != Rich::Rich2Gas ){
      continue;
    }

    //Track hit point (Ch ring centre)
    const Gaudi::XYZPoint & tkPtnLocal1 = segment2->pdPanelHitPointLocal();

    //this (inner loop) segment's ring centre
    double segXcentre = tkPtnLocal1.x();
    double segYcentre = tkPtnLocal1.y();

    //compare outer loop segment entry to all other segments and find separation
    double Xdifference = fabs(Xringcentre-segXcentre);
    double Ydifference = fabs(Yringcentre-segYcentre);
    double Xdifsq = Xdifference*Xdifference;
    double Ydifsq = Ydifference*Ydifference;
    double centreXYdif = sqrt(Xdifsq+Ydifsq);

    double cutringposition = centreXYdif - m_sizesepcut;


    //If ring centre too close to any other, return false
    if (cutringposition<0) {
      return false;
    }

  }//Segment comparison loop end

  const double geomEff = m_geomEffic->geomEfficiency( nonc_seg, pid );

  double cutgeoEff = 1.0;
  cutgeoEff = geomEff - m_sizegeocut;
   
  if (cutgeoEff<0) {
    return false;
  }
   
 
  double ringmin = 0.0;
  double ringmax = 0.0;
  int hittotal = 0;
  int hitsoutside = 0;
  int hitsinside = 0;//initialise all to zero for each segment
  double thetaExpTrue = 0.0; 
  double avtheta = 0.0;

  // Expected Cherenkov theta angle for particle type
  thetaExpTrue = ckAngleTool()->avgCherenkovTheta( nonc_seg, pid );
 

  const Rich::DetectorType rich = segment->trackSegment().rich();//Which RICH are we in? Hopefully 2!
  IPixelCreator::PixelRange range = pixelCreator()->range(rich);//Container of pixels for this rich

  //Use PID average Ch theta for segment
  avtheta = m_ckAngle->avgCherenkovTheta(nonc_seg, pid );
  
  //Define ring around average Ch theta
  double ringwidth = m_sizeringwidth;
  ringmin = avtheta-ringwidth;
  ringmax = avtheta+ringwidth;


  //Loop over all hit pixels
  for ( IPixelCreator::PixelRange::const_iterator iPixel = range.begin();
        iPixel != range.end(); ++iPixel )
  {
    RichRecPixel * pixel = *iPixel;//hit pixel

    const double sepsq = m_geomTool->trackPixelHitSep2(segment,pixel);//square of sep of pixel from track centre
    const double sep = sqrt(sepsq);

    // Which radiator?
    const Rich::RadiatorType rad2 = segment->trackSegment().radiator();

    // Check hit pixel lies within reasonable distance of track centre
    if ( sep < m_maxROI[rad2] )
    {
      hittotal = hittotal+1;//Counting total hits near this segment

      //Estimated Ch theta for this pixel
      const double ckThetaEsti = sep*m_scale[rad2];

      //Does hit lie outside ring region?
      double thetamaxicut = ckThetaEsti-ringmax;
      double thetaminicut = ckThetaEsti-ringmin;

      if (thetamaxicut>0){
        hitsoutside = hitsoutside+1;
      }

      if (thetaminicut<0){
        hitsinside = hitsinside+1;
      }

    } // boundary check
  }//loop over pixels

  //Test how many lie outside the "ring" area around average Ch theta
  int hitsoutofregion = hitsinside+hitsoutside;

  if (hittotal==0){
    hittotal = 1;
  }//To prevent dividing by zero

  double hittotaldoub = hittotal;
  double hitsoutofregiondoub = hitsoutofregion;
  double frachitsout = hitsoutofregiondoub/hittotaldoub;
  double cutfrachitsout = (1-frachitsout) - m_sizehitregioncut;

  if (cutfrachitsout<0){
    return false;
  }


  //Photon association, position and total photon counters, initialise zero for each segment before looping over photons
  int photonassoc = 0;
  int photontotal = 0;

  int photonphi1 = 0;
  int photonphi2 = 0;
  int photonphi3 = 0;
  int photonphi4 = 0;
  int photonphi5 = 0;
  int photonphi6 = 0;
  int photonphi7 = 0;
  int photonphi8 = 0;
  bool photonregionindicator = true;


  double pi = Gaudi::Units::pi;
  
  //Loop over photons, calculations
  for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
        iPhot != segment->richRecPhotons().end(); ++iPhot ){

    LHCb::RichRecPhoton * photon = *iPhot;

    photontotal = photontotal + 1;//counting total photons associated with this segment

    //Distribution of photons in phi
    double photonphi = photon->geomPhoton().CherenkovPhi();//reconstructed phi for this photon

    //Which phi region does photon lie in?
    //need to initialise region2 to some number or variable for each photon, won't get set a value if photons lie in a preceding region - will cause errors in ifs
    double region2 = 10;
    double region3 = 10;
    double region4 = 10;
    double region5 = 10;
    double region6 = 10;
    double region7 = 10;
    double region8 = 10;
    //Start with smallest phi region
    double region1 = photonphi-(0.25*pi);
       
    if (region1<0){
      photonphi1 = photonphi1+1;
      photonphi = 10;//prevents calculation of following ifs
    }
    else  region2 = photonphi-(0.5*pi);

    if (region2<0){
      photonphi2 = photonphi2+1;
      photonphi = 10;
    }
    else  region3 = photonphi-(0.75*pi);

    if (region3<0){
      photonphi3 = photonphi3+1;
      photonphi = 10;
    }
    else  region4 = photonphi-(1.0*pi);

    if (region4<0){
      photonphi4 = photonphi4+1;
      photonphi = 10;
    }
    else  region5 = photonphi-(1.25*pi);

    if (region5<0){
      photonphi5 = photonphi5+1;
      photonphi = 10;
    }
    else  region6 = photonphi-(1.5*pi);

    if (region6<0){
      photonphi6 = photonphi6+1;
      photonphi = 10;
    }
    else  region7 = photonphi-(1.75*pi);

    if (region7<0){
      photonphi7 = photonphi7+1;
      photonphi = 10;
    }
    else  region8 = photonphi-(2.0*pi);

    if (region8<0){
      photonphi8 = photonphi8+1;
      photonphi = 10;
    }


    // Pointer to photon's associated RichRecPixel
    RichRecPixel * pixel = photon->richRecPixel();
    if ( pixel ) { // Should never be null. This line is just to remove a compiler warning // }

      //No. of photons associated with this pixel (should be=1 if photon assigned to one track only)
      RichRecPixel::Photons & photons = pixel->richRecPhotons();
      int Nphotons = photons.size();

      //If this photon is associated with more than one track...
      if (Nphotons>1){
        ++photonassoc;//counting photons associated with this track and at least one other track
      }

    }//pixel
    }//photon loop end

    //Test how many photons associated with two tracks for this segment
   
    if (photontotal==0){
      photontotal = 1;
    }//to prevent dividing by zero

    if (photontotal==1){
      return false;
    }//don't consider tracks with only one associated hit - optional


    double photonassocdoub = photonassoc;
    double photontotaldoub = photontotal;
    double fracphotonsassoc = photonassocdoub/photontotaldoub;
    double cutfracphotonsassoc = (1-fracphotonsassoc) - m_sizephotonassoccut;

    if (cutfracphotonsassoc<0){
      return false;
    }

    //How many lie in each phi region?
    double photonphi1doub = photonphi1;
    double photonphi2doub = photonphi2;
    double photonphi3doub = photonphi3;
    double photonphi4doub = photonphi4;
    double photonphi5doub = photonphi5;
    double photonphi6doub = photonphi6;
    double photonphi7doub = photonphi7;
    double photonphi8doub = photonphi8;

    double fracphotonsphi1 = photonphi1doub/photontotaldoub;
    double fracphotonsphi2 = photonphi2doub/photontotaldoub;
    double fracphotonsphi3 = photonphi3doub/photontotaldoub;
    double fracphotonsphi4 = photonphi4doub/photontotaldoub;
    double fracphotonsphi5 = photonphi5doub/photontotaldoub;
    double fracphotonsphi6 = photonphi6doub/photontotaldoub;
    double fracphotonsphi7 = photonphi7doub/photontotaldoub;
    double fracphotonsphi8 = photonphi8doub/photontotaldoub;
      
    double cutfracphotonsphi1 = 0.0;
    double cutfracphotonsphi2 = 0.0;
    double cutfracphotonsphi3 = 0.0;
    double cutfracphotonsphi4 = 0.0;
    double cutfracphotonsphi5 = 0.0;
    double cutfracphotonsphi6 = 0.0;
    double cutfracphotonsphi7 = 0.0;
    double cutfracphotonsphi8 = 0.0;

    cutfracphotonsphi1 = fracphotonsphi1-m_sizephicut;
    cutfracphotonsphi2 = fracphotonsphi2-m_sizephicut;
    cutfracphotonsphi3 = fracphotonsphi3-m_sizephicut;
    cutfracphotonsphi4 = fracphotonsphi4-m_sizephicut;
    cutfracphotonsphi5 = fracphotonsphi5-m_sizephicut;
    cutfracphotonsphi6 = fracphotonsphi6-m_sizephicut;
    cutfracphotonsphi7 = fracphotonsphi7-m_sizephicut;
    cutfracphotonsphi8 = fracphotonsphi8-m_sizephicut;


    if ((cutfracphotonsphi1)>0){
      photonregionindicator = false;
    }
    if ((cutfracphotonsphi2)>0){
      photonregionindicator = false;
    }
    if ((cutfracphotonsphi3)>0){
      photonregionindicator = false;
    }
    if ((cutfracphotonsphi4)>0){
      photonregionindicator = false;
    }
    if ((cutfracphotonsphi5)>0){
      photonregionindicator = false;
    }
    if ((cutfracphotonsphi6)>0){
      photonregionindicator = false;
    }
    if ((cutfracphotonsphi7)>0){
      photonregionindicator = false;
    }
    if ((cutfracphotonsphi8)>0){
      photonregionindicator = false;
    }
      
    if (photonregionindicator==0){
      return false;
    }

    //If get to this point without returning then segment fulfils all criteria, i.e. is isolated
       
    return true;
}


//=============================================================================
