//-----------------------------------------------------------------------------
/** @file GAlign.cpp
 *
 *  Implementation file for RICH reconstruction tool : GAlign
 *
 *  CVS Log :-
 *  $Id: GAlign.cpp,v 1.30 2010-05-20 16:16:56 mdeissen Exp $
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
#include "Event/Node.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"

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
#include <vector>
// gsl stuff
#include "gsl/gsl_cdf.h"
//MC info
#include "Linker/LinkedTo.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"


using namespace LHCb;
using namespace Gaudi;



DECLARE_ALGORITHM_FACTORY( GAlign )

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
    bool acceptTr = false;
    const LHCb::Track *atrack = NULL;
    for ( iterT = inCont->begin(); iterT != inCont->end(); iterT++) {
      atrack = *iterT;
      m_totalTracks++;
      debug() << " ---> next track " << endreq;
      if ( atrack->nMeasurements() == 0 ) {
        // load the measurements on the track
        // calculate from the track what the measurement was
        StatusCode mPro = m_measProvider->load( const_cast<LHCb::Track &>(*atrack) );
        if ( ! mPro ) {
          info() << "Error re-creating measurements!" << endreq;
          return StatusCode::FAILURE;
        }
      }
      acceptTr =  m_trackselection->accept( *atrack );
      if( !acceptTr){
        m_RejectbySelector++;
        debug() << "---> track NOT accepted " << endreq;
        continue;
      }
      m_taConfig->MakeTrackParVec(); 
      debug() << "---> track IS  accepted " << endreq;
      plot(atrack->p(),"accepted track momentum","rejected track momentum",0.,10000.,1000);
      StatusCode sc =  m_taConfig->ResetGlVars();        // Reset MilleTool variables etc.
      //count multiple clusters per rank
      std::vector<int> cluster;
      for(int cl=0; cl < m_taConfig->NumAlignPars(); cl++){
        cluster[cl] = 0;
      }
      double meanZ= 0.;
      double sumZ =0.;
      int    nofZ =0;
      debug() << "Number of hits on track: " << atrack->lhcbIDs().end() - atrack->lhcbIDs().begin() << endreq;

      std::vector<LHCb::LHCbID>::const_iterator itID;
      for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
        LHCb::LHCbID id = *itID;
        if ( atrack->isOnTrack( id ) ){//&&  atrack->isMeasurementOnTrack( id )) {
          if ( ot_detector && id.isOT() ) {
            if ( sc.isFailure() ) {
              debug() << "Not processing any data from tracks!" << endreq;
            }
            int rank            = 0; // position in c-matrix for a detector element        
            double Z_position   = 0.;
            double stereo_angle = 0.0;
            double refZ         = 0.;
            struct Point meas;
            struct Point localPos;
            bool flagL=false;//leave it false here!
            Gaudi::XYZPoint modCenter;
            
            sc =  m_taConfig->Rank( id, rank );
            if ( sc.isFailure() ){
              debug() << "---> Couldn't calculate the rank of the object "<<endreq;
              continue;
            }
            debug()<<" GET MEANZ GET MEANZ GET MEANZ GET MEANZ "<<endreq;
            sc=m_taConfig->CalcResidual(id, rank,  meas,
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
        }//MD lhcbid
      }//MD lhcbid
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
      std::vector<double> locTr(Tparam,0);
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
          //****************************************************************
          // check if this LHCbID is really on the track
          LHCb::LHCbID id = *itID;
          //if ( !atrack->isOnTrack( id ) &&  !atrack->isMeasurementOnTrack( id )) continue;
          if ( !atrack->isOnTrack( id ) ) continue;
          if ( !ot_detector || !id.isOT() ) continue;
          //****************************************************************
          //varibales for CalcRes
          int rank     = 0;
          double weight = (1/1.44)*(1/1.44);
          double relaZ = 0.;
          double guess = 0.;
          double Z_position =0.;
          double refZ       =0.;
          double stereo_angle = 0.0;
          struct Point meas; meas.x=0; meas.y=0;
          struct Point localPos; localPos.x=0; localPos.y = 0;
          bool flagL =true;
          Gaudi::XYZPoint modCenter;

          sc =  m_taConfig->Rank( id, rank );
          if ( sc.isFailure() )
            debug() << "Not processing any data from tracks!" << endreq;

          m_taConfig->SetMeanZ(meanZ);

          debug() <<"3: -----------------------------------------------------------"<<endreq;            
          debug() <<"TRACKFIT LOOP TRACKFIT LOOP TRACKFIT LOOP TRACKFIT LOOP "<<endreq;
          debug() <<"RANK = "<<rank << " Tr="<<m_tr_cnt << "   at track iter "<< kk << " ====>  meas.x = " << meas.x<<endreq;
          for(unsigned int i=0;i<trParV.size();i++) debug() << "Param " <<i<<" = "<< trParV[i] <<endreq;
           
           
                       
          sc=m_taConfig->CalcResidual(id, rank,  meas,
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
          double mslope = tx + tQ*( 2*relaZ + 3*dRatio*relaZ*relaZ);
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


    
//           char meapl[100];
//           sprintf(meapl,"RES_gloIt_%d_fit_%d",m_iterations,kk);
//           plot(meas.x,meapl,meapl,-10.,10.,80); 
//           sprintf(meapl,"glo%d_res%d_/Layer%d",m_iterations,kk,rank);
//           plot(meas.x,meapl,meapl,-10.,10.,80); 
      
    	    
          DeOTDetector* m_ot       = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
          DeOTModule* module       = m_ot->findModule( id.otID() );
          const OTChannelID  chID  = id.otID();
          const unsigned int straw = chID.straw();
           
          m_hitRnk[rank] += 1;
                       
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
      //t_resids->farray("monoA",tmonoA,"n_tmonoA",n_tmonoA);
      //      t_resids->farray("uniquestrawID",tstrawunique,"n_uniquestrawID",n_tstrawunique);
      //t_resids->farray("strawID",tstraw,"n_strawID",n_tstraw);
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
        LHCb::LHCbID id = *itID;
        //          if ( atrack->isOnTrack( id ) &&  atrack->isMeasurementOnTrack( id )) {
        if ( atrack->isOnTrack( id )) {
          double weight       = (1/1.44)*(1/1.44);
          int rank            = 0;
          double Z_position   = 0.;
          double stereo_angle = 0.;
          double refZ         = 0.;
          struct Point meas; meas.x = 0; meas.y = 0;
          struct Point localPos; localPos.x=0; localPos.y = 0;
          Gaudi::XYZPoint modCenter;
          sc =  m_taConfig->Rank( id, rank );
          if ( sc.isFailure() )
            debug() << "---> Couldn't calculate the rank of the object "<<endreq;
          if ( sc.isSuccess() ) {
            //
            bool flagL=true;//true if local fit already done
            m_taConfig->SetMeanZ(meanZ);
            
            debug() <<"4: -----------------------------------------------------------"<<endreq;            
            debug() <<"FILLMATRIX LOOP FILLMATRIX LOOP FILLMATRIX LOOP FILLMATRIX LOOP"<<endreq;
            debug()  <<"RANK = "<<rank << " Tr="<<m_tr_cnt << endreq
                     <<"( Tr="<<m_tr_cnt   <<" :  x0= " << trParV[0] << " tx=" << trParV[1] 
                     <<" y0=" << trParV[2] << " ty= "   << trParV[3] <<  endreq;
            
            //  m_taConfig->SetMeanZ(meanZ);
            
            sc=m_taConfig->CalcResidual(id, rank,  meas, 
                                        flagL,Z_position, stereo_angle, refZ,localPos);
            if ( sc.isFailure() )
              continue;
            //plot(meas.x,"3rdloopmeas","measCalcR",-3000,3000,500);
            //              double relaZ = Z_position-meanZ;
            double relaZ = refZ;
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
          } 
        }
      }//lhcb ids 
      
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
                                                    chi2, residual, m_locrank);
      if ( locfit != StatusCode::SUCCESS ) {
        info() << "Track rejected , not used for filling the global matrix !" << endreq;
      }
      debug() << "--> locfit : "<< locfit <<" --> trrack par: x0=" << trParV[0] << " tx=" << trParV[1] 
              << " y0=" << trParV[2] << " ty=" << trParV[3] << " tq = " << trParV[4] << endreq;


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
  
  // add new parameter to misalignment constants from previous iteration
  // and save new geometry information in memory
  m_taConfig->PrintParameters(m_align,m_iterations);
  
  //  int cnt = 0;
  std::vector<bool> dofs                  = m_taConfig->NumDOF();
  std::map<std::string, int> cap          = m_taConfig->GetCMap();
  std::map<std::string,int>::iterator maI = cap.begin();

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


