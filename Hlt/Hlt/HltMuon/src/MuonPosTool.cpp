// $Id: MuonPosTool.cpp,v 1.2 2007-01-11 16:03:05 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "MuonPosTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonPosTool
//
// 2003-04-16 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MuonPosTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPosTool::MuonPosTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) 
    , m_stationNumber(0)
    , m_regionNumber(0)
    , m_DDS()
    ,m_debug(false){
  declareInterface<IMuonPosTool>(this);
  return ;
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StatusCode MuonPosTool::initialize(){
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;

  debug() << "Initialising the muon get info tool" << endreq;

  //  std::string numreg[] = {"1","2","3","4"};
  //  std::string numsta[] = {"1","2","3","4","5"};
  //  int chGridX[] ={1, 1, 1, 2};
  //  int chGridY[] ={1, 2, 4, 8};  
  
  m_muonDetector=getDet<DeMuonDetector>
	("/dd/Structure/LHCb/DownstreamRegion/Muon");

  std::string geoBase="/dd/Structure/LHCb/Muon/";

  /// get tile tool

  // Locate the detector service needed by the this tool
  m_DDS = svc<IDataProviderSvc>("DetectorDataSvc", true);
  if( !m_DDS ) {
    fatal() << "    Unable to locate DetectorDataSvc" << endreq;
    return StatusCode::FAILURE;
  }

  MuonBasicGeometry basegeometry(m_DDS,msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();

  m_padGridX.push_back(24);
  m_padGridX.push_back(48);
  m_padGridX.push_back(48);
  m_padGridX.push_back(12);
  m_padGridX.push_back(12);
  m_padGridY.push_back(8);
  m_padGridY.push_back(8);
  m_padGridY.push_back(8);
  m_padGridY.push_back(8);  
  m_padGridY.push_back(8);

  m_padSizeX.resize(20);
  m_padSizeY.resize(20);
  

  for (int i=0;i<5;i++){
    int channels=48*(m_padGridX[i]*m_padGridY[i]);
    
    m_xpos[i].resize(channels);
    m_ypos[i].resize(channels);
    m_zpos[i].resize(channels);    
  }

  LHCb::MuonTileID tile;
  int index=0;
  double xp,dx,yp,dy,zp,dz;
  for(int station=0;station<m_stationNumber;station++){    
    MuonLayout lay(m_padGridX[station],m_padGridY[station]);
    tile.setStation(station);
    tile.setLayout(lay);    
    index=0;
    
    for(int region=0;region<m_regionNumber;region++){
      tile.setRegion(region);
      for(int quarter=0;quarter<4;quarter++){        
        tile.setQuarter(quarter);
        for(unsigned y=0;y<m_padGridY[station];y++){         
          for (unsigned x=m_padGridX[station];x<2*m_padGridX[station];x++){
            tile.setX(x);
            tile.setY(y);   
            // const MuonTileID tt(tile);
            
            StatusCode sc =  m_muonDetector->
            Tile2XYZ(tile,xp,dx,yp,dy,zp,dz);
//m_tileTool->calcTilePos(tile,xp,dx,yp,dy,zp,dz);
            if (!sc) return StatusCode::FAILURE ;
            (m_xpos[station])[index]=xp;
            (m_ypos[station])[index]=yp;
            (m_zpos[station])[index]=zp;
            m_padSizeX[station*4+region]=dx;
            m_padSizeY[station*4+region]=dy;
             
                       
            
            if(station==4&&region==3&&quarter==0){
              //             log<<MSG::INFO<<index<<" "<<tile.nX()<<" "<<tile.nY()<< " "<<xp<<" "<<yp<<endreq;
              
            }
            
            index++;            
          }          
        }
        for(unsigned y=m_padGridY[station];y<2*m_padGridY[station];y++){         
          for (unsigned x=0;x<2*m_padGridX[station];x++){
            tile.setX(x);
            tile.setY(y);            
            m_muonDetector-> Tile2XYZ(tile,xp,dx,yp,dy,zp,dz);
            (m_xpos[station])[index]=xp;
            (m_ypos[station])[index]=yp;
            (m_zpos[station])[index]=zp;
            
            index++;            
          }          
        }        
      }      
    }    
  }
  return StatusCode::SUCCESS ;
}


StatusCode MuonPosTool::finalize() {
  if( m_DDS )      m_DDS->release();
  //if( m_tileTool ) toolSvc()->releaseTool( m_tileTool ); 
  return GaudiTool::finalize() ;
}


StatusCode MuonPosTool::calcTilePos(const LHCb::MuonTileID& tile, 
                                    double& x, double& deltax,
                                    double& y, double& deltay,
                                    double& z, double& deltaz)
{
     
  int station=tile.station();
  int region=tile.region();
  int quarter=tile.quarter();
  int tillNow=0;
  int perQuarter=3*m_padGridX[station]*m_padGridY[station];
  tillNow=(region*4+quarter)*perQuarter;
  unsigned int xpad=tile.nX();
  unsigned int ypad=tile.nY();
  unsigned int index=tillNow;     
     
  if(ypad<m_padGridY[station]){
    index=index+m_padGridX[station]*ypad+xpad-m_padGridX[station];
       
  }else{
    index=index+m_padGridX[station]*m_padGridY[station]+
      2*m_padGridX[station]*(ypad-m_padGridY[station])+xpad;
  }
     
  x= (m_xpos[station])[index];
  y= (m_ypos[station])[index];
  z= (m_zpos[station])[index];
  deltax=m_padSizeX[station*4+region];
  deltay=m_padSizeY[station*4+region];
  

  return StatusCode::SUCCESS ;

}



//=============================================================================
