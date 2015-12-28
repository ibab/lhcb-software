// Include files

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
// local
#include "MuonClusterRec.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonInterfaces/MuonHit.h"
#include "MuonDet/IMuonFastPosTool.h"
#include "MuonDet/DeMuonDetector.h"
using namespace LHCb;
using namespace std;
 

namespace {
int regX(LHCb::MuonTileID tile) {
  return ( (tile.quarter() > 1 ? -1 : 1) * tile.nX() );
}
int regY(LHCb::MuonTileID tile) {
  return ( ((tile.quarter() > 0 && tile.quarter()< 3) ? -1 : 1) * tile.nY() );
}
}
//-----------------------------------------------------------------------------
// Implementation file for class : MuonClusterRec
//
// clustering tool for the muon detector
// 2009-10-15 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonClusterRec )

MuonClusterRec::MuonClusterRec( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : base_class( type, name , parent )
{
  declareInterface<IMuonClusterRec>(this);
  declareProperty( "PosTool"          , m_posToolName = "MuonDetPosTool");
  declareProperty( "MaxPadsPerStation" , m_maxPadsPerStation = 1500);
}

MuonClusterRec::~MuonClusterRec() {
  clear();
}



StatusCode MuonClusterRec::initialize () {
  StatusCode sc = GaudiTool::initialize() ;

  if (!sc) return sc;
  m_muonDetector = getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    err()<<"error retrieving the Muon detector element "<<endmsg;
    return StatusCode::FAILURE;
  }
  m_posTool = tool<IMuonFastPosTool>(m_posToolName);
  if(!m_posTool) {
    error()<<"error retrieving the muon position tool "<<endmsg;
    return StatusCode::FAILURE;
  }

  incSvc()->addListener( this, IncidentType::EndEvent );
  return sc;
}

void MuonClusterRec::handle ( const Incident& incident )
{
  if ( IncidentType::EndEvent == incident.type() ) {
    verbose() << "End Event: clear everything"<<endmsg;
    clear() ;
  }
}

void MuonClusterRec::clear() {
  for(auto& h : m_clusters)
    delete h; // delete all the allocated MuonHit's
  m_clusters.clear();
  m_clustersDone=false;
}


StatusCode MuonClusterRec::finalize ()
{
  debug()<<" MuonClusterRec::finalize"<<endmsg;
  return   GaudiTool::finalize() ;
}


const std::vector<MuonHit*>* MuonClusterRec::clusters(const std::vector<MuonLogPad*>* pads,
                                                      bool force) {

  const int factor[5] = {3,1,1,1,1};	
  if( (m_clustersDone == false || force==true) && pads) {
    int nhits=0;
    std::map<MuonLogPad*,bool> usedPad;
    bool searchNeighbours=true;

    clear();


    //group pads by station
    debug() << "Log. pads before clustering:"<<endmsg;
    //make it a vector of vectors, to allow dynamic sizing
    std::vector< std::vector<MuonLogPad*> > stationPads=
      std::vector< std::vector<MuonLogPad*> >(m_muonDetector->stations(),std::vector<MuonLogPad*>(0));
    for (auto& isP : stationPads) isP.reserve(pads->size());

    if (msgLevel(MSG::DEBUG)) {
        for (const auto& pad : *pads) {
          if(! pad->truepad() ) continue;
          debug() << "LOGPAD Q"<<(pad->tile().quarter()+1)<<
            "M"<< (pad->tile().station()+1) << "R"<<(pad->tile().region()+1) <<
            " nX="<< pad->tile().nX() << " nY="<< pad->tile().nY() <<
            " time="<<pad->time() << " +/-" <<pad->dtime() << endmsg;
        }
    }
    for (const auto& pad : *pads) {
      if(pad->truepad() ) stationPads[ pad->tile().station() ].push_back(pad);
    }


    for (int station =0 ; station<m_muonDetector->stations() ; station++) {
      if (stationPads[station].size() > factor[station]*m_maxPadsPerStation) {
        info() << "skipping station M"<<station+1<<" with too many pads:"
               << stationPads[station].size() <<endmsg;
        continue;
      }
      for (auto ipad = stationPads[station].begin(); ipad != stationPads[station].end(); ipad++ )
      {
        if (! usedPad.count(*ipad))
        {
          // cluster seed
          usedPad[*ipad]=true;
          m_clusters.push_back(new MuonHit(m_muonDetector, *ipad,
                                           m_posTool ));
          MuonHit *muon_Hit = m_clusters.back();
          // store a progressive hit number for debugging purposes
          StatusCode sc = muon_Hit->setHitID(++nhits);
          // now search for adjacent pads
          searchNeighbours=true;
          while (searchNeighbours)
          {
            searchNeighbours=false;
            for (auto jpad = ipad+1; jpad != stationPads[station].end(); jpad++ )
            {
              if( usedPad.count(*jpad)) continue;
              bool takeit=false;
              int deltaRegion = abs((int)((*ipad)->tile().region()-(*jpad)->tile().region()));
              if (deltaRegion > 1) continue;
              if (deltaRegion == 0) { //same region: use logical position
                const std::vector<MuonLogPad*>* clPads = muon_Hit->logPads();
                for (auto clpad = clPads->begin() ; clpad != clPads->end() ; clpad++)
                {
                  if ((*clpad)->tile().region() != (*jpad)->tile().region() ) continue;
                  int deltaX = abs(regX((*clpad)->tile())-regX((*jpad)->tile()));
                  int deltaY = abs(regY((*clpad)->tile())-regY((*jpad)->tile()));
                  if ( (deltaX == 0 && deltaY == 1) ||
                       (deltaX == 1 && deltaY == 0) ) {
                    takeit=true;
                    break;
                  }
                }
              } else { // adjacent regions: use absolute position
                double x,dx,y,dy,z,dz;
                m_posTool->calcTilePos((*jpad)->tile(),x,dx,y,dy,z,dz);

                bool Xinside = ( x > muon_Hit->minX() && x < muon_Hit->maxX());
                bool Xadj = ( (x > muon_Hit->maxX() && x-dx-muon_Hit->maxX() < dx/2) ||
                              (x < muon_Hit->minX() && muon_Hit->minX()-x-dx  < dx/2) );
                bool Yinside = ( y > muon_Hit->minY() && y < muon_Hit->maxY());
                bool Yadj = ( (y > muon_Hit->maxY() && y-dy-muon_Hit->maxY() < dy/2) ||
                              (y < muon_Hit->minY() && muon_Hit->minY()-y-dy  < dy/2) );
                if ( (Xinside || Xadj) && (Yinside || Yadj) ) takeit=true;
              }
              if (takeit) {  // it's a neighbour, add it to the cluster
                usedPad[*jpad]=true;
                muon_Hit->addPad(*jpad);
                searchNeighbours=true; // we exit the loop and restart from ipad+1 ith the larger cluster
                break;
              }
            }
          } // end of neighbour search
        }// end of unused pad request
      }// end of loop on log. pads of a given station
    }// end of loop on stations
    m_clustersDone = true;
    if (msgLevel(MSG::DEBUG)) {
      debug() << "\n OBTAINED CLUSTERS:"<<endmsg;
      for (const auto& c : m_clusters) {
        debug() << "Cluster #"<<c->hitID()<<" in M"<<(c->station()+1)<<
          " with "<<c->npads()<<" pads   X="<<c->minX() << " - "<<c->maxX()
                << "  Y="<<c->minY() << " - "<<c->maxY() <<
          " first tile has Nx/Ny="<< c->tile().nX() << "/" << c->tile().nY() <<endmsg;
      }
    }
  }// end of clustering algorithm

  return &m_clusters;
}
