//-----------------------------------------------------------------------------
/** @file GAlign.cpp
 *
 *  Implementation file for RICH reconstruction tool : GAlign
 *
 *  CVS Log :-
 *  $Id: GAlign.cpp,v 1.18 2009-12-27 14:27:21 jblouw Exp $
 *
 *  @author J.Blouw Johan.Blouw@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
//--- for packed dsts ---------------
#include "GaudiKernel/DataObject.h"

//-----------------------------------
#include "Kernel/LHCbID.h"
#include "GAlign.h"
// Event
#include "TrackKernel/StateTraj.h"
#include "Event/Track.h"

//Det
#include "STDet/DeSTLayer.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeITSector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeTTHalfModule.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTModule.h"
#include "VeloDet/DeVelo.h"

// Conditions
#include "DetDesc/IAlignment.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/3DTransformationFunctions.h"
//#include "Event/STLiteCluster.h"
//#include "Event/VeloLiteCluster.h"

#include <iostream>
#include <fstream>
// gsl stuff
#include "gsl/gsl_cdf.h"
//MC info
#include "Linker/LinkedTo.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"


using namespace LHCb;
using namespace Gaudi;



DECLARE_ALGORITHM_FACTORY( GAlign );

//-----------------------------------------------------------------------------

GAlign::GAlign( const std::string& name,
                ISvcLocator* pSvcLocator ):
  GaudiTupleAlg ( name , pSvcLocator ),
  m_converged(false),
  velo_detector(false),
  tt_detector(false),
  it_detector(false),
  ot_detector(false),
  m_MaxIterations(1),
  m_ForceIterations(false),
  m_iterations(0),
  m_tr_cnt(0),
  m_meas_cnt(0),
  m_zmean(0),
  m_sigZmean(0),
  m_chi2(0.0),
  m_chi2rej(0.0),
  m_newScale(100000),
  m_outlier(0),
  m_out(false),
  m_badXval(false),
  m_fitconv(false),
  m_resMax(100),
  m_locrank(0),
  m_skipBigCluster(false),
  m_acceptedTr(0),
  m_badCluster(0),
  m_nNotConv(0),
  m_totalTracks(0),
  m_RejectbySelector(0),
  m_nEvents(0)
{
  // define track containers
  declareProperty("TAlignment_Config_Tool", m_AlignConfTool = "TAConfig");
  declareProperty("InputContainer", m_inputcontainer = TrackLocation::Default );
  declareProperty("Detectors", m_detectors);
  declareProperty("myChi2Scale",m_newScale=1000) ; 
  //define maximum number of iterations
  declareProperty("MaxIterations", m_MaxIterations = 5);
  declareProperty("forceIterations", m_ForceIterations = 0 );
  declareProperty("skipBigCluster", m_skipBigCluster = false );
  declareProperty("CutonTrackSlope", m_slopeCut = 0 );
}

GAlign::~GAlign() { 
  info() << "GAlign destructor" << endreq;
}

StatusCode GAlign::initialize() {
  info() << "Initialising GAlign alignment..." << endreq;
  
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) {
    error() << "Error initialising GAlign" << endreq;
    return sc;
  }
  // Retrieve measurement provider tool:
  m_measProvider = tool<IMeasurementProvider>( "MeasurementProvider");
  // Retrieve configuration tool
  m_taConfig = tool<ITAConfigTool>( "TAConfig" );
  // - trajecory poca tool
  m_trpoca = tool<ITrajPoca>( "TrajPoca" );
  // Track extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  //   // derivatives tool
  //   m_ders = tool<IDerivatives>( "Derivatives");
  m_newScale = m_taConfig->GetChi2Scale();
  m_resMax   = m_taConfig->GetOutlierMax();
  info() << " ---> starting chi2-cut scale = " <<  m_newScale 
         << "   outlier rejection if res > "<< m_resMax << endreq;
  info() << " ---> use only 2 hit cluster? 0=no; 1=yes   : " << m_skipBigCluster << endreq;
  
  sc = m_taConfig->Initialize( m_detectors );
  sc = m_taConfig->CacheDetElements();
  if ( sc.isFailure() ) { 
    error() << "Error caching detector elements" << endreq;
    return StatusCode::FAILURE;
  }
  // get the number of alignment (== global) parameters
  m_nGlPars = m_taConfig->NumAlignPars();
  m_align_err.resize( m_nGlPars );
  m_align.resize( m_nGlPars );
  m_estimated.resize( m_nGlPars );
  info() << "Number of global parameters = " << m_nGlPars << endreq;
  info() << "Size of m_estimated = " << m_estimated.size() << endreq;
  if ( sc.isFailure() ) {
    error() << "Error initalizing TAConfig" << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<bool> dofs = m_taConfig->NumDOF();
  int n_dofs = 0;
  // Count number of degrees of freedom
  for( unsigned int i = 0; i < dofs.size(); i++ ) {
    if ( dofs[i] ) n_dofs++;
  }
  int nDet =  m_nGlPars/n_dofs;
  m_hitRnk.resize(nDet,0.);
  info() << " ------> nDet = " << nDet << endreq;
  
  
  //get the input misalignments
  misalInput_X = m_taConfig->GetMisalInput_X(); 
  misalInput_Y = m_taConfig->GetMisalInput_Y(); 
  misalInput_Z = m_taConfig->GetMisalInput_Z(); 
  misalInput_A = m_taConfig->GetMisalInput_A(); 
  misalInput_B = m_taConfig->GetMisalInput_B(); 
  misalInput_C = m_taConfig->GetMisalInput_C(); 
  info() << " ---> size of misalInput_X = " << misalInput_X.size() << endreq;
  m_taConfig->MakeAlignParVec();
  

  //   //initialize the derivatives tool
  //   int n_dofs = 0;
  //   // Count number of degrees of freedom
  //   for( unsigned int i = 0; i < dofs.size(); i++ ) {
  //     if ( dofs[i] ) n_dofs++;
  //   }
  //   int trpars =  m_taConfig->NumTrPars();
  //   int nDet =  m_nGlPars/n_dofs;
  //   info() << " n_dofs = " << n_dofs 
  //          << " ndet = " << nDet 
  //          << " NumTrPars = " <<  trpars  << endreq;
  //   m_ders->initialize( m_taConfig->NumTrPars() , nDet, dofs );
  
  std::string s("Velo");
  velo_detector = m_taConfig->AlignDetector( s );
  s = "TT";
  tt_detector = m_taConfig->AlignDetector( s );
  s = "OT";
  ot_detector = m_taConfig->AlignDetector( s );
  s = "IT";
  it_detector = m_taConfig->AlignDetector( s );

  
  /************************************
   ** calculate reference system for **
   ** Lagrange Multiplier Constraints**
   ************************************/
  //  LagMultRef();
  info() <<"---> The input misalignments :"<<endreq;
  info() << std::setiosflags(std::ios::left) << setw(16) << " Delta X "
         << std::setiosflags(std::ios::left) << setw(16) << " Delta Y "
         << std::setiosflags(std::ios::left) << setw(16) << " Delta Z "
         << std::setiosflags(std::ios::left) << setw(16) << " Delta A "
         << std::setiosflags(std::ios::left) << setw(16) << " Delta B "
         << std::setiosflags(std::ios::left) << setw(16) << " Delta C "<< endreq;
  for(unsigned  i=0; i < misalInput_X.size(); ++i){
    info() << std::setiosflags(std::ios::left) << setw(16) << misalInput_X.at(i)  
           << std::setiosflags(std::ios::left) << setw(16) << misalInput_Y.at(i)  
           << std::setiosflags(std::ios::left) << setw(16) << misalInput_Z.at(i)  
           << std::setiosflags(std::ios::left) << setw(16) << misalInput_A.at(i)  
           << std::setiosflags(std::ios::left) << setw(16) << misalInput_B.at(i)  
           << std::setiosflags(std::ios::left) << setw(16) << misalInput_C.at(i) << endreq;
  }

  // add listener to incident svc
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) {
    return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  }
  incSvc->addListener(this, "GlobalMPedeFit");
  incSvc->addListener(this, "Converged");
  incSvc->addListener(this, "writeParameters");
  m_prev_par.resize( m_taConfig->NumAlignPars(),0.);
  m_new_par.resize(  m_taConfig->NumAlignPars(),0.);
  // Acquire the track selection tool
  m_trackselection = tool<IATrackSelectorTool>( "ATrackSelector");
  m_trackselection->initialize();

  //   for(int i=0;i<12;i++){
  //     m_new_par[i] = 0.;
  //     if(i==3) m_new_par[i] = -0.000956391;
  //     if(i==6) m_new_par[i] = -0.00199049;
  //   }
  
  //m_taConfig->SetAlignmentPar(m_new_par);


  return StatusCode::SUCCESS;
}

void GAlign::handle( const Incident &incident ) {
  if ( "GlobalMPedeFit" == incident.type() ) 
    this->GloFit();
  if ( "Converged" == incident.type() )
    this->Converged();
  if ( "writeParameters" == incident.type() )
    this->writeParameters();

}

StatusCode GAlign::queryInterface( const InterfaceID &id, void **ppI ) {
  // check the placeholder
  if ( 0 == ppI ) 
    return StatusCode::FAILURE;
  // check ID of the interface
  if ( IIncidentListener::interfaceID() == id ) {
    *ppI = static_cast<IIncidentListener*>(this);
  } else {
    return Algorithm::queryInterface( id, ppI );
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool GAlign::Converged() {
  info() << "---> Convergence ? " << m_converged << endreq;
  unsigned int conv_par = 0;
  unsigned int params = m_taConfig->NumAlignPars();
  unsigned int fixObj = m_taConfig->GetNoFixedObj();
  
  info() << " the new cut scale is = " << m_newScale << endreq;
  info() << "  m_taConfig->NumAlignPars() " <<   params << endreq
         << " # fixed obj = " << fixObj << endreq;
  debug() <<setw(12)<< "station"<<setw(12) <<"newPar" <<setw(12)<<"prevPar"<<setw(12)<<"sumPar"<<setw(12)<<"errPar" <<endreq;
  std::vector<double> stations( params,0);
  //count params that fullfill conv criteria
  for ( unsigned int station = 0 ; station < params;station++){
    double ratio = fabs(m_align[station] / m_prev_par[station]);//relative change of alignment parameter
    debug() <<setw(12)<< station << setw(12) << m_align[station] << setw(12) <<m_prev_par[station]
            <<setw(12)<< m_new_par[station]  << setw(12) <<m_align_err[station] << endreq;
    if(m_align[station]!=0.){
      //       if( fabs(m_align[station]) <= 1.5*m_align_err[station] ) {
      if( ratio <= 0.005 && m_newScale <=1.2 && m_newScale != -1) {
        conv_par++;
        debug() << "SUCCESS -------------------------------> " << conv_par << " rel. change : " << ratio << endreq;
      }
    }
    //MD 14-04
    m_prev_par[station] = m_new_par[station];
  }
  
  if(conv_par ==  (params-fixObj) ) {
    m_converged = 1;
    always() << "----> Alignment Algorithm converged ! " << endmsg;
  }
  return m_converged;
}

// //------------------------------------------
// //------------------------------------------
// //
// // StatusCode GAlign::execute() {
// //
// //-----------------------------------------
// //-----------------------------------------
// StatusCode GAlign::execute() {
//   debug()  << "***********************" << endreq
//            << "***  NEXT EVENT     ***" << endreq
//            << "***********************" << endreq;
//   // LHCb::PackedTracks* dst = get<LHCb::PackedTracks>( m_inputcontainer );
// //   LHCb::Tracks* newTracks = new LHCb::Tracks();
// //   put( newTracks, "outputCont" );
  
// //   StandardPacker pack;
  
// //   for ( std::vector<LHCb::PackedTrack>::const_iterator itS = dst->begin();
// //         dst->end() != itS; ++itS ) {
// //     const LHCb::PackedTrack& src = (*itS);
    
// //     LHCb::Track* track = new LHCb::Track( );
// //     newTracks->insert( track, src.key );
    
// //     track->setChi2PerDoF( pack.logPacked( src.chi2PerDoF ) );
// //     track->setNDoF(       src.nDoF );
// //     track->setFlags(      src.flags );
// //     for ( int kId = src.firstId; src.lastId > kId; ++kId ) {
// //       unsigned int id = *(dst->beginIds()+kId);
// //       track->addToLhcbIDs( LHCb::LHCbID( id ) );
// //     }
// //     //     for ( int kSt = src.firstState; src.lastState > kSt; ++kSt ) {
// //     //       LHCb::PackedState pSta = *(dst->beginState()+kSt);
// //     //       convertState( pSta, track , src);
// //     //     }
// //     for ( int kEx = src.firstExtra; src.lastExtra > kEx; ++kEx ) {
// //       std::pair<int,int> info = *(dst->beginExtra()+kEx);
// //       track->addInfo( info.first, pack.logPacked( info.second ) );
// //     }
// //   }
// //   if ( !exist<Tracks>( "outputCont" ) ) {  
// //     info() << "Error: track container does not exist: " << m_inputcontainer << endreq;
// //     return StatusCode::FAILURE;
// //   }
  
  
//   //   debug() << "--->  track container is there " << m_inputcontainer << endreq;
//   Tracks *inCont = get<Tracks>(m_inputcontainer);
//   //Tracks *inCont = get<Tracks>("outputCont");
//   debug() << "---> size of track container = " << inCont->size() << endreq;
//   if(inCont->size() == 0 ) 
//     return StatusCode::FAILURE;
//   Tracks::const_iterator iterT;
//   for ( iterT = inCont->begin(); iterT != inCont->end(); iterT++) {
//     Track *atrack = *iterT;
//     m_taConfig->MakeTrackParVec(); 
//     bool acceptTr =  m_trackselection->accept( *atrack );
//     if( !acceptTr){
//       debug() << "---> track not accepted !! " << endreq;
//       //plot(atrack->p(),"rejected track momentum","rejected track momentum",0.,10000.,1000);
//       plot(atrack->chi2PerDoF(),"rejected: chi2perdof","rejected: chi2perdof",0.,400.,400);
//       plot(1,"nRejectedTracks","nRrejectedTracks",0.,2.,2);
//       continue;
//     }
//     //  if( atrack->checkType(LHCb::Track::Long) && acceptTr) {
//     //if( acceptTr) {
//     debug() << "---> track accepted !! " << endreq;
//     StatusCode sc =  m_taConfig->ResetGlVars();        // Reset MilleTool variables etc.
//     int rank      = 0; // position in c-matrix for a detector element        
//     double meanZ  = 0.;
//     double sumZ   = 0.;
//     int    nofZ   = 0;
    
//     //count multiple clusters per rank
//     debug() << " size of cluster array " << m_taConfig->NumAlignPars() << endreq;
//     int    cluster[m_taConfig->NumAlignPars()]; 
//     for(int cl=0; cl < m_taConfig->NumAlignPars(); cl++){
//       cluster[cl] = 0;
//     }
//     std::vector<LHCb::LHCbID>::const_iterator itID;
//     for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
//       LHCb::LHCbID id = *itID;
//       //if ( atrack->isOnTrack( id )  &&  atrack->isMeasurementOnTrack( id )) {
//       if ( atrack->isOnTrack( id ) ){
//         if ( ot_detector && id.isOT() ) {
//           sc =  m_taConfig->Rank( id, rank );
//           if ( sc.isFailure() ) {
//             debug() << "---> Couldn't calculate the rank of the object "<<endreq;
//           }
//           double weight       = 0.;
//           double Z_position   = 0.;
//           double stereo_angle = 0.;
//           bool flagL          = false;
//           struct Point meas;
//           sc=m_taConfig->CalcResidual(atrack, id, rank,  meas,weight,
//                                       flagL ,Z_position, stereo_angle);
//           if ( sc.isFailure() ){
//             continue;
//           }
//           cluster[rank]++;
//           sumZ += Z_position;
//           nofZ++;
//         }
//       }
//       else info() << " ---> 1st id.isOnTrack ?  NO " << endreq;
//     }
//     meanZ = sumZ/nofZ; // mean z pos of all monolayer z's
//     plot(nofZ,"# LHCbIDs OTTrack","# LHCbIDs OTTrack",0.,80.,80);              
//     //---------------------------------------
//     //
//     //    choose only good tracks
//     //
//     //---------------------------------------
//     bool badcluster = false;
//     for(int a=0;a< m_taConfig->NumAlignPars();a++){
//       if(cluster[a] >=3 ) badcluster = true;
//     }
//     if(badcluster && m_skipBigCluster) {
//       m_badCluster++;
//       continue;
//     }
//     m_acceptedTr++;
//     //----------------------------------------

//     //meanZ = 0.;
//     //info() <<" ---> nofZ = " <<nofZ << " meanZ = " <<meanZ << "(sumZ = "<<sumZ <<")"<<endreq;

//     int Tparam = m_taConfig->NumTrPars();
//     StatusCode zeroSC;
//     double locTr[Tparam];
//     double locMat[4][4];        
//     if(Tparam == 4 ) {
//       zeroSC = m_taConfig->ZeroMatrVec(locMat, &locTr[0]);          
//     }
//     double locMatP[5][5];
//     if(Tparam == 5 ){
//       zeroSC = m_taConfig->ZeroMatrVecP(locMatP, &locTr[0]); 
//     }
//     std::vector<double> trParV(Tparam,0.);
//     std::vector<double> zero(Tparam,0.);
//     //md	std::vector<double> m_prevTrack(Tparam,0.);  

//     m_taConfig->SetTrackPar(zero); // set the local parameters for this track                 

	
//     /*****************************
//      ** track fit (iterative) ***
//      ****************************/
//     bool chiOK = true;
//     m_out      = false;
//     Tuple t_resids = nTuple("residuals","residuals");
//     std::vector<double> tres; // residuals for tuple
//     std::vector<double> trank; // rank for tuple
//     std::vector<int>    tmonoA; // if monoA=true  for tuple
//     std::vector<double> tzpos; 
//     std::vector<double> zref; 
//     std::vector<unsigned int> tstrawunique; // if monoA=true  for tuple
//     std::vector<unsigned int> tstraw; 
//     std::vector<unsigned int> tmodule; 
//     std::vector<unsigned int> tquarter; 
//     std::vector<unsigned int> tlayer; 
//     std::vector<unsigned int> tstation; 
//     std::map<double, double> mapdzthis;
//     std::map<double, double> mapdzprev;
    
//     for(int kk=0;kk<4;kk++){
// //       if(1){
// //         trParV[0]=locTr[0];
// //         trParV[1]=locTr[1];
// //         trParV[2]=locTr[2];
// //         trParV[3]=locTr[3];
// //         if(Tparam==5) trParV[4]+=locTr[4];
// //       }
      
//       trParV[0]+=locTr[0];
//       trParV[1]+=locTr[1];
//       trParV[2]+=locTr[2];
//       trParV[3]+=locTr[3];
//       if(Tparam==5) trParV[4]+=locTr[4];
      
//       m_taConfig->SetTrackPar(trParV); // set the local parameters for this track                 
//       if(Tparam==4) zeroSC = m_taConfig->ZeroMatrVec(locMat, &locTr[0]);          
//       if(Tparam==5) zeroSC = m_taConfig->ZeroMatrVecP(locMatP, &locTr[0]);          
//       double x0= trParV[0];
//       double tx= trParV[1];
//       double y0= trParV[2];
//       double ty= trParV[3];
//       double tQ;
//       if(Tparam==4) tQ = 0.;
//       if(Tparam==5) tQ = trParV[4];
//       // set params for taconfig::calcresidual
//       m_taConfig->SetTrackPar(trParV);           	  
      
//       debug() << "--> iter = " <<kk<< "  iterative fit trParV[] : x0=" << trParV[0] << " tx=" << trParV[1] 
//               << " y0=" << trParV[2] << " ty=" << trParV[3] <<  endreq;
//       //------------------------------------------
//       // prepare the maps which hold the dz = z(y)
//       //------------------------------------------
//       mapdzprev.clear();
//       mapdzprev = mapdzthis;
//       mapdzthis.clear();
     
//       double totChi2 = 0.;
//       double redChi2 = 0.;
//       tres.clear();
//       tzpos.clear();
//       zref.clear();
//       trank.clear();
//       tstraw.clear();
//       tstrawunique.clear();
//       tstation.clear();
//       tlayer.clear();
//       tquarter.clear();
//       tmodule.clear();
      
//       //      info() << "-----> ITERATION " << kk << endreq;
// //       info() << std::setiosflags(std::ios::left) << setw(10) << "prev dz"    << setw(5) 
// //              << std::setiosflags(std::ios::left) << setw(10) << "now dz"    << setw(5) 
// //              << std::setiosflags(std::ios::left) << setw(10) << "zsum"    << setw(5) 
// //              << std::setiosflags(std::ios::left) << setw(10) << "y"    << setw(5) 
// //              << std::setiosflags(std::ios::left) << setw(10) << "meas.x"    << setw(5) 
// //              << endreq;
      
// // //              << std::setiosflags(std::ios::left) << setw(10) << "tx"    << setw(5) 
// // //              << std::setiosflags(std::ios::left) << setw(10) << "y0"    << setw(5) 
// // //              << std::setiosflags(std::ios::left) << setw(10) << "ty"    << setw(5) 
// //              << std::setiosflags(std::ios::left) << setw(10) << "guess1"    << setw(5) 
// //              << std::setiosflags(std::ios::left) << setw(10) << "guess2"    << setw(5) 
// //              << endreq;
    
      
//       for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
//         //****************************************************************
//         // check if this LHCbID is really on the track
//         LHCb::LHCbID id = *itID;
//         //	if ( !atrack->isOnTrack( id ) &&  !atrack->isMeasurementOnTrack( id )) {
//         if ( !atrack->isOnTrack( id )){
//           debug() << " ---> id.isOnTrack ?  NO " << endreq;
//           continue;
//         }
//         //if ( atrack->isOnTrack( id ) ) {
//         if ( !ot_detector || !id.isOT() ) {
//           debug() << " ---> no ot ID " << endreq;
//           continue;
//         }
//         sc =  m_taConfig->Rank( id, rank );
//         if ( sc.isFailure() ) 
//           debug() << "---> Couldn't calculate the rank of the object "<<endreq;

//         //****************************************************************  
//         //varibales for CalcRes
//         double relaZ        = 0.;
//         double dz           = 0.;
//         double guess        = 0.;
//         double weight       = 0.;
//         double Z_position   = 0.;
//         double stereo_angle = 0.0;
//         bool flagL          = false;//true;
//         struct Point meas;
//         m_taConfig->SetMeanZ(meanZ);
		
//         sc=m_taConfig->CalcResidual(atrack, id, rank,  meas,weight,
//                                     flagL ,Z_position, stereo_angle);
//         if ( sc.isFailure() ){
//           continue;
//         }
//         relaZ = Z_position-meanZ;
//         double dzdy = 0.0036;
//         /******************
//          *  get new meas  *
//          ******************/
//         if(m_iterations !=0){
//           struct Point pred; //give pred to fillmatrix
//           if(Tparam==4){
//             pred.x = x0 + tx * relaZ;//Z_position;
//             pred.y = y0 + ty * relaZ;//Z_position;
//           }
//           if(Tparam==5){
//             pred.x = x0 + tx * relaZ
//               + tQ*(relaZ * relaZ * cos( stereo_angle )+ 2.2*1.e-3*relaZ*relaZ*relaZ*cos( stereo_angle ));
//           }
//           pred.y = y0 + ty * relaZ;//Z_position;
//           //  m_taConfig->NewMeasurement(meas.x,rank,pred,relaZ,stereo_angle);

//         }
//         /******************
//          * guess position *
//          ******************/
//         // 	      char namedx[100];
//         // 	      sprintf(namedx,"dx(y)_rank__%d",rank);
//         // 	      plot(  dzdy*posRec.y() * tx,namedx,namedx,-3,3,70);
//         // 	      plot(  dzdy*posRec.y()  ,"dzdy","dzdy",-10,10,70);
//         double guess2=-222;
        
//         if(Tparam==4 ){
//           //if(kk>=2) {
//           //y0 = y0+ ty*dzdy*(y0+ty*relaZ);		 
//           //relaZ += dzdy*(y0+ty*relaZ);
//           //}
//           struct Point pred; //give pred to fillmatrix
//           pred.x = x0 + tx * relaZ;//Z_position;
//           pred.y = y0 + ty * (relaZ+mapdzprev[Z_position]);//Z_position;
//           dz =  pred.y*dzdy;
//           mapdzthis.insert(std::make_pair(Z_position, dz)); 
//           //           info() << std::setiosflags(std::ios::left) << setw(10) << mapdzprev[Z_position]    << setw(5) 
//           //                  << std::setiosflags(std::ios::left) << setw(10) << dz    << setw(5) 
//           //                  << std::setiosflags(std::ios::left) << setw(10) << relaZ+dz    << setw(5) 
//           //                  << std::setiosflags(std::ios::left) << setw(10) << pred.y    << setw(5) ;
          
          
//           //         info() << "map(Z_posi)   = " << Z_position << " dz = " <<mapdzthis[Z_position] <<  endreq;
//           //         info() << "prev map(Z_posi) = " << Z_position << " dz = " <<mapdzprev[Z_position] <<  endreq;
          
//           guess = (x0+tx*relaZ)*cos(stereo_angle) + (y0+ty*relaZ)*sin(stereo_angle); 
//           guess2 = (x0+tx*(relaZ+dz))*cos(stereo_angle) + (y0+ty*(relaZ+dz))*sin(stereo_angle); 
//           //+(0.003601*(y0+ty*relaZ)* tx)*cos(stereo_angle) + (0.003601*(y0+ty*relaZ)* ty)*sin(stereo_angle) ;
//         }
//         //             if(Tparam==5){
//         //               guess = (x0+tx*relaZ)*cos(stereo_angle)+ (0.0036*(y0+ty*relaZ)* tx)*cos(stereo_angle) 
//         //                 + (y0+ty*relaZ)*sin(stereo_angle)+  (0.0036*(y0+ty*relaZ)* ty)*sin(stereo_angle)
//         //                 +  tQ*((relaZ * relaZ * cos( stereo_angle )) +  2.2*1.e-3*relaZ*relaZ*relaZ*cos( stereo_angle ));
//         //             }
          

//         //beruecksichtige OTTimes
//         //	      meas.x += meas.y;
//         double measOrig = meas.x;
//         double resNew   = measOrig-guess2;
//         meas.x         -= guess;
//         //meas.x          = resNew;
//         //        info() << std::setiosflags(std::ios::left) << setw(10) << meas.x    << setw(5) << endreq;

//         //         info() << std::setiosflags(std::ios::left) << setw(10) << meas.x    << setw(3) 
//         //                << std::setiosflags(std::ios::left) << setw(10) << measOrig-guess2    << setw(3) 
//         //                <<endreq;
//         //----------------------------------------
//         // get the fabs of the slope angle alpha;
//         // the direction of doca is defined by
//         // meas.s
//         //--------------------------------------
//         double alpha = fabs(atan(1/tx));
//         double doca  = meas.x*sin(alpha);
//         meas.x=doca;
       
//         totChi2        += weight*meas.x*meas.x; 


//         char Orig[100];       
//         sprintf(Orig,"res_%d",kk);
//         plot(meas.x,Orig,Orig,-7,7,50);
//         char meapl[100];
//         sprintf(meapl,"glo%d_res%d_/Layer%d",m_iterations,kk,rank);
//         DeOTDetector* m_ot = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
//         DeOTModule* module = m_ot->findModule( id.otID() );
//         const OTChannelID  chID  = id.otID();
//         const unsigned int straw = chID.straw();
//         bool mLayA = module->monoLayerA(straw);
//         if (mLayA) sprintf(meapl,"glo%d_res%d_/Layer%d-monoA",m_iterations,kk,rank);
//         if (!mLayA) sprintf(meapl,"glo%d_res%d_/Layer%d-monoB",m_iterations,kk,rank);
//         //plot(meas.x,meapl,meapl,-4.,4,60); 

//         tres.push_back(meas.x);//for tuple		
//         trank.push_back(rank);
//         tzpos.push_back(relaZ);
//         zref.push_back(meanZ);
//         tstraw.push_back(straw);
//         tmodule.push_back(module->moduleID());
//         tlayer.push_back(module->layerID());
//         tquarter.push_back(module->quarterID());
//         tstation.push_back(module->stationID());
//         unsigned int strawID = chID.uniqueStraw();
//         tstrawunique.push_back(strawID);
//         if (mLayA)  tmonoA.push_back(1);
//         if (!mLayA) tmonoA.push_back(0);
//         //info() << " --> stereo = " << stereo_angle << " --> meas.x = " << meas.x << endreq; 

//         /*****************
//          * remove outlier*
//          *****************/
//         if (kk >3 && fabs(meas.x) > m_resMax) {
//           m_outlier++;
//           m_out = true;
//           char nout[100];
//           sprintf(nout,"rank of outlier,Iteration_%d",m_iterations);
//           plot(rank,nout, nout,0.,30.,30);
//           break;
//         }
//         /*****************
//          * fill matrix   *
//          *****************/
//         //relaZ = relaZ+mapdzthis[Z_position];
//         if(Tparam==4)  m_taConfig->ConfMatrix(meas.x,weight,rank,relaZ,&locTr[0],locMat, stereo_angle);
//         if(Tparam==5)  m_taConfig->ConfMatrixP(meas.x,weight,rank,relaZ,&locTr[0],locMatP,stereo_angle);
 
//       }//IDs



//       /************************
//        *  chi2 check && outlier*
//        *************************/
//       if(m_out) break;
//       char chi2[100];
//       sprintf(chi2,"totChi2_iter_%d",kk);
//       plot(totChi2,chi2,chi2,0.,100.,100);
//       //----- reduced chi2 ------------------
//       double nHits = trank.size();
//       redChi2  = totChi2/nHits; 
//       sprintf(chi2,"redChi2_iter_%d",kk);
//       plot(redChi2,chi2,chi2,0.,10.,100);
//       //----- chi2 probability ---------------
//       double ndof = nHits-Tparam;
//       double chi2max = gsl_cdf_chisq_Qinv (1e-15, ndof); 
//       double prob = totChi2 < chi2max ? gsl_cdf_chisq_Q(totChi2,ndof) : 0; 
//       sprintf(chi2,"Chi2prob_iter_%d",kk);
//       plot(prob,chi2,chi2,0.,1.,100);
      

        
//       m_chi2 = totChi2;
//       if(kk !=0 && totChi2 > m_newScale && m_newScale != -1) {
//         chiOK = false;
//         break;
//       }

//       if(Tparam==4){
//         m_locrank = m_taConfig->InvMatrix(locMat,&locTr[0],Tparam);
//         if(m_locrank !=4 ) info() << " BAD RANK for LOCAL FIT !! ( rank = " << m_locrank<< " ) " << endreq;
//       }
//       if(Tparam==5) {
//         m_locrank = m_taConfig->InvMatrixP(locMatP,&locTr[0],Tparam);
//         if(m_locrank !=5 ) debug() << " BAD RANK for LOCAL FIT !! " << endreq;
//       }

//       //sprintf(chi2,"used totChi2_iter_%d",kk);
//       ///plot(totChi2,chi2,chi2,0.,100.,100);
//       //       info() << std::setiosflags(std::ios::left) << setw(10) << totChi2    << setw(3) <<endreq 
//       //              << std::setiosflags(std::ios::left) << setw(10) << locTr[0]   << setw(5) <<endreq 
//       //              << std::setiosflags(std::ios::left) << setw(10) << locTr[1]   << setw(5) <<endreq 
//       //              << std::setiosflags(std::ios::left) << setw(10) << locTr[2]   << setw(5) <<endreq 
//       //              << std::setiosflags(std::ios::left) << setw(10) << locTr[3]   << setw(5) <<endreq;

//       totChi2 = 0.;
//       debug() << "--> Iteration " << kk << " trrack par locTr: x0=" << locTr[0] << "+-"<< sqrt(locMat[0][0] )
//               << " tx=" << locTr[1] << "+-"<< sqrt(locMat[1][1])
//               << " y0=" << locTr[2] << "+-"<< sqrt(locMat[2][2])
//               << " ty=" << locTr[3] << "+-"<< sqrt(locMat[3][3])
//               << " tQ = " << locTr[4] << endreq;
//     }//kk


    


//     // update trpar only if previous track is good
//     // otherwise we store the track which had a bad 
//     // chi2 or bad outlier
//     std::vector<double> trError(4,0);
//     if(!m_out && chiOK){
//       for( int i=0; i< Tparam;i++){
//         trParV[i] += locTr[i];
//         trError[i]= sqrt(locMat[i][i]);
//       }
//     }
   

//     //dirty hack to select downstream and upstream cosmics
//     // ty < 0  == downstream
//     // ty > 0  == upstream
//     //    if( trParV[3] <= 0.0000001){
//     //     if( trParV[3] >= -0.0000001){
//     //       warning() << " ---> Select downstream/upstream tracks !!!! " << endreq;
//     //       m_out = true; //set true for clearance of tuple vectors
//     //     }
      
   

//     if(m_out || (!chiOK) ){
//       tres.clear();
//       tzpos.clear();
//       zref.clear();
//       trank.clear();
//       tstraw.clear();
//       tstrawunique.clear();
//       tstation.clear();
//       tlayer.clear();
//       tquarter.clear();
//       tmodule.clear();
//       for(int i=0; i<Tparam;i++){
//         trParV[i]= -9999;
//       }
//     }
    
//     tres.resize(50);// take care : additional zero!!! 50 because for cosmics we have sometimes 
//     trank.resize(50);                              // more than 30 LHCbIDs per track
//     tmonoA.resize(50);
//     tstrawunique.resize(50);
//     tstraw.resize(50);
//     tzpos.resize(50);
//     zref.resize(50);
//     tmodule.resize(50);
//     tquarter.resize(50);
//     tlayer.resize(50);
//     tstation.resize(50);
//     trParV.resize(Tparam);
//     unsigned n_tres   = tres.size();
//     unsigned n_trank  = trank.size();
//     unsigned n_tmonoA = tmonoA.size();
//     unsigned n_tstrawunique = tstrawunique.size();
//     unsigned n_tstraw   = tstraw.size();
//     unsigned n_tmodule  = tmodule.size();
//     unsigned n_tquarter = tquarter.size();
//     unsigned n_tlayer   = tlayer.size();
//     unsigned n_tstation = tstation.size();
//     unsigned n_trParV   = trParV.size();
//     unsigned n_trError  = trError.size();
//     unsigned n_tzpos    = tzpos.size();
//     unsigned n_zref     = zref.size();
//     t_resids->farray("TrackParameter",trParV,"n_TrackParameter",n_trParV);
//     t_resids->farray("TrackParError",trError,"n_TrackParError",n_trError);
//     t_resids->farray("Z",tzpos,"n_Z",n_tzpos);
//     t_resids->farray("refZ",zref,"n_refZ",n_zref);
//     t_resids->farray("Trresid",tres,"n_tres",n_tres);
//     t_resids->farray("rank",trank,"n_trank",n_trank);
//     t_resids->farray("monoA",tmonoA,"n_tmonoA",n_tmonoA);
//     t_resids->farray("uniquestrawID",tstrawunique,"n_uniquestrawID",n_tstrawunique);
//     t_resids->farray("strawID",tstraw,"n_strawID",n_tstraw);
//     t_resids->farray("moduleID",tmodule,"n_moduleID",n_tmodule);
//     t_resids->farray("quarterID",tquarter,"n_quarterID",n_tquarter);
//     t_resids->farray("layerID",tlayer,"n_layerID",n_tlayer);
//     t_resids->farray("stationID",tstation,"n_stationID",n_tstation);
//     t_resids->column("usedChi2",m_chi2);
//     t_resids->column("NofIter",m_iterations);
//     t_resids->column("trackNo",m_tr_cnt);
//     t_resids->column("OutlierRejection",m_out);
//     t_resids->column("Chi2Rejection", (!chiOK));
//     t_resids->column("nTracksPerEvent", inCont->size());
//     t_resids->write();

//     if(m_out) {
//       debug()<< "--> REJECTED DUE TO OUTLIER" << endreq;
//       continue; //outlier-> next track
//     }
//     if ( !chiOK) {
//       debug()<< "--> REJECTED DUE TO CHISQUARE" << endreq;
//       m_chi2rej++;
//       continue; //next track
//     }

    
//     m_chi2 = 0.; //reset chi2

//     m_taConfig->SetTrackPar(trParV); 
//     std::vector< std::vector<double> > locVec;
//     MatrixToVector(&locMat[0][0],Tparam,Tparam,locVec,Tparam);
    
//     info() << "--> final params= "  << " : x0=" << trParV[0] 
//             << " tx=" << trParV[1] << " y0=" << trParV[2] << " ty=" << trParV[3]
//             << " tQ = " << trParV[4] << endreq;
    

//     /********************************************
//      **** 3rd loop to fill Matrix in Millepede **
//      ********************************************/
//     m_meas_cnt = 0;
//     for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
//       LHCb::LHCbID id = *itID;
      
//       //if ( atrack->isOnTrack( id ) &&  atrack->isMeasurementOnTrack( id )) {
//       if ( atrack->isOnTrack( id ) ){
//         //if ( atrack->isOnTrack( id )) {
//         double weight       = 1.;
//         double Z_position   = 0.;
//         double stereo_angle = 0.0;
//         struct Point meas;
//         sc =  m_taConfig->Rank( id, rank );
//         if ( sc.isFailure() ) 
//           debug() << "---> Couldn't calculate the rank of the object "<<endreq;
//         if ( sc.isSuccess() ) {
//           bool flagL        = true;//true if local fit already done
//           m_taConfig->SetMeanZ(meanZ);
//           sc=m_taConfig->CalcResidual(atrack, id, rank,  meas, weight,
//                                       flagL,Z_position, stereo_angle);
//           if ( sc.isFailure() )
//             continue;
//           //plot(meas.x,"3rdloopmeas","measCalcR",-3000,3000,500);
//           double relaZ = Z_position-meanZ;
//           struct Point pred; //give pred to fillmatrix
//           if(Tparam==4) pred.x = trParV[0] + trParV[1] * relaZ;//Z_position;
//           //               if(Tparam==5){
//           //                 pred.x = trParV[0] + trParV[1] * relaZ
//           //                   + trParV[4]
//           //                   *(relaZ * relaZ * cos( stereo_angle )+  2.2*1.e-3*relaZ*relaZ*relaZ*cos( stereo_angle ));
//           //               }
//           pred.y = trParV[2] + trParV[3] * relaZ;//Z_position;
          
//           info() << "3rd loop pred.x " << pred.x << " pred.y " << pred.y << endreq;
          
//           //relaZ = relaZ+mapdzthis[Z_position];          
//           /******************
//            *  get new meas  *
//            ******************/
//           //m_taConfig->NewMeasurement(meas.x,rank,pred,relaZ,stereo_angle);
//           //  		Gaudi::XYZPoint pRec;
//           // 		m_extrapolator->position((*atrack),Z_position,pRec);
//           // 		relaZ += 0.0036*pRec.y();
//           //with OTTimes
//           //meas.x += meas.y; // !!!! TAKE CARE OF ERROR IN 'FillMatrix()' !!!!

//           info() << "3rd loop meas.x " << meas.x << endreq;

//           sc = m_taConfig->FillMatrix( rank, pred,meas.x, stereo_angle,relaZ,1.44);
//           if ( sc.isFailure() ){
//             info() << " FAILED to call FillMatrix(..) " << endmsg;
//             continue;
//           }
//         } else 
//           continue;
//         //*****************************************************
//       }// if isonTrack && hasMeasurement
//     } //lhcb ids 
      
//     //        if(chi2ok){
//     m_tr_cnt++;      
//     m_estimated.resize(m_nGlPars,0.);//MD 26-03 m_prev_par;
//     double chi2     = 0;
//     double residual = -99999.9;
//     StatusCode locfit = m_taConfig->LocalTrackFit(m_tr_cnt,locVec,trParV,0,m_estimated, 
//                                                   chi2, residual, m_iterations,m_locrank);
//     if ( locfit != StatusCode::SUCCESS ) {
//       info() << "Track rejected , not used for filling the global matrix !" << endreq;
//     }
//     debug() << "--> !!! trrack par: x0=" << trParV[0] << " tx=" << trParV[1] << " y0=" << trParV[2] << " ty=" << trParV[3]
//             << endreq;
//     //  	if(locfit){
//     // 	  plot(1,"LocalTrackFit SUCCESS!", 0,2,2);
//     // 	  plot(trParV[1],"tx","slope in x track", -3,3,100);
//     // 	  plot(trParV[3],"ty","slope in y track", -3,3,100);
//     // 	  plot(trParV[0],"x0"," x0 track", -50,50,100);
//     // 	  plot(trParV[2],"y0"," y0 track",- 100,100,400);
//     // 	  plot(residual,"chi2/ndf_MP"," Chi2/nDOF (from loop 2)' ", -0.2,10,100);
//     // 	}
//     //compare positions & slopes 
//     //iterate over z positions
//     //         Gaudi::XYZPoint hitposRec;
//     //         Gaudi::XYZVector slopesRec;
//     //         std::vector< double > recslope;
//     //         std::vector< double > recpos;
//     //         //std::vector< double > hitpos;
//     //         //std::vector< double > slopes;
//     //         std::vector< double > zPos;
//     //         double zPosition[7] = {0.,2500.,5000.,6000.,7000.,7838.,9415.};
        
//     //         for(int i=0; i<7; ++i){
//     //           zPos.push_back(zPosition[i]);
//     //           m_extrapolator->position( (*atrack), zPosition[i], hitposRec);
//     //           m_extrapolator->slopes(   (*atrack), zPosition[i] ,slopesRec);
//     //           recslope.push_back( slopesRec.x() );
//     //           recslope.push_back( slopesRec.y() );
//     //           recslope.push_back( zPosition[i] );
//     //           recpos.push_back( hitposRec.x() );
//     //           recpos.push_back( hitposRec.y() );
//     //           recpos.push_back( zPosition[i] );
//     //           //calculate positions & slope from own trackfit
//     //           //  hitpos.push_back( trParV[0]+zPosition[i]*trParV[2] );//x
//     //           //slopes.push_back(trParV[2]);//x
//     //           // hitpos.push_back( trParV[4]+zPosition[i]*trParV[6] );//y
//     //           //hitpos.push_back( zPosition[i]);//z
//     //           //slopes.push_back(trParV[6]);
//     //           // slopes.push_back( zPosition[i]);//z
//     //         }
        
//     //         //book a tuple
//     //         Tuple t_tracks = nTuple("tracksInformation","mytracksTuple");
//     //         //MD tuple struct
//     //         t_tracks->column("trackkey", atrack->key() );
//     //         t_tracks->column("iteration_No",m_iterations);
//     //         t_tracks->column("successfit",locfit);
//     //         t_tracks->column("trackCounter",m_tr_cnt);
//     //         std::vector< double > fitoff;
//     //         //         std::vector< double > fitoffErr;
//     //         std::vector< double > fitslope;
//     //         //         std::vector< double > fitslopeErr;
//     //         fitoff.push_back(trParV[0]);
//     //         fitoff.push_back(trParV[2]);
//     //         //         fitoffErr.push_back(trParV[1]);
//     //         //         fitoffErr.push_back(trParV[5]);
//     //         fitslope.push_back(trParV[1]);
//     //         fitslope.push_back(trParV[3]);
//     //         if(Tparam==5) fitslope.push_back(trParV[4]);
//     //         int n_fitslope = trParV.size() - 2;
//     //         //         fitslopeErr.push_back(trParV[3]);
//     //         //         fitslopeErr.push_back(trParV[7]);
//     //         t_tracks->farray("fit_xy_offset",fitoff, "n", 2);
//     //         //         t_tracks->farray("x_y_offset_fit_err",fitoffErr, "n", 2);
//     //         t_tracks->farray("fit_xy_slope",fitslope, "n_fitlope", n_fitslope);
//     //         //         t_tracks->farray("x_y_slope_fit_err",fitslopeErr, "n", 2);
//     //         //         int n_hitpos = hitpos.size();
//     //         //         int n_zPos = zPos.size();
//     //         //         int n_slopes = slopes.size();
//     //         //         //info()<<"n_hitpos = " << n_hitpos << endreq;
//     //         //         t_tracks->farray("x_y_z_positionsFit",hitpos,"n_hitpos", n_hitpos);
//     //         //         t_tracks->farray("z_Positions",zPos,"n_zPos",n_zPos);	
//     //         //         t_tracks->farray("slopes_x_y_zFit",slopes,"n_slopes",n_slopes);
//     //         int n_recsl = recslope.size();
//     //         int n_recpos= recpos.size();
//     //         t_tracks->farray("x_y_z_slope_Reco",recslope, "n_recsl", n_recsl );
//     //         t_tracks->farray("x_y_z_position_Reco",recpos, "n_recpos", n_recpos  );
//     //         t_tracks->column("pseudorapidity_Rec",atrack->pseudoRapidity() );
//     //         t_tracks->column("phi_Rec",atrack->phi() );
//     //         t_tracks->column("chi2_Rec",atrack->chi2() );
//     //         t_tracks->write();
        
//     // 	//         }//if chi2ok
//     //         else if(!chi2ok){
//     //           std::vector<double>  zero(4,0.);
//     //           m_taConfig->SetTrackPar(zero, m_tr_cnt); // set the local parameters for this track  to 0               
//     //           debug() << " goto next track..."<<endreq;
//     //         }
//     //}//long tracks and required #OTHits 
//   }//tracks 
//   //}// input container


  
//   return StatusCode::SUCCESS;
// }


StatusCode GAlign::execute() {
  debug() << "Into GAlign execute (1)" << endreq
          << "***********************" << endreq
          << "***  NEXT EVENT     ***" << endreq
          << "***********************" << endreq;
  m_nEvents++;
  if ( ! exist<Tracks>( m_inputcontainer ) ) {
    info() << "Error: no tracks in container " << m_inputcontainer << endreq;
    return StatusCode::FAILURE;
  } else {
    
    //-----------------------------
    //
    //  get tracks and loop
    //  over tracks
    //
    //----------------------------
    Tracks *inCont = get<Tracks>(m_inputcontainer);
    Tracks::const_iterator iterT;
    for ( iterT = inCont->begin(); iterT != inCont->end(); iterT++) {
      Track *atrack = *iterT;
      m_totalTracks++;
      debug() << " ---> next track " << endreq;
      
      //plot(1,"# of all tracks (/Rec/track/Best)","alltracks",0.,3.,3);
      if( atrack->checkType(LHCb::Track::Long) ) {
        //plot(1,"# of longtracks","longtracks",0.,3.,3);
      }

      m_taConfig->MakeTrackParVec(); 
      if ( atrack->nMeasurements() == 0 ) {
        // load the measurements on the track
        //m_measProvider->load();
        // calculate from the track what the measurement was
        StatusCode mPro = m_measProvider->load( *atrack );
        if ( ! mPro ) {
          info() << "Error re-creating measurements!" << endreq;
          return StatusCode::FAILURE;
        }
      }
      
      bool acceptTr =  m_trackselection->accept( *atrack );
      if( !acceptTr){
          m_RejectbySelector++;
          debug() << "---> track NOT accepted " << endreq;
      }
      if( acceptTr) {
        debug() << "---> track IS  accepted " << endreq;
        plot(atrack->p(),"accepted track momentum","rejected track momentum",0.,10000.,1000);
        
        StatusCode sc =  m_taConfig->ResetGlVars();        // Reset MilleTool variables etc.
        //count multiple clusters per rank
        int    cluster[m_taConfig->NumAlignPars()];
        for(int cl=0; cl < m_taConfig->NumAlignPars(); cl++){
            cluster[cl] = 0;
        }
//        int rank = 0;
        // position in c-matrix for a detector element
        double meanZ=0.;
        double sumZ =0.;
        int    nofZ =0;
        std::vector<LHCb::LHCbID>::const_iterator itID;
        debug() << "Number of hits on track: " << atrack->lhcbIDs().end() - atrack->lhcbIDs().begin() << endreq;
        for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
          int rank            = 0; // position in c-matrix for a detector element        
          double weight       = 0.;
          double Z_position   = 0.;
          double stereo_angle = 0.0;
          double refZ         = 0.;
          struct Point meas;
          struct Point localPos;
          bool flagL=false;//leave it false here!
          Gaudi::XYZPoint modCenter;
       
          LHCb::LHCbID id = *itID;      
          debug() << "Checking whether hits are on track, and in OT" << endreq;
          if ( !atrack->isOnTrack( id )   ) continue;
          if ( !ot_detector || !id.isOT() ) continue;
          sc =  m_taConfig->Rank( id, rank );
          if ( sc.isFailure() ){
            debug() << "---> Couldn't calculate the rank of the object "<<endreq;
            continue;
          }
          
          debug()<<" GET MEANZ GET MEANZ GET MEANZ GET MEANZ "<<endreq;
          sc=m_taConfig->CalcResidual(atrack, id, rank,  meas,weight,
                                      flagL ,Z_position, stereo_angle,refZ,localPos);
          if ( sc.isFailure() ){
            debug() << "Failure to calculate residual... will continue!" << endreq;
            continue;
          }
          cluster[rank]++;
          sumZ += Z_position;
          nofZ++;
          
          
        }
        if ( nofZ < 1 )  {
	   debug() << "No OT hits found on track!" << endreq;
           continue;
        }
        meanZ = sumZ/nofZ;
        //meanZ = 0.;
        debug() <<" ---> nofZ = " <<nofZ << " meanZ = " <<meanZ << "(sumZ = "<<sumZ <<")"<<endreq;

        //---------------------------------------
        //
        //    choose only good tracks
        //
        //---------------------------------------
        bool badcluster = false;
        for(int a=0; a<m_taConfig->NumAlignPars(); a++){
            if(cluster[a] >=3 ) badcluster = true;
        }
        if(badcluster && m_skipBigCluster) {
            m_badCluster++;
            continue;
        }
        m_acceptedTr++;
        
        DeOTDetector* deotdet             = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
        std::vector<DeOTStation*> station = deotdet->stations();
        double stsum = 0.0;
        double stmean = 0.0;
        Gaudi::Rotation3D Rot; Gaudi::XYZVector Tra;
        for(unsigned i=0;i<station.size();i++){
          station.at(i)->geometry()->toGlobalMatrixNominal().GetDecomposition( Rot, Tra);    
          stsum += Tra.z();
        }
        stmean = stsum/3.;
        debug() << "meanZ " << meanZ << " stmean "<< stmean << endreq;
        // meanZ = stmean;
        



        int Tparam = m_taConfig->NumTrPars();
        StatusCode zeroSC;
        double locTr[Tparam];
        double locMat[4][4];
        double locMatP[5][5];
        std::vector<double> trParV(Tparam,0.);
        std::vector<double>  zero(Tparam,0.);
        std::vector<double> m_prevTrack(Tparam,0.);  

        m_taConfig->SetTrackPar(zero); // set the local parameters for this track                 
        if(Tparam==4) zeroSC = m_taConfig->ZeroMatrVec(locMat, &locTr[0]);          
        if(Tparam==5) zeroSC = m_taConfig->ZeroMatrVecP(locMatP, &locTr[0]);          
	
        debug() << "--> Track = "<<m_tr_cnt<<endreq;
        
        /*****************************
         ** track fit (iterative) ***
         ****************************/
        bool chiOK = true;
        m_out      = false;
        m_badXval  =  false;
        m_fitconv  =  false;
        Tuple t_resids = nTuple("residuals","residuals");
        std::vector<double> tres; // residuals for tuple
        std::vector<double> trank; // rank for tuple
        std::vector<int>    tmonoA; // if monoA=true  for tuple
        std::vector<double> tzpos; 
        std::vector<double> zref; 
        std::vector<unsigned int> tstrawunique; // if monoA=true  for tuple
        std::vector<unsigned int> tstraw; 
        std::vector<unsigned int> tmodule; 
        std::vector<unsigned int> tquarter; 
        std::vector<unsigned int> tlayer; 
        std::vector<unsigned int> tstation; 

        for(unsigned int kk=0;kk<10;kk++){
          unsigned int itTrdelta = 3; // after iteration 'itTrdelta' calculate change of track params
          unsigned int converged = 0;
          debug() <<"1: -----------------------------------------------------------"<<endreq;            
          debug() << "--> Tr="<<m_tr_cnt<<" nIds="<<atrack->nLHCbIDs()<<" type="<<atrack->type() <<endreq
                  << " kk = "<<kk<<" change to prev parameters: " << endreq;
          for(int m=0;m<Tparam;m++)
            debug() << m << " " << locTr[m] <<endreq;
          
          
          if(kk<=itTrdelta){
            for(int n=0;n<Tparam;n++)
              trParV[n] = locTr[n];
          }
          
          if(kk > itTrdelta){
            for(int n=0;n<Tparam;n++)
              trParV[n] += locTr[n];
            //check the change of track parameters
            if(fabs(locTr[0]) < 1e-3) converged++;
            if(fabs(locTr[1]) < 1e-5) converged++;
            if(fabs(locTr[2]) < 1e-3) converged++;
            if(fabs(locTr[3]) < 1e-5) converged++;
            if(Tparam==5 && fabs(locTr[4]) < 1e-6) converged++;
          }
          if(Tparam == 4  && converged == 4) break;
          if(Tparam == 5  && converged == 5) break;
          if(kk==9){ // the track hasn't converged until now
            m_fitconv = true;
            m_nNotConv++;
            break;
          }
          
          m_taConfig->SetTrackPar(trParV); // set the local parameters for this track                 
          if(Tparam==4) zeroSC = m_taConfig->ZeroMatrVec(locMat, &locTr[0]);          
          if(Tparam==5) zeroSC = m_taConfig->ZeroMatrVecP(locMatP, &locTr[0]);          
          double x0= trParV[0];
          double tx= trParV[1];
          double y0= trParV[2];
          double ty= trParV[3];
          double tQ=0.;
          if(Tparam==5) tQ=trParV[4];
          
          debug() << "--> " << kk << " begin: x0=" << trParV[0] << " tx=" << trParV[1] 
                  << " y0=" << trParV[2] << " ty=" << trParV[3] <<  endreq;
          debug() <<"2: -----------------------------------------------------------"<<endreq;
          
          //---------------------------------------
          // prepare track parameter for tuple
          //--------------------------------------          
          double totChi2 = 0.;
          double redChi2 = 0.;
          tres.clear();
          tzpos.clear();
          zref.clear();
          trank.clear();
          tstraw.clear();
          tstrawunique.clear();
          tstation.clear();
          tlayer.clear();
          tquarter.clear();
          tmodule.clear();

          for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
            int rank            = 0; // position in c-matrix for a detector element        
            double weight       = 0.;
            double Z_position   = 0.;
            double stereo_angle = 0.0;
            double refZ         = 0.;
            double relaZ        = 0.;
            double guess        = 0.;
            struct Point meas; meas.x=0; meas.y=0;
            struct Point localPos; localPos.x=0; localPos.y = 0;
            bool flagL =true;
            Gaudi::XYZPoint modCenter;
            // check if this LHCbID is really on the track
            LHCb::LHCbID id = *itID;
            if ( !atrack->isOnTrack( id )   ) continue;
            if ( !ot_detector || !id.isOT() ) continue;
            sc =  m_taConfig->Rank( id, rank );
            if ( sc.isFailure() ){
              debug() << "---> Couldn't calculate the rank of the object "<<endreq;
              continue;
            }
            m_taConfig->SetMeanZ(meanZ);

            debug() <<"3: -----------------------------------------------------------"<<endreq;            
            debug() <<"TRACKFIT LOOP TRACKFIT LOOP TRACKFIT LOOP TRACKFIT LOOP "<<endreq;
            debug() <<"RANK = "<<rank << " Tr="<<m_tr_cnt << "   at track iter "<< kk << " ====>  meas.x = " << meas.x<<endreq;
            for(unsigned int i=0;i<trParV.size();i++) debug() << "Param " <<i<<" = "<< trParV[i] <<endreq;
            
            
                        
            sc=m_taConfig->CalcResidual(atrack, id, rank,  meas,weight,
                                        flagL ,Z_position, stereo_angle,refZ, localPos);
            if ( sc.isFailure() ){
              debug() << "Failure in calculating residual!" << endreq;
              continue;
            }
            relaZ = refZ;
            if(ty != 0.)  plot2D(localPos.x,localPos.y,"X_m_VS_Y_m","X_m_VS_Y_m",-200.,200.,-3000.,3000,200,1000);            
            /******************
             * guess position *
             ******************/
            Gaudi::XYZPoint predP;
            Gaudi::XYZVector slRec;
            double dRatio = -3.81831e-4;
            
            if(Tparam==4 ){
              debug() << " bef guess used relaZ = " << relaZ << "  localPos.y " << localPos.y << endreq;              
              guess = (x0+tx*relaZ)*cos(stereo_angle) + (y0+ty*relaZ)*sin(stereo_angle);
              //from LHCb trackfit
              //m_extrapolator->position((*atrack),Z_position,predP);
              //m_extrapolator->slopes((*atrack),Z_position,slRec);
            }
            if(Tparam==5){
              debug() << " bef guess used relaZ = " << relaZ << "  localPos.y " << localPos.y << endreq;              
              guess = (x0+tx*relaZ)*cos(stereo_angle)+ (y0+ty*relaZ)*sin(stereo_angle)
                +  tQ*((relaZ * relaZ * cos( stereo_angle )) +  dRatio*relaZ*relaZ*relaZ*cos( stereo_angle ));
              //+  tQ*((relaZ * relaZ * cos( stereo_angle )));
            }

            double mMeas     = -999.; // meas to fill the matrix with
            /*********************************
             ****
             ****        fill matrix      ****
             ****
             *********************************/
            if( kk <  itTrdelta ) mMeas = meas.x ;//- guess;
            //if( kk >= itTrdelta ) mMeas = meas.x - guess;
            double mslope = x0 + tQ*( 2*relaZ + 3*dRatio*relaZ*relaZ);
            if( kk >= itTrdelta ) mMeas = (meas.x - guess)/(sqrt(1+mslope*mslope));
            //             if(kk==1 || kk==2) {
            //               // nur fuer Layer
            //               //relaZ += dzdy*localPos.y;
            //               //   relaZ += dzdy*refZ; //refZ == Y_m
            //               mMeas = meas.x;
            //             }
            debug() << "--> before CONFMATRIX :     Tparam = " << Tparam<<endreq
                    << "                             mMeas = "<< mMeas << endreq
                    << "                             meanZ = " << meanZ << " Zposition = " << Z_position << endreq;
            if(Tparam==4) 
		StatusCode sc = m_taConfig->ConfMatrix(mMeas,weight,rank,relaZ,&locTr[0],locMat, stereo_angle);
            if(Tparam==5) 
		StatusCode sc = m_taConfig->ConfMatrixP(mMeas,weight,rank,relaZ,&locTr[0],locMatP,stereo_angle);
            debug() << "                        used relaZ = " << relaZ << " localPos.y " << localPos.y << endreq;
            
            //beruecksichtige OTTimes, wenn m_driftT = true (TAConfig)
            //            meas.x += meas.y;
            debug() << "--> used meas.x= "<< meas.x << endreq;
            meas.x -= guess;
            debug() <<"--> meas.x-guess = " << meas.x << endreq;
            totChi2 += weight*meas.x*meas.x; 


	    
            char meapl[100];
            sprintf(meapl,"RES_gloIt_%d_fit_%d",m_iterations,kk);
            plot(meas.x,meapl,meapl,-10.,10.,80); 
            sprintf(meapl,"glo%d_res%d_/Layer%d",m_iterations,kk,rank);
            plot(meas.x,meapl,meapl,-10.,10.,80); 
	      
     	    
            DeOTDetector* m_ot       = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
            DeOTStation* station     = m_ot->findStation( id.otID() );
            DeOTModule* module       = m_ot->findModule( id.otID() );
            const OTChannelID  chID  = id.otID();
            const unsigned int straw = chID.straw();
            bool mLayA = module->monoLayerA(straw);
            if (mLayA) sprintf(meapl,"glo%d_res%d_/Layer%d-monoA",m_iterations,kk,rank);
            if (!mLayA) sprintf(meapl,"glo%d_res%d_/Layer%d-monoB",m_iterations,kk,rank);
            debug() << "Hit was in module: " << meapl << endreq;
	    plot(meas.x,meapl,meapl,-4.,4,60); 
            sprintf(meapl,"rank_iter%d",m_iterations);
            plot(rank,meapl,meapl,0.,220.,220);
            sprintf(meapl,"T_%d--hitMod",station->stationID());
            plot(module->moduleID(),meapl,meapl,0.,10.,10);


            //--------------------
            // plots zum debuggen
            //--------------------
            plot(tQ,"tQ","tQ",-1e-4,1e-4,1000);
            plot( tQ*((relaZ * relaZ * cos( stereo_angle )) +  dRatio*relaZ*relaZ*relaZ*cos( stereo_angle )),
                  "tQ_anteil","tQ_anteil",-10,10,250);
            
            // berechne abstand in x ebene
            if(kk >= itTrdelta){
              double myX =  (x0+tx*relaZ) +  tQ*((relaZ * relaZ ) +  dRatio*relaZ*relaZ*relaZ);
              //                tQ*(relaZ * relaZ );
              double myY = y0+ty*relaZ;
              char fitit[100];
              if(stereo_angle == 0.){
                sprintf(fitit,"DTX_myfit-otfit_kk=%d_X",kk);
                plot(mslope-slRec.x(),fitit,fitit,-1,1,1000);

                sprintf(fitit,"TX_myfit_kk=%d_X",kk);
                plot(mslope,fitit,fitit,-1,1,1000);
                sprintf(fitit,"TX_otfit_kk=%d_X",kk);
                plot(slRec.x(),fitit,fitit,-1,1,1000);
                sprintf(fitit,"DTY_myfit-otfit_kk=%d_X",kk);
                plot(ty-slRec.y(),fitit,fitit,-1,1,1000);

                sprintf(fitit,"DX_myfit-otfit_kk=%d_X",kk);
                plot(myX-predP.x(),fitit,fitit,-6,6,400);
           
                sprintf(fitit,"DY_myfit-otfit_kk=%d_X",kk);
                plot(myY-predP.y(),fitit,fitit,-10,10,500);
              }
              if(stereo_angle != 0.){
                sprintf(fitit,"UV_DTX_myfit-otfit_kk=%d_UV",kk);
                plot(mslope-slRec.x(),fitit,fitit,-1,1,1000);
                sprintf(fitit,"TX_myfit_kk=%d_UV",kk);
                plot(mslope,fitit,fitit,-1,1,1000);
                sprintf(fitit,"TX_otfit_kk=%d_UV",kk);
                plot(slRec.x(),fitit,fitit,-1,1,1000);
                sprintf(fitit,"UV_DTY_myfit-otfit_kk=%d_UV",kk);
                plot(ty-slRec.y(),fitit,fitit,-1,1,1000);
                sprintf(fitit,"UV_DX_myfit-otfit_kk=%d_UV",kk);
                plot(myX-predP.x(),fitit,fitit,-6,6,400);
                sprintf(fitit,"UV_DY_myfit-otfit_kk=%d_UV",kk);
                plot(myY-predP.y(),fitit,fitit,-10,10,500);
              }

              sprintf(fitit,"T_%d-Xdiff",station->stationID());
              plot(myX-predP.x(),fitit,fitit,-6,6,100);
              sprintf(fitit,"T_%d-Ydiff",station->stationID());
              plot(myY-predP.y(),fitit,fitit,-6,6,100);
              
            }

            
            m_hitRnk[rank] += 1;
	    debug() << "Filled debug plots..." << endreq;
            
                        
            tres.push_back(meas.x);//for tuple		
            trank.push_back(rank);
            tzpos.push_back(relaZ);
            zref.push_back(meanZ);
            tstraw.push_back(straw);
            tmodule.push_back(module->moduleID());
            tlayer.push_back(module->layerID());
            tquarter.push_back(module->quarterID());
            tstation.push_back(module->stationID());
            debug() << "Pushed back a lot..." << endreq;
            unsigned int strawID = chID.uniqueStraw();
            tstrawunique.push_back(strawID);
            if (mLayA)  tmonoA.push_back(1);
            if (!mLayA) tmonoA.push_back(0);
            
            //info() << " --> stereo = " << stereo_angle << " --> meas.x = " << meas.x << endreq; 
            /*****************
             * remove outlier*
             *****************/
            debug() << "Removing outliers" << endreq;
            if (kk > itTrdelta && fabs(meas.x) > m_resMax) {
              m_outlier++;
              m_out = true;
              char nout[100];
              sprintf(nout,"rank of outlier,Iteration_%d",m_iterations);
              plot(rank,nout, nout,0.,220.,220);
              break;
            }
          }//IDs
          debug() << "Left lhcbid loop..." << endreq;

          if(trank.size() <= 12 ){
            m_badXval = true;
            debug() << "Problem with too few hits on track!" << endreq;
            break; //in this case most of the hits of the tracks are rejected
          }

          /************************
           *   outlier   *
           *************************/
          if(m_out) {
	    debug() << "Found an outlier; breaking out of loop!" << endreq;
	    break;
          }
          char chi2[100];
          //sprintf(chi2,"totChi2_iter_%d",kk);
          //plot(totChi2,chi2,chi2,0.,100.,100);
          double nHits = trank.size();
          double ndof = nHits-Tparam;
          //----- reduced chi2 ------------------
          debug() << "Calculating redChi2: totChi2 = " << totChi2 << " ndof = " << ndof << endreq;
          redChi2  = totChi2/ndof;
          debug() << "redChi2_iter = " << redChi2 << endreq;
          sprintf(chi2,"redChi2_iter_%d",kk);
          plot(redChi2,chi2,chi2,0.,100.,250);
          //----- chi2 probability ---------------
          double chi2max = gsl_cdf_chisq_Qinv (1e-15, ndof);
          double prob = totChi2 < chi2max ? gsl_cdf_chisq_Q(totChi2,ndof) : 0;
          sprintf(chi2,"Chi2prob_iter_%d",kk);
          plot(prob,chi2,chi2,0.,1.,100);
          m_chi2 = totChi2;
          if(kk !=0 && redChi2 > m_newScale && m_newScale != -1) {
              chiOK = false;
              debug() << "Chi2 bad; breaking out of loop" << endreq;
              break;
          }

          if(Tparam==4){
            m_locrank = m_taConfig->InvMatrix(locMat,&locTr[0],Tparam);
            if(m_locrank !=4 ){
              info() << " BAD RANK for LOCAL FIT !! ( rank = " << m_locrank<< " ) " << endreq;
              m_locrank = -1;
              break;
            }
            
          }
          debug() << "inverting local matrix.."<< endreq;
          if(Tparam==5) {
            m_locrank = m_taConfig->InvMatrixP(locMatP,&locTr[0],Tparam);
            if(m_locrank !=5 ){
              debug() << " BAD RANK for LOCAL FIT !! " << endreq;
              m_locrank = -1;
              break;
            }
            
          }
          
          //sprintf(chi2,"used totChi2_iter_%d",kk);
          ///plot(totChi2,chi2,chi2,0.,100.,100);
          m_chi2 = totChi2;
          totChi2 = 0.;
          debug() << "--> Iteration " << kk << " trrack par locTr: x0=" << locTr[0] << " tx=" << locTr[1] 
                  << " y0=" << locTr[2] << " ty=" << locTr[3] << " tQ = " << locTr[4] 
                  << endreq;
          
        }//kk
        //------------------------------
        //
        // cut on the track slope
        //
        // if track has wrong slope,
        // handle it as outlier
        //-----------------------------
        if(m_slopeCut == -1  && trParV[3] >0.){
            m_out = true;
            // reject upstream tracks
            m_outlier++;
        }
        if(m_slopeCut == +1  && trParV[3] <0.){
            m_out = true;
            // reject downstream tracks
            m_outlier++;
        }

        //----- reduced chi2 ------------------
        char chchi2[100];
        double redChi2 = -999.;
        double nHits = trank.size();
        //info() << "tr " << m_tr_cnt << " "<<" nHits " << nHits << " totchi " << m_chi2 <<endreq;
        double ndof = nHits-Tparam;
        if(!m_out){
          redChi2  = m_chi2/ndof; 
          sprintf(chchi2,"redChi2_gl%d",m_iterations);
          plot(redChi2,chchi2,chchi2,0.,10.,100);
          //----- chi2 probability ---------------
          double chi2max = gsl_cdf_chisq_Qinv (1e-15, ndof); 
          double prob = m_chi2 < chi2max ? gsl_cdf_chisq_Q(m_chi2,ndof) : 0; 
          sprintf(chchi2,"Chi2prob_global_%d",m_iterations);
          plot(prob,chchi2,chchi2,0.,1.,100);
        }
        
        
        //          if(kk !=0 && redChi2 > m_newScale && m_newScale != -1) {
        if( !m_out && redChi2 > m_newScale && m_newScale != -1) {
          chiOK = false;
        }



        // update trpar only if previous track is good
        // otherwise we store the track which had a bad 
        // chi2 or bad outlier
        std::vector<double> trError(Tparam,0);
        if(!m_out && chiOK){
          for( int i=0; i< Tparam;i++){
            trParV[i] += locTr[i];
            if(Tparam==4) trError[i]= sqrt(locMat[i][i]);
            if(Tparam==5) trError[i]= sqrt(locMatP[i][i]);
          }
        }
        if(m_out || (!chiOK) || m_locrank ==-1 || m_badXval || m_fitconv){
          tres.clear();
          tzpos.clear();
          zref.clear();
          trank.clear();
          tstraw.clear();
          tstrawunique.clear();
          tstation.clear();
          tlayer.clear();
          tquarter.clear();
          tmodule.clear();
          for(int i=0; i<Tparam;i++){
            trParV[i]= -9999;
          }
        }




        tres.resize(50);// take care : additional zero!!! 50 because for cosmics we have sometimes 
        trank.resize(50);                              // more than 30 LHCbIDs per track
        tmonoA.resize(50);
        tstrawunique.resize(50);
        tstraw.resize(50);
        tzpos.resize(50);
        zref.resize(50);
        tmodule.resize(50);
        tquarter.resize(50);
        tlayer.resize(50);
        tstation.resize(50);
        trParV.resize(Tparam);
        unsigned n_tres   = tres.size();
        unsigned n_trank  = trank.size();
        unsigned n_tmonoA = tmonoA.size();
        unsigned n_tstrawunique = tstrawunique.size();
        unsigned n_tstraw   = tstraw.size();
        unsigned n_tmodule  = tmodule.size();
        unsigned n_tquarter = tquarter.size();
        unsigned n_tlayer   = tlayer.size();
        unsigned n_tstation = tstation.size();
        unsigned n_trParV   = trParV.size();
        unsigned n_trError  = trError.size();
        unsigned n_tzpos    = tzpos.size();
        unsigned n_zref     = zref.size();
        t_resids->farray("TrackParameter",trParV,"n_TrackParameter",n_trParV);
        t_resids->farray("TrackParError",trError,"n_TrackParError",n_trError);
        t_resids->farray("Z",tzpos,"n_Z",n_tzpos);
        t_resids->farray("refZ",zref,"n_refZ",n_zref);
        t_resids->farray("Trresid",tres,"n_tres",n_tres);
        t_resids->farray("rank",trank,"n_trank",n_trank);
        t_resids->farray("monoA",tmonoA,"n_tmonoA",n_tmonoA);
        t_resids->farray("uniquestrawID",tstrawunique,"n_uniquestrawID",n_tstrawunique);
        t_resids->farray("strawID",tstraw,"n_strawID",n_tstraw);
        t_resids->farray("moduleID",tmodule,"n_moduleID",n_tmodule);
        t_resids->farray("quarterID",tquarter,"n_quarterID",n_tquarter);
        t_resids->farray("layerID",tlayer,"n_layerID",n_tlayer);
        t_resids->farray("stationID",tstation,"n_stationID",n_tstation);
        t_resids->column("usedChi2",m_chi2);
        t_resids->column("NofIter",m_iterations);
        t_resids->column("trackNo",m_tr_cnt);
        t_resids->column("OutlierRejection",m_out);
        t_resids->column("Chi2Rejection", (!chiOK));
        t_resids->column("nTracksPerEvent", inCont->size());
        t_resids->write();

        if(m_out || (!chiOK) || m_locrank ==-1 || m_badXval || m_fitconv){
            tres.clear();
            tzpos.clear();
            zref.clear();
            trank.clear();
            tstraw.clear();
            tstrawunique.clear();
            tstation.clear();
            tlayer.clear();
            tquarter.clear();
            tmodule.clear();
            for(int i=0; i<Tparam;i++){
                trParV[i]= -9999;
            }
        }
        
        if(m_out) {
          debug()<< "--> REJECTED DUE TO OUTLIER" << endreq;
          plot(2,"# outlier","# outlier",0.,3.,3);
          continue; //outlier-> next track
        }
        if ( !chiOK) {
          debug()<< "--> REJECTED DUE TO CHISQUARE" << endreq;
          m_chi2rej++;
          plot(m_chi2,"# chi2","# chi2",0.,10000.,1000);
          continue; //next track
        }        
        if ( m_locrank ==-1) {
          debug()<< "--> REJECTED DUE TO BAD RANK" << endreq;
          continue; //next track
        }        
        if ( m_badXval) {
          debug()<< "--> REJECTED DUE TO BAD MEASURED X VALUE" << endreq;
          continue; //next track
        }         
        if ( m_fitconv) {
          debug()<< "--> REJECTED DUE TO NOT CONVERGED TRACK FIT" << endreq;
          continue; //next track
        }        
        m_chi2 = 0.; //reset chi2

        m_taConfig->SetTrackPar(trParV); 
        std::vector< std::vector<double> > locVec;
        if(Tparam==4){
          debug() << "--> final params in matrix form"  <<endreq;
          for(int h=0;h<Tparam;h++)
            debug() << h <<" "<<  trParV[h] <<" +- "<<sqrt(locMat[h][h]) <<endreq;
          MatrixToVector(&locMat[0][0],Tparam,Tparam,locVec);
          debug() << "--> final params in vector form"  <<endreq;
          for(int h=0;h<Tparam;h++)
            debug() << h <<" "<<  trParV[h] <<" +- "<<sqrt(locVec[h][h]) <<endreq;
            
        }
        if(Tparam==5){
          debug() << "--> final params in matrix form"  <<endreq;
          for(int h=0;h<Tparam;h++)
            debug() << h <<" "<<  trParV[h] <<" +- "<<sqrt(locMatP[h][h]) <<endreq;
          MatrixToVector(&locMatP[0][0],Tparam,Tparam,locVec);
          debug() << "--> final params in vector form"  <<endreq;
          for(int h=0;h<Tparam;h++)
            debug() << h <<" "<<  trParV[h] <<" +- "<<sqrt(locVec[h][h]) <<endreq;
        }
        
        


        /********************************************
         **** 3rd loop to fill Matrix in Millepede **
         ********************************************/
        m_meas_cnt = 0;
        for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
          int rank            = 0; // position in c-matrix for a detector element        
          double weight       = 0.;
          double Z_position   = 0.;
          double stereo_angle = 0.0;
          double refZ         = 0.;
//          double guess        = 0.;
//          double dzdy         = 0.0036;
          struct Point meas; meas.x=0; meas.y=0;
          struct Point localPos; localPos.x=0; localPos.y = 0;
          bool flagL =true;
          Gaudi::XYZPoint modCenter;
          
          LHCb::LHCbID id = *itID;
          if ( !atrack->isOnTrack( id )   ) continue;
          if ( !ot_detector || !id.isOT() ) continue;
          sc =  m_taConfig->Rank( id, rank );
          if ( sc.isFailure() ){
            debug() << "---> Couldn't calculate the rank of the object "<<endreq;
            continue;
          }

          debug() <<"4: -----------------------------------------------------------"<<endreq;            
          debug() <<"FILLMATRIX LOOP FILLMATRIX LOOP FILLMATRIX LOOP FILLMATRIX LOOP"<<endreq;
          debug()  <<"RANK = "<<rank << " Tr="<<m_tr_cnt << endreq
                   <<"( Tr="<<m_tr_cnt   <<" :  x0= " << trParV[0] << " tx=" << trParV[1] 
                   <<" y0=" << trParV[2] << " ty= "   << trParV[3] <<  endreq;
          
          m_taConfig->SetMeanZ(meanZ);
          sc=m_taConfig->CalcResidual(atrack, id, rank,  meas, weight,
                                      flagL,Z_position, stereo_angle, refZ,localPos);
          if ( sc.isFailure() )
            continue;
          //plot(meas.x,"3rdloopmeas","measCalcR",-3000,3000,500);
          //              double relaZ = Z_position-meanZ;
          double relaZ = refZ;
          //m_extrapolator->position((*atrack),Z_position,predP);
          //pred.x = predP.x();
          //pred.y = predP.y();
          //if(Tparam==4) localPos.x = trParV[0] + trParV[1] * relaZ;//Z_position;
          //               if(Tparam==5){
          //                 pred.x = trParV[0] + trParV[1] * relaZ + trParV[4]
          //                   *(relaZ * relaZ+  2.2*1.e-3*relaZ*relaZ*relaZ);
          //               }
          //localPos.y = trParV[2] + trParV[3] * relaZ;//Z_position;
          /******************
           *  get new meas  *
             ******************/
          //relaZ += dzdy*localPos.y; // nur fuer layer
          debug() << " ---> bef FillMatrix used relaZ = " << relaZ << endreq;
          debug() << " ---> locPos x, y  = " << localPos.x <<" " <<  localPos.y <<endreq;
          
          //with OTTimes
          //meas.x += meas.y; // !!!! TAKE CARE OF ERROR IN 'FillMatrix()' !!!!
          m_taConfig->FillMatrix( rank, localPos,meas.x, stereo_angle,relaZ,1/sqrt(weight));
          if ( sc.isFailure() ){
            info() << " FAILED to call FillMatrix(..) " << endmsg;
            continue;
          }
        } //lhcb ids 
        
        //        if(chi2ok){
        m_tr_cnt++;      
        
        /**************************************************************
         *  do local track fit with MP only when there was a track going through
         *  the detector part one wants to align.
         **********************************************************************/
        //plot(1,"LocalTrackFit - calls", 0, 2 ,2);
        m_estimated.assign(m_nGlPars,0.);//MD 26-03 m_prev_par;
        double chi2 = 0;
        double residual = -99999.9;
        
        StatusCode locfit = m_taConfig->LocalTrackFit(m_tr_cnt,locVec,trParV,0,m_estimated, 
                                                      chi2, residual, m_iterations,m_locrank);
        if ( locfit != StatusCode::SUCCESS ) {
          info() << "Track rejected , not used for filling the global matrix !" << endreq;
        }
        debug() << "--> locfit : "<< locfit <<" --> trrack par: x0=" << trParV[0] << " tx=" << trParV[1] 
                << " y0=" << trParV[2] << " ty=" << trParV[3] << " tq = " << trParV[4] << endreq;


        //---------------------------------------------------------------
        //  	if(locfit){
        // 	  plot(1,"LocalTrackFit SUCCESS!", 0,2,2);
        // 	  plot(trParV[1],"tx","slope in x track", -3,3,100);
        // 	  plot(trParV[3],"ty","slope in y track", -3,3,100);
        // 	  plot(trParV[0],"x0"," x0 track", -50,50,100);
        // 	  plot(trParV[2],"y0"," y0 track",- 100,100,400);
        // 	  plot(residual,"chi2/ndf_MP"," Chi2/nDOF (from loop 2)' ", -0.2,10,100);
        // 	}


//         //compare positions & slopes 
//         //iterate over z positions
//         Gaudi::XYZPoint hitposRec;
//         Gaudi::XYZVector slopesRec;
//         std::vector< double > recslope;
//         std::vector< double > recpos;
//         //std::vector< double > hitpos;
//         //std::vector< double > slopes;
//         std::vector< double > zPos;
//         double zPosition[7] = {0.,2500.,5000.,6000.,7000.,7838.,9415.};
        
//         for(int i=0; i<7; ++i){
//           zPos.push_back(zPosition[i]);
//           m_extrapolator->position( (*atrack), zPosition[i], hitposRec);
//           m_extrapolator->slopes(   (*atrack), zPosition[i] ,slopesRec);
//           recslope.push_back( slopesRec.x() );
//           recslope.push_back( slopesRec.y() );
//           recslope.push_back( zPosition[i] );
//           recpos.push_back( hitposRec.x() );
//           recpos.push_back( hitposRec.y() );
//           recpos.push_back( zPosition[i] );
//           //calculate positions & slope from own trackfit
//           //  hitpos.push_back( trParV[0]+zPosition[i]*trParV[2] );//x
//           //slopes.push_back(trParV[2]);//x
//           // hitpos.push_back( trParV[4]+zPosition[i]*trParV[6] );//y
//           //hitpos.push_back( zPosition[i]);//z
//           //slopes.push_back(trParV[6]);
//           // slopes.push_back( zPosition[i]);//z
//         }
        
//         //book a tuple
//         Tuple t_tracks = nTuple("tracksInformation","mytracksTuple");
//         //MD tuple struct
//         t_tracks->column("trackkey", atrack->key() );
//         t_tracks->column("iteration_No",m_iterations);
//         t_tracks->column("successfit",locfit);
//         t_tracks->column("trackCounter",m_tr_cnt);
//         std::vector< double > fitoff;
//         //         std::vector< double > fitoffErr;
//         std::vector< double > fitslope;
//         //         std::vector< double > fitslopeErr;
//         fitoff.push_back(trParV[0]);
//         fitoff.push_back(trParV[2]);
//         //         fitoffErr.push_back(trParV[1]);
//         //         fitoffErr.push_back(trParV[5]);
//         fitslope.push_back(trParV[1]);
//         fitslope.push_back(trParV[3]);
//         if(Tparam==5) fitslope.push_back(trParV[4]);
//         int n_fitslope = trParV.size() - 2;
//         //         fitslopeErr.push_back(trParV[3]);
//         //         fitslopeErr.push_back(trParV[7]);
//         t_tracks->farray("fit_xy_offset",fitoff, "n", 2);
//         //         t_tracks->farray("x_y_offset_fit_err",fitoffErr, "n", 2);
//         t_tracks->farray("fit_xy_slope",fitslope, "n_fitlope", n_fitslope);
//         //         t_tracks->farray("x_y_slope_fit_err",fitslopeErr, "n", 2);
//         //         int n_hitpos = hitpos.size();
//         //         int n_zPos = zPos.size();
//         //         int n_slopes = slopes.size();
//         //         //info()<<"n_hitpos = " << n_hitpos << endreq;
//         //         t_tracks->farray("x_y_z_positionsFit",hitpos,"n_hitpos", n_hitpos);
//         //         t_tracks->farray("z_Positions",zPos,"n_zPos",n_zPos);	
//         //         t_tracks->farray("slopes_x_y_zFit",slopes,"n_slopes",n_slopes);
//         int n_recsl = recslope.size();
//         int n_recpos= recpos.size();
//         t_tracks->farray("x_y_z_slope_Reco",recslope, "n_recsl", n_recsl );
//         t_tracks->farray("x_y_z_position_Reco",recpos, "n_recpos", n_recpos  );
//         t_tracks->column("pseudorapidity_Rec",atrack->pseudoRapidity() );
//         t_tracks->column("phi_Rec",atrack->phi() );
//         t_tracks->column("chi2_Rec",atrack->chi2() );
//         t_tracks->write();
        
        // 	//         }//if chi2ok
        //         else if(!chi2ok){
        //           std::vector<double>  zero(4,0.);
        //           m_taConfig->SetTrackPar(zero, m_tr_cnt); // set the local parameters for this track  to 0               
        //           debug() << " goto next track..."<<endreq;
        //         }
      }//long tracks and required #OTHits 
    }//tracks 
  }// input container
  return StatusCode::SUCCESS;
}


/*************************
 ****    GLOBAL FIT  *****
 **************************/

StatusCode GAlign::GloFit() {

    info() << " ---> Number events analyzed         : " << m_nEvents << endreq;
    info() << " ---> total tracks loaded            : " << m_totalTracks << endreq;
    info() << " ---> tracks rejected by TrackSelect.: " << m_RejectbySelector << endreq;
  info() << " ---> bad Cluster (>=3 hits)         : " << m_badCluster  << endreq;
  info() << " ---> accepted tracks                : " << m_acceptedTr << endreq;
  info() << " ---> tracks rejected due to chi2    : " << m_chi2rej << " (chi max = " <<m_newScale 
         << ", min = "<< m_taConfig->GetMinChi2() << ")" <<endreq;
  info() << " ---> tracks rejected due to outlier : " << m_outlier << endreq;
  info() << " ---> tracks not converged           : " << m_nNotConv << endreq;

  m_nEvents = 0;
  m_totalTracks = 0;
  m_RejectbySelector = 0;
  m_badCluster = 0;
  m_acceptedTr = 0;
  m_chi2rej    = 0.;
  m_outlier    = 0.;
  m_tr_cnt     = 0;
  m_iterations++;  
  int alipar       =  m_taConfig->NumAlignPars();
  int n_misalInput = misalInput_X.size();//size for tuple
  std::vector<double> stations(alipar,0);//for histos...
  m_align.clear();
  m_align.resize( alipar,0. );  
  m_align_err.clear();
  m_align_err.resize( alipar,0. );

  //do global fit
  m_taConfig->GlobalFit( m_align, m_align_err, m_hitRnk);
  for ( int station = 0 ; station <  alipar ;station++){
    m_new_par[station]  += m_align[station];
  }
  
  /*
  // convert local shifts in measurement direction into 
  // x shifts in global frame
  int cnt_rnk = -1;
  std::multimap<int, std::pair<double, double> > stereoMap      = m_taConfig->GetRnkZAngleMap();
  std::multimap<int, std::pair<double, double> >::iterator rank = stereoMap.begin();
  for(; rank != stereoMap.end(); rank++){
  if(cnt_rnk == rank->first) continue;
  cnt_rnk               = rank->first;
  double stereo         = (rank->second).second;
  m_align[rank->first]  = m_align[rank->first]*cos(stereo);
  info() << "--> U/V Conversion: nObject=" << rank->first << " stereo="<< stereo <<endreq; 
  }
  */
  
  // add new parameter to misalignment constants from previous iteration
  // and save new geometry information in memory
  m_taConfig->PrintParameters(m_align,m_iterations);
  
//  int cnt = 0;
  std::vector<bool> dofs                  = m_taConfig->NumDOF();
  std::map<std::string, int> cap          = m_taConfig->GetCMap();
  std::map<std::string,int>::iterator maI = cap.begin();
//   for ( ; maI != cap.end(); ++maI ) {
//     debug() << "Parameter name = " << maI->first << " ;rank = " << maI->second;
//     IDetectorElement *det = getDet<IDetectorElement>(maI->first);
//     IGeometryInfo *geo    = det->geometry();
//     const Gaudi::Transform3D misalGlobal = geo->toGlobalMatrix();
//     const Gaudi::Transform3D nominGlobal = geo->toGlobalMatrixNominal();
//     const Gaudi::Transform3D deltaGlobal = misalGlobal*nominGlobal.Inverse();
//     const Gaudi::Transform3D deltalocal  = nominGlobal.Inverse()*deltaGlobal*nominGlobal;
//     std::vector<double> rotlocal(3,0.), shiftlocal(3,0.);
//     DetDesc::getZYXTransformParameters(deltalocal, shiftlocal, rotlocal);
    

//     debug() <<  maI->second << setw(12)
//             << " dx = " <<setw(12) << shiftlocal[0] << " dy = " <<setw(12) << shiftlocal[1] 
//             << " dz = " <<setw(12) << shiftlocal[2] 
//             << " da = " <<setw(12) << rotlocal[0] 
//             << " db = " << setw(12) << rotlocal[1] << " dc = " << setw(12)<< rotlocal[2] << endreq;
    
//     if ( dofs[0] ){
//       m_new_par[maI->second] =  shiftlocal[0];
//       cnt++;
//     }
//     if ( dofs[1] ) {
//       m_new_par[maI->second+cnt* m_taConfig->nRanks()] = shiftlocal[1]; 
//       cnt++;
//     }
    
//     if ( dofs[2] ) {
//       m_new_par[maI->second+cnt* m_taConfig->nRanks()] = shiftlocal[2];
//       cnt++;
//     }
//     if ( dofs[3] ) {
//       m_new_par[maI->second+cnt* m_taConfig->nRanks()] = rotlocal[0]; 
//       cnt++;
//     }
//     if ( dofs[4] ) {
//       m_new_par[maI->second+cnt* m_taConfig->nRanks()] = rotlocal[1]; 
//       cnt++;
//     }
//     if ( dofs[5] ) {
//       m_new_par[maI->second+cnt* m_taConfig->nRanks()] = rotlocal[2]; 
//       cnt++;
//     }
//     cnt=0;
    
//   }
  

  Tuple t_global = nTuple("globalPar","global_Parameter");
  t_global->farray("globalParam",m_new_par,"n_globalParam",alipar);
  t_global->farray("globalParam_err",m_align_err,"n_globalParam_err",alipar);
  t_global->farray("delToprePar",m_align,"n_globalParam",alipar);
  t_global->farray("stations",stations,"n_stations",alipar);
  t_global->column("iteration",m_iterations);
  t_global->column("No_parameter",alipar);

  t_global->farray("misalInput_X",misalInput_X,"n_misalInput_X",n_misalInput);
  t_global->farray("misalInput_Y",misalInput_Y,"n_misalInput_Y",n_misalInput);
  t_global->farray("misalInput_Z",misalInput_Z,"n_misalInput_Z",n_misalInput);
  t_global->farray("misalInput_A",misalInput_A,"n_misalInput_A",n_misalInput);
  t_global->farray("misalInput_B",misalInput_B,"n_misalInput_B",n_misalInput);
  t_global->farray("misalInput_C",misalInput_C,"n_misalInput_C",n_misalInput);

  t_global->write();
  if(m_newScale != -1){  
    if(sqrt(m_newScale) > m_taConfig->GetMinChi2() )  m_newScale=sqrt(m_newScale);
    else m_newScale = m_taConfig->GetMinChi2();
  }
  else info() << " No Chi2 cut in 'TAlignment.opts' selected !" << endreq;

  info() <<setw(16)<< "rank "<<setw(16)<<"misalIn X "<<setw(16) <<"misalIn Y"<<setw(16)<<"misalIn Z"
         <<setw(16)<<"misalIn A"  <<setw(16)<<"misalIn B" <<setw(16)<<"misalIn C" <<endreq;
  for ( int i = 0 ; i <  m_taConfig->nRanks() ;i++){
    info() <<setw(16)<< i << setw(16)<<misalInput_X[i]<<setw(16)<<misalInput_Y[i]<<setw(16)<<misalInput_Z[i]
           <<setw(16)<<misalInput_A[i]<<setw(16)<<misalInput_B[i]<<setw(16)<<misalInput_C[i]<< endreq;
  }
  info() << " " << endreq;
  info() <<setw(16)<< "station"<<setw(16)<<setw(16)<<"prevPar"<<setw(16) <<"deltoprevP"<<setw(16)
         <<"newPar"<<setw(16)<<"errPar" <<endreq;
  for ( int station = 0 ; station <  alipar ;station++){
    if(station%m_taConfig->nRanks() == 0) info() << setw(16) <<  " ------- next DOF ------- " << endreq;   
    info() << setw(16)<< station <<setw(16)<< setw(16)  << m_prev_par[station] <<setw(16)
           << m_align[station]   <<setw(16)<< m_new_par[station]   <<setw(16) << m_align_err[station] <<   endreq;
    
  }

  info() << "------ Number of hits per rank ------------- " <<  m_hitRnk.size() << endreq;
  for ( maI= cap.begin(); maI != cap.end(); ++maI ) {
    info() << "Rank " << maI->second << " = " << maI->first 
           << "   " << m_hitRnk.at(maI->second)  <<endreq;
  }
  //set hits to zero for next iteration
  for(unsigned a=0;a<m_hitRnk.size();a++){
    m_hitRnk.at(a) = 0.;
  }
  
  
  return StatusCode::SUCCESS;
}

bool GAlign::writeParameters()
{
  std::vector<bool> dof = m_taConfig->NumDOF();
  int npar = m_taConfig->nRanks();
  //txt file for study of pulls
  //  if(m_taConfig->NumAlignPars()==m_taConfig->nRanks()){
    std::ofstream outParams("ParameterForPulls.txt",std::ios::app);
    if (outParams.fail() ){
      return Warning("Failed to open matrix output file",StatusCode::FAILURE);
    }
    int cnt =0;
    for(int r=0;r<npar;r++){
      if(dof[0]){ outParams <<"x " << misalInput_X[r] <<" "<< m_new_par[r] <<" "<<misalInput_X[r]-m_new_par[r] 
                            <<" "<<m_align_err[r]<<std::endl; //fill file row wise
      cnt++;
      }
      if(dof[2]){ outParams <<"z "<<  misalInput_Z[r] <<" "<< m_new_par[r+cnt*npar] <<" "<<misalInput_Z[r]-m_new_par[r+cnt*npar] 
                            <<" "<<m_align_err[r+cnt*npar]<<std::endl; //fill file row wise
      cnt++;
      }
      if(dof[3]){ outParams <<"a "<<  misalInput_A[r] <<" "<< m_new_par[r+cnt*npar] <<" "<<misalInput_A[r]-m_new_par[r+cnt*npar] 
                            <<" "<<m_align_err[r+cnt*npar]<<std::endl; //fill file row wise
      cnt++;
      }
      if(dof[4]){ outParams <<"b "<<  misalInput_B[r] <<" "<< m_new_par[r+cnt*npar] <<" "<<misalInput_B[r]-m_new_par[r+cnt*npar] 
                            <<" "<<m_align_err[r+cnt*npar]<<std::endl; //fill file row wise
      cnt++;
      }
      if(dof[5]){ outParams <<"c "<<  misalInput_C[r] <<" "<< m_new_par[r+cnt*npar] <<" "<<misalInput_C[r]-m_new_par[r+cnt*npar] 
                            <<" "<<m_align_err[r+cnt*npar]<<std::endl; //fill file row wise
      cnt++;
      
      }
      cnt =0;
    }
    
    //}
  return true;
}


StatusCode GAlign::finalize(){

  info() << "==> Finalize" << endreq;
  StatusCode sc = GaudiTupleAlg::finalize();
  info() << " --> finalize ? " << sc << endreq;
  return sc;
}



/*********************************************
 *                                           *
 *   VectortoArray                           *
 * convering vectors to arrays              *
 *                                           *
 *********************************************/
void GAlign::VectortoArray(const std::vector<double> &the_vector, double the_array[] ){
  std::vector<double>::const_iterator the_iterator = the_vector.begin();
  unsigned int counter = 0;
  while( the_iterator < the_vector.end() ){
    the_array[counter] = *the_iterator;
    //    the_array[counter] = the_vector[counter];
    the_iterator++;
    counter++;
  }
}
void GAlign::MatrixToVector(double* mat,int dim1, int dim2 ,std::vector< std::vector<double> > &vec)
{

  for(int i=0;i<dim1;i++){
    vec.push_back(std::vector<double> ());
    for(int j=0;j<dim2;j++){
      // info() << "matrix = " << i<<" "<<j<<" ="<<mat[i*dim2+j]<<endreq;
      double val = mat[i*dim2+j];
      vec[i].push_back(val);
      //info() << "vv " << i<<" "<<j << " " <<vec[i][j] << endreq;
    }
  }
}
/*******************************
 ** calculate the reference   **
 ** System for LM constraints **
 ** if input misal is known   **
 *******************************/
void GAlign::LagMultRef()
{
  double meanOffX_c   =0.;
  double shearOffX_c  =0.;
  double meanOffY_c   =0.;
  double shearOffY_c  =0.;
  double meanOffX_a   =0.;
  double shearOffX_a  =0.;
  double meanOffY_a   =0.;
  double shearOffY_a  =0.;
  //   double meanOffU   =0.;
  //   double meanOffV   =0.;
  //   double shearOffU  =0.;
  //   double shearOffV  =0.;
  double meanOffZ   =0.;
  double scaleOffZ  =0.;

  std::vector<double> stereo  = m_taConfig->GetStereoAngle();
  std::vector<double> detel_z = m_taConfig->GetDetEl_Z();
  double mean_z               = m_taConfig->GetDetEl_meanZ();
  double sigZmean             = 0.;
  
  info() << " mean_z = "<<mean_z<<endreq;
  for(int i=0;i<m_nGlPars;++i){
    info() << " i = " << i << " " << detel_z.at(i)<< " stereo "<< stereo.at(i) << endreq;
    sigZmean += (detel_z.at(i)-mean_z)*(detel_z.at(i)-mean_z);
  }
  sigZmean /= m_nGlPars;
  info() << " sigZmean = " << sigZmean << endreq;
  
  for(int i=0;i<m_nGlPars;++i){
    //Cside
    if(i%2==0){
      meanOffX_c  += misalInput_X.at(i)*cos(stereo.at(i));
      shearOffX_c += misalInput_X.at(i)*( (detel_z.at(i)-mean_z)/sigZmean)*cos(stereo.at(i));///
      meanOffY_c  += misalInput_X.at(i)*sin(stereo.at(i));
      shearOffY_c += misalInput_X.at(i)*( (detel_z.at(i)-mean_z)/sigZmean  )*sin(stereo.at(i));
    }
    //Aside
    if(i%2!=0){
      meanOffX_a  += misalInput_X.at(i)*cos(stereo.at(i));
      shearOffX_a += misalInput_X.at(i)*( (detel_z.at(i)-mean_z)/sigZmean)*cos(stereo.at(i));///
      meanOffY_a  += misalInput_X.at(i)*sin(stereo.at(i));
      shearOffY_a += misalInput_X.at(i)*( (detel_z.at(i)-mean_z)/sigZmean  )*sin(stereo.at(i));
    }
    
    meanOffZ += misalInput_Z.at(i);
    scaleOffZ += misalInput_Z.at(i)*(detel_z.at(i)-mean_z)/sigZmean;
    
  }
  
  meanOffX_c  /= m_nGlPars/2;
  shearOffX_c /= m_nGlPars/2;
  meanOffY_c  /= m_nGlPars/2;
  shearOffY_c /= m_nGlPars/2;

  meanOffX_a  /= m_nGlPars/2;
  shearOffX_a /= m_nGlPars/2;
  meanOffY_a  /= m_nGlPars/2;
  shearOffY_a /= m_nGlPars/2;
  //shearOffY /= sigZmean;
  
  //meanOffU  /= m_nGlPars;
  //shearOffU /= m_nGlPars;
  //   meanOffV  /= m_nGlPars;
  //   shearOffV /= m_nGlPars;
  meanOffZ  /= m_nGlPars;
  scaleOffZ  /= m_nGlPars;
  
  info()<<"CSIDE"<<endreq;
  info()<< " meanoffx = " << meanOffX_c << " shearOffX = " << shearOffX_c 
        << " meanoffY = " << meanOffY_c << " shearOffY = " << shearOffY_c <<endreq;
  info()<<"ASIDE"<<endreq;
  info()<< " meanoffx = " << meanOffX_a << " shearOffX = " << shearOffX_a 
        << " meanoffY = " << meanOffY_a << " shearOffY = " << shearOffY_a <<endreq;
  info() << "calculate LM offset for : "<<endreq;
  //new input parameter
  for(int i=0;i<m_nGlPars;++i){

    if(m_nGlPars >12){
      //Cside
      if(i%2==0){
        info() << "Cside old misal X "<<misalInput_X.at(i) << " z = "<< detel_z.at(i) <<" meanZ = "<<mean_z;
        misalInput_X.at(i) = misalInput_X.at(i)- meanOffX_c*cos(stereo.at(i)) - shearOffX_c*(detel_z.at(i)-mean_z)*cos(stereo.at(i));
        //b      -meanOffY*sin(stereo.at(i)) - shearOffY*(detel_z.at(i)-mean_z)*sin(stereo.at(i)); 
        info() << " --> LM offset X " << misalInput_X.at(i) <<endreq;
      }
      //Cside
      if(i%2!=0){
        info() << "Aside old misal X "<<misalInput_X.at(i) << " z = "<< detel_z.at(i) <<" meanZ = "<<mean_z;
        misalInput_X.at(i) = misalInput_X.at(i)- meanOffX_a*cos(stereo.at(i)) - shearOffX_a*(detel_z.at(i)-mean_z)*cos(stereo.at(i));
        //b      -meanOffY*sin(stereo.at(i)) - shearOffY*(detel_z.at(i)-mean_z)*sin(stereo.at(i)); 
        info() << " --> LM offset X " << misalInput_X.at(i) <<endreq;
      }
    }
    else{
      misalInput_X.at(i) = misalInput_X.at(i)- meanOffX_a*cos(stereo.at(i)) - shearOffX_a*(detel_z.at(i)-mean_z)*cos(stereo.at(i));
    }
  }
  
}



