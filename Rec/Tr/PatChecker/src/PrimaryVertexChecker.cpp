// Include files
// from Gaudi
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "VeloDet/DeVelo.h"
#include <Linker/LinkedTo.h>
#include <Event/MCTrackInfo.h>
#include "Event/L0DUReport.h"
#include "GaudiUtils/HistoStats.h"
#include "AIDA/IHistogram1D.h"

// local
#include "PrimaryVertexChecker.h"

bool sortmlt(MCPVInfo first, MCPVInfo second) {
     return first.nRecTracks > second.nRecTracks;
}

//-----------------------------------------------------------------------------
// Implementation file for class : PrimaryVertexChecker
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(PrimaryVertexChecker)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrimaryVertexChecker::PrimaryVertexChecker(const std::string& name,
                 ISvcLocator* pSvcLocator)
  : GaudiTupleAlg (name,pSvcLocator)
  , m_forcedBtool(NULL)
{
    declareProperty("nTracksToBeRecble",  m_nTracksToBeRecble = 5);
    declareProperty("produceNtuple",      m_produceNtuple     = false);
    declareProperty("RequireVelo",        m_requireVelo       = true);
    declareProperty("produceHistogram",   m_produceHistogram  = true);
    declareProperty("dzIsolated",         m_dzIsolated        = 10.0 * Gaudi::Units::mm);
    declareProperty("matchByTracks",      m_matchByTracks     = true);
    declareProperty("inputTracksName",    m_inputTracksName   = LHCb::TrackLocation::Default);
    declareProperty("inputVerticesName",  m_inputVerticesName = LHCb::RecVertexLocation::Primary);    

}
//=============================================================================
// Destructor
//=============================================================================
PrimaryVertexChecker::~PrimaryVertexChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrimaryVertexChecker::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // Must be executed first
  if(sc.isFailure()) debug() << "==> Initialize" << endmsg;

  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endmsg;
    return StatusCode::FAILURE;
  }

  m_nevt                 = 0;
  m_nMCPV                = 0; 
  m_nRecMCPV             = 0; 
  m_nMCPV_isol           = 0; 
  m_nRecMCPV_isol        = 0; 
  m_nMCPV_close          = 0; 
  m_nRecMCPV_close       = 0;
  m_nFalsePV             = 0;
  m_nFalsePV_real        = 0;
  m_nMCPV_1mult          = 0;
  m_nRecMCPV_1mult       = 0;
  m_nMCPV_isol_1mult     = 0; 
  m_nRecMCPV_isol_1mult  = 0;
  m_nMCPV_close_1mult    = 0;
  m_nRecMCPV_close_1mult = 0;
  m_nMCPV_2mult          = 0;
  m_nRecMCPV_2mult       = 0;
  m_nMCPV_isol_2mult     = 0; 
  m_nRecMCPV_isol_2mult  = 0;
  m_nMCPV_close_2mult    = 0;
  m_nRecMCPV_close_2mult = 0;
  m_nMCPV_3mult          = 0;
  m_nRecMCPV_3mult       = 0;
  m_nMCPV_isol_3mult     = 0; 
  m_nRecMCPV_isol_3mult  = 0;
  m_nMCPV_close_3mult    = 0;
  m_nRecMCPV_close_3mult = 0;
  // for L0 accepted PV of B signal
  m_nL0PvOfB    = 0;
  m_nRecL0PvOfB = 0;
  m_nBFalse = 0;
  m_nRecBFalse = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrimaryVertexChecker::execute() {
  debug() << "==> Execute" << endmsg;

  // Event
  m_nevt++;

  // Get input
  std::vector<LHCb::Track*> vecOfTracks;
  std::vector<LHCb::RecVertex*> vecOfVertices;
  std::string trackLoc;  
  bool tracks_ok   = getInputTracks(vecOfTracks, trackLoc);
  bool vertices_ok = getInputVertices(vecOfVertices);

  if (!tracks_ok && m_matchByTracks) {
    return StatusCode::SUCCESS; // return SUCCESSS anyway
  }

  if (!vertices_ok) {
    return StatusCode::SUCCESS; // return SUCCESSS anyway
  }

  if(debugLevel())
    debug() << trackLoc << " # tracks: " << vecOfTracks.size() << "  # vertices: " << vecOfVertices.size() << endmsg;

  // Fill reconstucted PV info
  std::vector<RecPVInfo> recpvvec;
  std::vector<LHCb::RecVertex*>::iterator itRecV;
  for(itRecV = vecOfVertices.begin(); vecOfVertices.end() != itRecV;
               itRecV++) {
    LHCb::RecVertex* pv;
    pv = *itRecV;
    RecPVInfo recinfo;
    recinfo.pRECPV= pv;
    recinfo.position = pv->position();
    Gaudi::SymMatrix3x3 covPV = pv->covMatrix();
    double sigx = sqrt(covPV(0,0));
    double sigy = sqrt(covPV(1,1));
    double sigz = sqrt(covPV(2,2));
    Gaudi::XYZPoint a3d(sigx,sigy,sigz);
    recinfo.positionSigma = a3d;
    recinfo.nTracks = pv->tracks().size();
    recinfo.nVeloTracks = count_velo_tracks(pv);
    recinfo.indexMCPVInfo = -1;
    recpvvec.push_back(recinfo);
  }

  // Fill MC PV info
  std::vector<MCPVInfo> mcpvvec;
  LHCb::MCVertices* mcvtx = 
                    get<LHCb::MCVertices>(LHCb::MCVertexLocation::Default);
  for(LHCb::MCVertices::const_iterator itMCV = mcvtx->begin();
      mcvtx->end() != itMCV; itMCV++) {
    const LHCb::MCParticle* motherPart = (*itMCV)->mother();
    if(0 == motherPart) {
      if((*itMCV)->type() == LHCb::MCVertex::ppCollision) {
        MCPVInfo mcprimvert;
        mcprimvert.pMCPV = *itMCV;
        mcprimvert.nRecTracks = 0;
        mcprimvert.nRecBackTracks = 0;
        mcprimvert.indexRecPVInfo = -1;
        mcprimvert.nCorrectTracks = 0;
        mcprimvert.multClosestMCPV = 0;
        mcprimvert.distToClosestMCPV = 999999.;        
        mcprimvert.m_mcPartInMCPV.clear();
        mcprimvert.m_recTracksInMCPV.clear();
        mcpvvec.push_back(mcprimvert);
      }
    }
  }

  if ( m_matchByTracks ) {

    count_reconstructed_tracks(mcpvvec, vecOfTracks, trackLoc);

  } else {

    count_reconstructible_mc_particles(mcpvvec);

  }        

  std::sort(mcpvvec.begin(),mcpvvec.end(),sortmlt);

  std::vector<MCPVInfo> rblemcpv;
  std::vector<MCPVInfo> not_rble_but_visible;
  std::vector<MCPVInfo>::iterator itmc;
  for (itmc = mcpvvec.begin(); mcpvvec.end() != itmc; itmc++) {
    if(itmc->nRecTracks >= m_nTracksToBeRecble) rblemcpv.push_back(*itmc);
    if(itmc->nRecTracks < m_nTracksToBeRecble && itmc->nRecTracks > 1) not_rble_but_visible.push_back(*itmc);
  }


  // match MC and REC PVs
  if ( m_matchByTracks ) {

    for(int ipv = 0; ipv < (int) recpvvec.size(); ipv++) {
      match_mc_vertex_by_tracks(ipv, recpvvec, rblemcpv);
    }

  } else {

    for(int ipv = 0; ipv < (int) recpvvec.size(); ipv++) {
      match_mc_vertex_by_distance(ipv, recpvvec, rblemcpv);
    }

  }        

  if(debugLevel()) {
    debug() << endmsg << " MC vertices " << endmsg;
    debug() << " ===================================" << endmsg;
    for (int imcpv = 0; imcpv < (int) rblemcpv.size(); imcpv++) {
      std::string ff = " ";
      LHCb::MCVertex* mcpv = rblemcpv[imcpv].pMCPV;
      if ( rblemcpv[imcpv].indexRecPVInfo < 0 ) ff="  NOTRECO";
      debug() <<  format(" %3d %3d  xyz ( %7.4f %7.4f %8.3f )   nrec = %4d", 
                         imcpv,  rblemcpv[imcpv].indexRecPVInfo, 
                         mcpv->position().x(), mcpv->position().y(), mcpv->position().z(),
                         rblemcpv[imcpv].nRecTracks) << ff << endmsg;
    }
    debug() << " -----------------------------------" << endmsg << endmsg;

    debug() << endmsg << " REC vertices " << endmsg;
    debug() << " ===================================" << endmsg;
    for(int ipv = 0; ipv < (int)recpvvec.size(); ipv++) {
      std::string ff = " ";
      if ( recpvvec[ipv].indexMCPVInfo < 0 ) ff="  FALSE  "; 
      debug() << format(" %3d %3d  xyz ( %7.4f %7.4f %8.3f )  ntra = %4d   sigxyz ( %7.4f %7.4f %8.4f )   chi2/NDF = %7.2f",
                        ipv,recpvvec[ipv].indexMCPVInfo, 
                        recpvvec[ipv].position.x(),recpvvec[ipv].position.y(),recpvvec[ipv].position.z(),
                        recpvvec[ipv].nTracks,
                        recpvvec[ipv].nVeloTracks,
                        recpvvec[ipv].positionSigma.x(),recpvvec[ipv].positionSigma.y(),recpvvec[ipv].positionSigma.z(),
                        recpvvec[ipv].pRECPV->chi2PerDoF()) << ff << endmsg;
      
    }
    debug() << " -----------------------------------" << endmsg;

  }

  // find nr of false PV

  int nFalsePV = 0;
  int nFalsePV_real = 0;
  for(int ipv = 0; ipv < (int) recpvvec.size(); ipv++) {
    if (recpvvec[ipv].indexMCPVInfo < 0 ) {
      nFalsePV++; 
      bool vis_found = false;
      for(unsigned int imc = 0; imc < not_rble_but_visible.size() ; imc++) {
        if ( not_rble_but_visible[imc].indexRecPVInfo > -1 ) continue;
        double dist = fabs(mcpvvec[imc].pMCPV->position().z() - recpvvec[ipv].position.z());
        if (  dist < 5.0 * recpvvec[ipv].positionSigma.z() ) {
          vis_found = true;
	  not_rble_but_visible[imc].indexRecPVInfo = 10;
          break;
	}
      } // imc
      if ( !vis_found ) nFalsePV_real++;
    }
  }
  // Fill distance to closest recble MC PV and its multiplicity
  std::vector<MCPVInfo>::iterator itmcl;
  for(itmcl = rblemcpv.begin(); rblemcpv.end() != itmcl; itmcl++) {
    std::vector<MCPVInfo>::iterator cmc = closestMCPV(rblemcpv,itmcl);   
    double dist = 999999.;
    int mult = 0;
    if(cmc != rblemcpv.end()) {
      dist = (cmc->pMCPV->position() - itmcl->pMCPV->position()).R();
      mult = cmc->nRecTracks;
    }
    itmcl->distToClosestMCPV = dist;
    itmcl->multClosestMCPV = mult; 
  }

  // Counters
  int nMCPV                 = rblemcpv.size();
  int nRecMCPV              = 0;
  int nMCPV_isol            = 0;
  int nRecMCPV_isol         = 0;
  int nMCPV_close           = 0;
  int nRecMCPV_close        = 0;
  int nMCPV_1mult           = 0;
  int nRecMCPV_1mult        = 0;
  int nMCPV_isol_1mult      = 0;
  int nRecMCPV_isol_1mult   = 0;
  int nMCPV_close_1mult     = 0;
  int nRecMCPV_close_1mult  = 0;
  int nMCPV_2mult           = 0;
  int nRecMCPV_2mult        = 0;
  int nMCPV_isol_2mult      = 0;
  int nRecMCPV_isol_2mult   = 0;
  int nMCPV_close_2mult     = 0;
  int nRecMCPV_close_2mult  = 0;
  int nMCPV_3mult           = 0;
  int nRecMCPV_3mult        = 0;
  int nMCPV_isol_3mult      = 0;
  int nRecMCPV_isol_3mult   = 0;
  int nMCPV_close_3mult     = 0;
  int nRecMCPV_close_3mult  = 0;

  for(itmc = rblemcpv.begin(); rblemcpv.end() != itmc; itmc++) {
    if(itmc->distToClosestMCPV > m_dzIsolated) nMCPV_isol++;
    if(itmc->distToClosestMCPV < m_dzIsolated) nMCPV_close++;
    if(itmc->indexRecPVInfo > -1) {
      nRecMCPV++;
      if(itmc->distToClosestMCPV > m_dzIsolated) nRecMCPV_isol++;
      if(itmc->distToClosestMCPV < m_dzIsolated) nRecMCPV_close++;
    }
  }

  // rblemcpv is already sorted

  // highest mult
  if (rblemcpv.size() > 0 ) {
    nMCPV_1mult++;
    double dist = rblemcpv[0].distToClosestMCPV;
    if(dist > m_dzIsolated) nMCPV_isol_1mult++;
    if(dist < m_dzIsolated) nMCPV_close_1mult++;
    if(rblemcpv[0].indexRecPVInfo > -1) {
      nRecMCPV_1mult++;
      if(dist > m_dzIsolated) nRecMCPV_isol_1mult++;
      if(dist < m_dzIsolated) nRecMCPV_close_1mult++;
    }
  }
  // second highest
  if (rblemcpv.size() > 1 ) {
    nMCPV_2mult++;
    double dist = rblemcpv[1].distToClosestMCPV;
    if(dist > m_dzIsolated) nMCPV_isol_2mult++;
    if(dist < m_dzIsolated) nMCPV_close_2mult++;
    if(rblemcpv[1].indexRecPVInfo > -1) {
      nRecMCPV_2mult++;
      if(dist > m_dzIsolated) nRecMCPV_isol_2mult++;
      if(dist < m_dzIsolated) nRecMCPV_close_2mult++;
    }
  }
  // third highest
  if (rblemcpv.size() > 2 ) {
    nMCPV_3mult++;
    double dist = rblemcpv[2].distToClosestMCPV;
    if(dist > m_dzIsolated) nMCPV_isol_3mult++;
    if(dist < m_dzIsolated) nMCPV_close_3mult++;
    if(rblemcpv[2].indexRecPVInfo > -1) {
      nRecMCPV_3mult++;
      if(dist > m_dzIsolated) nRecMCPV_isol_3mult++;
      if(dist < m_dzIsolated) nRecMCPV_close_3mult++;
    }
  }

  
  m_nMCPV                 +=  nMCPV;
  m_nRecMCPV              +=  nRecMCPV; 
  m_nMCPV_isol            +=  nMCPV_isol;
  m_nRecMCPV_isol         +=  nRecMCPV_isol;
  m_nMCPV_close           +=  nMCPV_close;
  m_nRecMCPV_close        +=  nRecMCPV_close;
  m_nFalsePV              +=  nFalsePV;
  m_nFalsePV_real         +=  nFalsePV_real;
  m_nMCPV_1mult           +=  nMCPV_1mult;
  m_nRecMCPV_1mult        +=  nRecMCPV_1mult; 
  m_nMCPV_isol_1mult      +=  nMCPV_isol_1mult;
  m_nRecMCPV_isol_1mult   +=  nRecMCPV_isol_1mult;
  m_nMCPV_close_1mult     +=  nMCPV_close_1mult;
  m_nRecMCPV_close_1mult  +=  nRecMCPV_close_1mult;
  m_nMCPV_2mult           +=  nMCPV_2mult;
  m_nRecMCPV_2mult        +=  nRecMCPV_2mult; 
  m_nMCPV_isol_2mult      +=  nMCPV_isol_2mult;
  m_nRecMCPV_isol_2mult   +=  nRecMCPV_isol_2mult;
  m_nMCPV_close_2mult     +=  nMCPV_close_2mult;
  m_nRecMCPV_close_2mult  +=  nRecMCPV_close_2mult;
  m_nMCPV_3mult           +=  nMCPV_3mult;
  m_nRecMCPV_3mult        +=  nRecMCPV_3mult; 
  m_nMCPV_isol_3mult      +=  nMCPV_isol_3mult;
  m_nRecMCPV_isol_3mult   +=  nRecMCPV_isol_3mult;
  m_nMCPV_close_3mult     +=  nMCPV_close_3mult;
  m_nRecMCPV_close_3mult  +=  nRecMCPV_close_3mult;
  
  for(itmc = rblemcpv.begin(); rblemcpv.end() != itmc; itmc++) {
    double dx         = -99999.;
    double dy         = -99999.;
    double dz         = -99999.;
    double errx       = -99999.;
    double erry       = -99999.;
    double errz       = -99999.;
    int indRec        = itmc->indexRecPVInfo;
    int reconstructed = 0;
    int ntracks_pvrec = 0;
    int nvelotracks_pvrec = 0;
    int ntracks_pvmc  = 0;
    int dtrcks        = 0;
    int pvevt         = 0;
    int assoctrks     = 0;
    int nassoctrks    = 0;
    if(indRec > -1) {
      pvevt++;
      reconstructed = 1;
      dx = recpvvec[indRec].position.x() - itmc->pMCPV->position().x();
      dy = recpvvec[indRec].position.y() - itmc->pMCPV->position().y();
      dz = recpvvec[indRec].position.z() - itmc->pMCPV->position().z();
      errx = recpvvec[indRec].positionSigma.x();
      erry = recpvvec[indRec].positionSigma.y();
      errz = recpvvec[indRec].positionSigma.z();
      ntracks_pvrec = recpvvec[indRec].nTracks;
      nvelotracks_pvrec = recpvvec[indRec].nVeloTracks;
      ntracks_pvmc = itmc->pMCPV->products().size();
      dtrcks = ntracks_pvmc - ntracks_pvrec;
      // Filling histograms
      if(m_produceHistogram) {
        plot(itmc->pMCPV->position().x(), 1001, "xmc", -0.25, 0.25, 50);
        plot(itmc->pMCPV->position().y(), 1002, "ymc", -0.25, 0.25, 50);
        plot(itmc->pMCPV->position().z(), 1003, "zmc", -20, 20, 50);
        plot(recpvvec[indRec].position.x(), 1011, "xrd", -0.25, 0.25, 50);
        plot(recpvvec[indRec].position.y(), 1012, "yrd", -0.25, 0.25, 50);
        plot(recpvvec[indRec].position.z(), 1013, "zrd", -20,20, 50);
        plot(dx, 1021, "dx", -0.25, 0.25, 50);
        plot(dy, 1022, "dy", -0.25, 0.25, 50);
        plot(dz, 1023, "dz", -0.5, 0.5, 50);
        plot(dx/errx, 1031, "pullx", -5., 5., 50);
        plot(dy/erry, 1032, "pully", -5., 5., 50);
        plot(dz/errz, 1033, "pullz", -5., 5., 50);
        plot(double(ntracks_pvrec), 1041, "ntracks_pvrec", 0., 150., 50);
        plot(double(dtrcks), 1042, "mcrdtracks", 0., 150., 50);
        plot(double(nvelotracks_pvrec), 1043, "nvelotracks_pvrec", 0., 150., 50);
        if(pvevt == 1) {
          plot(double(recpvvec.size()), 1051, "nPVperEvt", -0.5, 5.5, 6);
          for(int ipvrec = 0; ipvrec < (int)recpvvec.size(); ipvrec++) {
            assoctrks = assoctrks + recpvvec[ipvrec].nTracks;
          }
          nassoctrks = vecOfTracks.size() - assoctrks;
          plot(double(nassoctrks), 1052, "nassoctrks", 0., 150., 50);
        }
      }
    }

    int isolated = 0;
    double dist_closest = itmc->distToClosestMCPV;
    if(dist_closest > m_dzIsolated) {
      isolated = 1;
    }

    // Filling ntuple
    if(m_produceNtuple) {
      Tuple myTuple = nTuple(101, "PV_nTuple", CLID_ColumnWiseTuple);
      myTuple->column("reco",     double(reconstructed));
      myTuple->column("isol",     double(isolated));
      myTuple->column("ntracks",  double(ntracks_pvrec));
      myTuple->column("nvelotracks",  double(nvelotracks_pvrec));
      myTuple->column("nrectrmc", double(itmc->nRecTracks));
      myTuple->column("dzclose",   dist_closest);
      myTuple->column("nmcpv",    double(rblemcpv.size()));
      myTuple->column("dx",       dx);
      myTuple->column("dy",       dy);
      myTuple->column("dz",       dz);
      myTuple->column("errx",     errx);
      myTuple->column("erry",     erry);
      myTuple->column("errz",     errz);
      myTuple->column("evtnr",    double(m_nevt));
      myTuple->write();
    }
  }

  // get the efficiency of PV the signal B is produced
  const LHCb::MCParticle*     mcSignal = m_forcedBtool->forcedB();
  if( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) ) {
     LHCb::L0DUReport* l0rep = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default); 
     bool L0DecGlobal   = l0rep->decision();
     if (L0DecGlobal) {
       if(mcSignal) {
	 m_nL0PvOfB++;
	 const LHCb::MCVertex* bpv = mcSignal->primaryVertex();
	 bool pvfound = false;
	 std::vector<RecPVInfo>::iterator itRec;
	 for(itRec = recpvvec.begin(); itRec != recpvvec.end(); itRec++) {
	   double distsig = fabs(itRec->position.z() - bpv->position().z())/itRec->positionSigma.z();
	   if(distsig<5.) pvfound = true;
	 }
	 if(pvfound) m_nRecL0PvOfB++;
       }
     }
  }

  // check if B secondary vertex was reconstructed as false PV
  if(mcSignal) {
    if ( mcSignal->endVertices().size()>0 ) {
      m_nBFalse++;
      Gaudi::XYZPoint  bfalsevtx;
      bool bfalse = false;
      bfalsevtx = (*(mcSignal->endVertices().begin()))->position();
      std::vector<RecPVInfo>::iterator itRec;
      for(itRec = recpvvec.begin(); itRec != recpvvec.end(); itRec++) {
        if (itRec->indexMCPVInfo < 0) { 
  	   double distsig = fabs(itRec->position.z() - bfalsevtx.z())/itRec->positionSigma.z();
  	   if(distsig<5.) bfalse = true;
  	 }
       }
       if(bfalse) m_nRecBFalse++;
    }
  }
    
  return StatusCode::SUCCESS;

}

void  PrimaryVertexChecker::match_mc_vertex_by_tracks(int ipv, std::vector<RecPVInfo>&  rinfo, std::vector<MCPVInfo>& mcpvvec) {

  LHCb::RecVertex* invtx = rinfo[ipv].pRECPV;
  typedef const SmartRefVector<LHCb::Track>      PVTRACKS ;
  PVTRACKS& tracksIn =  invtx->tracks();
  PVTRACKS::const_iterator itin;

  int indexmc = -1;
  double ratiomax = 0.;
  for(int imc = 0; imc < (int) mcpvvec.size(); imc++) {
    if ( mcpvvec[imc].indexRecPVInfo  > -1) continue;
    int ntrin = 0;
    for ( std::vector<LHCb::Track*>::iterator itr = mcpvvec[imc].m_recTracksInMCPV.begin(); 
          itr != mcpvvec[imc].m_recTracksInMCPV.end(); itr++) {
      for ( itin = tracksIn.begin(); itin !=tracksIn.end(); itin++) {
	const LHCb::Track* ptr = *itin;
         if( ptr == *itr ) {
            ntrin++;
            break;
	 }
      }
    }
    double ratio = 1.*ntrin/tracksIn.size();
    if ( ratio > ratiomax ) {
      ratiomax = ratio;
      indexmc = imc;
    }     
  } // imc
  if (ratiomax > 0.05 ) {
    rinfo[ipv].indexMCPVInfo = indexmc;
    mcpvvec[indexmc].indexRecPVInfo = ipv;   
  }


}

void  PrimaryVertexChecker::match_mc_vertex_by_distance(int ipv, std::vector<RecPVInfo>& rinfo, std::vector<MCPVInfo>& mcpvvec) {

  double mindist = 999999.;
  int indexmc = -1;

  for(int imc = 0; imc < (int) mcpvvec.size(); imc++) {
    if ( mcpvvec[imc].indexRecPVInfo  > -1) continue;
    double dist = fabs(mcpvvec[imc].pMCPV->position().z() -
                       rinfo[ipv].position.z());
    if(dist < mindist) {
      mindist = dist;
      indexmc = imc; 
    }
  }
  if ( indexmc > -1 ) {
    if(mindist < 5.0 * rinfo[ipv].positionSigma.z()) {
      rinfo[ipv].indexMCPVInfo = indexmc;
      mcpvvec[indexmc].indexRecPVInfo = ipv;   
    }
  }

}


std::vector<MCPVInfo>::iterator PrimaryVertexChecker::closestMCPV(std::vector<MCPVInfo>& rblemcpv,
                                                                  std::vector<MCPVInfo>::iterator& itmc) {

  std::vector<MCPVInfo>::iterator itret = rblemcpv.end();
  double mindist = 999999.;
  if(rblemcpv.size() < 2) return itret;
  std::vector<MCPVInfo>::iterator it;
  for (it = rblemcpv.begin(); it != rblemcpv.end(); it++) {
    if(it->pMCPV != itmc->pMCPV ) {
      double dist = (it->pMCPV->position() - itmc->pMCPV->position()).R();
      if(dist < mindist) {
        mindist = dist;
        itret = it;
      }
    }
  }
  return itret;
}

void PrimaryVertexChecker::collectProductss(LHCb::MCVertex* mcpv, LHCb::MCVertex* mcvtx, 
                               std::vector<LHCb::MCParticle*>& allprods) {

  SmartRefVector<LHCb::MCParticle> daughters = mcvtx->products();
  SmartRefVector<LHCb::MCParticle>::iterator idau;
  for(idau = daughters.begin(); idau != daughters.end(); idau++) {
    double dv2 = (mcpv->position() - (*idau)->originVertex()->position()).Mag2();
    if(dv2 > (100.* Gaudi::Units::mm) * (100.* Gaudi::Units::mm)) continue;
    LHCb::MCParticle* pmcp = *idau;
    allprods.push_back(pmcp);
    SmartRefVector<LHCb::MCVertex> decays = (*idau)->endVertices();
    SmartRefVector<LHCb::MCVertex>::iterator ivtx;
    for(ivtx = decays.begin(); ivtx != decays.end(); ivtx++)  {
      collectProductss(mcpv, *ivtx, allprods);
    }
  }
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrimaryVertexChecker::finalize() {

  debug() << "==> Finalize" << endmsg;


  info() << " ============================================" << endmsg;
  info() << " Efficiencies for reconstructible MC vertices: " << endmsg;
  info() << " ============================================" << endmsg;
  info() << " " << endmsg;

  info() << " MC PV is reconstructible if at least " << m_nTracksToBeRecble 
         << "  tracks are reconstructed" << endmsg;
  info() << " MC PV is isolated if dz to closest reconstructible MC PV >  " 
         << m_dzIsolated << " mm" << endmsg;
  std::string ff = "by counting tracks";
  if ( !m_matchByTracks ) ff = "by dz distance";
  info() << " REC and MC vertices matched:  " 
         <<  ff << endmsg;


  info() << " " << endmsg;

            printRat("All",       m_nRecMCPV ,       m_nMCPV );
            printRat("Isolated",  m_nRecMCPV_isol,   m_nMCPV_isol );
            printRat("Close",     m_nRecMCPV_close,  m_nMCPV_close );
            printRat("False rate",m_nFalsePV ,       m_nRecMCPV+m_nFalsePV );

	    if(debugLevel()) {
  	      printRat("Real false rate", m_nFalsePV_real , m_nRecMCPV+m_nFalsePV_real); 
	    }

            info() << endmsg;
            printRat("L0 accepted PV of B", m_nRecL0PvOfB, m_nL0PvOfB);
            printRat("False PV as B", m_nRecBFalse, m_nBFalse);
            info() << endmsg;


  info() << "      --------------------------------------------" << endmsg;
  info() << "           Substatistics: " << endmsg;
  info() << "      --------------------------------------------" << endmsg;
  info() << "      1st PV (highest multiplicity): " << endmsg;
            printRat("All",      m_nRecMCPV_1mult ,      m_nMCPV_1mult ) ;
            printRat("Isolated", m_nRecMCPV_isol_1mult,  m_nMCPV_isol_1mult ) ;
            printRat("Close",    m_nRecMCPV_close_1mult, m_nMCPV_close_1mult ) ;

  info() << "      ---------------------------------------" << endmsg;
  info() << "      2nd PV: " << endmsg;
            printRat("All",      m_nRecMCPV_2mult ,      m_nMCPV_2mult ) ;
            printRat("Isolated", m_nRecMCPV_isol_2mult , m_nMCPV_isol_2mult ) ;
            printRat("Close",    m_nRecMCPV_close_2mult, m_nMCPV_close_2mult ) ;

  info() << "      ---------------------------------------" << endmsg;
  info() << "      3rd PV: " << endmsg;
            printRat("All",      m_nRecMCPV_3mult ,       m_nMCPV_3mult ) ;
            printRat("Isolated", m_nRecMCPV_isol_3mult ,  m_nMCPV_isol_3mult ) ;
            printRat("Close",    m_nRecMCPV_close_3mult , m_nMCPV_close_3mult ) ;
	    
  info() << " " << endmsg;
  if(debugLevel()) {
    info() << " * Real false rate means: no visible MC PV within 5 sigma of REC PV." 
           << " Visible MC PV: 2 tracks reconstructed" << endmsg;
    info() << " " << endmsg;
  }
  const AIDA::IHistogram1D* dx = histo( HistoID(1021) ) ;
  const AIDA::IHistogram1D* pullx = histo( HistoID(1031) ) ;
  const AIDA::IHistogram1D* dy = histo( HistoID(1022) ) ;
  const AIDA::IHistogram1D* pully = histo( HistoID(1032) ) ;
  const AIDA::IHistogram1D* dz = histo( HistoID(1023) ) ;
  const AIDA::IHistogram1D* pullz = histo( HistoID(1033) ) ;
  if( dx ) {
    info() << "      ---------------------------------------" << endmsg;
    info() << "dx:    "
	   << format( "mean =  %5.3f +/- %5.3f, RMS =  %5.3f +/- %5.3f",
		      dx->mean(), Gaudi::Utils::HistoStats::meanErr(dx),
		      dx->rms(), Gaudi::Utils::HistoStats::rmsErr(dx)) << endmsg ;
  }
  if( dy ) {
    info() << "dy:    "
	   << format( "mean =  %5.3f +/- %5.3f, RMS =  %5.3f +/- %5.3f",
		      dy->mean(), Gaudi::Utils::HistoStats::meanErr(dy),
		      dy->rms(), Gaudi::Utils::HistoStats::rmsErr(dy)) << endmsg ;
  }
  if( dz ) {
    info() << "dz:    "
	   << format( "mean =  %5.3f +/- %5.3f, RMS =  %5.3f +/- %5.3f",
		      dz->mean(), Gaudi::Utils::HistoStats::meanErr(dz),
		      dz->rms(), Gaudi::Utils::HistoStats::rmsErr(dz)) << endmsg ;
  }
  info() << "      ---------------------------------------" << endmsg;
  if( pullx ) {
    info() << "pullx: "
	   << format( "mean =  %5.3f +/- %5.3f, RMS =  %5.3f +/- %5.3f",
		      pullx->mean(), Gaudi::Utils::HistoStats::meanErr(pullx),
		      pullx->rms(), Gaudi::Utils::HistoStats::rmsErr(pullx)) << endmsg ;
  }
  if( pully ) {
    info() << "pully: "
	   << format( "mean =  %5.3f +/- %5.3f, RMS =  %5.3f +/- %5.3f",
		      pully->mean(), Gaudi::Utils::HistoStats::meanErr(pully),
		      pully->rms(), Gaudi::Utils::HistoStats::rmsErr(pully)) << endmsg ;
  }
  if( pullz ) {
    info() << "pullz: "
	   << format( "mean =  %5.3f +/- %5.3f, RMS =  %5.3f +/- %5.3f",
		      pullz->mean(), Gaudi::Utils::HistoStats::meanErr(pullz),
		      pullz->rms(), Gaudi::Utils::HistoStats::rmsErr(pullz)) << endmsg ;
  }
  info() << " ============================================" << endmsg;

  return GaudiTupleAlg::finalize();  // Must be called after all other actions
}

void PrimaryVertexChecker::printRat(std::string mes, int a, int b) {

  double rat = 0.;
  if(b>0) rat = 1.0*a/b;

  // reformat message
  unsigned int len = 20;
  std::string pmes = mes;
  while(pmes.length() < len) {
    pmes+=" ";
  }
  pmes+= " : ";

  info() << pmes << format(" %6.3f ( %7d / %8d )", rat,a,b) << endmsg; 

}

void PrimaryVertexChecker::count_reconstructed_tracks(std::vector<MCPVInfo>& mcpvvec, 
                   std::vector<LHCb::Track*>& vecOfTracks, std::string trackLoc) {

  LinkedTo<LHCb::MCParticle> trackMClink(eventSvc(), msgSvc(), trackLoc);


  // Find # of reconstructed tracks of every MC PV
  std::vector<MCPVInfo>::iterator itinfomc;
  for(itinfomc = mcpvvec.begin(); mcpvvec.end() != itinfomc; itinfomc++) {
    LHCb::MCVertex* avtx = itinfomc->pMCPV;

    SmartRefVector<LHCb::MCParticle> parts = avtx->products();
    std::vector<LHCb::MCParticle*> allproducts;
    collectProductss(avtx, avtx, allproducts);

    LHCb::Track* recTrack = 0;
    std::vector<LHCb::MCParticle*>::iterator imcp;
    for(imcp = allproducts.begin(); allproducts.end() != imcp; imcp++) {
      LHCb::MCParticle* pmcp = *imcp;
      int isReco = 0;
      for (std::vector<LHCb::Track*>::iterator itrec = vecOfTracks.begin(); itrec != vecOfTracks.end(); itrec++ ) {
        LHCb::MCParticle* partmc = trackMClink.first((*itrec)->key());
        if (partmc && partmc == pmcp) {
          recTrack = (*itrec);
          if (!m_requireVelo || recTrack->hasVelo()){
            isReco = 1;
            break;
          }
        }
      }
      if(pmcp->particleID().threeCharge() != 0  &&  isReco ) {
        double dv2 = (avtx->position() - pmcp->originVertex()->position()).Mag2();
        if( dv2 < 0.0001 ) {
          itinfomc->m_mcPartInMCPV.push_back(pmcp);
          itinfomc->m_recTracksInMCPV.push_back(recTrack);
        }
      }
    }
    itinfomc->nRecTracks = itinfomc->m_mcPartInMCPV.size();       
  }
}

int  PrimaryVertexChecker::count_velo_tracks(LHCb::RecVertex* RecVtx){
  SmartRefVector< LHCb::Track >  vtx_tracks = RecVtx->tracks();
  int nVeloTracks = 0;
  for (unsigned int it = 0; it < vtx_tracks.size(); it++ ) {
    const LHCb::Track* ptr =  vtx_tracks[it];
    if (ptr->hasVelo()) nVeloTracks++;
  }
  return nVeloTracks;
}

void PrimaryVertexChecker::count_reconstructible_mc_particles(std::vector<MCPVInfo>& mcpvvec) {

  MCTrackInfo trInfo(eventSvc(), msgSvc());
  std::vector<MCPVInfo>::iterator itinfomc;
  for(itinfomc = mcpvvec.begin(); mcpvvec.end() != itinfomc; itinfomc++) {
    LHCb::MCVertex* avtx = itinfomc->pMCPV;
    std::vector<LHCb::MCParticle*> mcPartInMCPV;
    SmartRefVector<LHCb::MCParticle> parts = avtx->products();
    std::vector<LHCb::MCParticle*> allproducts;
    collectProductss(avtx, avtx, allproducts);
    std::vector<LHCb::MCParticle*>::iterator imcp;

    for(imcp = allproducts.begin(); allproducts.end() != imcp; imcp++) {

      LHCb::MCParticle* pmcp = *imcp;

      if(pmcp->particleID().threeCharge() != 0  &&  (!m_requireVelo || trInfo.hasVelo(pmcp))) {

        double dv2 = (avtx->position() - pmcp->originVertex()->position()).Mag2();
        if(dv2 < 0.0000001 && pmcp->p() > 100.* Gaudi::Units::MeV) {
          mcPartInMCPV.push_back(pmcp);

        }

      }
    }
    itinfomc->nRecTracks = mcPartInMCPV.size();       
  }


}


bool PrimaryVertexChecker::getInputTracks( std::vector<LHCb::Track*>& vecOfTracks,  std::string& trackLoc) {


  std::string tracksName   = m_inputTracksName;

  if ( m_inputTracksName == "Offline" ) {
    tracksName = LHCb::TrackLocation::Default;
  } else if ( m_inputTracksName == "3D" ) {
    tracksName = LHCb::TrackLocation::Velo;
  } else if ( m_inputTracksName == "2D" ) {
    tracksName = LHCb::TrackLocation::RZVelo;
  }

  if ( tracksName == "none" ) {
    debug() << " Tracks not specified " << tracksName << endmsg;
    return false;
  }

  trackLoc = tracksName;  

  LHCb::Tracks* usedTracks;

  usedTracks   = getOrCreate<LHCb::Tracks,LHCb::Tracks>( tracksName );
  if ( usedTracks->size() == 0 ) return false;

  std::vector<LHCb::Track*>::const_iterator itT;
  for ( itT = usedTracks->begin(); usedTracks->end() != itT; itT++ ) {
    LHCb::Track* ptr = (*itT);
    vecOfTracks.push_back(ptr);
  }

  return true;

}


bool PrimaryVertexChecker::getInputVertices( std::vector<LHCb::RecVertex*>& vecOfVertices ) {

  std::string verticesName = m_inputVerticesName;

  if ( m_inputVerticesName == "Offline" ) {
    verticesName = LHCb::RecVertexLocation::Primary;
  } else if ( m_inputVerticesName == "3D" ) {
    verticesName =  LHCb::RecVertexLocation::Velo3D;
  } else if ( m_inputVerticesName == "2D" ) {
    verticesName = LHCb::RecVertexLocation::Velo2D;
  }

  if ( verticesName == "none" ) {
    debug() << " Vertices not specified " << verticesName << endmsg;
    return false;
  }

  LHCb::RecVertices* recoVertices;

  recoVertices  = getOrCreate<LHCb::RecVertices,LHCb::RecVertices>( verticesName );
  if ( recoVertices->size() == 0 ) return false;

  std::vector<LHCb::RecVertex*>::const_iterator itVer;
  for ( itVer = recoVertices->begin(); recoVertices->end() != itVer; itVer++ ) {
    LHCb::RecVertex* ppv = (*itVer);
    vecOfVertices.push_back(ppv);
  }

  return true;

}
