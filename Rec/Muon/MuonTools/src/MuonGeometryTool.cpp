// $Id: MuonGeometryTool.cpp,v 1.4 2002-08-05 12:53:04 dhcroft Exp $
// Include files
#include <cmath>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartDataPtr.h"

// from MuonDet
#include "MuonDet/DeMuonRegion.h"

// local
#include "MuonGeometryTool.h"
#include "MuonTools/MuonGeometry.h"
#include "MuonTools/IMuonTileXYZTool.h"

#include "MuonKernel/MuonTileID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonGeometryTool
//
// 29/04/2002 : David Hutchcroft
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MuonGeometryTool>          s_factory ;
const        IToolFactory& MuonGeometryToolFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonGeometryTool::MuonGeometryTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  declareInterface<IMuonGeometryTool>(this);

  MsgStream log(msgSvc(), "MuonGeometryTool");
  log << MSG::DEBUG << "Initialising the muon geometry tool" << endreq;

  /// get tile tool
  StatusCode sc =
    toolSvc()->retrieveTool("MuonTileIDXYZ", m_tileTool);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create MuonTileIDToXYZ tool" << endreq;
  }

  // Locate the detector service needed by the this tool
  m_DDS = 0;
  if( serviceLocator() ) {
    StatusCode sc=StatusCode::FAILURE;
    sc = serviceLocator()->service( "DetectorDataSvc", m_DDS, true );
  }

  // set station and region size to 0 until read from XML
  m_NStation = 0;
  m_NRegion = 0;

}
//=============================================================================

StatusCode MuonGeometryTool::nStation(int &NStation){

  if(0 == m_NStation){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return sc;
    }
  }
  NStation = m_NStation;
  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::nRegion(int &NRegion){
  if(0 == m_NRegion){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return sc;
    }
  }
  NRegion = m_NRegion;
  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::getStationBox(const int &station,
                                           double &deltaX,
                                           double &deltaY,
                                           double &z, double &deltaZ){
  if(0 ==  m_NStation){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return sc;
    }
  }

  if(0 > station || m_NStation <= station){
    return StatusCode::FAILURE;
  }

  if( m_regionExtent.empty() ){
    StatusCode sc = fillLocals();
    if(sc.isFailure()){
      return sc;
    }
  }

  // copy R4 from local arrays (outside only for a station)
  deltaX = regionExtent(station,3).outerDeltaX;
  deltaY = regionExtent(station,3).outerDeltaY;
  z = regionExtent(station,3).z;
  deltaZ = regionExtent(station,3).deltaZ;

  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::getRegionBox(const int &station,
                                          const int & region,
                                          double &outerDeltaX,
                                          double &innerDeltaX,
                                          double &outerDeltaY,
                                          double &innerDeltaY,
                                          double &z, double &deltaZ){
  if(0 ==  m_NStation){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return sc;
    }
  }

  if(0 > station || m_NStation <= station ||
     0 > region || m_NRegion <= region){
    return StatusCode::FAILURE;
  }

  if( m_regionExtent.empty() ){
    StatusCode sc = fillLocals();
    if(sc.isFailure()){
      return sc;
    }
  }

  // copy parameters from local arrays
  outerDeltaX = regionExtent(station,region).outerDeltaX;
  innerDeltaX = regionExtent(station,region).innerDeltaX;

  outerDeltaY = regionExtent(station,region).outerDeltaY;
  innerDeltaY = regionExtent(station,region).innerDeltaY;

  z = regionExtent(station,region).z;
  deltaZ = regionExtent(station,3).deltaZ;

  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::getPadSize(const int &station,
                                        const int &region,
                                        double &sizeX,
                                        double &sizeY){
  if(0 ==  m_NStation){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return sc;
    }
  }

  if(0 > station || m_NStation <= station ||
     0 > region || m_NRegion <= region){
    return StatusCode::FAILURE;
  }

  if( m_padExtent.empty() ){
    StatusCode sc = fillLocals();
    if(sc.isFailure()){
      return sc;
    }
  }

  // copy from local arrays
  sizeX = padExtent(station,region).sizeX;
  sizeY = padExtent(station,region).sizeY;

  return StatusCode::SUCCESS;
}

int MuonGeometryTool::logChanHorizGridX(const int &station,
                                        const int &region){
  if(m_logHorizGridX.empty()){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return 0;
    }
  }
  if( station < 0 || station >= m_NStation || 
      region < 0 || region >= m_NRegion ){
    return 0;
  }
  return m_logHorizGridX[station * m_NRegion + region];
}

int MuonGeometryTool::logChanHorizGridY(const int &station,
                                        const int &region){
  if(m_logHorizGridX.empty()){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return 0;
    }
  }
  if( station < 0 || station >= m_NStation || 
      region < 0 || region >= m_NRegion ){
    return 0;
  }
  return m_logHorizGridY[station * m_NRegion + region];
}

int MuonGeometryTool::logChanVertGridX(const int &station,
                                       const int &region){
  if(m_logHorizGridX.empty()){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return 0;
    }
  }
  if( station < 0 || station >= m_NStation || 
      region < 0 || region >= m_NRegion ){
    return 0;
  }
  return m_logVertGridX[station * m_NRegion + region];
}

int MuonGeometryTool::logChanVertGridY(const int &station,
                                       const int &region){
  if(m_logHorizGridX.empty()){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return 0;
    }
  }
  if( station < 0 || station >= m_NStation || 
      region < 0 || region >= m_NRegion ){
    return 0;
  }
  return m_logVertGridY[station * m_NRegion + region];
}

int MuonGeometryTool::padGridX(const int &station,
                               const int &region){
  if(m_logHorizGridX.empty()){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return 0;
    }
  }
  if( station < 0 || station >= m_NStation || 
      region < 0 || region >= m_NRegion ){
    return 0;
  }
  return m_padGridX[station * m_NRegion + region];
}

int MuonGeometryTool::padGridY(const int &station,
                                       const int &region){
  if(m_logHorizGridX.empty()){
    StatusCode sc = fillSystemNumbers();
    if(!sc){
      return 0;
    }
  }
  if( station < 0 || station >= m_NStation || 
      region < 0 || region >= m_NRegion ){
    return 0;
  }
  return m_padGridY[station * m_NRegion + region];
}

StatusCode MuonGeometryTool::fillLocals(){
  // this gets all of the wanted parameters from the MuonTileToXYZTool
  // basically this just knows the tiles to ask for

  m_regionExtent.resize(m_NStation*m_NRegion);
  m_padExtent.resize(m_NStation*m_NRegion);

  MuonLayout twelfthLayout(1,1); // always the same

  // First regions, need to get tiles for each corner of each region
  int station;
  for(station = 0 ; station < m_NStation ; station++){

    int region;
    for(region = 0 ; region < m_NRegion ; region++){

      double minX,maxX,minY,maxY; // take abs values here
      double minZ, maxZ; // "normal" averageing

      int twelfth;
      for(twelfth = 0; twelfth < 12 ; twelfth++){

        unsigned int quarter;
        unsigned int ix,iy;

        if(0==twelfth){
          quarter = 0;
          ix = 1;
          iy = 0;
        }else if(1==twelfth){
          quarter = 0;
          ix = 1;
          iy = 1;
        }else if(2==twelfth){
          quarter = 0;
          ix = 0;
          iy = 1;
        }else if(3==twelfth){
          quarter = 1;
          ix = 0;
          iy = 1;
        }else if(4==twelfth){
          quarter = 1;
          ix = 1;
          iy = 1;
        }else if(5==twelfth){
          quarter = 1;
          ix = 1;
          iy = 0;
        }else if(6==twelfth){
          quarter = 2;
          ix = 1;
          iy = 0;
        }else if(7==twelfth){
          quarter = 2;
          ix = 0;
          iy = 1;
        }else if(8==twelfth){
          quarter = 2;
          ix = 1;
          iy = 1;
        }else if(9==twelfth){
          quarter = 3;
          ix = 0;
          iy = 1;
        }else if(10==twelfth){
          quarter = 3;
          ix = 1;
          iy = 1;
        }else{
          quarter = 3;
          ix = 1;
          iy = 0;
        }

        MuonTileID tile(station,0,0,twelfthLayout,region,quarter,ix,iy);

        double x,Dx,y,Dy,z,Dz;
        StatusCode sc = m_tileTool->calcTilePos(tile,x,Dx,y,Dy,z,Dz);
        if(sc.isFailure()){
          return sc;
        }
        if(0 == twelfth){
          minX = fabs(x) - Dx;
          maxX = fabs(x) + Dx;
          minY = fabs(y) - Dy;
          maxY = fabs(y) + Dy;
          minZ = z - Dz;
          maxZ = z + Dz;
        } else {
          if(minX > fabs(x) - Dx) { minX = fabs(x) - Dx; }
          if(maxX < fabs(x) + Dx) { maxX = fabs(x) + Dx; }
          if(minY > fabs(y) - Dy) { minY = fabs(y) - Dy; }
          if(maxY < fabs(y) + Dy) { maxY = fabs(y) + Dy; }
          if(minZ > z - Dz) { minZ = fabs(z) - Dz; }
          if(maxZ < z + Dz) { maxZ = fabs(z) + Dz; }
        }

      } // loop over twelfths

      regionExtent(station,region).outerDeltaX = maxX;
      regionExtent(station,region).innerDeltaX = minX;
      regionExtent(station,region).outerDeltaY = maxY;
      regionExtent(station,region).innerDeltaY = minY;
      regionExtent(station,region).z = (maxZ + minZ) /2.0;
      regionExtent(station,region).deltaZ = (maxZ - minZ) /2.0;

      // get a pad in the region to find the size
      MuonLayout padLayout(padGridX(station,region),
                           padGridY(station,region));
      MuonTileID padTile(station,0,0,padLayout,region,0,
                         padGridX(station,region),
                         padGridY(station,region));

      double x,Dx,y,Dy,z,Dz;
      StatusCode sc = m_tileTool->calcTilePos(padTile,x,Dx,y,Dy,z,Dz);
      if(sc.isFailure()){
        return sc;
      }

      padExtent(station,region).sizeX = 2.*Dx;
      padExtent(station,region).sizeY = 2.*Dy;

    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::fillSystemNumbers(){

  MsgStream log(msgSvc(), "MuonGeometryTool");
  // TDS path to the Muon system  is of the form /dd/Structure/LHCb/Muon
  SmartDataPtr<DetectorElement> muonSystem(m_DDS,"/dd/Structure/LHCb/Muon");
  if(!muonSystem){
    log << MSG::ERROR << "Could not read /dd/Structure/LHCb/Muon from TDS"
        << endreq;
    return StatusCode::FAILURE;
  }

  // Childern of the Muon system detector element are the stations
  std::vector<IDetectorElement*> muonStations =
    muonSystem->childIDetectorElements();

  m_NStation = muonStations.size();

  // now get the regions for the first station
  std::vector<IDetectorElement*>::const_iterator iIDE = muonStations.begin();
  std::vector<IDetectorElement*> muonRegions =
    (*iIDE)->childIDetectorElements();

  m_NRegion = muonRegions.size();

  // set up some vectors
  m_logHorizGridX.resize(m_NStation*m_NRegion);
  m_logHorizGridY.resize(m_NStation*m_NRegion);
  m_logVertGridX.resize(m_NStation*m_NRegion);
  m_logVertGridY.resize(m_NStation*m_NRegion);
  m_padGridX.resize(m_NStation*m_NRegion);
  m_padGridY.resize(m_NStation*m_NRegion);

  // get the childeren of a station
  int station;
  for( iIDE = muonStations.begin(), station = 0 ;
       iIDE != muonStations.end() ;
       iIDE++, station++ ){

    // now get the regions for this station
    std::vector<IDetectorElement*> muonRegions =
      (*iIDE)->childIDetectorElements();

    // loop over the regions
    int region;
    std::vector<IDetectorElement*>::const_iterator iIDER;
    for( iIDER = muonRegions.begin(), region = 0 ;
         iIDER != muonRegions.end() ;
         iIDER++, region++ ){

      //get number of chambers in the region
      // now get the regions for this station
      std::vector<IDetectorElement*> muonChambers =
      (*iIDER)->childIDetectorElements();
      
      DeMuonRegion * pRegion = dynamic_cast<DeMuonRegion *>(*iIDER);
      if(!pRegion){
        log << MSG::ERROR << "Could not cast to DeMuonRegion *"
            << endreq;
        return StatusCode::FAILURE;
      }

      int nChambers = muonChambers.size();

      // fill the vectors:
      // first get FE channel size in number across a chamber
      int FEAnodeX = pRegion->FEAnodeX();
      int FEAnodeY = pRegion->FEAnodeY();
      int FECathodeX = pRegion->FECathodeX();
      int FECathodeY = pRegion->FECathodeY();

      // now scale across a quater region: type on number of chambers
      if( 12 == nChambers ){
        // nothing to do already correct
      }else if( 24 == nChambers ){
        // X is still fine Y need to be doubled
        FEAnodeY *= 2;
        FECathodeY *= 2;
      }else if( 48 == nChambers ){
        // X is still fine Y need to be quadrupled
        FEAnodeY *= 4;
        FECathodeY *= 4;
      }else if( 192 == nChambers ){
        // X need to be duobled Y*8
        FEAnodeX *= 2;
        FECathodeX *= 2;
        FEAnodeY *= 8;
        FECathodeY *= 8;
      }else{
        MsgStream log(msgSvc(), "MuonGeometryTool");
        log << MSG::FATAL
            << "The chamber number in a region is not 12,24,48 or 192 it was:"
            << nChambers << " the code can not handle this change to the "
            << "muon system geometry, please rewrite MuonGeometryTool before "
            << "continuing, thankyou for your understanding"
            << endreq;
        return StatusCode::FAILURE;
      }

      // now get the maps from FE to logical channels
      MuonParameters::ReadoutType type[2];
      int xmap[2];
      int ymap[2];

      int nMaps = pRegion->numberLogicalMap();

      int iMap;
      for( iMap = 0 ; iMap < nMaps ; iMap++ ){
        StatusCode sc = pRegion->logicalMap(iMap,type[iMap],
                                            xmap[iMap],ymap[iMap]);
        if(!sc){
          return sc;
        }
      }

      // OK this is the ugly bit: need to take cases.
      // First anode only
      if( 0 != FEAnodeX && 0 == FECathodeX ){
        // and only one map
        if( nMaps == 1 ){
          // so horiz, vert and pad all same
          m_logHorizGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
          m_logHorizGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];
          m_logVertGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
          m_logVertGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];
          m_padGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
          m_padGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];
        }else{
          // two maps to be crossed
          if(xmap[0] < xmap[1]){
            // xmap[0] is the vertical map, xmap[1] is the horizontal map
            m_logVertGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
            m_logVertGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];

            m_logHorizGridX[station*m_NRegion + region] = FEAnodeX / xmap[1];
            m_logHorizGridY[station*m_NRegion + region] = FEAnodeY / ymap[1];

            m_padGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
            m_padGridY[station*m_NRegion + region] = FEAnodeY / ymap[1];
          }else{
            // xmap[1] is the vertical map, xmap[0] is the horizontal map
            m_logVertGridX[station*m_NRegion + region] = FEAnodeX / xmap[1];
            m_logVertGridY[station*m_NRegion + region] = FEAnodeY / ymap[1];

            m_logHorizGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
            m_logHorizGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];

            m_padGridX[station*m_NRegion + region] = FEAnodeX / xmap[1];
            m_padGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];
          }
        }
      }

      // now cathode only
      if( 0 != FECathodeX && 0 == FEAnodeX ){
        // and only one map
        if( nMaps == 1 ){
          // so horiz, vert and pad all same
          m_logHorizGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
          m_logHorizGridY[station*m_NRegion + region] = FECathodeY / ymap[0];
          m_logVertGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
          m_logVertGridY[station*m_NRegion + region] = FECathodeY / ymap[0];
          m_padGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
          m_padGridY[station*m_NRegion + region] = FECathodeY / ymap[0];
        }else{
          // two maps to be crossed
          if(xmap[0] < xmap[1]){
            // xmap[0] is the vertical map, xmap[1] is the horizontal map
            m_logVertGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
            m_logVertGridY[station*m_NRegion + region] = FECathodeY / ymap[0];

            m_logHorizGridX[station*m_NRegion + region] = FECathodeX / xmap[1];
            m_logHorizGridY[station*m_NRegion + region] = FECathodeY / ymap[1];

            m_padGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
            m_padGridY[station*m_NRegion + region] = FECathodeY / ymap[1];
          }else{
            // xmap[1] is the vertical map, xmap[0] is the horizontal map
            m_logVertGridX[station*m_NRegion + region] = FECathodeX / xmap[1];
            m_logVertGridY[station*m_NRegion + region] = FECathodeY / ymap[1];

            m_logHorizGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
            m_logHorizGridY[station*m_NRegion + region] = FECathodeY / ymap[0];

            m_padGridX[station*m_NRegion + region] = FECathodeX / xmap[1];
            m_padGridY[station*m_NRegion + region] = FECathodeY / ymap[0];
          }
        }
      }

      // now "both" readouts
      if( 0 != FECathodeX && 0 != FEAnodeX ){
        // must be two maps for Andode and cathode seperatly
        if( MuonParameters::Anode == type[0] ){
          if ( FEAnodeX / xmap[0] > FECathodeX / xmap[1] ){
            // Anode (map[0]) is the vertical
            m_logVertGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
            m_logVertGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];
            
            m_logHorizGridX[station*m_NRegion + region] = FECathodeX / xmap[1];
            m_logHorizGridY[station*m_NRegion + region] = FECathodeY / ymap[1];
            
            m_padGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
            m_padGridY[station*m_NRegion + region] = FECathodeY / ymap[1];
          }else{
            // Cathode (map[1]) is the vertical
            m_logVertGridX[station*m_NRegion + region] = FECathodeX / xmap[1];
            m_logVertGridY[station*m_NRegion + region] = FECathodeY / ymap[1];

            m_logHorizGridX[station*m_NRegion + region] = FEAnodeX / xmap[0];
            m_logHorizGridX[station*m_NRegion + region] = FEAnodeY / ymap[0];

            m_padGridX[station*m_NRegion + region] = FECathodeX / xmap[1];
            m_padGridY[station*m_NRegion + region] = FEAnodeY / ymap[0];
          }
        }else{
          //map over was reversed, otherwise the same
          if ( FEAnodeX / xmap[1] > FECathodeX / xmap[0] ){
            // Anode (map[1]) is the vertical
            m_logVertGridX[station*m_NRegion + region] = FEAnodeX / xmap[1];
            m_logVertGridY[station*m_NRegion + region] = FEAnodeY / ymap[1];
            
            m_logHorizGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
            m_logHorizGridY[station*m_NRegion + region] = FECathodeY / ymap[0];
            
            m_padGridX[station*m_NRegion + region] = FEAnodeX / xmap[1];
            m_padGridY[station*m_NRegion + region] = FECathodeY / ymap[0];
          }else{
            // Cathode (map[1]) is the vertical
            m_logVertGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
            m_logVertGridY[station*m_NRegion + region] = FECathodeY / ymap[0];

            m_logHorizGridX[station*m_NRegion + region] = FEAnodeX / xmap[1];
            m_logHorizGridX[station*m_NRegion + region] = FEAnodeY / ymap[1];

            m_padGridX[station*m_NRegion + region] = FECathodeX / xmap[0];
            m_padGridY[station*m_NRegion + region] = FEAnodeY / ymap[1];
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
