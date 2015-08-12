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
#include <TRandom.h>

// local
#include "PVResMonitor.h"

bool sortmlti(MCPVInfo first, MCPVInfo second) {
     return first.nRecTracks > second.nRecTracks;
}

//-----------------------------------------------------------------------------
// Implementation file for class : PVResMonitor
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(PVResMonitor)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVResMonitor::PVResMonitor(const std::string& name,
                 ISvcLocator* pSvcLocator)
  : GaudiTupleAlg (name,pSvcLocator), m_forcedBtool(0), m_pvsfit(0)
{
    declareProperty("nTracksToBeRecble",  m_nTracksToBeRecble = 5);
    declareProperty("produceNtuple",      m_produceNtuple     = true);
    declareProperty("produceHistogram",   m_produceHistogram  = true);
    declareProperty("dzIsolated",         m_dzIsolated        = 10.0 * Gaudi::Units::mm);
    declareProperty("inputTracksName",    m_inputTracksName   = LHCb::TrackLocation::Default);
    declareProperty("inputVerticesName",  m_inputVerticesName = LHCb::RecVertexLocation::Primary);
}
//=============================================================================
// Destructor
//=============================================================================
PVResMonitor::~PVResMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVResMonitor::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // Must be executed first
  if(sc.isFailure()) debug() << "==> Initialize" << endmsg;

  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endmsg;
    return StatusCode::FAILURE;
  }

  // Access PVOfflineTool
  m_pvsfit = tool<IPVOfflineTool>("PVOfflineTool",this);
  if(!m_pvsfit) {
    err() << "Unable to retrieve the PVOfflineTool" << endmsg;
    return  StatusCode::FAILURE;
  }

  m_nevt      = 0;
  m_nVtx      = 0;
  m_nPartVtx  = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVResMonitor::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  // Event
  m_nevt++;

  // Get input
  std::vector<LHCb::Track*> vecOfTracks;
  std::vector<LHCb::RecVertex*> vecOfVertices;
  std::string trackLoc;
  bool tracks_ok   = getInputTracks(vecOfTracks, trackLoc);
  bool vertices_ok = getInputVertices(vecOfVertices);

  std::vector<LHCb::RecVertex> halfVrt1, halfVrt2, fullVrt;
  std::vector<const LHCb::Track*> halfTracks1, halfTracks2;

  if (!tracks_ok) {
    return StatusCode::SUCCESS; // return SUCCESS anyway
  }

  if (!vertices_ok) {
    return StatusCode::SUCCESS; // return SUCCESS anyway
  }

  if(msgLevel(MSG::DEBUG))
    debug() << trackLoc << " # tracks: " << vecOfTracks.size() << "  # vertices: " << vecOfVertices.size() << endmsg;

  // Fill reconstructed PV info
  std::vector<LHCb::RecVertex*>::iterator itRecV;
  for(itRecV = vecOfVertices.begin(); vecOfVertices.end() != itRecV;
               itRecV++) {
    LHCb::RecVertex* pv;
    pv = *itRecV;
    fullVrt.push_back(*pv);
    int nTracks      = pv->tracks().size();
    //vertex is reconstructible if consist of at least 6 tracks
    if (nTracks>=2*m_nTracksToBeRecble) m_nVtx++;

    int halfend = int(std::floor(double(nTracks)/2.));

    int ntracks_part  = 0;
    TRandom* rndm = new TRandom(0);
    std::vector<int> rvect;
    int k = 0;

    while (k<halfend){
      int r = rndm->Integer(nTracks-1);
      int repeat = (int) std::count(rvect.begin(), rvect.end(), r);
      if (repeat==0){
        rvect.push_back(r);
        k++;
      }
    } //end of while
    std::sort(rvect.begin(), rvect.end());

    if(msgLevel(MSG::DEBUG))
      debug() << "No of randoms " << rvect.size() << "   and tracks " << nTracks << endmsg;

    int prev_size = int(halfTracks2.size());
    SmartRefVector< LHCb::Track >  vtx_tracks = pv->tracks();

    //filling half vertices
    for (unsigned int it = 0; it < vtx_tracks.size(); it++ ) {
      const LHCb::Track* ptr =  vtx_tracks.at(it);
      int rnow = *rvect.begin();
      if (int(it)==rnow) {
        rvect.erase(rvect.begin());
        halfTracks1.push_back(ptr);
      }
      else halfTracks2.push_back(ptr);
    } //end of track filling loop

    ntracks_part  = (int)halfTracks1.size();
    //checing if the size of half vertices is the same
    if ((int)halfTracks2.size()>ntracks_part && (nTracks>2))
      halfTracks2.erase(halfTracks2.begin()+prev_size+int(rndm->Integer((int)halfTracks2.size()-1-prev_size)));
  } // end of loop over vertices

  //fitting vertices
  StatusCode scfit  = m_pvsfit->reconstructMultiPVFromTracks(halfTracks1, halfVrt1);
  StatusCode scfit2 = m_pvsfit->reconstructMultiPVFromTracks(halfTracks2, halfVrt2);

  int reconstructed  = 0;
  int reconstructed2 = 0;
  double dx          = -99999.;
  double dy          = -99999.;
  double dz          = -99999.;
  double errx        = -99999.;
  double erry        = -99999.;
  double errz        = -99999.;

  int ntracks_part  = 0;
  int ntracks_part2 = 0;
  int dtrcks = 0;

  std::vector<LHCb::RecVertex>::iterator halfIter;
  int oIt=0;
  std::vector<int> link1, link2;
  if (halfVrt1.size()!=0)    matchByDistance(fullVrt, halfVrt1, link1);
  if (halfVrt2.size()!=0)    matchByDistance(fullVrt, halfVrt2, link2);
  if(msgLevel(MSG::DEBUG)) debug() << "Number of full  vertices " << fullVrt.size()  << endmsg;
  if(msgLevel(MSG::DEBUG)) debug() << "Number of half  vertices " << halfVrt1.size() << endmsg;
  if(msgLevel(MSG::DEBUG)) debug() << "Number of half2 vertices " << halfVrt2.size() << endmsg;

  for (halfIter=fullVrt.begin(); halfIter!=fullVrt.end(); halfIter++){
    LHCb::RecVertex vrtf = *halfIter;
    int nTracks = vrtf.tracks().size();
    ntracks_part = int(floor(double(nTracks)/2.));
    ntracks_part2= ntracks_part;
    if (halfVrt1.size()!=0 && halfVrt2.size()!=0){
      dtrcks = nTracks - ntracks_part;
      if (link1.at(oIt)==-1) continue;
      reconstructed  = 1;
      m_nPartVtx++;
      if (link2.at(oIt)==-1) continue;
      reconstructed2 = 1;
        //calculate distance and so on...
      Gaudi::SymMatrix3x3 covPV_part1 = halfVrt1.at(link1.at(oIt)).covMatrix();
      double sigx_part1 = (covPV_part1(0,0));
      double sigy_part1 = (covPV_part1(1,1));
      double sigz_part1 = (covPV_part1(2,2));

      Gaudi::SymMatrix3x3 covPV_part2 = halfVrt2.at(link2.at(oIt)).covMatrix();
      double sigx_part2 = (covPV_part2(0,0));
      double sigy_part2 = (covPV_part2(1,1));
      double sigz_part2 = (covPV_part2(2,2));

      dx = halfVrt1.at(link1.at(oIt)).position().x()-halfVrt2.at(link2.at(oIt)).position().x();
      dy = halfVrt1.at(link1.at(oIt)).position().y()-halfVrt2.at(link2.at(oIt)).position().y();
      dz = halfVrt1.at(link1.at(oIt)).position().z()-halfVrt2.at(link2.at(oIt)).position().z();
      dx = dx/sqrt(2.);
      dy = dy/sqrt(2.);
      dz = dz/sqrt(2.);
      errx = sqrt(sigx_part1 + sigx_part2)/sqrt(2.);
      erry = sqrt(sigy_part1 + sigy_part2)/sqrt(2.);
      errz = sqrt(sigz_part1 + sigz_part2)/sqrt(2.);
      ntracks_part  = halfVrt1.at(link1.at(oIt)).tracks().size();
      ntracks_part2 = halfVrt2.at(link2.at(oIt)).tracks().size();
      dtrcks = nTracks - ntracks_part;

      if(m_produceHistogram) {
        plot(dx, 1021, "dx", -0.25, 0.25, 50);
        plot(dy, 1022, "dy", -0.25, 0.25, 50);
        plot(dz, 1023, "dz", -0.5, 0.5, 50);
        plot(dx/errx, 1031, "pullx", -5., 5., 50);
        plot(dy/erry, 1032, "pully", -5., 5., 50);
        plot(dz/errz, 1033, "pullz", -5., 5., 50);
        plot(double(nTracks), 1041, "Original PV nTracks", 0., 150., 50);
        plot(double(dtrcks), 1042, "dtracks", 0., 150., 50);
      }
    }
    if(m_produceNtuple) {
      Tuple myTuple = nTuple(101, "PV_nTuple", CLID_ColumnWiseTuple);
      myTuple->column("reco",     double(reconstructed));
      myTuple->column("reco2",     double(reconstructed2));
//       myTuple->column("isol",     double(isolated));
      myTuple->column("ntracks_part",  double(ntracks_part));
      myTuple->column("ntracks_part2",  double(ntracks_part2));
      myTuple->column("ntracks", double(nTracks));
//       myTuple->column("dzclose",   dist_closest);
//       myTuple->column("nmcpv",    double(rblemcpv.size()));
      myTuple->column("dx",       dx);
      myTuple->column("dy",       dy);
      myTuple->column("dz",       dz);
      myTuple->column("errx",     errx);
      myTuple->column("erry",     erry);
      myTuple->column("errz",     errz);
      myTuple->column("evtnr",    double(m_nevt));
      myTuple->write();
    }
    oIt++;
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode PVResMonitor::finalize() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;


  info() << " ============================================" << endmsg;
  info() << " Efficiencies for reconstructed vertices:    " << endmsg;
  info() << " ============================================" << endmsg;
  info() << " " << endmsg;

  info() << " PV is reconstructible if at least " << m_nTracksToBeRecble
         << "  tracks are reconstructed" << endmsg;
//   info() << " PV is isolated if dz to closest reconstructible MC PV >  "
//          << m_dzIsolated << " mm" << endmsg;
  std::string ff = "by counting tracks";
/*  if ( !m_matchByTracks )*/ ff = "by dz distance";
  info() << " Two splited vertices matched:  "
         <<  ff << endmsg;


  info() << " " << endmsg;
    printRat("All",       m_nPartVtx,       m_nVtx );

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
//
  return GaudiTupleAlg::finalize();  // Must be called after all other actions
}

//=============================================================================
//  Match vertices by distance
//=============================================================================
void  PVResMonitor::matchByDistance(std::vector<LHCb::RecVertex>& vertfull, std::vector<LHCb::RecVertex>& verthalf, std::vector<int>& link) {

  if (verthalf.size() > vertfull.size() && msgLevel(MSG::DEBUG)) debug() << "half.size > full.size" << endmsg;
  for(int imc=0; imc<(int)vertfull.size(); imc++) {
//     if ( mcpvvec[imc].indexRecPVInfo  > -1) continue;
    double mindist = 999999.;
    int indexrec = -1;
    for(int irec=0; irec<(int)verthalf.size(); irec++){
      if (std::count(link.begin(), link.end(), irec)!=0) continue;
      double dist = fabs(verthalf.at(irec).position().z()-vertfull.at(imc).position().z());
      if(dist < mindist) {
        mindist = dist;
        indexrec = irec;
      }
    }
    if(msgLevel(MSG::DEBUG)) debug() << "original vertex " << imc << " linked to " << indexrec << " half vertex." << endmsg;
    link.push_back(indexrec);
  }

  for(int imc=0; imc<(int)vertfull.size(); imc++) {
    int count = std::count(link.begin(), link.end(), imc);
    if (count > 1 && msgLevel(MSG::DEBUG)) debug() << "linked twice to vertex " << imc << endmsg;
  }
}

//=============================================================================
//  printRat
//=============================================================================
void PVResMonitor::printRat(std::string mes, int a, int b) {

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

//=============================================================================
//  Get input tracks
//=============================================================================
bool PVResMonitor::getInputTracks( std::vector<LHCb::Track*>& vecOfTracks,  std::string& trackLoc) {


  std::string tracksName   = m_inputTracksName;

  if ( m_inputTracksName == "Offline" ) {
    tracksName = LHCb::TrackLocation::Default;
  } else if ( m_inputTracksName == "3D" ) {
    tracksName = LHCb::TrackLocation::Velo;
  } else if ( m_inputTracksName == "2D" ) {
    tracksName = LHCb::TrackLocation::RZVelo;
  }

  if ( tracksName == "none" ) {
    if(msgLevel(MSG::DEBUG)) debug() << " Tracks not specified " << tracksName << endmsg;
    return false;
  }

  trackLoc = tracksName;

  LHCb::Tracks* usedtracks;

  try {
    usedtracks   = get<LHCb::Tracks>( tracksName );
  } catch (...) {
    if(msgLevel(MSG::DEBUG)) debug() << " No tracks at " << tracksName << endmsg;
    return false;
  }

  std::vector<LHCb::Track*>::const_iterator itT;
  for ( itT = usedtracks->begin(); usedtracks->end() != itT; itT++ ) {
    LHCb::Track* ptr = (*itT);
    vecOfTracks.push_back(ptr);
  }

  return true;

}

//=============================================================================
//  Get input vertices
//=============================================================================
bool PVResMonitor::getInputVertices( std::vector<LHCb::RecVertex*>& vecOfVertices ) {

  std::string verticesName = m_inputVerticesName;

  if ( m_inputVerticesName == "Offline" ) {
    verticesName = LHCb::RecVertexLocation::Primary;
  } else if ( m_inputVerticesName == "3D" ) {
    verticesName =  LHCb::RecVertexLocation::Velo3D;
  } else if ( m_inputVerticesName == "2D" ) {
    verticesName = LHCb::RecVertexLocation::Velo2D;
  }

  if ( verticesName == "none" ) {
    if(msgLevel(MSG::DEBUG)) debug() << " Vertices not specified " << verticesName << endmsg;
    return false;
  }

  LHCb::RecVertices* recoVertices;

  try {
    recoVertices  = get<LHCb::RecVertices>( verticesName );
  } catch (...) {
    if(msgLevel(MSG::DEBUG)) debug() << " No vertices at " << verticesName << endmsg;
    return false;
  }

  std::vector<LHCb::RecVertex*>::const_iterator itVer;
  for ( itVer = recoVertices->begin(); recoVertices->end() != itVer; itVer++ ) {
    LHCb::RecVertex* ppv = (*itVer);
    vecOfVertices.push_back(ppv);
  }

  return true;

}
