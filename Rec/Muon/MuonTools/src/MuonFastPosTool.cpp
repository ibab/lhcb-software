// Include files 
#include "boost/assign/list_of.hpp"

// local
#include "MuonFastPosTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonFastPosTool
//
// 2003-04-16 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MuonFastPosTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MuonFastPosTool::MuonFastPosTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : GaudiTool ( type, name, parent )
    , m_stationNumber(0)
    , m_regionNumber(0)
{
  declareInterface<IMuonFastPosTool>(this);
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StatusCode MuonFastPosTool::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if (sc.isFailure()) return sc;

  debug() << "Initialising the muon get info tool" << endmsg;

  m_muonDetector=getDet<DeMuonDetector>
    ("/dd/Structure/LHCb/DownstreamRegion/Muon");

  /// get tile tool
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();

  std::vector<unsigned int> tmp1 = boost::assign::list_of(24)(48)(48)(12)(12);
  std::vector<unsigned int> tmp2 = boost::assign::list_of( 8)( 8)( 8)( 8)( 8);
  m_padGridX = tmp1;
  m_padGridY = tmp2;

  m_padSizeX.resize(20);
  m_padSizeY.resize(20);

  for (int i=0;i<5;i++){
    int channels=48*(m_padGridX[i]*m_padGridY[i]);
    m_pos[i].resize(channels);
  }

  LHCb::MuonTileID tile;
  for(int station=0;station<m_stationNumber;station++){
    tile.setStation(station);
    tile.setLayout( MuonLayout(m_padGridX[station],m_padGridY[station]) );
    int index=0;
    for(int region=0;region<m_regionNumber;region++){
      tile.setRegion(region);
      for(int quarter=0;quarter<4;quarter++){
        tile.setQuarter(quarter);
        for(unsigned y=0;y<m_padGridY[station];y++){
          tile.setY(y);
          for (unsigned x=m_padGridX[station];x<2*m_padGridX[station];x++){
            tile.setX(x);

            double xp,dx,yp,dy,zp,dz;
            sc =  m_muonDetector->Tile2XYZ(tile,xp,dx,yp,dy,zp,dz);
            if (sc.isFailure() ) return sc;
            (m_pos[station])[index++]=Gaudi::XYZPoint(xp,yp,zp);
            m_padSizeX[station*4+region]=(float) dx;
            m_padSizeY[station*4+region]=(float) dy;
          }
        }
        for(unsigned y=m_padGridY[station];y<2*m_padGridY[station];y++){
          tile.setY(y);
          for (unsigned x=0;x<2*m_padGridX[station];x++){
            tile.setX(x);
            double xp,dx,yp,dy,zp,dz;
            sc = m_muonDetector->Tile2XYZ(tile,xp,dx,yp,dy,zp,dz);
            if (sc.isFailure()) return sc;
            (m_pos[station])[index++]=Gaudi::XYZPoint(xp,yp,zp);
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS ;
}



StatusCode MuonFastPosTool::calcTilePos(const LHCb::MuonTileID& tile,
                                        double& x, double& deltax,
                                        double& y, double& deltay,
                                        double& z, double& /*deltaz*/)
{

  int station=tile.station();
  int region=tile.region();
  int quarter=tile.quarter();
  int perQuarter=3*m_padGridX[station]*m_padGridY[station];
  unsigned int xpad=tile.nX();
  unsigned int ypad=tile.nY();
  unsigned int index=(region*4+quarter)*perQuarter;

  if(ypad<m_padGridY[station]){
    index=index+m_padGridX[station]*ypad+xpad-m_padGridX[station];
  }else{
    index=index+m_padGridX[station]*m_padGridY[station]+
      2*m_padGridX[station]*(ypad-m_padGridY[station])+xpad;
  }

  const Gaudi::XYZPoint& p = m_pos[station][index];
  x = p.x();
  y = p.y();
  z = p.z();
  deltax = m_padSizeX[station*4+region];
  deltay = m_padSizeY[station*4+region];

  return StatusCode::SUCCESS ;
}
//=============================================================================
