// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <boost/assign/list_of.hpp> // for 'vector_list_of()'
// from Boost
#include <boost/lambda/bind.hpp>

// from VeloDet
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/DeVelo.h"
#include "Event/FitNode.h"

#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"


using namespace Gaudi::Units;
using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;


// local
#include "VAlign.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VAlign
//
// 2005-07-28 : Sebastien Viret
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( VAlign );

IPVOfflineTool* m_pvtool;


std::vector<const LHCb::Track*> myconvert( const SmartRefVector<LHCb::Track> & tracks )
{
  std::vector<const LHCb::Track*> rc(tracks.size()) ;
  std::copy(tracks.begin(),tracks.end(),rc.begin()) ;
  return rc ;
}


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VAlign::VAlign( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_vertexer("TrackVertexer")
// The following variables will be set via a joboptions files
{
  declareProperty("Internal_Alignment"        , m_step1 = true);
  declareProperty("Internal_Mod_Left"         , m_VELOmap_l = std::vector<bool>(20,1));
  declareProperty("Internal_Mod_Right"        , m_VELOmap_r = std::vector<bool>(21,1));
  declareProperty("Internal_DOF"              , m_align = std::vector<bool>(6,1));
  declareProperty("Internal_EQs"              , m_constrain  =  std::vector<bool>(9,1));
  declareProperty("Box_DOF"                   , m_alignb = std::vector<bool>(6,1));
  // collect all the vectors initializations and run ifdef block for switching from and to C++11
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  std::vector<double> sigmaVec = boost::assign::list_of(0.01)(0.01)(0.005)(0.0005)(0.0005)(0.0002);
  declareProperty("Internal_PTerms"           , m_sigma  =  sigmaVec);
  std::vector<double> resCutVec = boost::assign::list_of(0.3)(0.06);
  declareProperty("Internal_Residual_Cut"     , m_residual_cut = resCutVec);
  std::vector<double> sigmabVec = boost::assign::list_of(10.0)(10.0)(10.0)(0.03)(0.03)(0.03);
  declareProperty("Box_PTerms"                , m_sigmab = sigmabVec);
  std::vector<bool> boxVec = boost::assign::list_of(1)(1)(1)(1)(1)(0);
  declareProperty("Box_EQs"                   , m_constrainb = boxVec);
  std::vector<double> resCutbVec = boost::assign::list_of(0.5)(0.4);
  declareProperty("Box_Residual_Cut"          , m_residual_cutb = resCutbVec); 
#else  
  declareProperty("Internal_PTerms"           , m_sigma  =  boost::assign::list_of(0.01)(0.01)(0.005)
                  (0.0005)(0.0005)(0.0002));
  declareProperty("Internal_Residual_Cut"     , m_residual_cut = boost::assign::list_of(0.3)(0.06));
  declareProperty("Box_PTerms"                , m_sigmab = boost::assign::list_of(10.0)(10.0)(10.0)
                  (0.03)(0.03)(0.03));
  declareProperty("Box_EQs"                   , m_constrainb =boost::assign::list_of(1)(1)(1)(1)(1)(0));
  declareProperty("Box_Residual_Cut"          , m_residual_cutb = boost::assign::list_of(0.5)(0.4)); 
#endif
  declareProperty("Box_Alignment"             , m_step2 = true);
  declareProperty("Box_VELOopen"              , m_VELOopen = false);
  declareProperty("Box_MinTracks_perPV"       , m_PV_trackmin =10);
  declareProperty("General_Startfactor"       , m_starfactr= 100.0 );
  declareProperty("General_Maxtracks"         , m_maxtrack= 100 );
  declareProperty("Monitor_Constants"         , m_moni_constants = false);   
  declareProperty("Monitor_PV"                , m_moni_PV = false);  
  declareProperty("Monitor_Overlaps"          , m_moni_overlaps = false);  
  declareProperty("Monitor_Tracks"            , m_moni_tracks = false);  
  declareProperty("Monitor_Tracks_num"        , m_nTrackSample = 50);  
  declareProperty("Monitor_Events"            , m_moni_events = false);  
  declareProperty("TrackStoreTool"            , my_TrackStore = "TrackStore");
  declareProperty("MillepedeTool"             , my_Millepede  = "Millepede");
  declareProperty( "TrackContainer"           , m_trackContainerName = LHCb::TrackLocation::Velo );  
  declareProperty( "MinAdcPerTrack"           , m_minadcpertrack = 0. );
  declareProperty( "MaxTheta"                 ,m_maxtheta=4.);

}
//=============================================================================
// Destructor
//=============================================================================
VAlign::~VAlign() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VAlign::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  info() << "======== VAlign Settings ================ " << endmsg;
  info() << "Track Container           :"<<m_trackContainerName << endmsg;
  info() << "Internal_Alignment        :"<< m_step1 << endmsg;
  info() << "Internal_Mod_Left         :"<< m_VELOmap_l << endmsg;
  info() << "Internal_Mod_Right        :"<< m_VELOmap_r << endmsg;
  info() << "Internal_DOF              :"<< m_align << endmsg;
  info() << "Internal_PTerms           :"<< m_sigma << endmsg;
  info() << "Internal_EQs              :"<< m_constrain << endmsg;
  info() << "Internal_Residual_Cut     :"<< m_residual_cut << endmsg;
  info() << "Box_Alignment             :"<< m_step2 << endmsg;
  info() << "Box_VELOopen              :"<< m_VELOopen << endmsg;
  info() << "Box_DOF                   :"<< m_alignb << endmsg;
  info() << "Box_PTerms                :"<< m_sigmab << endmsg;
  info() << "Box_EQs                   :"<< m_constrainb << endmsg;
  info() << "Box_Residual_Cut          :"<< m_residual_cutb << endmsg;
  info() << "Box_MinTracks_perPV       :"<< m_PV_trackmin << endmsg;
  info() << "General_Startfactor       :"<< m_starfactr << endmsg;
  info() << "General_Maxtracks         :"<< m_maxtrack << endmsg;
  info() << "Monitor_Constants         :"<< m_moni_constants << endmsg;   
  info() << "Monitor_PV                :"<< m_moni_PV << endmsg;  
  info() << "Monitor_Overlaps          :"<< m_moni_overlaps << endmsg;  
  info() << "Monitor_Tracks            :"<< m_moni_tracks << endmsg;  
  info() << "Monitor_Tracks_num        :"<< m_nTrackSample << endmsg;  
  info() << "Monitor_Events            :"<< m_moni_events << endmsg;  
  info() << "TrackStoreTool            :"<< my_TrackStore << endmsg;
  info() << "MillepedeTool             :"<< my_Millepede  << endmsg;
  info() << "========================================= " << endmsg;

  nEvents = 0;          // Total number of events (event tag is necessary for box alignment)
  nTrackSample = 0;     // Number of track in the blind control sample

  nLeft_tracks  = 0;    // Number of left tracks
  nRight_tracks = 0;    // Number of right tracks

  VAlign::DefineVeloMap();         // Define the stations to be aligned
  VAlign::GetAlignmentConstants(); // Get the previous constants from the CondDB  


//  The TrackStore TOOL is performing the track selection before MILLEPEDE feeding
//  it transforms Tracks into AlignTracks objects which are formatted for MILLEPEDE
//
//  Selection cuts are set externally via the Joboption ($VELOALIGNOPTS/TrackStore.opts)  
//  Selection cuts for Pattern Recognition are set in $VELOALIGNOPTS/TRGtune.opts  

  
  my_tracks = tool<ITrackStore>(my_TrackStore);

  selected_tracks.clear(); // Containers of align tracks 
  control_tracks.clear();  // Control sample (tracks not used for alignment)
  overlap_tracks.clear();  // Containers of overlap tracks 


  vector_PV_tracks.clear();                // (needed for STEP2: PV tracks)
  vector_PV_tracksR.clear();                // (needed for STEP2: PV tracks)
  vector_PV_tracksL.clear();                // (needed for STEP2: PV tracks)
  my_PV_finder    = new PVHisto();  // The PV finding tool
  my_PV_finderR    = new PVHisto();  // The RPV finding tool
  my_PV_finderL    = new PVHisto();  // The LPV finding tool
  //to evaluate PV by linear fit
  PV_tracks_linfit.clear();                // (needed for STEP2: PV tracks)
  PV_tracksR_linfit.clear();                // (needed for STEP2: PV tracks)
  PV_tracksL_linfit.clear();                // (needed for STEP2: PV tracks)

  m_pvtool =tool<IPVOfflineTool>("PVOfflineTool", this);
  m_expectTool = tool<IVeloExpectation>( "VeloExpectation");

  sc = m_vertexer.retrieve() ;

  ntotaleTracks = 0;
  ntotaleTracks2 = 0;
  return sc;
  
  
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode VAlign::execute() {
  debug() << "==> Execute" << endmsg;

  ++nEvents;
  int nTracks = 0;

  // Get the Tracks
  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_trackContainerName  );
  //LHCb::Tracks* tracks = get<LHCb::Tracks>( LHCb::TrackLocation::Velo );
  //LHCb::Tracks* tracks = get<LHCb::Tracks>( LHCb::TrackLocation::Default );
  debug() << "Retrieved Tracks       : " << tracks->size() << endmsg;
  ntotaleTracks +=tracks->size();
  // Get event number from the ODIN bank

  if ( exist<LHCb::ODIN>( LHCb::ODINLocation::Default )){
    LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    debug() << "Run "     << odin->runNumber()
           << ", Event " << odin->eventNumber() << endmsg;
    m_runodin=odin->runNumber();
    m_eventodin= odin->eventNumber();
    m_bunchid= odin->bunchId();
    
  } else {
    Warning("No ODIN Header").ignore();
    m_runodin=0;
    m_eventodin=0;
    m_bunchid= 0;
  }
  
  int tracknumperev=tracks->size();
  LHCb::VeloClusters* m_rawClusters;
  if(!exist<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default)){
    return Error( " ==> There is no VeloClusters in TES " );
  }else{
    m_rawClusters=get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
  }
//   LHCb::VeloClusters* m_veloClusters;
//   m_veloClusters = get<LHCb::VeloClusters>( "Raw/Velo/Clusters" );
//   int hitnumperevent=m_veloClusters->size();
//   debug() << "Retrieved clusters       : " << hitnumperevent << endmsg;
  int hitnumperevent=0;
     
  if ( tracks->size() == 0 || tracks->size() >= m_maxtrack ){
    debug() << "Event skipped (busy or no tracks)" << endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::Tracks::const_iterator iTrack;
  int tralignnumperevent=0;
  int troverlapnumperevent=0;

  std::vector<const LHCb::Track*> trackvector;
  std::vector<const LHCb::Track*> lefttracks;
  std::vector<const LHCb::Track*> righttracks;

  for (iTrack = tracks->begin(); iTrack != tracks->end(); ++iTrack ){    
    ++nTracks;
    debug() <<nTracks<<" track"<<endmsg;
    LHCb::Track* my_fitted_track = *iTrack ;          // retrieve track
    if (my_fitted_track->firstState().slopes().theta()>m_maxtheta) continue;
    
    //int nExpectedHits = m_expectTool->nExpected(*my_fitted_track);
    //int nVeloHits = my_fitted_track->measurements().size();
    const std::vector<LHCbID>& vids = my_fitted_track->lhcbIDs();
    int nVeloHits = std::count_if(vids.begin(), vids.end(),bind(&LHCbID::isVelo,_1));
    double zmin= +800.;
    double zmax= -350.;
    unsigned int sensnrmin=105;
    unsigned int sensnrmax=0;
    unsigned int sensnpmin=105;
    unsigned int sensnpmax=0;
    float adcpertrack=0.;

    for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
      if (!iter->isVelo()){
        verbose()<<"No Velo Cluster"<<endmsg;
        continue;
      }
      verbose()<<"Velo Cluster"<<endmsg;
      LHCb::VeloCluster *cluster;
      cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject( (iter)->channelID() ); 
      adcpertrack+=cluster->totalCharge();

      if (iter->isVelo()){
        //evaluation of the z of first (zmin) and last (zmax) hit
        VeloChannelID chan = iter->veloID(); 
        const DeVeloSensor* sensor = m_velo->sensor(chan);
        if (sensor->z() <zmin)
          zmin = sensor->z();
        if ((chan.sensor()<sensnrmin) && (chan.sensor()<64))
          sensnrmin=chan.sensor();
        if ( (chan.sensor()<sensnpmin) && (chan.sensor()>=64))
          sensnpmin = chan.sensor();
        if (sensor->z() > zmax)
          zmax = sensor->z();
        if ((chan.sensor()>sensnrmax) && (chan.sensor()<64))
          sensnrmax=chan.sensor();
        if ( (chan.sensor()>sensnpmax) && (chan.sensor()>=64))
          sensnpmax = chan.sensor();
      }
    }  // loop ids  
    unsigned int sensnmin=105, sensnmax=0;
    sensnmax= ((sensnpmax-64)>sensnrmax) ? (sensnpmax-64) : sensnrmax ;
    sensnmin= ((sensnpmin-64)<sensnrmin) ? (sensnpmin-64) : sensnrmin ;
    debug()<<" zmin=" <<zmin <<" zmax="<<zmax<<" sensnmin="<<sensnmin<<" sensnmax="<<sensnmax<<endmsg;
    //Evaluation of expected hit per sensor
    //Loop over sensors
    int nExpectedHits =0;
    for(unsigned int j=(sensnmin); j<=sensnmax; j++){
      debug()<<"sensor number "<<j<<endmsg;
      if (j!=6){
        bool nExpectedHits_sensorR = m_expectTool -> isInside(*my_fitted_track, j);
        if ( nExpectedHits_sensorR)
          nExpectedHits+=1;
      }
      if (((j+64)!=65) && ((j+64)!=91) && ((j+64)!=93)){
        bool nExpectedHits_sensorP = m_expectTool -> isInside(*my_fitted_track, j+64);
        if ( nExpectedHits_sensorP)
          nExpectedHits+=1;
      }
    }//end of loop over sensors
    adcpertrack = adcpertrack/nVeloHits;
    if (adcpertrack < m_minadcpertrack)
      continue;
    

    debug() <<"Number of expected hits "<< nExpectedHits <<endmsg;

    /////trackvector.push_back(*iTrack);
    
    
    VeloTrack my_align_track  = VeloTrack();  // create AlignTrack 
    
    my_align_track.setNEvent(nEvents);
    my_align_track.setNTrack(nTracks);
      
    //   Select the track according to the cuts defined in $VELOALIGNOPTS/TrackStore.opts	  
    //   and the geometry defined in $VELOALIGNOPTS/VeloAlign.opts  
    debug() <<"Transform track into VeloTrack for Align " <<endmsg;
    my_tracks->TransformTrack(my_fitted_track,my_align_track,&VELOmap[0]); 
    debug() <<"Check type of track " <<endmsg;


     //test silvia
    if (my_align_track.nType() == 0)
      lefttracks.push_back(*iTrack);   
    if (my_align_track.nType() == 1)
      righttracks.push_back(*iTrack);
    if (my_align_track.nType() == 0 || my_align_track.nType() == 1)
      trackvector.push_back(*iTrack);


    if (m_moni_tracks){
      VAlign::fill_trackmoni(my_align_track,0);
      VAlign::fill_params2(my_align_track,0, nVeloHits,adcpertrack);
      VAlign::fill_params3(my_align_track,0,nExpectedHits,nVeloHits,adcpertrack);
      ntotaleTracks2 +=1;
    }
      
    if (my_align_track.nIsGood())  
    {
      debug() <<"Good track"<<endmsg;
      if (my_align_track.nType() == 0 || my_align_track.nType() == 1)
      {
        debug() <<"Right or Left track"<<endmsg;
        if (m_moni_tracks && (nTrackSample < m_nTrackSample)) // Request control sample
        {
          nTrackSample++;
          control_tracks.push_back(my_align_track); 	    
          VAlign::fill_params(my_align_track,0);	 
          debug() <<"Moni of control tracks"<<endmsg;
        }
        else
        {
          if (my_align_track.nType() == 0){
            nLeft_tracks++;
            //lefttracks.push_back(*iTrack);   
          }
          if (my_align_track.nType() == 1){
            nRight_tracks++; 
            //righttracks.push_back(*iTrack);
          }
          //trackvector.push_back(*iTrack);

          selected_tracks.push_back(my_align_track);
          tralignnumperevent+=1;
          debug() <<"Add new track for alignment"<<endmsg;
        }
      }
        
      if (my_align_track.nType() == 2 && m_step2) // Overlap tracks (monitored by default)
      {
        debug() <<"Overlap track"<<endmsg;
        if (m_moni_overlaps) VAlign::fill_overlaps(my_align_track,0);	  
        overlap_tracks.push_back(my_align_track);
        troverlapnumperevent+=1;
      }
      //         trackvector.push_back(*iTrack);
      //         if (my_align_track.nType() == 0) lefttracks.push_back(*iTrack);
      //         if (my_align_track.nType() == 1) righttracks.push_back(*iTrack);      
    }
    else
        debug() <<"No good track"<<endmsg;
  } // End of loop on event tracks
  
  debug() <<"Event Monitor"<<endmsg;
  if (m_moni_events)  VAlign::fill_infoevent( nEvents, tracknumperev,hitnumperevent,
                                             tralignnumperevent, troverlapnumperevent);
  
  debug() <<"Looking for Vertices in Right or Left side"<<endmsg;

  //Evaluate PV for Left and Right side using only 'good' tracks for alignment
  //Select PV with at least m_PV_trackmin tracks in right side and  m_PV_trackmin tracks in left side
  //we assume that this PV is the same and we use these tracks for PV alignment 
  //but we exclude events where we have more than one PV in right side or in left side
  
  debug() <<lefttracks.size()<<" Left tracks and "<<righttracks.size() <<" right tracks"<<endmsg;

  std::vector<LHCb::RecVertex> outvtxvec;
  std::vector<LHCb::RecVertex> leftoutvtxvec;
  std::vector<LHCb::RecVertex> rightoutvtxvec;

    
  StatusCode scfit = m_pvtool->reconstructMultiPVFromTracks(trackvector, outvtxvec);
  if (scfit != StatusCode::SUCCESS ) {
    error() <<"Bad PV fit result"<<endmsg;
    return StatusCode::SUCCESS;
  }
  debug() <<"Reconstruct PV"<<endmsg;


  /*  
  //StatusCode scfit = m_pvtool->reconstructMultiPV(outvtxvec);

//   std::vector<const LHCb::Track*> trackvector2;
//   LHCb::Tracks::const_iterator iTrack;
//   for (iTrack = (*(*((ipvOff->tracks()).begin()))); 
//        iTrack != (ipvOff->tracks()).end(); ++iTrack )    
//     trackvector2.push_back(*iTrack);
  if (pvcontainer->size()==1){
    std::vector<LHCb::RecVertex> outvtxvec2;
    debug() <<"Retrieve PV"<<endmsg;
    LHCb::RecVertex* ipvOff=(*(pvcontainer->begin()));
    debug() <<pvcontainer->size()<<" PV in this event"<<endmsg;
    std::vector<const LHCb::Track*> trackvector2= myconvert((ipvOff->tracks()));
    if (trackvector2.size()> m_PV_trackmin){
    StatusCode scfit2 = m_pvtool->reconstructMultiPVFromTracks(trackvector2, outvtxvec2);
    if (scfit != StatusCode::SUCCESS && scfit2 != StatusCode::SUCCESS) {
      error() <<"Bad PV fit result"<<endmsg;
      return StatusCode::SUCCESS;
    }
    
    //vertex with track get from 'official PV
    LHCb::RecVertex ipvMultiPVfit=*(outvtxvec2.begin());
    //vertex with all tracks (except in case align selection) by offline Tool
    LHCb::RecVertex ipvMyMultiPVfit=*(outvtxvec.begin());


    info() <<pvcontainer->size()<<" Official PV, 1st one    =(" <<
      ipvOff->position().x()<<","<<ipvOff->position().y() << ") Ntr="<<(ipvOff->tracks()).size()<<endmsg;

    std::vector<const LHCb::Track*> PVtracks_temp = myconvert(ipvOff->tracks()) ;
    //   for (std::vector<const LHCb::Track*>::const_iterator pvTrack = PVtracks_temp.begin() ; 
    //        pvTrack != PVtracks_temp.end(); ++pvTrack)
    //     info()<<"Track Position 1st state "<<((*pvTrack)->position()).x() <<" "<<
    //       ((*pvTrack)->position()).y() <<" "<< ((*pvTrack)->position()).z()<<endmsg;  
//     for (std::vector<const LHCb::Track*>::const_iterator pvTrack = PVtracks_temp.begin() ; 
//          pvTrack != PVtracks_temp.end(); ++pvTrack)
//       info()<<(*pvTrack)->nLHCbIDs() <<" Track Slopes 1st state "<<((*pvTrack)->slopes()).x() <<" "<<
//         ((*pvTrack)->slopes()).y() <<" "<< ((*pvTrack)->slopes()).z()<<endmsg;  
    
    info()<<outvtxvec2.size()   <<" Offline Fit PV, 1st one =(" <<
      ipvMultiPVfit.position().x()<<","<<ipvMultiPVfit.position().y() << ") Ntr="<<(ipvMultiPVfit.tracks()).size()<<endmsg;

    std::vector<const LHCb::Track*> PVtracks_temp2 = myconvert(ipvMultiPVfit.tracks()) ;
//     for (std::vector<const LHCb::Track*>::const_iterator pvTrack = PVtracks_temp2.begin() ; 
//          pvTrack != PVtracks_temp2.end(); ++pvTrack)
//       info()<<"Track Position 1st state "<<((*pvTrack)->position()).x() <<" "<<
//         ((*pvTrack)->position()).y() <<" "<< ((*pvTrack)->position()).z()<< endmsg;  
//     for (std::vector<const LHCb::Track*>::const_iterator pvTrack = PVtracks_temp2.begin() ; 
//          pvTrack != PVtracks_temp2.end(); ++pvTrack)
//       info()<<(*pvTrack)->nLHCbIDs() <<" Track Slopes 1st state "<<((*pvTrack)->slopes()).x() <<" "<<
//         ((*pvTrack)->slopes()).y() <<" "<< ((*pvTrack)->slopes()).z()<< endmsg;  
    
    info()<<outvtxvec.size()   <<" My Off  Fit PV, 1st one =(" <<
      ipvMyMultiPVfit.position().x()<<","<<ipvMyMultiPVfit.position().y() << ") Ntr="<<
      (ipvMyMultiPVfit.tracks()).size()<<endmsg;
    std::vector<const LHCb::Track*> PVtracks_temp3 = myconvert(ipvMyMultiPVfit.tracks()) ;
//     for (std::vector<const LHCb::Track*>::const_iterator pvTrack = PVtracks_temp3.begin() ; 
//          pvTrack != PVtracks_temp3.end(); ++pvTrack)
//       info()<<"Track Position 1st state "<<((*pvTrack)->position()).x() <<" "<<
//         ((*pvTrack)->position()).y() <<" "<< ((*pvTrack)->position()).z()<< endmsg;  
//     for (std::vector<const LHCb::Track*>::const_iterator pvTrack = PVtracks_temp3.begin() ; 
//          pvTrack != PVtracks_temp3.end(); ++pvTrack)
//       info()<<(*pvTrack)->nLHCbIDs() <<" Track Slopes 1st state "<<((*pvTrack)->slopes()).x() <<" "<<
//         ((*pvTrack)->slopes()).y() <<" "<< ((*pvTrack)->slopes()).z()<< endmsg;  
    }
  }
  
  */


  debug() <<"PV on left side"<<endmsg;
  m_pvtool->reconstructMultiPVFromTracks(righttracks, rightoutvtxvec);
  debug() <<"Looking for Vertices in Right side"<<endmsg;
  m_pvtool->reconstructMultiPVFromTracks(lefttracks, leftoutvtxvec);

  debug() <<"Looking for Vertices in Right and Left side"<<endmsg;
  int n_goodPV=0;
  int n_goodleftPV=0;
  int n_goodrightPV=0;
  
  debug() <<"Loop on Vertices"<<endmsg; 
  //Counts number of PV with at least m_PV_trackmin tracks per side  
  LHCb::RecVertex all_pv;
  LHCb::RecVertex left_pv;
  LHCb::RecVertex right_pv;
  for( std::vector<LHCb::RecVertex>::const_iterator ipv = outvtxvec.begin() ;
       ipv != outvtxvec.end(); ++ipv ) 
    if( myconvert(ipv->tracks()).size() >= (2*m_PV_trackmin)){
      n_goodPV+=1;
      all_pv=(*ipv);
    }
  
  for( std::vector<LHCb::RecVertex>::const_iterator ipv = leftoutvtxvec.begin() ;
       ipv != leftoutvtxvec.end(); ++ipv ) 
    if( myconvert(ipv->tracks()).size() >= m_PV_trackmin){
      n_goodleftPV+=1;
      left_pv=(*ipv);
    }
  for( std::vector<LHCb::RecVertex>::const_iterator ipv = rightoutvtxvec.begin() ;
       ipv != rightoutvtxvec.end(); ++ipv ) 
    if( myconvert(ipv->tracks()).size() >= m_PV_trackmin) {
      n_goodrightPV+=1;
      right_pv=(*ipv);
    }
  
  debug() <<"Found "<<n_goodPV << " PV and "<<n_goodrightPV << " Right PV and "<<
    n_goodleftPV << " Left PV"<<endmsg; 

  //use the events with only one PV in left and right sides with at least m_PV_trackmin
  //if( (n_goodleftPV==1) && (n_goodrightPV==1))
  //silvia test
  if((leftoutvtxvec.size()==1) && (rightoutvtxvec.size()==1) && (n_goodleftPV==1) && (n_goodrightPV==1))
  {  
    std::vector<const LHCb::Track*> vector_rightleftPV_track;
    
    //loop on vertex on left side
    if(n_goodleftPV==1){
      LHCb::RecVertex* ipv=&left_pv;
      std::vector<const LHCb::Track*> PVtracks = myconvert(ipv->tracks()) ;
      if( PVtracks.size() >= m_PV_trackmin ) {
        // fit two vertices
        debug() <<"Retrieve tracks"<<endmsg;  
        std::vector<const LHCb::Track*>::const_iterator  pvTrack;
        for (pvTrack = PVtracks.begin() ; pvTrack != PVtracks.end(); ++pvTrack){
          LHCb::Track* my_fitted_track =(LHCb::Track*) (*pvTrack) ;          // retrieve track
          debug() <<"new Track"<<endmsg;
          VeloTrack my_align_track  = VeloTrack();  // create AlignTrack 
          my_align_track.setNEvent(nEvents);
          my_align_track.setNTrack(nTracks);
          debug() <<"Event="<<nEvents<<" Track n="<<nTracks<<endmsg;
          my_tracks->TransformTrack(my_fitted_track,my_align_track,&VELOmap[0]);
          my_align_track.setNPV_x(ipv->position().x()); 
          my_align_track.setNPV_y(ipv->position().y()); 
          my_align_track.setNPV_z(ipv->position().z()); 
          my_align_track.setNPV_chi((ipv->chi2()/ipv->nDoF()));
          debug() <<"Track is transformed"<<endmsg;
          //test silvia
            if (my_align_track.nType() == 0){
              PV_tracksL.push_back(my_align_track);
              PV_tracks.push_back(my_align_track);
              vector_rightleftPV_track.push_back(my_fitted_track);
              debug() <<"Left track good for alignment"<<endmsg;
            }
            
//           if (my_align_track.nIsGood())
//             if (my_align_track.nType() == 0){
//               PV_tracksL.push_back(my_align_track);
//               PV_tracks.push_back(my_align_track);
//               vector_rightleftPV_track.push_back(my_fitted_track);
//               debug() <<"Left track good for alignment"<<endmsg;
//             }
        }
      }
    }
    if (PV_tracksL.size()>0){
      vector_PV_tracksL.push_back(PV_tracksL);
      debug() <<"Fill left tracks"<<endmsg;
    }
    else
      debug() <<"No good left tracks"<<endmsg;

    //loop  on vertex on right side
    if(n_goodrightPV==1){
      LHCb::RecVertex* ipv=&right_pv;      
      std::vector<const LHCb::Track*> PVtracks = myconvert(ipv->tracks()) ;
      if( PVtracks.size() >= m_PV_trackmin ) {
        debug() <<"Retrieve tracks"<<endmsg;  
        std::vector<const LHCb::Track*>::const_iterator  pvTrack;
        for (pvTrack = PVtracks.begin() ; pvTrack != PVtracks.end(); ++pvTrack){
          LHCb::Track* my_fitted_track =(LHCb::Track*) (*pvTrack) ;          // retrieve track
          debug() <<"new Track"<<endmsg;
          VeloTrack my_align_track  = VeloTrack();  // create AlignTrack 
          my_align_track.setNEvent(nEvents);
          my_align_track.setNTrack(nTracks);
          debug() <<"Event="<<nEvents<<" Track n="<<nTracks<<endmsg;
          my_tracks->TransformTrack(my_fitted_track,my_align_track,&VELOmap[0]);
          my_align_track.setNPV_x(ipv->position().x()); 
          my_align_track.setNPV_y(ipv->position().y()); 
          my_align_track.setNPV_z(ipv->position().z()); 
          my_align_track.setNPV_chi((ipv->chi2()/ipv->nDoF()));
          debug() <<"Track is transformed"<<endmsg;
          //test silvia
            if (my_align_track.nType() == 1){
              PV_tracksR.push_back(my_align_track);
              PV_tracks.push_back(my_align_track);
              vector_rightleftPV_track.push_back(my_fitted_track);
              debug() <<"Right Track good for alignment"<<endmsg;
            }

//           if (my_align_track.nIsGood())
//             if (my_align_track.nType() == 1){
//               PV_tracksR.push_back(my_align_track);
//               PV_tracks.push_back(my_align_track);
//               vector_rightleftPV_track.push_back(my_fitted_track);
//               debug() <<"Right Track good for alignment"<<endmsg;
//             }
        }
      }  
    }
    if (PV_tracksR.size()>0){
      vector_PV_tracksR.push_back(PV_tracksR);
      debug() <<"Fill right tracks"<<endmsg;
    }
    else
      debug() <<"No good right tracks"<<endmsg;

    //fit together left and right tracks to determine 'unbiased' vertex
    LHCb::RecVertex* revertex = m_vertexer->fit(  vector_rightleftPV_track ) ;
    for (VeloTracks::iterator itrack = PV_tracks.begin(); 
         itrack != PV_tracks.end(); ++itrack ) {
          itrack->setNPV_x(revertex->position().x()); 
          itrack->setNPV_y(revertex->position().y()); 
          itrack->setNPV_z(revertex->position().z()); 
          itrack->setNPV_chi((revertex->chi2()/revertex->nDoF()));
    }
  
    if (PV_tracks.size()>0){
      vector_PV_tracks.push_back(PV_tracks);
      debug() <<"Fill right tracks"<<endmsg;
    }
    else
      debug() <<"No good right tracks"<<endmsg;
    
    PV_tracksR.clear();
    PV_tracksL.clear();
    PV_tracks.clear();
  }
  else{
    debug() <<"No Vertice good for alignment"<<endmsg;  
  }

  debug() <<"End execute"<<endmsg;
  //return sc;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VAlign::finalize() {

  debug() << "==> Finalize" << endmsg;

  misal_left.resize(126);  // This vector will receive the alignment constants
  error_left.resize(126);  // This vector will receive the alignment errors
  pulls_left.resize(126);  // This vector will receive the alignment pulls

  misal_right.resize(126);  
  error_right.resize(126);  
  pulls_right.resize(126);  

  misal_box.resize(18);  
  error_box.resize(18);  
  pulls_box.resize(18); 

  for (int j=0; j<126; j++)
  {
    misal_right[j] = 0.;
    misal_left[j] = 0.;
  }


  
  //  Millepede TOOL configuration is contained in a MilleConfig object 
  //  which is defined externally via the Joboption ($VELOALIGNOPTS/Millepede.opts)

  my_align  = tool<IMillepede>(my_Millepede);
  my_Config = new MilleConfig();  

  if (m_step1)
  {

    // Set the configuration for the left box internal alignment

    my_Config->InitMilleTool(my_align,0, m_align, m_sigma, m_constrain, 
                             4, m_starfactr, 3, m_residual_cut, &VELOmap[0],
                             nLeft_tracks);

    VAlign::FindAlignment(my_Config);           // Make the global fit 
  
    int n_stat  = my_Config->GetNstations();   // The total number of stations to align
  
    for (int i=0;i<21;i++) 
    {
      if (m_VELOmap_l[i])  // Useless otherwise
      {
        for (int jj=0; jj<6; jj++)
        {
          misal_left[i+jj*21] = mis_const[my_Config->Get_CorrectStation(i)+jj*n_stat];
          error_left[i+jj*21] = mis_error[my_Config->Get_CorrectStation(i)+jj*n_stat];
          pulls_left[i+jj*21] = mis_pull[my_Config->Get_CorrectStation(i)+jj*n_stat];
        }
      }
      else
      {
        for (int jj=0; jj<6; jj++)
        {
          misal_left[i+jj*21] = -999.;
          error_left[i+jj*21] = -999.;
          pulls_left[i+jj*21] = -999.;
        }
      }
    }
    
    if (m_moni_constants) VAlign::fill_misalignments(misal_left, error_left, pulls_left, 2);
    
    // Now set the configuration for the right box internal alignment
    
    my_Config->InitMilleTool(my_align,1, m_align, m_sigma, m_constrain, 
                             4, m_starfactr, 3, m_residual_cut, &VELOmap[0],
                             nRight_tracks);

    VAlign::FindAlignment(my_Config);           // Make the global fit
    
    n_stat  = my_Config->GetNstations();   // The total number of stations to align

    for (int i=0;i<21;i++) 
    {
      if (m_VELOmap_r[i])  // Useless otherwise
      {
        for (int jj=0; jj<6; jj++)
        {
          misal_right[i+jj*21] = mis_const[my_Config->Get_CorrectStation(i)+jj*n_stat];
          error_right[i+jj*21] = mis_error[my_Config->Get_CorrectStation(i)+jj*n_stat];
          pulls_right[i+jj*21] = mis_pull[my_Config->Get_CorrectStation(i)+jj*n_stat];
        }
      }
      else
      {
        for (int jj=0; jj<6; jj++)
        {
          misal_right[i+jj*21] = -999.;
          error_right[i+jj*21] = -999.;
          pulls_right[i+jj*21] = -999.;
        }
      }
    }
    
    if (m_moni_constants) VAlign::fill_misalignments(misal_right, error_right, pulls_right, 3);
  }  
  
  if (m_moni_tracks)  // Monitor the residuals of the control sample AFTER alignment
  {
    VeloTracks::const_iterator itrack;
    int iteration = 0;
    
    for (itrack = control_tracks.begin(); itrack != control_tracks.end(); ++itrack ) 
    {
      VeloTrack my_corrected_track  = VeloTrack();  // create AlignTrack 
	
      my_corrected_track.setNEvent(control_tracks[iteration].nEvent());
	
      my_Config->correcTrack(control_tracks[iteration],my_corrected_track,
			     misal_left,misal_right,&VELOmap[0]);

      VAlign::fill_params(my_corrected_track,1);	 

      iteration++;
    }
//     //Silvia ???????? comment or not 
//     iteration = 0;
//     for (itrack = selected_tracks.begin(); itrack != selected_tracks.end(); ++itrack ) 
//     {
//       VeloTrack my_corrected_track  = VeloTrack();  // create AlignTrack 
	
//       my_corrected_track.setNEvent(selected_tracks[iteration].nEvent());
	
//       my_Config->correcTrack(selected_tracks[iteration],my_corrected_track,
//                              misal_left,misal_right,&VELOmap[0]);

//      iteration++;
//    }

  }

      
  // Finally perfom the box alignment

  if (m_step2)
  {
    for (int i=0;i<18;i++) misal_box[i] = 0.;

    if (m_VELOopen) // Just a quick check
    {
      for (int i=0;i<6;i++) m_constrainb[i] = false;
    }    

    my_Config->InitBox(my_align,m_alignb, m_sigmab, m_constrainb, 
                       4.*m_starfactr, m_residual_cutb, 
                       (zmoy_R+zmoy_L)/2,2*nEvents);
  
    VAlign::FindAlignment(my_Config);  // Make the global fit  

    for (int i=0;i<18;i++) 
    {
      misal_box[i] = mis_const[i];  
      error_box[i] = mis_error[i];  
      pulls_box[i] = mis_pull[i];  
    }
    
    if (m_moni_constants) 
      VAlign::fill_misalignments(misal_box, error_box, pulls_box, 6);
  }   
  
  VAlign::updateConditions( misal_left, misal_right, misal_box, error_left, error_right );
  
  info() << "Total Number of retrieved tracks " << ntotaleTracks <<endmsg;
  info() << "Total Number of monitored tracks " << ntotaleTracks2 <<endmsg;
  info() << "Total Number of alignment tracks " << (nRight_tracks + nLeft_tracks  + overlap_tracks.size()) <<endmsg;
  
  info() << " Num. of R tracks= " << nRight_tracks <<" Num. of L tracks= " << nLeft_tracks << endmsg;
  info() << " Num. of overlap tracks= " << overlap_tracks.size() << endmsg;
  
  return  m_vertexer.release() && GaudiTupleAlg::finalize();  // must be called after all other actions
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


//-----------------------------------------------------------
//  FINDALIGNMENT:  perform global params fit
//-----------------------------------------------------------
//
//  my_config    = the alignment configuration
//
//-----------------------------------------------------------


StatusCode VAlign::FindAlignment(MilleConfig *my_config) 
{
  
  MsgStream log(msgSvc(), name());
  
  int n_stat  = my_config->GetNstations();   // The total number of stations to align
  
  mis_const.resize(6*n_stat);
  mis_error.resize(6*n_stat);
  mis_pull.resize(6*n_stat);
  
  for (int j=0; j<6*n_stat; j++)
  {
    mis_const[j] = 0.;
    mis_error[j] = 0.;
    mis_pull[j]  = 0.;
  }

  debug() << "Entering FindAlignment" << endmsg;
  
  // First we perform loop on tracks or vertices, depending which alignment step we are considering
  
  VeloTracks::const_iterator itrack;
  
  if (my_config->isInternal())   // Internal Alignment 
  {
    int iteration = 0;
    
    for (itrack = selected_tracks.begin(); itrack != selected_tracks.end(); 
         ++itrack ) {
      if (selected_tracks[iteration].nType() == my_config->isLeft())
        my_config->PutTrack(selected_tracks[iteration],my_align);
      
      iteration++;
    }
  }
  else // Box alignment
  {

    // Start by putting in the overlap tracks 

    if (!m_VELOopen) // Don't use overlap in VELO open mode
    {
      int iteration = 0;

      for (itrack = overlap_tracks.begin(); itrack != overlap_tracks.end(); ++itrack ) 
      {
        VeloTrack my_corrected_track = VeloTrack();  // create AlignTrack 
        
        my_corrected_track.setNEvent(overlap_tracks[iteration].nEvent());
        
        my_config->correcTrack(overlap_tracks[iteration],my_corrected_track,
                               misal_left,misal_right,&VELOmap[0]);
        
        if (my_corrected_track.nIsGood()) 
          my_config->PutOverlapTrack(my_corrected_track,my_align);
        
        iteration++;
      }
    }

    //Loop on all tracks and put in PV_tracks the tracks that are no overlap track and 
    //that are good track 
    std::vector<VeloTracks>::iterator i_pvTrackVector;
    int npv=1;
    for( i_pvTrackVector= vector_PV_tracks.begin();
         i_pvTrackVector != vector_PV_tracks.end(); ++i_pvTrackVector, ++npv){
      PV_tracks_corrected.clear();
      PV_tracksR_corrected.clear();
      PV_tracksL_corrected.clear();
      PV_tracks_linfit.clear();
      PV_tracksR_linfit.clear();
      PV_tracksL_linfit.clear();
      debug() << npv << " vertex has "<<  i_pvTrackVector->size() << " tracks" <<endmsg;
      
      for (VeloTracks::iterator jtrack = i_pvTrackVector->begin(); jtrack != i_pvTrackVector->end(); ++jtrack)
      {
        debug()<<"Track type= " << jtrack->nType() <<
          "Is this track good? " << jtrack->nIsGood() << endmsg;
        //no overlap tracks
        if(jtrack->nType() < 2){ 
          VeloTrack my_corrected_track  = VeloTrack();  // create AlignTrack 
          
          my_corrected_track.setNEvent(npv);
          
          my_config->correcTrack((*jtrack),my_corrected_track,
                                 misal_left,misal_right,&VELOmap[0]);

          my_corrected_track.GetTrackSlope();

          my_corrected_track.setNPV_x((*jtrack).nPV_x());
          my_corrected_track.setNPV_y((*jtrack).nPV_y());
          my_corrected_track.setNPV_z((*jtrack).nPV_z());

          //for PV method we want to use all tracks        
          // test silvia
          //if (my_corrected_track.nIsGood()){
            PV_tracks_corrected.push_back(my_corrected_track);
            debug() << "track type " << jtrack->nType() <<endmsg;
            
            if (jtrack->nType() ==1)
              PV_tracksR_corrected.push_back(my_corrected_track);
            if (jtrack->nType() ==0)
              PV_tracksL_corrected.push_back(my_corrected_track);
            //}
          
        }
      }

      for (VeloTracks::iterator jtrack = i_pvTrackVector->begin(); jtrack != i_pvTrackVector->end(); ++jtrack)
      {
        debug()<<"Track type= " << jtrack->nType() <<
          "Is this track good? " << jtrack->nIsGood() << endmsg;
        
        //no overlap tracks
        if(jtrack->nType() < 2)
        { 
          VeloTrack my_corrected_track  = VeloTrack();  // create AlignTrack 
          
          my_corrected_track.setNEvent(npv);
          
          my_config->correcTrack((*jtrack),my_corrected_track,
                                 misal_left,misal_right,&VELOmap[0]);
          my_corrected_track.GetTrackSlope();

          my_corrected_track.setNPV_x((*jtrack).nPV_x());
          my_corrected_track.setNPV_y((*jtrack).nPV_y());
          my_corrected_track.setNPV_z((*jtrack).nPV_z());


          // fill the vector to evaluate the PV with linear fit
          //test silvia
          //for PV method we want to use all tracks        
          //if (my_corrected_track.nIsGood()){
            PV_tracks_linfit.push_back(my_corrected_track);
            if (jtrack->nType() ==1)
              PV_tracksR_linfit.push_back(my_corrected_track);
            if (jtrack->nType() ==0)
              PV_tracksL_linfit.push_back(my_corrected_track);
            //}
        }
      }
      
      
      
      debug() << npv << " vertex has "<<  PV_tracks_corrected.size() << " accepted tracks" <<endmsg;
      my_config->PutPVTrack(PV_tracks_corrected,my_align,0,zmoy_L,zmoy_R,
                            m_VELOopen,m_PV_trackmin);

      debug() <<"PV by Linear Fit for event "<< npv<< " " << PV_tracks_linfit.size() <<endmsg;

      debug() <<"OFFICIAL VERTEX npv ="<< npv<< " " <<endmsg;
      debug() << "Global " << (vector_PV_tracks[npv-1].begin())->nPV_x() <<" " <<
        (vector_PV_tracks[npv-1].begin())->nPV_y() <<" " << 
        (vector_PV_tracks[npv-1].begin())->nPV_z() << endmsg;
      debug() << "Left   " << (vector_PV_tracksL[npv-1].begin())->nPV_x() <<" " << 
        (vector_PV_tracksL[npv-1].begin())->nPV_y() <<" " << 
        (vector_PV_tracksL[npv-1].begin())->nPV_z() << endmsg;
      debug() << "Right  " << (vector_PV_tracksR[npv-1].begin())->nPV_x() <<" " << 
        (vector_PV_tracksR[npv-1].begin())->nPV_y() <<" " << 
        (vector_PV_tracksR[npv-1].begin())->nPV_z() << endmsg;
      debug() << "Global " << endmsg;

      //double Vz_official=(vector_PV_tracks[npv-1].begin())->nPV_z();
      
      //my_PV_finder->FitPV(PV_tracks_linfit,0, Vz_official);
      my_PV_finder->FitPV(PV_tracks_linfit,0);

      debug() << (*PV_tracks_linfit.begin()).nPV_x() <<" " << 
        (*PV_tracks_linfit.begin()).nPV_y()<<" " << 
        (*PV_tracks_linfit.begin()).nPV_z() << endmsg;
      debug() << "Left   " <<endmsg;
      //my_PV_finderL->FitPV(PV_tracksL_linfit,0, Vz_official);      
      my_PV_finderL->FitPV(PV_tracksL_linfit,0);      
      debug() << (*PV_tracksL_linfit.begin()).nPV_x() <<" " << 
        (*PV_tracksL_linfit.begin()).nPV_y()<<" " << 
        (*PV_tracksL_linfit.begin()).nPV_z() << endmsg;
      debug() << "Right  " <<endmsg;
      //my_PV_finderR->FitPV(PV_tracksR_linfit,0, Vz_official);
      my_PV_finderR->FitPV(PV_tracksR_linfit,0);
      debug() << (*PV_tracksR_linfit.begin()).nPV_x() <<" " << 
        (*PV_tracksR_linfit.begin()).nPV_y()<<" " << 
        (*PV_tracksR_linfit.begin()).nPV_z() << endmsg;

      debug() <<"Difference between Official and Linear"<< " " <<endmsg;
      debug() << "Left   " <<endmsg;
      debug() << (*PV_tracksL_linfit.begin()).nPV_x()-(vector_PV_tracksR[npv-1].begin())->nPV_x() <<" " << 
        (*PV_tracksL_linfit.begin()).nPV_y()-(vector_PV_tracksR[npv-1].begin())->nPV_y() <<" " << 
        (*PV_tracksL_linfit.begin()).nPV_z()-(vector_PV_tracksR[npv-1].begin())->nPV_z() << endmsg;
      debug() << "Right  " <<endmsg;
      debug() << (*PV_tracksR_linfit.begin()).nPV_x()-(vector_PV_tracksR[npv-1].begin())->nPV_x() <<" " << 
        (*PV_tracksR_linfit.begin()).nPV_y()-(vector_PV_tracksR[npv-1].begin())->nPV_y() <<" " << 
        (*PV_tracksR_linfit.begin()).nPV_z()-(vector_PV_tracksR[npv-1].begin())->nPV_z() << endmsg;



      if (m_moni_PV) {
        m_velo = getDet<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
        
        const DeVeloSensor* leftSensor = (m_velo->sensor(0));
        const DeVeloSensor* rightSensor = (m_velo->sensor(1)) ;
        Gaudi::XYZPoint globalLeftPoint,globalRightPoint;
        Gaudi::XYZPoint leftPoint((vector_PV_tracksL[npv-1].begin())->nPV_x(),
                                  (vector_PV_tracksL[npv-1].begin())->nPV_y(),
                                  (vector_PV_tracksL[npv-1].begin())->nPV_z());    
        Gaudi::XYZPoint rightPoint((vector_PV_tracksR[npv-1].begin())->nPV_x(),
                                  (vector_PV_tracksR[npv-1].begin())->nPV_y(),
                                  (vector_PV_tracksR[npv-1].begin())->nPV_z());
        globalLeftPoint = leftSensor->globalToVeloHalfBox(leftPoint);
        globalRightPoint = rightSensor->globalToVeloHalfBox(rightPoint);
        

        VAlign::fill_officialPV_Lin(nEvents,npv,
                                    vector_PV_tracks[npv-1].size(),
                                    (vector_PV_tracks[npv-1].begin())->nPV_x(),
                                    (vector_PV_tracks[npv-1].begin())->nPV_y(),
                                    (vector_PV_tracks[npv-1].begin())->nPV_z(),
                                      (vector_PV_tracks[npv-1].begin())->nPV_chi(),
                                    vector_PV_tracksL[npv-1].size(),
                                    (vector_PV_tracksL[npv-1].begin())->nPV_x(),
                                    (vector_PV_tracksL[npv-1].begin())->nPV_y(),
                                    (vector_PV_tracksL[npv-1].begin())->nPV_z(),
                                      (vector_PV_tracksL[npv-1].begin())->nPV_chi(),
                                    vector_PV_tracksR[npv-1].size(),
                                    (vector_PV_tracksR[npv-1].begin())->nPV_x(),
                                    (vector_PV_tracksR[npv-1].begin())->nPV_y(),
                                    (vector_PV_tracksR[npv-1].begin())->nPV_z(),
                                      (vector_PV_tracksR[npv-1].begin())->nPV_chi(),
                                    PV_tracks_linfit.size(), 
                                    (*PV_tracks_linfit.begin()).nPV_x(), 
                                    (*PV_tracks_linfit.begin()).nPV_y(), 
                                    (*PV_tracks_linfit.begin()).nPV_z(),
                                    (*PV_tracks_linfit.begin()).nPV_chi(),
                                    PV_tracksL_linfit.size(), 
                                    (*PV_tracksL_linfit.begin()).nPV_x(), 
                                    (*PV_tracksL_linfit.begin()).nPV_y(), 
                                    (*PV_tracksL_linfit.begin()).nPV_z(),
                                    (*PV_tracksL_linfit.begin()).nPV_chi(),
                                    PV_tracksR_linfit.size(), 
                                    (*PV_tracksR_linfit.begin()).nPV_x(), 
                                    (*PV_tracksR_linfit.begin()).nPV_y(), 
                                    (*PV_tracksR_linfit.begin()).nPV_z(),
                                    (*PV_tracksR_linfit.begin()).nPV_chi(),
                                    globalLeftPoint.x(),globalLeftPoint.y(),globalLeftPoint.z(),
                                    globalRightPoint.x(),globalRightPoint.y(),globalRightPoint.z());
      }
    }
  }
  
  
  // Millepede matrix is now filled

  my_align->MakeGlobalFit(&mis_const[0],&mis_error[0],&mis_pull[0]);   // Perform the global fit
  
  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
//  DEFINEVELOMAP:  define the geometry to align
//-----------------------------------------------------------
//
//-----------------------------------------------------------


StatusCode VAlign::DefineVeloMap() 
{

//  First retrieve the velo detector element

  m_velo = getDet<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
   
  if ( 0 == m_velo ) {
    debug() << "Can't get velo!!!." << endmsg;
    return StatusCode::FAILURE;
   }

  // Fill the maps, depending on the chosen geometry
  // with the Z values taken from the detector element

  int nonzer_l = 0;
  int nonzer_r = 0;

  zmoy_R = 0.;
  zmoy_L = 0.;
  szmoy_R = 0.;
  szmoy_L = 0.;

  for (int i=0;i<42;i++) VELOmap[i] = 0.;

  for (int i=0; i<21; i++)
  {
    if (m_VELOmap_l[i]) VELOmap[i]    = (m_velo->sensor(2*i))->z();
    if (m_VELOmap_r[i]) VELOmap[i+21] = (m_velo->sensor(2*i+1))->z();
  }

  // Compute the parameters necessary for the canonical approximation

  for (int i=0; i<21; i++)
  {
    if (VELOmap[i] != 0.)    
    {  
      zmoy_L += VELOmap[i];
      nonzer_l++;
    }
    if (VELOmap[i+21] != 0.) 
    {
      zmoy_R += VELOmap[i+21];
      nonzer_r++;
    }
  }

  zmoy_L /= nonzer_l;
  zmoy_R /= nonzer_r;

  for (int i=0; i<21; i++)
  {
    debug() << VELOmap[i+21] << endmsg;

    if (VELOmap[i] != 0.)    szmoy_L += (VELOmap[i] - zmoy_L)*(VELOmap[i] - zmoy_L);
    if (VELOmap[i+21] != 0.) szmoy_R += (VELOmap[i+21] - zmoy_R)*(VELOmap[i+21] - zmoy_R);
  }

  szmoy_L /= nonzer_l;
  szmoy_R /= nonzer_r;

  // Some Debugging Printouts, in order to check the geometry

  std::string Rig,Lef;

  debug() << "___________________________________________" << endmsg;
  debug() << "|                                         |" << endmsg;
  debug() << "|        VELO GEOMETRY FOR ALIGNMENT      |" << endmsg;
  debug() << "|_________________________________________|" << endmsg;   
  debug() << "|             |             |             |" << endmsg;   
  debug() << "|   Station   |     Left    |    Right    |" << endmsg;   
  debug() << "|_____________|_____________|_____________|" << endmsg;   
  debug() << "|             |             |             |" << endmsg;   

  for (int i=0; i<21; i++)
  {
    (!m_VELOmap_l[i]) ? Lef = "OFF" : Lef = "ON"; 
    (!m_VELOmap_r[i]) ? Rig = "OFF" : Rig = "ON";     

    debug() << "|     " << std::setw(2) << i 
	    << "      |    " << std::setw(3) << Lef 
	    << "      |    " << std::setw(3) << Rig << "      |" << endmsg;   
  }

  debug() << "|_____________|_____________|_____________|" << endmsg;   
  debug() << "" << endmsg;  

  return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
//  GETALIGNMENTCONSTANTS:  get the previous alignment constants
//                          from the CondDB
//-----------------------------------------------------------
//
//  This method uses the AlignmentCondition class
//
//-----------------------------------------------------------


StatusCode VAlign::GetAlignmentConstants() 
{

  MsgStream log(msgSvc(), name());

  std::string det_velo     = "/dd/Conditions/Alignment/Velo/VeloSystem";
  std::string det_rightbox = "/dd/Conditions/Alignment/Velo/VeloRight";
  std::string det_leftbox  = "/dd/Conditions/Alignment/Velo/VeloLeft";
  std::string det_module   = "/dd/Conditions/Alignment/Velo/Module";

  int module_num;

  misal_init_right.resize(126);
  misal_init_left.resize(126);
  misal_init_box.resize(12);

  std::string det_number_unit,det_number_diza,det_name;
  std::vector<double> translations,rotations;

  AlignmentCondition* myCond;  // Alignment condition

  for (int i=0; i<21; i++)   // First get the alignment conditions from the stations
  {
    for (int side=0; side<2; side++)   // 0 left, 1 right
    {
      module_num = 2*i+side; 

      det_number_unit  = VAlign::itos(module_num%10);
      det_number_diza  = VAlign::itos((module_num-module_num%10)/10);
      det_name  = det_module+det_number_diza+det_number_unit;

      myCond = getDet<AlignmentCondition>(det_name);
      translations = myCond->paramAsDoubleVect("dPosXYZ");
      rotations    = myCond->paramAsDoubleVect("dRotXYZ");

      // Now just put the misalignments in their respective containers
      
      if (side == 0)  // Left module
      {
	for (int j=0; j<3; j++) misal_init_left[i+21*j] = translations[j];
	for (int j=3; j<6; j++) misal_init_left[i+21*j] = rotations[j%3];	
      }
      else
      {
	for (int j=0; j<3; j++) misal_init_right[i+21*j] = translations[j];
	for (int j=3; j<6; j++) misal_init_right[i+21*j] = rotations[j%3];	
      }
    }
  }

  // Constants for the left box

  det_name  = det_leftbox;

  myCond = getDet<AlignmentCondition>(det_name);
  translations = myCond->paramAsDoubleVect("dPosXYZ");
  rotations    = myCond->paramAsDoubleVect("dRotXYZ");

  for (int j=0; j<3; j++) misal_init_box[2*j] = translations[j];
  for (int j=3; j<6; j++) misal_init_box[2*j] = rotations[j%3];
 
  // Constants for the right box

  det_name  = det_rightbox;

  myCond = getDet<AlignmentCondition>(det_name);
  translations = myCond->paramAsDoubleVect("dPosXYZ");
  rotations    = myCond->paramAsDoubleVect("dRotXYZ");

  for (int j=0; j<3; j++) misal_init_box[2*j+1] = translations[j];
  for (int j=3; j<6; j++) misal_init_box[2*j+1] = rotations[j%3];

  // Fill the nTuple with corresponding values

  std::vector<double> dummy_box, dummy_internal;

  dummy_box.resize(12);
  dummy_internal.resize(126);

  for (int i=0; i<126; i++) dummy_internal[i] = -999.; 
  for (int i=0; i<12; i++)  dummy_box[i] = -999.; 

  if (m_moni_constants)
  {
    VAlign::fill_misalignments(misal_init_left, dummy_internal, dummy_internal, 0);
    VAlign::fill_misalignments(misal_init_right, dummy_internal, dummy_internal, 1);
    VAlign::fill_misalignments(misal_init_box, dummy_box, dummy_box, 4);
  }

  return StatusCode::SUCCESS;
}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Monitoring nTuples : used for development, should be switched off in 
//                      the future (replaced by histograms) 
//
/////////////////////////////////////////////////////

StatusCode VAlign::fill_params(VeloTrack& my_track, int my_step) 
{

  debug() << my_step <<" Control track on event : " << my_track.nEvent() << endmsg;

  Tuple tuple=nTuple("TrackClinfo", "Test sample");
  
  double slx, sly, x0, y0;
  int n_side = 0;
  
  slx  = my_track.nSlope_x();
  x0   = my_track.nXo_x();
  sly  = my_track.nSlope_y();
  y0   = my_track.nYo_y();

  int Ncoords = my_track.nGoodCoordinates();

  for (int k=0; k<Ncoords; k++)
  {
    if (slx==0.0 && sly==0.0) return StatusCode::SUCCESS;
  
    (my_track.nType() == 0)      
      ? n_side =0
      : n_side =1;

    double n_stationB = ((my_track.Coords()[k]).first).z();
    double n_X = ((my_track.Coords()[k]).first).x();
    double n_Y = ((my_track.Coords()[k]).first).y();
  
    double n_resX = n_X-(slx*n_stationB+x0);
    double n_resY = n_Y-(sly*n_stationB+y0);
    tuple->column( "run",m_runodin);
    tuple->column( "evt",m_eventodin);
    tuple->column( "bunchid",m_bunchid);
    tuple->column( "step",    my_step);
    tuple->column( "side",    n_side);
    tuple->column( "vx",      slx*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+x0);
    tuple->column( "vy",      sly*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+y0);
    tuple->column( "vz",      -(slx*x0+sly*y0)/(slx*slx+sly*sly));
    tuple->column( "station", ((my_track.Coords()[k]).first).z());
    tuple->column( "X_clus",  ((my_track.Coords()[k]).first).x());
    tuple->column( "Y_clus",  ((my_track.Coords()[k]).first).y());
    tuple->column( "res_X",      n_resX);
    tuple->column( "res_Y",      n_resY);
    tuple->write();
  }

  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////

StatusCode VAlign::fill_params2(VeloTrack& my_track, int my_step,
                                int nVeloHits,float adcpertrack) 
{

  debug() << my_step << " Track Cluster Moni on event : " << my_track.nEvent() << endmsg;

  Tuple tuple=nTuple("ClusterMoni", "Full sample");
  
  double slx, sly, x0, y0;
  int n_side = 0;
  
  slx  = my_track.nSlope_x();
  x0   = my_track.nXo_x();
  sly  = my_track.nSlope_y();
  y0   = my_track.nYo_y(); 
  int Ncoords = my_track.nGoodCoordinates();
  (my_track.nType() == 0)      
    ? n_side =0
    : n_side =1;


  for (int k=0; k<Ncoords; k++)
  {
    if (slx==0.0 && sly==0.0) return StatusCode::SUCCESS;
  
    double n_stationB = ((my_track.Coords()[k]).first).z();
    double n_X = ((my_track.Coords()[k]).first).x();
    double n_Y = ((my_track.Coords()[k]).first).y();
  
    double n_resX = n_X-(slx*n_stationB+x0);
    double n_resY = n_Y-(sly*n_stationB+y0);
    //double n_resR = sqrt((n_X+n_resX) *(n_X+n_resX) +
    //                     (n_Y+n_resY) *(n_Y+n_resY))
    //  -sqrt(n_X*n_X+n_Y*n_Y);
    double n_resR = sqrt((slx*n_stationB+x0)*(slx*n_stationB+x0)+
                         (sly*n_stationB+y0)*(sly*n_stationB+y0)) -sqrt(n_X*n_X+n_Y*n_Y);
    
    double n_resP = atan2((sly*n_stationB+y0),(slx*n_stationB+x0))-
      atan2(n_Y,n_X);
     //double n_resP = atan2((n_Y+(sly*n_stationB+y0)),(n_X+(slx*n_stationB+x0)))-
                          //atan2(n_Y,n_X);
    
    tuple->column( "run",m_runodin);
    tuple->column( "evt",m_eventodin);
    tuple->column( "bunchid",m_bunchid);
    tuple->column( "step",    my_step);
    tuple->column( "side",    n_side);
    tuple->column( "nhits", nVeloHits);
    tuple->column( "adcpertrack", adcpertrack);
    tuple->column( "slx",     slx);
    tuple->column( "sly",     sly);
    tuple->column( "x0",      x0);
    tuple->column( "y0",      y0);
    tuple->column( "vx",      slx*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+x0);
    tuple->column( "vy",      sly*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+y0);
    tuple->column( "vz",      -(slx*x0+sly*y0)/(slx*slx+sly*sly));
    tuple->column( "station", ((my_track.Coords()[k]).first).z());
    tuple->column( "nstation", n_stationB);
    tuple->column( "X_clus",  ((my_track.Coords()[k]).first).x());
    tuple->column( "Y_clus",  ((my_track.Coords()[k]).first).y());
    tuple->column( "R_clus",  (sqrt(((my_track.Coords()[k]).first).x()*
                                    ((my_track.Coords()[k]).first).x()+
                                    ((my_track.Coords()[k]).first).y()*
                                    ((my_track.Coords()[k]).first).y())));
    tuple->column( "P_clus",  (atan2(((my_track.Coords()[k]).first).y(),
                                     ((my_track.Coords()[k]).first).x())));
    tuple->column( "res_X",      n_resX);
    tuple->column( "res_Y",      n_resY);
    tuple->column( "res_R",      n_resR);
    tuple->column( "res_P",      n_resP);
    tuple->write();
  }

  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////

StatusCode VAlign::fill_params3(VeloTrack& my_track, int my_step, 
                                int nExpectedHits,int nVeloHits,
                                float adcpertrack) 
{

  debug() << my_step <<" Track Moni on event : " << my_track.nEvent() << endmsg;
  
  double slx, sly, x0, y0;
  int n_side = 0;
  
  slx  = my_track.nSlope_x();
  x0   = my_track.nXo_x();
  sly  = my_track.nSlope_y();
  y0   = my_track.nYo_y();

  int Ncoords = my_track.nGoodCoordinates();
  (my_track.nType() == 0)      
    ? n_side =0
    : n_side =1;

  debug() << "Control track on event : " << my_track.nEvent() << endmsg;

  Tuple tuple=nTuple("Trackmoni", "Full sample");
  tuple->column( "run",m_runodin);
  tuple->column( "evt",m_eventodin);
  tuple->column( "bunchid",m_bunchid);
  tuple->column( "ncoord", Ncoords);
  tuple->column( "nexpecthits", nExpectedHits);
  tuple->column( "nhits", nVeloHits);
  tuple->column( "adcpertrack", adcpertrack);
  tuple->column( "step",    my_step);
  tuple->column( "side",    n_side);
  tuple->column( "slx",     slx);
  tuple->column( "sly",     sly);
  tuple->column( "x0",      x0);
  tuple->column( "y0",      y0);
  tuple->column( "vx",      slx*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+x0);
  tuple->column( "vy",      sly*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+y0);
  tuple->column( "vz",      -(slx*x0+sly*y0)/(slx*slx+sly*sly));
  tuple->write();

  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////
StatusCode VAlign::fill_infoevent(int event, int tracknumperev,int hitnumperevent,
                                 int tralignnumperevent, int troverlapnumperevent)
{

  debug() << "Event information ntuple " << endmsg;

  Tuple tuple=nTuple("EvInfo", "Event info");

  tuple->column( "event",   event);
  tuple->column( "run",m_runodin);
  tuple->column( "evt",m_eventodin);
  tuple->column( "bunchid",m_bunchid);
  tuple->column( "trnum",  tracknumperev);
  tuple->column( "aligntrnum",tralignnumperevent );
  tuple->column( "overlaptrnum",  troverlapnumperevent );   
  tuple->column( "hitnum", hitnumperevent);
  tuple->write();


  return StatusCode::SUCCESS;
}	  
/////////////////////////////////////////////////////
StatusCode VAlign::fill_overlaps(VeloTrack& my_track, int my_step)
{

  debug() << my_step <<" Overlap track point on event : " << my_track.nEvent() << endmsg;

  Tuple tuple=nTuple("Overlaps", "Overlap track info");

  for (unsigned int j=0; j<my_track.Coords().size(); j++)
  {
    tuple->column( "run",m_runodin);
    tuple->column( "evt",m_eventodin);
    tuple->column( "bunchid",m_bunchid);
    tuple->column( "event",   my_track.nEvent());
    tuple->column( "track",   my_track.nTrack());
    tuple->column( "type",    my_track.nType()+my_step);
    tuple->column( "x",       ((my_track.Coords()[j]).first).x());
    tuple->column( "y",       ((my_track.Coords()[j]).first).y());
    tuple->column( "z",       ((my_track.Coords()[j]).first).z());
    tuple->column( "station", ((my_track.Coords()[j]).second).z());
    tuple->write();
  }

  return StatusCode::SUCCESS;
}	  

/////////////////////////////////////////////////////

StatusCode VAlign::fill_officialPV_Lin(int nt_ev,int nt_pvn, 
                                        int nt_trn, double nt_pvx,double nt_pvy,double nt_pvz
                                       ,double nt_chi2,
                                        int nt_trnl, double nt_pvxl,double nt_pvyl,double nt_pvzl,
                                       double nt_chi2l,
                                        int nt_trnr, double nt_pvxr,double nt_pvyr,double nt_pvzr,
                                       double nt_chi2r,
                                       int nt_trn_lin, double nt_pvx_lin,double nt_pvy_lin,double nt_pvz_lin,double nt_chi2_lin,
                                       int nt_trnl_lin, double nt_pvxl_lin,double nt_pvyl_lin,double nt_pvzl_lin,
                                       double nt_chi2l_lin,
                                       int nt_trnr_lin, double nt_pvxr_lin,double nt_pvyr_lin,double nt_pvzr_lin,
                                       double nt_chi2r_lin,
                                       double loc_pvxl, double loc_pvyl, double loc_pvzl,
                                       double loc_pvxr, double loc_pvyr, double loc_pvzr)

{

  debug() << "Official and Linear PV on event : " << nt_ev << endmsg;

  Tuple tuple=nTuple("OfficialPVLin", "Official and Linear PV info");
  tuple->column( "run",m_runodin);
  tuple->column( "evt",m_eventodin);
  tuple->column( "bunchid",m_bunchid);
  tuple->column( "event", nt_ev);
  tuple->column( "pvn", nt_pvn);
  tuple->column( "trn", nt_trn);
  tuple->column( "pvx", nt_pvx);
  tuple->column( "pvy", nt_pvy);
  tuple->column( "pvz", nt_pvz);
  tuple->column( "chi2dof", nt_chi2);
  tuple->column( "trnl", nt_trnl);
  tuple->column( "pvxl", nt_pvxl);
  tuple->column( "pvyl", nt_pvyl);
  tuple->column( "pvzl", nt_pvzl);
  tuple->column( "chi2dofl", nt_chi2l);
  tuple->column( "trnr", nt_trnr);
  tuple->column( "pvxr", nt_pvxr);
  tuple->column( "pvyr", nt_pvyr);
  tuple->column( "pvzr", nt_pvzr);
  tuple->column( "chi2dofr", nt_chi2r);
  tuple->column( "trn_lin", nt_trn_lin);
  tuple->column( "pvx_lin", nt_pvx_lin);
  tuple->column( "pvy_lin", nt_pvy_lin);
  tuple->column( "pvz_lin", nt_pvz_lin);
  tuple->column( "chi2dof_lin", nt_chi2_lin);
  tuple->column( "trnl_lin", nt_trnl_lin);
  tuple->column( "pvxl_lin", nt_pvxl_lin);
  tuple->column( "pvyl_lin", nt_pvyl_lin);
  tuple->column( "pvzl_lin", nt_pvzl_lin);
  tuple->column( "chi2dofl_lin", nt_chi2l_lin);
  tuple->column( "trnr_lin", nt_trnr_lin);
  tuple->column( "pvxr_lin", nt_pvxr_lin);
  tuple->column( "pvyr_lin", nt_pvyr_lin);
  tuple->column( "pvzr_lin", nt_pvzr_lin);
  tuple->column( "chi2dofr_lin", nt_chi2r_lin);
  tuple->column( "pvxl_loc", loc_pvxl);
  tuple->column( "pvyl_loc", loc_pvyl);
  tuple->column( "pvzl_loc", loc_pvzl);
  tuple->column( "pvxr_loc", loc_pvxr);
  tuple->column( "pvyr_loc", loc_pvyr);
  tuple->column( "pvzr_loc", loc_pvzr);
  tuple->write();

  return StatusCode::SUCCESS;
}	  

/////////////////////////////////////////////////////
StatusCode VAlign::fill_trackmoni(VeloTrack& my_track, int my_step)
{

  debug() << my_step  <<" Overlap track on event : " << my_track.nEvent() << endmsg;

  Tuple tuple=nTuple("FirstCoordMoni", "All used Track info");

  for (unsigned int j=0; j<my_track.Coords().size(); j++)
  {
    tuple->column( "run",m_runodin);
    tuple->column( "evt",m_eventodin);
    tuple->column( "bunchid",m_bunchid);
    tuple->column( "event",   my_track.nEvent());
    tuple->column( "track",   my_track.nTrack());
    tuple->column( "type",    my_track.nType()+my_step);
    tuple->column( "x",       ((my_track.Coords()[j]).first).x());
    tuple->column( "y",       ((my_track.Coords()[j]).first).y());
    tuple->column( "z",       ((my_track.Coords()[j]).first).z());
    tuple->column( "station", ((my_track.Coords()[j]).second).z());
    tuple->write();
  }

  return StatusCode::SUCCESS;
}	  

/////////////////////////////////////////////////////
StatusCode VAlign::fill_misalignments(std::vector<double> constants, std::vector<double> errors, 
				     std::vector<double> pulls, int my_step)
{
 
  int n_stat = constants.size()/6; // 21 for internal stuff, 2 for the boxes 

  Tuple tuple=nTuple("Constants", "Misalignment values");

  for (int i=0;i<n_stat;i++) 
  {
    for (int jj=0; jj<6; jj++)
    {
  
      tuple->column( "type",       my_step);
      tuple->column( "station",    i);
      tuple->column( "DOF",        jj);
      tuple->column( "mis_value",  constants[i+n_stat*jj]);
      tuple->column( "mis_error",  errors[i+n_stat*jj]);
      tuple->column( "mis_pull",   pulls[i+n_stat*jj]);
  
      tuple->write();
    }
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////
StatusCode VAlign::updateConditions( std::vector<double> const_left, std::vector<double> const_right, 
                                    std::vector<double> const_box ,
                                    std::vector<double> err_left, std::vector<double> err_right)
{
  std::string det_velo     = "/dd/Conditions/Alignment/Velo/VeloSystem";
  std::string cond_veloright = "/dd/Conditions/Alignment/Velo/VeloRight";
  std::string cond_veloleft  = "/dd/Conditions/Alignment/Velo/VeloLeft";
  std::string det_veloright   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight";
  std::string det_veloleft   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft";
  std::string det_module_right   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module";
  std::string det_module_left   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module";
  std::string cond_module   = "/dd/Conditions/Alignment/Velo/Module";

  int module_num;

  std::string det_number_unit, det_number_diza, det_name, cond_name;
  std::vector<double> translations;
  Tuple tuple=nTuple("ModuleResult", "Align Result");


  if ( m_step1 ) {
    for (int i=0; i<21; i++)   // First get the alignment conditions from the stations
    {
      for (int side=0; side<2; side++)   // 0 left, 1 right
      {
        if ( ( side == 0 ) && ! ( m_VELOmap_l[ i ] ) ) continue;
        if ( ( side == 1 ) && ! ( m_VELOmap_r[ i ] ) ) continue;
  
        module_num = 2*i+side; 
        det_number_unit  = VAlign::itos(module_num%10);
        det_number_diza  = VAlign::itos((module_num-module_num%10)/10);
        if ( side == 0 ) {
          det_name  = det_module_left+det_number_diza+det_number_unit;
        }  
        else {
          det_name  = det_module_right+det_number_diza+det_number_unit;
        }  
        cond_name  = cond_module+det_number_diza+det_number_unit;

        IDetectorElement *det = getDet<IDetectorElement>(det_name);
        IGeometryInfo *geo = det->geometry();
        AlignmentCondition *myCond = getDet<AlignmentCondition>(cond_name);
        // Convert alignment parameters into
        // a vector containing translations
        // a vector containing rotations and
        // a vector with the pivot point. 
        // That vector should actually be obtained from the detector element!
        std::vector<double> translation;
        std::vector<double> e_trans;
        translation.push_back(0.0);
        translation.push_back(0.0);
        translation.push_back(0.0);
        e_trans.push_back(0.0);
        e_trans.push_back(0.0);
        e_trans.push_back(0.0);
        std::vector<double> rotation;
        std::vector<double> e_rot;
        rotation.push_back(0.0);
        rotation.push_back(0.0);
        rotation.push_back(0.0);
        e_rot.push_back(0.0);
        e_rot.push_back(0.0);
        e_rot.push_back(0.0);
        std::vector<double> pivot;
        // Get the pivot vector from the geometry object:
        Gaudi::XYZPoint P = geo->toGlobal( Gaudi::XYZPoint() );
        pivot.push_back( P.X() );
        pivot.push_back( P.Y() );
        pivot.push_back( P.Z() );
        // fill the translation & rotation vectors
        // by looping over the number of degrees of freedom
        // the vector m_align is filled according to the following scheme:
        // first loop over detector elements, then loop over the degrees of 
        // freedom.
        // !!!
        int n_stat;
        if ( side == 0 ) {
          n_stat = const_left.size()/6; // 21 for internal stuff, 2 for the boxes 
        }
        else {
          n_stat = const_right.size()/6; // 21 for internal stuff, 2 for the boxes 
        }
        for ( unsigned int d = 0; d < 3; d++ ) {
          unsigned int pos1 = i + n_stat * d; // translations
          unsigned int pos2 = i + n_stat * (d + 3); //rotations
          if ( side == 0 ) {
            translation[d] = const_left[ pos1 ];
            rotation[d] = const_left[ pos2 ];
            e_trans[d] = err_left[ pos1 ];
            e_rot[d] = err_left[ pos2 ];
          }
          else {
            translation[d] = const_right[ pos1 ];
            rotation[d] = const_right[ pos2 ];
            e_trans[d] = err_right[ pos1 ];
            e_rot[d] = err_right[ pos2 ];
          }
        }
        
        if (m_moni_constants){
          debug() << "Translation vector: " << translation << "\n"
                  << "Rotation vector   : " << rotation << "\n"
                  << "pivot point       : " << pivot << endreq;
          
          tuple->column( "side", side);
          tuple->column( "nmod",  i );
          tuple->column( "rotx", rotation[0]);
          tuple->column( "roty", rotation[1]);
          tuple->column( "rotz", rotation[2]);
          tuple->column( "transx", translation[0]);
          tuple->column( "transy", translation[1]);
          tuple->column( "transz", translation[2]);
          tuple->column( "erotx", e_rot[0]);
          tuple->column( "eroty", e_rot[1]);
          tuple->column( "erotz", e_rot[2]);
          tuple->column( "etransx", e_trans[0]);
          tuple->column( "etransy", e_trans[1]);
          tuple->column( "etransz", e_trans[2]);

          const Gaudi::Transform3D halfDelta = DetDesc::localToGlobalTransformation( translation, rotation, pivot);
          const Gaudi::Transform3D frame = geo->parentIGeometryInfo()->parentIGeometryInfo()->toLocalMatrix() 
            * geo->toGlobalMatrix();
          // frame = moduleToHalf = VELO_half->toLocal * module->toGlobal 	
          const Gaudi::Transform3D localDelta = DetDesc::localDeltaFromAnyFrameDelta( frame, geo, halfDelta );
          myCond->offNominalMatrix( localDelta );
          updMgrSvc()->invalidate(myCond);
          std::vector<double> newtranslation(3,0);
          std::vector<double> newrotation(3,0);
          const std::vector<double> newpivot(3,0);
          DetDesc::getZYXTransformParameters( localDelta, newtranslation, newrotation, newpivot );
          
          tuple->column( "drotx", newrotation[0]);
          tuple->column( "droty", newrotation[1]);
          tuple->column( "drotz", newrotation[2]);
          tuple->column( "dtransx", newtranslation[0]);
          tuple->column( "dtransy", newtranslation[1]);
          tuple->column( "dtransz", newtranslation[2]);
          tuple->write();
        }
        
      }//side
    } //i module
  } 
  if ( m_step2 ) {  // VELO half alignment
    Gaudi::Transform3D leftSideInverse = Gaudi::Transform3D();
    for (int side=0; side<2; side++)   // 0 left, 1 right
    {
      if ( side == 0 ) {
        det_name  = det_veloleft;
        cond_name = cond_veloleft;
      }  
      else {
        det_name  = det_veloright;
        cond_name = cond_veloright;
      }  

      IDetectorElement *det = getDet<IDetectorElement>(det_name);
      IGeometryInfo *geo = det->geometry();
      AlignmentCondition *myCond = getDet<AlignmentCondition>(cond_name);
      // Convert alignment parameters into
      // a vector containing translations
      // a vector containing rotations and
      // a vector with the pivot point. 
      // That vector should actually be obtained from the detector element!
      std::vector<double> translation;
      translation.push_back(0.0);
      translation.push_back(0.0);
      translation.push_back(0.0);
      std::vector<double> rotation;
      rotation.push_back(0.0);
      rotation.push_back(0.0);
      rotation.push_back(0.0);
      std::vector<double> pivot;
      pivot.push_back( 0.0 );
      pivot.push_back( 0.0 );
      if( side == 0 ) { 
        pivot.push_back( zmoy_L );
      }
      else { 
        pivot.push_back( zmoy_R );
      }
      // fill the translation & rotation vectors
      // by looping over the number of degrees of freedom
      // the vector m_align is filled according to the following scheme:
      // first loop over detector elements, then loop over the degrees of 
      // freedom.
      // !!!
      int n_stat = const_box.size()/6; // 21 for internal stuff, 2 for the boxes 
      info() << "n_stat " << n_stat << endmsg;

      for ( unsigned int d = 0; d < 3; d++ ) {
        if ( m_VELOopen ) {
          unsigned int pos1 = side + n_stat * d; // translations
          unsigned int pos2 = side + n_stat * (d + 3); //rotations
          translation[d] = const_box[ pos1 ];
          rotation[d] = const_box[ pos2 ];
        }
        else {
          unsigned int pos1 = 2 + n_stat * d; // translations
          int sign = 1;
          //silvia comment No comments... ????????
          if ( d == 1 ) sign = -1; // sign change for y rotation
          unsigned int pos2 = 2 + n_stat * (d + 3); //rotations
          translation[d] = const_box[ pos1 ];
          rotation[d] = sign * const_box[ pos2 ];
        }
      }
  
      info() << "Translation vector: " << translation << "\n"
    	        << "Rotation vector   : " << rotation << "\n"
  	        << "pivot point       : " << pivot << endreq;
  
      //for ( int jjj = 0; jjj < const_box.size(); jjj++ ) {
      //  info() << jjj << " " << const_box[ jjj ] << endmsg;
      //}
 
      if ( ( !m_VELOopen ) && ( side == 0 ) ) {
        debug() << "Setting left half transformation to identity" << endmsg;
        leftSideInverse = myCond->toNominalMatrix();
        info() << "leftCondOld " << myCond->toXml() << endmsg;
        myCond->offNominalMatrix( Gaudi::Transform3D() );
        updMgrSvc()->invalidate(myCond);
        info() << "leftCondNew " << myCond->toXml() << endmsg;
      }
      else {
        const Gaudi::Transform3D halfDelta = DetDesc::localToGlobalTransformation( translation, rotation, pivot);
        const Gaudi::Transform3D localDelta = geo->ownToOffNominalMatrix() * halfDelta * leftSideInverse;
        info() << "rightCondOld " << myCond->toXml() << endmsg;
        myCond->offNominalMatrix( localDelta );
        updMgrSvc()->invalidate(myCond);
        info() << "rightCondNew " << myCond->toXml() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}


