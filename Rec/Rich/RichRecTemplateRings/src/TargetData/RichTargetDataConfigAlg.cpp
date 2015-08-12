// Include files 

// local
#include "RichTargetDataConfigAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTargetDataConfigAlg
//
// 2007-05-14 : Sajan EASO
//-----------------------------------------------------------------------------
using namespace Rich::Rec::TemplateRings;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichTargetDataConfigAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

RichTargetDataConfigAlg::RichTargetDataConfigAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : RichRingRecAlgBase ( name , pSvcLocator ),
    m_trSelector        ( NULL )   
{
  declareProperty("SelectOnlyTracksAboveMomentumThreshold", m_selectTracksAboveMomentumThreshold=true);
  declareProperty("SelectOnlySaturatedTracksForRings",  m_selectSaturatedTracksForRings = false );
  declareProperty("SelectAllTrackSegments",  m_selectAllSegments = false );
  
}
//=============================================================================
// Destructor
//=============================================================================
RichTargetDataConfigAlg::~RichTargetDataConfigAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichTargetDataConfigAlg::initialize() {
  const StatusCode sc = RichRingRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  acquireTool( "TrackSelector",       m_trSelector, this );  
  
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichTargetDataConfigAlg::execute() {


    AcquireTargetRichHitInfo();
    AcquireTargetTrackInfo();
    
     // histogram the hits if the corresponding switch is activated.    
     if(rt()->RParam()->  ActivateSingleEvHisto()) {
       for(int irad = rt()->RParam()->MinRadiator(); irad <=  rt()->RParam()->MaxRadiator();    ++irad){
           rt()->HSingleEv()->PlotHitsAndTracksInEv(irad);
       }
     }
     
    
  return StatusCode::SUCCESS;
}
//=============================================================================
// get Target Hits Data
//=============================================================================
StatusCode RichTargetDataConfigAlg::AcquireTargetRichHitInfo() 
{
  StatusCode sc=  StatusCode::SUCCESS;
  rt()->tgD()->clearHitInputs();
  int aNumHitsInp = (int) richPixels()->size();
  //debug()<<" Rich AcquireTargerRichHitInfo() Number of RichHits "<<aNumHitsInp<<endmsg;

  
  //  if( aNumHitsInp > 0 ) {


    VC r1h; r1h.reserve(aNumHitsInp);
    VC r2h; r2h.reserve(aNumHitsInp);
    VI r1inv;  r1inv.reserve(aNumHitsInp);
    VI r2inv; r2inv.reserve(aNumHitsInp);  
    int r1s=0;
    int r2s=0;
 if( aNumHitsInp > 0 ) {
    // there are RICH hits in this event    

    // Loop through all the RICH hits
    for ( LHCb::RichRecPixels::const_iterator iPix = richPixels()->begin();
        iPix != richPixels()->end(); ++iPix ){
       const LHCb::RichRecPixel * pixel = *iPix;

      
      // local position on HPD panels
      const Gaudi::XYZPoint & lPos = pixel->localPosition();
      int invIndex = iPix-(richPixels()->begin());
      
      // Which detector ?
      //const Rich::DetectorType rich = pixel->detector();
      if( rt()->tgD()->RichDetNumFromHitZ( pixel->globalPosition().z()) == 0 ) {
        r1h.push_back(lPos);
        r1inv.push_back(invIndex);      
        r1s++;      
      }else {
        r2h.push_back(lPos);
        r2inv.push_back(invIndex);
        r2s++;      
      }
      
    }
    
 } else {
    
     debug() << "RichRingRec TargetData: No Rich Target hits data in this event " << endmsg;
    
  }
    
    // Store the Target info.
    debug()<<" Rich TargetDataConfigAlg Acquire target data hits rich1 rich2   "<<r1s<<"      "<<r2s<<endmsg;
    
     rt()->tgD()->setNumHitsInp(aNumHitsInp);
     rt()->tgD()->StoreTargetHitData(r1h,r1s,r1inv);
     rt()->tgD()->StoreTargetHitData(r2h,r2s,r2inv);
  
  
  return sc;
  
}

StatusCode RichTargetDataConfigAlg::AcquireTargetTrackInfo() 
{
  StatusCode sc=  StatusCode::SUCCESS;
   rt()->tgD()->clearTrackInputs();
  
  int aNumtk=-1;
  int aNumMaxTk= (int) richSegments()->size();
  // info()<<" Number of input segments in this event = "<<aNumMaxTk<<endmsg;
  
    VC t0h; VD m0h; t0h.reserve(aNumMaxTk); m0h.reserve(aNumMaxTk);
    VC t1h; VD m1h; t1h.reserve(aNumMaxTk); m1h.reserve(aNumMaxTk);
    VC t2h; VD m2h; t2h.reserve(aNumMaxTk); m2h.reserve(aNumMaxTk);
    int n0s=0;    int n1s=0; int n2s=0;
    VI ti0; ti0.reserve(aNumMaxTk);
    VI ti1; ti1.reserve(aNumMaxTk);
    VI ti2; ti2.reserve(aNumMaxTk);


  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;
    if(!m_selectAllSegments ) {
      
      if ( !m_trSelector->trackSelected( segment->richRecTrack() ) ) continue;
    }
    
    const Gaudi::XYZPoint & pdPoint = segment->pdPanelHitPoint();  // track proj on hpd panel in global coord

    if( !((pdPoint.x() ==0.0) && ( pdPoint.y() == 0.0 )) ) {  // avoid unphysical track coord.
      const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();
      const Rich::RadiatorType rad = tkSeg.radiator();   // which radiator

      double tkMomz=40000.0; // use a dummy positive value to start with.
      double tkTotMom=60000.0; // use a dummy value to start with.
      if( ! (rt()->RParam()->ActivateWithoutTrackMomentumInfo() ) ) {
         tkMomz = tkSeg.bestMomentum().z();
         tkTotMom = pow( (tkSeg.bestMomentum().Mag2()), 0.5);     // track reconstructed momentum
      }
      
      if(tkMomz >0.0 ) { // avoid tracks going backwards
       aNumtk++;


       bool SelectedforMom=true;
       if(m_selectTracksAboveMomentumThreshold) { // select only those tracks above a momentum threshold

          int irad=1;
          if(rad == Rich::Aerogel ) {
            irad=0;
          }else if ( rad== Rich::Rich2Gas ){
            irad=2;
          }


          if(rt()->RConst()->RichParticleCherenkovThreshold(irad,0) > tkTotMom ) {
           // test on the threshold for the first particle type
            SelectedforMom=false;
          }
          

          
       } // end test for Momentum threshold

       bool SelectedforSat=true;
       if(m_selectSaturatedTracksForRings) { // select only saturated tracks
         if( ( rad == Rich::Aerogel) && ( rt()->RParam()->MinTrackMomentumSelectInAerogel() > tkTotMom ))   SelectedforSat=false;
         if( ( rad == Rich::Rich1Gas) && ( rt()->RParam()->MinTrackMomentumSelectInRich1Gas() > tkTotMom )) SelectedforSat=false;
         if( ( rad == Rich::Rich2Gas) && ( rt()->RParam()->MinTrackMomentumSelectInRich2Gas() > tkTotMom )) SelectedforSat=false;
         
       }
       

       if( SelectedforMom && SelectedforSat ) {


         const Gaudi::XYZPoint & pdPointLocal = segment->pdPanelHitPointLocal();  
                                                         //track proj on hpd panel in local coord
       
         int invIndex = iSeg- (richSegments()->begin());
         if(rad==Rich::Aerogel ) {
           n0s++;   
           t0h.push_back(pdPointLocal);
           ti0.push_back(invIndex);
           m0h.push_back(  tkTotMom);         
         }else if (rad== Rich::Rich1Gas) {
           n1s++;
           t1h.push_back(pdPointLocal);
           ti1.push_back(invIndex);
           m1h.push_back(  tkTotMom);

         }else if (rad==Rich::Rich2Gas ) {
           n2s++;
           t2h.push_back(pdPointLocal);
           ti2.push_back(invIndex);
           m2h.push_back(  tkTotMom);

           // info() <<"Target acquire track track num "<<rad
           // <<"  "<<invIndex<<"  "<<aNumtk<<"  "<<n2s<<"  "<<pdPointLocal<<endmsg;
           
         }
         
       
       } // end selectedForMomentum
       
      }  
      
    }
    
    
  } // end loop on segments
  
  //  store the track info
  rt()->tgD()-> setNumChTrack(aNumtk);
  rt()->tgD()-> StoreTargetTrackData(n0s,t0h,ti0,m0h );
  rt()->tgD()-> StoreTargetTrackData(n1s,t1h,ti1,m1h);
  rt()->tgD()-> StoreTargetTrackData(n2s,t2h,ti2,m2h);
  

  return sc;
  
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichTargetDataConfigAlg::finalize() {

  return RichRingRecAlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
