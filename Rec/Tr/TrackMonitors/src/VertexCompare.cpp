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
//#include <TRandom.h>

// local
#include "VertexCompare.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexCompare
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VertexCompare)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexCompare::VertexCompare(const std::string& name,
                 ISvcLocator* pSvcLocator)
  : GaudiTupleAlg (name,pSvcLocator), m_forcedBtool(0)
{
//     declareProperty("nTracksToBeRecble",  m_nTracksToBeRecble = 5);
    declareProperty("produceNtuple",      m_produceNtuple     = true);
    declareProperty("produceHistogram",   m_produceHistogram  = true);
//     declareProperty("dzIsolated",         m_dzIsolated        = 10.0 * Gaudi::Units::mm);
//     declareProperty("inputTracksName",    m_inputTracksName   = LHCb::TrackLocation::Default);
    declareProperty("inputVerticesName1",  m_inputVerticesName1 = LHCb::RecVertexLocation::Primary);
    declareProperty("inputVerticesName2",  m_inputVerticesName2 = LHCb::RecVertexLocation::Primary);
}
//=============================================================================
// Destructor
//=============================================================================
VertexCompare::~VertexCompare() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VertexCompare::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // Must be executed first
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endmsg;
    return StatusCode::FAILURE;
  }

  // Access PVOfflineTool
//   m_pvsfit = tool<IPVOfflineTool>("PVOfflineTool",this);
//   if(!m_pvsfit) {
//     err() << "Unable to retrieve the PVOfflineTool" << endmsg;
//     return  StatusCode::FAILURE;
//   }

  m_nevt      = 0;
  m_nVtx      = 0;
  m_nPartVtx  = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VertexCompare::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  std::vector<LHCb::RecVertex*> vecOfVertices1;
  std::vector<LHCb::RecVertex*> vecOfVertices2;

  bool vertices_ok = getInputVertices(vecOfVertices1, vecOfVertices2);

  std::vector<LHCb::RecVertex> fullVrt1, fullVrt2;

  if (!vertices_ok) {
    return StatusCode::SUCCESS; // return SUCCESS anyway
  }
  if (debugLevel()) debug() << 
     "  Vtx Properities       x       y       z      chi2/ndof     ntracks" 
                            << endmsg;
  // Fill reconstructed PV info
  std::vector<LHCb::RecVertex*>::iterator itRecV1;
  for(itRecV1 = vecOfVertices1.begin(); vecOfVertices1.end() != itRecV1;
               itRecV1++) {
    LHCb::RecVertex* pv;
    pv = *itRecV1;
    fullVrt1.push_back(*pv);
//     int nTracks      = pv->tracks().size();
    if (debugLevel()) debug() << m_inputVerticesName1 << endmsg;
    if (debugLevel()) debug() << "              " << pv->position().x() << "   " 
                              << pv->position().y() << "   " << pv->position().z()  
                              << "   " << pv->chi2PerDoF()  << "   " << pv->tracks().size() << endmsg;
  } // end of loop over vertices1

  std::vector<LHCb::RecVertex*>::iterator itRecV2;
  for(itRecV2 = vecOfVertices2.begin(); vecOfVertices2.end() != itRecV2;
               itRecV2++) {
    LHCb::RecVertex* pv;
    pv = *itRecV2;
    fullVrt2.push_back(*pv);
//     int nTracks      = pv->tracks().size();
    if (debugLevel()) debug() << m_inputVerticesName2 << endmsg;
    if (debugLevel()) debug() << "              " 
                              << pv->position().x() << "   " 
                              << pv->position().y() << "   " 
                              << pv->position().z()  << "   " 
                              << pv->chi2PerDoF()  << "   " 
                              << pv->tracks().size() << endmsg;
  } // end of loop over vertices2
  if (debugLevel()) debug() << "fullVrt1 size   " << fullVrt1.size() << endmsg;
  if (debugLevel()) debug() << "fullVrt2 size   " << fullVrt2.size() << endmsg;
  int size_diff = fullVrt1.size() - fullVrt2.size();
  if(m_produceHistogram) {
    plot(double(size_diff), 1001, "size_diff", -5.5, 5.5, 11);
  }
  if(m_produceNtuple) {
    Tuple myTuple_evt = nTuple(101, "PV_nTuple_evt", CLID_ColumnWiseTuple);
    myTuple_evt->column("size_diff",  double(size_diff));
    myTuple_evt->column("size_1",  double(fullVrt1.size()));
    myTuple_evt->column("size_2",  double(fullVrt2.size()));
    myTuple_evt->write();
  }
  double dx          = -99999.;
  double dy          = -99999.;
  double dz          = -99999.;
  double errx        = -99999.;
  double erry        = -99999.;
  double errz        = -99999.;
  double covxx1      = -99999.;
  double covyy1      = -99999.;
  double covzz1      = -99999.;
  double covxy1      = -99999.;
  double covxz1      = -99999.;  
  double covyz1      = -99999.;

  double covxx2      = -99999.;
  double covyy2      = -99999.;
  double covzz2      = -99999.;
  double covxy2      = -99999.;
  double covxz2      = -99999.;  
  double covyz2      = -99999.;

  int ntracks_part  = 0;
  int ntracks_part2 = 0;
  int dtracks = 0;

  std::vector<LHCb::RecVertex>::iterator fullIter;
  int oIt=0;
  std::vector<int> link;
  if (fullVrt1.size()!=0 && fullVrt2.size()!=0) matchByDistance(fullVrt1, fullVrt2, link);
  else return StatusCode::SUCCESS;
  for (fullIter=fullVrt1.begin(); fullIter!=fullVrt1.end(); fullIter++){
    LHCb::RecVertex vrtf = *fullIter;
    m_nVtx++;
    if (link.at(oIt)==-1) continue;
    Gaudi::SymMatrix3x3 covPV_part1 = vrtf.covMatrix();
    double sigx_part1 = (covPV_part1(0,0));
    double sigy_part1 = (covPV_part1(1,1));
    double sigz_part1 = (covPV_part1(2,2));
    covxx1 = sigx_part1;
    covyy1 = sigy_part1;
    covzz1 = sigz_part1;
    covxy1 = (covPV_part1(0,1));
    covxz1 = (covPV_part1(0,2));
    covyz1 = (covPV_part1(1,2));
    

    Gaudi::SymMatrix3x3 covPV_part2 = fullVrt2.at(link.at(oIt)).covMatrix();
    double sigx_part2 = (covPV_part2(0,0));
    double sigy_part2 = (covPV_part2(1,1));
    double sigz_part2 = (covPV_part2(2,2));

    covxx2 = sigx_part2;
    covyy2 = sigy_part2;
    covzz2 = sigz_part2;
    covxy2 = (covPV_part2(0,1));
    covxz2 = (covPV_part2(0,2));
    covyz2 = (covPV_part2(1,2));
    
    double x1 = vrtf.position().x();
    double y1 = vrtf.position().y();
    double z1 = vrtf.position().z();
    
    double x2 = fullVrt2.at(link.at(oIt)).position().x();
    double y2 = fullVrt2.at(link.at(oIt)).position().y();
    double z2 = fullVrt2.at(link.at(oIt)).position().z();
                
    dx = vrtf.position().x()-fullVrt2.at(link.at(oIt)).position().x();
    dy = vrtf.position().y()-fullVrt2.at(link.at(oIt)).position().y();
    dz = vrtf.position().z()-fullVrt2.at(link.at(oIt)).position().z();

    errx = sqrt(sigx_part1 + sigx_part2);
    erry = sqrt(sigy_part1 + sigy_part2);
    errz = sqrt(sigz_part1 + sigz_part2);
    ntracks_part  = vrtf.tracks().size();
    ntracks_part2 = fullVrt2.at(link.at(oIt)).tracks().size();
    dtracks = ntracks_part - ntracks_part2;

    if(m_produceHistogram) {
      plot(dx, 1021, "dx", -0.25, 0.25, 50);
      plot(dy, 1022, "dy", -0.25, 0.25, 50);
      plot(dz, 1023, "dz", -1.5, 1.5, 60);
      plot(x1, 2021, "x1", -0.25, 0.25, 50);
      plot(y1, 2022, "y1", -0.25, 0.25, 50);
      plot(z1, 2023, "z1", -100., 100., 50);
      plot(x2, 3021, "x2", -0.25, 0.25, 50);
      plot(y2, 3022, "y2", -0.25, 0.25, 50);
      plot(z2, 3023, "z2", -100., 100., 50);
      plot(std::sqrt(sigx_part1), 4011, "x err 1",0., .1, 50);
      plot(std::sqrt(sigy_part1), 4012, "y err 1",0., .1, 50);
      plot(std::sqrt(sigz_part1), 4013, "z err 1",0., .5, 50);
      plot(std::sqrt(sigx_part2), 4021, "x err 2",0., .1, 50);
      plot(std::sqrt(sigy_part2), 4022, "y err 2",0., .1, 50);
      plot(std::sqrt(sigz_part2), 4023, "z err 2",0., .5, 50);
      plot(dx/errx, 1031, "pullx", -5., 5., 50);
      plot(dy/erry, 1032, "pully", -5., 5., 50);
      plot(dz/errz, 1033, "pullz", -5., 5., 50);
      plot(double(ntracks_part) , 1041, "ntracks_part1", 0., 150., 50);
      plot(double(ntracks_part2), 1042, "ntracks_part2", 0., 150., 50);
      plot(double(dtracks), 1043, "dtracks", 0., 150., 50);
      // dz to get false vertex rate from data
      if ( vecOfVertices1.size() > 1 ) {
        for ( unsigned int i1 = 0; i1 < vecOfVertices1.size(); i1++ ) {
          for ( unsigned int i2 = 0; i2 < vecOfVertices1.size(); i2++ ) {
            if (i2 != i1) {  
              double vdz = vecOfVertices1[i1]->position().z() - vecOfVertices1[i2]->position().z();
                plot(vdz,1201,"dz vertices 1", -150.,150.,100);
            }
          }
        }        
      }
      if ( vecOfVertices2.size() > 1 ) {
        for ( unsigned int i1 = 0; i1 < vecOfVertices2.size(); i1++ ) {
          for ( unsigned int i2 = 0; i2 < vecOfVertices2.size(); i2++ ) {
            if (i2 != i1) {  
              double vdz = vecOfVertices2[i1]->position().z() - vecOfVertices2[i2]->position().z();
              plot(vdz, 1202, "dz vertices 2", -150.,150.,100);
            }
          }
        }        
      }

    }
    if(m_produceNtuple) {
      Tuple myTuple = nTuple(102, "PV_nTuple", CLID_ColumnWiseTuple);
      myTuple->column("ntracks_part",  double(ntracks_part));
      myTuple->column("ntracks_part2",  double(ntracks_part2));
      myTuple->column("dtracks", double(dtracks));
      myTuple->column("dx",       dx);
      myTuple->column("dy",       dy);
      myTuple->column("dz",       dz);
      myTuple->column("x1",       x1);
      myTuple->column("y1",       y1);
      myTuple->column("z1",       z1);
      myTuple->column("x2",       x2);
      myTuple->column("y2",       y2);
      myTuple->column("z2",       z2);
      myTuple->column("errx",     errx);
      myTuple->column("erry",     erry);
      myTuple->column("errz",     errz);
      myTuple->column("covxx1",   covxx1);
      myTuple->column("covyy1",   covyy1);
      myTuple->column("covzz1",   covzz1);
      myTuple->column("covxy1",   covxy1);
      myTuple->column("covxz1",   covxz1);
      myTuple->column("covyz1",   covyz1);
      myTuple->column("covxx2",   covxx2);
      myTuple->column("covyy2",   covyy2);
      myTuple->column("covzz2",   covzz2);
      myTuple->column("covxy2",   covxy2);
      myTuple->column("covxz2",   covxz2);
      myTuple->column("covyz2",   covyz2);

      myTuple->write();
    }
    oIt++;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VertexCompare::finalize() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;


  info() << " ============================================" << endmsg;
  info() << " Efficiencies for reconstructed vertices:    " << endmsg;
  info() << " ============================================" << endmsg;
  info() << " " << endmsg;

  info() << " There are " << m_nVtx
         << " pairs of vertices in processed events" << endmsg;
//   info() << " PV is isolated if dz to closest reconstructible MC PV >  "
//          << m_dzIsolated << " mm" << endmsg;
//   std::string ff = "by counting tracks";
// /*  if ( !m_matchByTracks )*/ ff = "by dz distance";
//   info() << " Two splited vertices matched:  "
//          <<  ff << endmsg;


//   info() << " " << endmsg;
//     printRat("All",       m_nPartVtx,       m_nVtx );

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
void  VertexCompare::matchByDistance(std::vector<LHCb::RecVertex>& vertfull, 
                                     std::vector<LHCb::RecVertex>& verthalf, std::vector<int>& link) {

  if (verthalf.size()>vertfull.size() && debugLevel()) debug() << "half.size > full.size" << endmsg;
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
    if(debugLevel()) debug() << "original vertex " << imc << " linked to " << indexrec << " half vertex." << endmsg;
    link.push_back(indexrec);
  }

  for(int imc=0; imc<(int)vertfull.size(); imc++) {
    int count = std::count(link.begin(), link.end(), imc);
    if (count >1 && debugLevel()) debug() << "linked twice to vertex " << imc << endmsg;
  }
}

//=============================================================================
//  printRat
//=============================================================================
void VertexCompare::printRat(std::string mes, int a, int b) {

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
//  Get input vertices
//=============================================================================
bool VertexCompare::getInputVertices( std::vector<LHCb::RecVertex*>& vecOfVertices1, 
                                      std::vector<LHCb::RecVertex*>& vecOfVertices2 ) {

  std::string verticesName1 = m_inputVerticesName1;
  std::string verticesName2 = m_inputVerticesName2;
  
  
  if ( m_inputVerticesName1 == "Offline" ) {
    verticesName1 = LHCb::RecVertexLocation::Primary;
  } else if ( m_inputVerticesName1 == "3D" ) {
    verticesName1 =  LHCb::RecVertexLocation::Velo3D;
  } else if ( m_inputVerticesName1 == "2D" ) {
    verticesName1 = LHCb::RecVertexLocation::Velo2D;
  }
  
  if ( m_inputVerticesName2 == "Offline" ) {
    verticesName2 = LHCb::RecVertexLocation::Primary;
  } else if ( m_inputVerticesName2 == "3D" ) {
    verticesName2 =  LHCb::RecVertexLocation::Velo3D;
  } else if ( m_inputVerticesName2 == "2D" ) {
    verticesName2 = LHCb::RecVertexLocation::Velo2D;
  }

  if ( verticesName1 == "none" ) {
    debug() << " Vertices 1 not specified " << verticesName1 << endmsg;
    return false;
  }
    if ( verticesName2 == "none" ) {
    debug() << " Vertices 2 not specified " << verticesName2 << endmsg;
    return false;
  }

  LHCb::RecVertices* recoVertices1;
  LHCb::RecVertices* recoVertices2;

  try {
    recoVertices1  = get<LHCb::RecVertices>( verticesName1 );
  } catch (...) {
    debug() << " No vertices at " << verticesName1 << endmsg;
    return false;
  }

  try {
    recoVertices2  = get<LHCb::RecVertices>( verticesName2 );
  } catch (...) {
    debug() << " No vertices at " << verticesName2 << endmsg;
    return false;
  }
  
  std::vector<LHCb::RecVertex*>::const_iterator itVer1;
  for ( itVer1 = recoVertices1->begin(); recoVertices1->end() != itVer1; itVer1++ ) {
    LHCb::RecVertex* ppv1 = (*itVer1);
    vecOfVertices1.push_back(ppv1);
  }
  
  std::vector<LHCb::RecVertex*>::const_iterator itVer2;
  for ( itVer2 = recoVertices2->begin(); recoVertices2->end() != itVer2; itVer2++ ) {
    LHCb::RecVertex* ppv2 = (*itVer2);
    vecOfVertices2.push_back(ppv2);
  }

  return true;

}
