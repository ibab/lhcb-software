// $Id: MuonGeometryTool.cpp,v 1.3 2002-06-21 13:48:38 cattanem Exp $
// Include files 
#include <cmath>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

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
    toolSvc()->retrieveTool("MuonTileIDXYZ", m_tileTool, this );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create MuonTileIDToXYZ tool" << endreq;
  }

  // set value to -1 to indicate arrays not filled
  regionExtent[0][0].z = -999.9;
  padExtent[0][0].sizeX = -999.9;


}
//=============================================================================

StatusCode MuonGeometryTool::getStationBox(const int &station,
                                           double &deltaX,
                                           double &deltaY,
                                           double &z, double &deltaZ){
  if(0 > station || 4 < station){
    return StatusCode::FAILURE;
  }

  if( 0>= regionExtent[0][0].z){
    StatusCode sc = fillLocals();
    if(sc.isFailure()){
      return sc;
    }
  }

  // copy R4 from local arrays (outside only for a station)
  deltaX = regionExtent[station][3].outerDeltaX;
  deltaY = regionExtent[station][3].outerDeltaY;
  z = regionExtent[station][3].z;
  deltaZ = regionExtent[station][3].deltaZ;
  
  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::getRegionBox(const int &station,
                                          const int & region,
                                          double &outerDeltaX,
                                          double &innerDeltaX,
                                          double &outerDeltaY,
                                          double &innerDeltaY,
                                          double &z, double &deltaZ){
  
  if(0 > station || 4 < station || 0 > region || 3 < region){
    return StatusCode::FAILURE;
  }

  if( 0>= regionExtent[0][0].z){
    StatusCode sc = fillLocals();
    if(sc.isFailure()){
      return sc;
    }
  }

  // copy parameters from local arrays
  outerDeltaX = regionExtent[station][region].outerDeltaX;
  innerDeltaX = regionExtent[station][region].innerDeltaX;

  outerDeltaY = regionExtent[station][region].outerDeltaY;
  innerDeltaY = regionExtent[station][region].innerDeltaY;

  z = regionExtent[station][region].z;
  deltaZ = regionExtent[station][3].deltaZ;

  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::getPadSize(const int &station, 
                                        const int &region,
                                        double &sizeX, 
                                        double &sizeY){

  if(0 > station || 4 < station || 0 > region || 3 < region){
    return StatusCode::FAILURE;
  }
 
  if( 0 >= padExtent[0][0].sizeX ){
     StatusCode sc = fillLocals();
    if(sc.isFailure()){
      return sc;
    }
  }

  // copy from local arrays
  sizeX = padExtent[station][region].sizeX;
  sizeY = padExtent[station][region].sizeY;
  
  return StatusCode::SUCCESS;
}

StatusCode MuonGeometryTool::fillLocals(){
  // this gets all of the wanted parameters from the MuonTileToXYZTool
  // basically this just knows the tiles to ask for


  MuonLayout twelfthLayout(1,1); // always the same

  // First regions, need to get tiles for each corner of each region
  unsigned int station;
  for(station = 0 ; station < 5 ; station++){
    
    unsigned int region;
    for(region = 0 ; region < 4 ; region++){
      
      double minX,maxX,minY,maxY; // take abs values here
      double minZ, maxZ; // "normal" averageing

      unsigned int twelfth;
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
      
      regionExtent[station][region].outerDeltaX = maxX;
      regionExtent[station][region].innerDeltaX = minX;
      regionExtent[station][region].outerDeltaY = maxY;
      regionExtent[station][region].innerDeltaY = minY;
      regionExtent[station][region].z = (maxZ + minZ) /2.0;
      regionExtent[station][region].deltaZ = (maxZ - minZ) /2.0;

      // get a pad in the region to find the size
      MuonLayout padLayout(MuonGeometry::padGridX[station][region],
                           MuonGeometry::padGridY[station][region]);
      MuonTileID padTile(station,0,0,padLayout,region,0,
                         MuonGeometry::padGridX[station][region],
                         MuonGeometry::padGridY[station][region]);
      
      double x,Dx,y,Dy,z,Dz;
      StatusCode sc = m_tileTool->calcTilePos(padTile,x,Dx,y,Dy,z,Dz);
      if(sc.isFailure()){
        return sc;
      }

      padExtent[station][region].sizeX = 2.*Dx;
      padExtent[station][region].sizeY = 2.*Dy;
      
    }
  }
  return StatusCode::SUCCESS;
}
