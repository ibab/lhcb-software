// $Id: PrimaryVertexFinderAlg.cpp,v 1.2 2002-07-18 17:58:23 gcorti Exp $
// Include files
///--------------------------------------------------------- 
/// std c++ libs
#include <iostream>
///---------------------------------------------------------
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
///---------------------------------------------------------
///< local
#include "PrimaryVertexFitterTool.h"
#include "PrimaryVertexFinderAlg.h"
///---------------------------------------------------------
///< from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ParticleProperty.h"
///---------------------------------------------------------
///< std services
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
///---------------------------------------------------------
// from STLnewmycmt
#include <vector>
///---------------------------------------------------------
/// from CLHEP
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
///---------------------------------------------------------
// from LHCbKernel
#include "Relations/IAssociator.h"
///---------------------------------------------------------
///< from Event
#include "Event/EventHeader.h"
#include "Event/PrimVertex.h"
#include "Event/TrStoredTrack.h"
#include "Event/TrStoredMeasurement.h"
#include "Event/TrState.h"
#include "Event/TrStateL.h"
#include "Event/TrStateP.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrimaryVertexFinderAlg
//
// 03/04/2002 : Krzysztof Ciba
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrimaryVertexFinderAlg>          s_factory ;
const        IAlgFactory& PrimaryVertexFinderAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrimaryVertexFinderAlg::PrimaryVertexFinderAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ),
    m_inputContainer    (TrStoredTrackLocation::Default),
    m_outputContainer   (VertexLocation::Primary),
    m_chi2min           (3.0),
    m_zcut              (250.0),
    m_minNumberOfTracks (2),
    m_maxIteration      (200)  
{ 

  declareProperty("InputData",     m_inputContainer );
  declareProperty("OutputData",    m_outputContainer );
  declareProperty("MinimumChi2",   m_chi2min );
  declareProperty("MinTracks",     m_minNumberOfTracks );
  declareProperty("MaxIteration",  m_maxIteration );
  declareProperty("Zcut",          m_zcut );
}

//=============================================================================
// Destructor
//=============================================================================
PrimaryVertexFinderAlg::~PrimaryVertexFinderAlg() {

}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrimaryVertexFinderAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  ///< take PrimaryVertexFitterTool
  sc = toolSvc()->retrieveTool("PrimaryVertexFitterTool", m_pvft, this);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "====> Unable to create PrimaryVertexFitterTool tool" 
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "<== PrimaryVertexFitterTool retrived" << endreq;    
  }

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrimaryVertexFinderAlg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  /////////////////////////////////////////////////////////////////////
  // retiving event
  /////////////////////////////////////////////////////////////////////
  SmartDataPtr< EventHeader > evHdr( eventSvc(), 
				     EventHeaderLocation::Default);

  if (! evHdr) {
    log << MSG::ERROR << "!!! Unable to retrive event !!!" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "*** Event = " << evHdr->evtNum()
        << " Run = " << evHdr->runNum() << endreq;
  }
  /////////////////////////////////////////////////////////////////////
  // clear all vectors
  if (0 != m_tr.size()) {
    m_tr.clear();    
  }
  
  if (0 != m_vtx.size()) {
    m_vtx.clear();
  }
  /////////////////////////////////////////////////////////////////////
  //int i,j,k; ///<  counters
  int i,j; ///<  counters
  /////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////// 
  ///< check the existence of output directory   
  SmartDataPtr<DataObject> outDir( eventSvc(), 
				   m_outputContainer );
  Vertices* createdVertices = new Vertices();
  
  if( 0 == outDir ) { // touch the output directory
    log << MSG::DEBUG << "OutputDirectory = "
        << m_outputContainer << " does not exist " << endreq;
    log << MSG::DEBUG << "Trying to register it..." << endreq;
    
    // register output container
    sc = eventSvc()->registerObject(m_outputContainer, createdVertices);
    
    if( sc.isFailure() ) {
      if( 0 !=  createdVertices ) { delete createdVertices ; }
      log << MSG::ERROR << "Unable to register the output container = "
          << m_outputContainer << endreq;
      log << MSG::ERROR << "Status is " << sc << endreq;
      // return sc;
    }  
  } 
  
  //////////////////////////////////////////////////////////////////////
  ///< prepare table nzclose & nzweight
  long nbins = 500;
  long nzclose[nbins];
  double nweight[nbins];
  double zmin = -m_zcut; // mm
  double zmax = m_zcut; // mm  

  for (j = 0; j < nbins; j++) {
    nzclose[j] = 0;
    nweight[j] = 0.0;
  }
  
  log << MSG::INFO 
      << "Taking TrSoredTracks from " << m_inputContainer << endreq;

  /// ptrs to KeyedContainers...
  /// ...TrStoredTracks
  SmartDataPtr< TrStoredTracks > stTrack (eventSvc(), m_inputContainer);
  if (0 == stTrack) {
    log << MSG::ERROR << "Can't find TrStoredTrack !!!" << endreq;
    return StatusCode::FAILURE;
  }
  
  KeyedContainer< TrStoredTrack >::iterator iST;
  m_NumberOfTracks = stTrack->size();
  log << MSG::INFO << "Found " << 
    m_NumberOfTracks << " in TES" << endreq;

  i=0;
  ///< read parameters
  for (iST = stTrack->begin(); 
       iST != stTrack->end();
       iST++, i++) {
    
    TrStoredTrack* str = (*iST);
    
    // Pick which track to use from all available ones
    if( !str->unique() || !str->forward() ) continue;

    MyTrack* wtra = new MyTrack;
    
    log << MSG::DEBUG 
        << "==> Taking parameters of track #" << i 
        << " with #" << str->measurements().size() << " measurements and # "
        << str->states().size() << " states. " << endreq;
    
    SmartRef<TrStoredTrack> SRtrack = str;
    wtra->track = SRtrack;
    
    //const 
    const TrState* myState = str->closestState(0.0);
    TrState* sta = myState->clone();
    TrStateP* myStateP = dynamic_cast<TrStateP*>(sta);
    if (0 == myStateP) {
      log << MSG::DEBUG 
          << "There are no TrStateP  at z = 0.0 !!!" << endreq;
      return StatusCode::FAILURE;
    }

    ///< ok, now we have it, so take parameters
    Hep3Vector origin; ///< local origin of this track
    Hep3Vector slope;  ///< local slope of this track
    
    origin.setX( myStateP->x() );
    origin.setY( myStateP->y() );
    origin.setZ( 0.0 );
    
    slope.setX( myStateP->tx() );
    slope.setY( myStateP->ty() );
    slope.setZ( 1. );
    
    // Propagate origin  to the reference point
    origin     += slope * (-origin.z());
    
    ///< position and slope
    wtra->X = origin.getX();
    wtra->Y = origin.getY();
    wtra->Z = origin.getZ();

    wtra->Slx = slope.getX();
    wtra->Sly = slope.getY();
    
    ///< covariant matrix
    HepSymMatrix cov(5,1);
    cov = myStateP->stateCov();
    wtra->Cov = cov;
    wtra->Pt = myStateP->p();

    ///< IP and ErrIP
    wtra->d0 = sqrt((wtra->X*wtra->X) + (wtra->Y*wtra->Y));
    wtra->errd0 = 2 * sqrt( ( wtra->X * wtra->X * wtra->Cov[0][0] )
                            +( wtra->Y * wtra->Y * wtra->Cov[1][1]) );
    
    log << MSG::DEBUG << "tr-" << i 
        << " " << wtra->X 
        << " " << wtra->Y 
        << " " << wtra->Z
        << " " << wtra->Slx
        << " " << wtra->Sly
        << " " << wtra->d0
        << " " << wtra->errd0
        << " " << wtra->Pt
        << endreq;    

    ///< fill zclose
    double zxint = - wtra->X/wtra->Slx;
    double zyint = - wtra->Y/wtra->Sly;
    
    double xsq = pow(wtra->X,2);
    double ysq = pow(wtra->Y,2);
    double zsq = pow(wtra->Z,2);
    double norm = sqrt(xsq + ysq + zsq);
    
    HepVector3D UnitVector(wtra->X / norm,
                           wtra->Y / norm, 
                           wtra->Z / norm);
    
    double uvx = UnitVector.x();
    double uvy = UnitVector.y();
    
    double wwx = uvx*uvx/(1.0 + uvx*uvx);
    double wwy = uvy*uvy/(1.0 + uvy*uvy);
    
    double zclose = (wwx*zxint + wwy*zyint)/(wwx + wwy);
    
    
    if (zclose > zmax) continue;
    if (zclose < zmin) continue;
    
    int nb = int((zclose - zmin)/(zmax - zmin)*nbins);
    
    wtra->nBin = nb;
    
    double weight = wwx+wwy;
    nzclose[nb] += 1;
    nweight[nb] += weight;
    ///< done!

    m_tr.push_back(*wtra);
    delete wtra;
  }
  ///< end of reading parameters  
 ///< shuold I find more then 3 pvtx in event ???
  //  int Max[6] = {-1, -1, -1, -1, -1, -1};  ///< maximum in nzclose
  //int Bin[6] = {-1, -1, -1, -1, -1, -1};  ///< index of maximum in nzclose

  ///< find maximum in nzclose -> preliminary position of PV
  int Max1, Max2, Max3; ///< maximum in nzclose
  int Bin1, Bin2, Bin3; ///< index of 1,2 & 3 maximum
  int NumbOfPV, usedBins;
  Max1 = Max2 = Max3 = -1; 
  Bin1 = Bin2 = Bin3 = -1; 
  usedBins = 4; ///< take +/- 4 bins with tracks to fit PV
  
  for ( i = 0; i < nbins; i++) { ///< 1st pass -> 1st PV
    if ((nzclose[i] > Max1) && (nzclose[i] != 0)) {
      Max1 = nzclose[i];
      Bin1 = i;
    }
  }
  
  for (i = 0; i < nbins; i++) { ///< 2nd pass -> 2nd PV 
    if ((i > (Bin1 - usedBins) ) && (i < (Bin1 + usedBins))) continue;
    if ((nzclose[i] > Max2) && (nzclose[i] != 0)) {
      Max2 = nzclose[i];
      Bin2 = i;
    }
  }
  
  for ( i = 0; i < nbins; i++) { ///< 3rd pass -> 3rd PV
    if   ((i > (Bin1 - usedBins)) && (i < (Bin1 + usedBins))) continue;
    if   ((i > (Bin2 - usedBins)) && (i < (Bin2 + usedBins))) continue;
    if ((nzclose[i] > Max3) && (nzclose[i] != 0)) {
      Max3 = nzclose[i];
      Bin3 = i;
    }
  }
  
  
  
  log << MSG::DEBUG << "MAX1 BIN1 MAX2 BIN2 MAX3 BIN3" << endreq;
  log << MSG::DEBUG 
      << Max1 << " " << Bin1 << " " 
      << Max2 << " " << Bin2 << " " 
      << Max3 << " " << Bin3 << endreq;
  

  if ((Max1 > 0) && (Max2 > 0) && (Max3 > 0)) {
    log << MSG::INFO << "Found 3 max in zclose" << endreq;
    log << MSG::INFO << "1st prePV " << nzclose[Bin1]
        << " in bin nr " << Bin1 << endreq;
    log << MSG::INFO << "2nd prePV " << nzclose[Bin2]
        << " in bin nr " << Bin2 << endreq;
    log << MSG::INFO << "3rd prePV " << nzclose[Bin3]
        << " in bin nr " << Bin3 << endreq;
    NumbOfPV = 3;
  } else if ((Max1 > 0) && (Max2 > 0) && (Max3 < 0)) {
    log << MSG::INFO << "Found 2 max in zclose" << endreq;
    log << MSG::INFO << "1st prePV " << nzclose[Bin1]
        << " in bin nr " << Bin1 << endreq;
    log << MSG::INFO << "2nd prePV " << nzclose[Bin2]
        << " in bin nr " << Bin2 << endreq;
    NumbOfPV = 2;
  } else if ((Max1 > 0) && (Max2 < 0) && (Max3 < 0)) {
    log << MSG::INFO << "Found 1 max in zclose" << endreq;
    log << MSG::INFO << "1st prePV " << nzclose[Bin1]
        << " in bin nr " << Bin1 << endreq;
    NumbOfPV = 1;
  } else if ((Max1 < 0) && (Max2 < 0) && (Max3 < 0)) {
    log << MSG::INFO << "Found 0 max in zclose" << endreq;
    NumbOfPV = 0;
    return StatusCode::SUCCESS;
  }

  /// loop over all proto-PV
  int kPV;
  for (kPV = 0; kPV < NumbOfPV; kPV++) {
   
    log << MSG::INFO 
        << "==> Searching for " << kPV+1 << " PV position <==" << endreq;
    
    std::vector<MyTrack>::iterator itr;    

    for (itr = m_tr.begin(); itr != m_tr.end(); itr++) {
      (*itr).isUsed = false;
    }
    
    m_NumberOfTracksUsed = 0;

    if (kPV == 0 ) {
      for (itr = m_tr.begin(); itr != m_tr.end(); itr++) {
        if ( ( (*itr).nBin >= (Bin1 - usedBins) ) &&
             ( (*itr).nBin <= (Bin1 + usedBins) ) ) {
          (*itr).isUsed = true;
          m_NumberOfTracksUsed++;
        }
      }
    } else if (kPV == 1) {
      for (itr = m_tr.begin(); itr != m_tr.end(); itr++) {
        if ( ( (*itr).nBin >= (Bin2 - usedBins) ) && 
             ( (*itr).nBin <= (Bin2 + usedBins) ) ) {
          (*itr).isUsed = true;
          m_NumberOfTracksUsed++;
        }
      }
    } else if (kPV == 2) {
      for (itr = m_tr.begin(); itr != m_tr.end(); itr++) {
        if ( ( (*itr).nBin >= (Bin3 - usedBins) ) &&
             ( (*itr).nBin <= (Bin3 + usedBins) ) ) {
          (*itr).isUsed = true;
          m_NumberOfTracksUsed++;
        }
      }
    }

    log << MSG::INFO 
        << "* * * Nb of tracks before fit #" 
        << m_NumberOfTracksUsed << " * * *" <<endreq;
    
    if (m_NumberOfTracksUsed < m_minNumberOfTracks) {
      log << MSG::INFO 
          << "==> There should be more then 2 tracks to find PV position!!!" 
          << endreq;
      log << MSG::DEBUG << "===> Failed to find PV" << endreq;
      continue;
    }
    
    MyVertex* vtx = m_pvft->fitter(m_tr,
				   m_maxIteration,
				   m_chi2min, 
				   "Gauss",
				   m_minNumberOfTracks);

    if (vtx == NULL) {
      log << MSG::DEBUG << "===> Failed to find PrimVtx position..." << endreq;
    } else {
      //  nbvtx++;

      vtx->event = evHdr->evtNum();
      m_vtx.push_back(*vtx);

      log << MSG::INFO << "==> Creating PrimaryVertex #" << kPV+1 << endreq;      
      PrimVertex* vertex = new PrimVertex();
      
      HepVector3D pos(vtx->X, vtx->Y, vtx->Z);
      vertex->setPosition(pos);
      
      vertex->setChi2(vtx->chi2);
      
      HepSymMatrix error(3,0);
      
      error[0][0] = vtx->Cov[0][0];
      error[1][0] = error[0][1] =  vtx->Cov[1][0];
      error[1][1] = vtx->Cov[1][1];
      error[2][0] = error[0][2] = vtx->Cov[2][0];
      error[2][1] = error[1][2] = vtx->Cov[2][1];
      error[2][2] = vtx->Cov[2][2];
      
      vertex->setPositionErr(error);

      vertex->setType(Vertex::Primary);
      vertex->setNDoF(vtx->NbDeg);

      std::vector< SmartRef < TrStoredTrack > >::iterator itra;
      for (itra = vtx->tracks.begin();
           itra != vtx->tracks.end();
           itra++) {  
        TrStoredTrack* tra = *itra;
        vertex->addToTracks(tra);
      }
      
      createdVertices->insert(vertex);
      
      log << MSG::DEBUG 
          << "Primary vertex " << kPV+1 << " added to TES" << endreq;
      sc = StatusCode::SUCCESS;    
    }
  }
   
  return sc;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrimaryVertexFinderAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}
//=============================================================================
