// $Id: MuonPosTool.cpp,v 1.2 2005-02-16 10:01:55 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "MuonPosTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonPosTool
//
// 2003-04-16 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MuonPosTool>          s_factory ;
const        IToolFactory& MuonPosToolFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPosTool::MuonPosTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : AlgTool ( type, name , parent ) {
  declareInterface<IMuonPosTool>(this);

  MsgStream log(msgSvc(), "MuonPosTool");
  log << MSG::DEBUG << "Initialising the muon get info tool" << endreq;


  std::string numreg[] = {"1","2","3","4"};
  std::string numsta[] = {"1","2","3","4","5"};
  int chGridX[] ={1, 1, 1, 2};
  int chGridY[] ={1, 2, 4, 8};  
  std::string geoBase="/dd/Structure/LHCb/Muon/";

  /// get tile tool
  StatusCode sc =
    toolSvc()->retrieveTool("MuonTileIDXYZ", m_tileTool);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create MuonTileIDToXYZ tool" << endreq;
    throw GaudiException (" MuonTileIDXYZ not found ",
                          "MuonPosToolException",StatusCode::FAILURE);
  }
  //  sc=toolSvc()->retrieveTool("MuonGetInfoTool",m_pGetInfo);
  // if(sc.isFailure())return StatusCode::FAILURE;

  // Locate the detector service needed by the this tool
  m_DDS = 0;
  sc = service( "DetectorDataSvc", m_DDS, true );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate DetectorDataSvc" << endreq;
    throw GaudiException (" DetectorDataService not found ",
                          "MuonPosToolException",StatusCode::FAILURE);

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

  for (int i=0;i<5;i++){
    int channels=48*(m_padGridX[i]*m_padGridY[i]);
    
    m_xpos[i].resize(channels);
    m_ypos[i].resize(channels);
    m_zpos[i].resize(channels);    
  }

  MuonTileID tile;
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
        for(int y=0;y<m_padGridY[station];y++){         
          for (int x=m_padGridX[station];x<2*m_padGridX[station];x++){
            tile.setX(x);
            tile.setY(y);   
            // const MuonTileID tt(tile);
            
            StatusCode sc =
              m_tileTool->calcTilePos(tile,xp,dx,yp,dy,zp,dz);
            (m_xpos[station])[index]=xp;
            (m_ypos[station])[index]=yp;
            (m_zpos[station])[index]=zp;
            if(station==4&&region==3&&quarter==0){
              //             log<<MSG::INFO<<index<<" "<<tile.nX()<<" "<<tile.nY()<< " "<<xp<<" "<<yp<<endreq;
              
            }
            
            index++;            
          }          
        }
        for(int y=m_padGridY[station];y<2*m_padGridY[station];y++){         
          for (int x=0;x<2*m_padGridX[station];x++){
            tile.setX(x);
            tile.setY(y);            
            m_tileTool->calcTilePos(tile,xp,dx,yp,dy,zp,dz);
            (m_xpos[station])[index]=xp;
            (m_ypos[station])[index]=yp;
            (m_zpos[station])[index]=zp;
            
            index++;            
          }          
        }        
      }      
    }    
  }
}


MuonPosTool::~MuonPosTool() {
  if( m_DDS )      m_DDS->release();
  if( m_tileTool ) toolSvc()->releaseTool( m_tileTool ); 
}


StatusCode MuonPosTool::calcTilePos(const MuonTileID& tile, 
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

  return StatusCode::SUCCESS ;

}



//=============================================================================
