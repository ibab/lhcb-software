// $Id: MuonTileIDXYZ.cpp,v 1.4 2002-04-02 13:05:45 dhcroft Exp $
// Include files 
#include <cstdio>
#include <cmath>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// from MuonDet
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/DeMuonGasGap.h"


// from MuonKernel
#include "MuonKernel/MuonSystemLayout.h"

// as regarding detector geometry
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/SolidBox.h"

//From CLHEP
#include "CLHEP/Geometry/Transform3D.h"

// local
#include "MuonTileIDXYZ.h"
#include "MuonTools/MuonChannel.h"
#include "MuonTools/MuonGeometry.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTileIDXYZ
//
// 07/03/2002 : David Hutchcroft
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MuonTileIDXYZ>          s_factory ;
const        IToolFactory& MuonTileIDXYZFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTileIDXYZ::MuonTileIDXYZ( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : AlgTool ( type, name , parent ) ,   
    m_DDS(0)
{
  declareInterface<IMuonTileXYZTool>(this);

  MsgStream log(msgSvc(), "MuonTileIDXYZ");
  log << MSG::INFO << "Initialising the MuonTile to position tool" << endreq;  

  // Locate the detector service needed by the this tool
  m_DDS = 0;
  if( serviceLocator() ) {
    StatusCode sc=StatusCode::FAILURE;
    sc = serviceLocator()->service( "DetectorDataSvc", m_DDS, true );
  }

  // null the chamber locations: actually only need to null z
  // Yay! a fully fortran compilant section
  int i,j;
  for(i=0;i<5;i++){
    //R1
    for(j=0;j<12;j++){
      chamR1[i][j].z = -1000. ;
    }
    //R2
    for(j=0;j<24;j++){
      chamR2[i][j].z = -1000. ;
    }
    //R3
    for(j=0;j<48;j++){
      chamR3[i][j].z = -1000. ;
    }
    //R4
    for(j=0;j<192;j++){
      chamR4[i][j].z = -1000. ;
    }
  }
  // same for station extents
  for(i=0; i<5; i++){
    stationExtent[i].z = -1000. ;
  }

  // make the chamber layout 

  MuonLayout R1Layout(MuonGeometry::chamberGridX[0],
                      MuonGeometry::chamberGridY[0]);

  MuonLayout R2Layout(MuonGeometry::chamberGridX[1],
                      MuonGeometry::chamberGridY[1]);

  MuonLayout R3Layout(MuonGeometry::chamberGridX[2],
                      MuonGeometry::chamberGridY[2]);

  MuonLayout R4Layout(MuonGeometry::chamberGridX[3],
                      MuonGeometry::chamberGridY[3]);

  MuonStationLayout chamberStationLayout(R1Layout,R2Layout,
                                         R3Layout,R4Layout);
                                         

  m_chamberSystemLayout = new MuonSystemLayout(chamberStationLayout);
}

MuonTileIDXYZ::~MuonTileIDXYZ(){
  delete m_chamberSystemLayout;
}

StatusCode MuonTileIDXYZ::calcTilePos(const MuonTileID& tile, 
                                      double& x, double& deltax,
                                      double& y, double& deltay,
                                      double& z, double& deltaz){
  MsgStream log(msgSvc(), name());

  // OK how to make this work.....
  // first locate the station and region from the tile
  unsigned int station = tile.station();
  unsigned int region  = tile.region();

  // now compare the layout parameter to possible "levels"
  // currently chamber, logical channel, pad
  if( MuonGeometry::chamberGridX[region] ==  tile.layout().xGrid() && 
      MuonGeometry::chamberGridY[region] ==  tile.layout().yGrid() ){
    // chambers
    log << MSG::DEBUG << "Found a tile laying out chambers" << endreq;
    
    StatusCode sc = getXYZChamberTile(tile,x,deltax,y,deltay,z,deltaz);
    if(!sc.isSuccess()){
      log << MSG::ERROR << "Failed to get xyz from chamber" << endreq;
      return sc;
    }
  }else if( MuonGeometry::padGridX[station][region] == 
            tile.layout().xGrid() && 
            MuonGeometry::padGridY[station][region] == 
            tile.layout().yGrid() ){
    // muon pads and logical channels with a 1:1 mapping to pads
    
    log << MSG::DEBUG 
        << "Found a tile laying out pads" 
        << endreq;
    
    StatusCode sc = getXYZPad(tile,x,deltax,y,deltay,z,deltaz);
    if(!sc.isSuccess()){
        log << MSG::ERROR << "Failed to get xyz from chamber" << endreq;
        return sc;
    }
  }else if( MuonGeometry::logChanHorizGridX[station][region] == 
            tile.layout().xGrid() && 
            MuonGeometry::logChanHorizGridY[station][region] == 
            tile.layout().yGrid() ){
    // horizontal logical channels            
    
    log << MSG::DEBUG 
        << "Found a tile laying out horizontal logical channels" 
        << endreq;
    
    StatusCode sc = getXYZLogical(tile,x,deltax,y,deltay,z,deltaz);
    if(!sc.isSuccess()){
        log << MSG::ERROR << "Failed to get xyz from chamber" << endreq;
        return sc;
    }
  }else if( MuonGeometry::logChanVertGridX[station][region] == 
            tile.layout().xGrid() && 
            MuonGeometry::logChanVertGridY[station][region] == 
            tile.layout().yGrid() ){
    // vertical logical channels            
    
    log << MSG::DEBUG 
        << "Found a tile laying out vertical logical channels" 
        << endreq;
    
    StatusCode sc = getXYZLogical(tile,x,deltax,y,deltay,z,deltaz);
    if(!sc.isSuccess()){
        log << MSG::ERROR << "Failed to get xyz from chamber" << endreq;
        return sc;
    }
  } else {
    log << MSG::ERROR 
        << "Did not understand the MuonTileID encoding" 
        << " xGrid=" << tile.layout().xGrid() 
        << " yGrid=" << tile.layout().yGrid() 
        <<endreq;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

StatusCode MuonTileIDXYZ::locateChamberTile(const double& x, 
                                            const double& y, 
                                            const double& z, 
                                            MuonTileID& tile){
  // this code takes an x,y,z position and finds and returns the chamber
  // containing the point: 
  // actually look in box defined by the extent of the active gas gaps

  // this function ignores gas gap pointer
  DeMuonGasGap *tmpGap;
  StatusCode sc = locateChamberTileAndGap(x, y, z, tile, tmpGap);
  return sc;
}


StatusCode MuonTileIDXYZ::locateChamberTileAndGap(const double& x, 
                                                  const double& y, 
                                                  const double& z, 
                                                  MuonTileID& tile,
                                                  DeMuonGasGap* &pGasGap){
  // this code takes an x,y,z position and finds and returns the chamber
  // containing the point: 
  // actually look in box defined by the extent of the active gas gaps
  MsgStream log(msgSvc(), name());

  if( 0 >= stationExtent[0].z ){
    // need to fill the stationExtent
    StatusCode sc = fillStationExtent();
    if(!sc.isSuccess()){
      log << MSG::ERROR << "Something went wrong getting station positions"
          << endreq;
    }
  }

  int station = 0;
  // add 1 mm for saftey against rounding errors
  while(station < 5 && 
        ( fabs(stationExtent[station].z - z) >= 
          1. + stationExtent[station].dz )){
    station++;
  }
  if(station == 5) {
    log << MSG::WARNING << "Could not locate z in a station" << endreq;
    return StatusCode::FAILURE;
  }
  
  // now come the potentially painful optimisation bit
  //
  // first estimate the chamber tile: this is prone to error and so hunting 
  // around the tile may be required.
  //
  // quarter gets a 2 mm possible overlap due to in front chambers 
  // (from 20cm fore/aft shift at 10mr projection)
  // projecting into the next quarter, I said this would be painful
  std::vector<int> tryQuarter;
  if( x >= -2. && y >= -2. ) {
    tryQuarter.push_back(0);
  }
  if( x >= -2. && y <= 2.){
    tryQuarter.push_back(1);
  }
  if( x <= 2. && y <= 2.){
    tryQuarter.push_back(2);
  }
  if( x <= 2. && y >= -2.) {
    tryQuarter.push_back(3);
  }
  
  log << MSG::DEBUG << " Found " << tryQuarter.size() << " quaters to search"
      << endreq;  

  // now guess the region: count in 8ths x/statio.dx 
  double xFraction = 8. * fabs(x) / stationExtent[station].dx;
  double yFraction = 8. * fabs(y) / stationExtent[station].dy;

  // check not in beam pipe or outside area
  if( 0.999979 > xFraction && 0.999979 > yFraction ) {
    log << MSG::WARNING << " Found a point in the beam pipe! " << endreq;
    return StatusCode::FAILURE;
  }

  if( xFraction > 8.000021 || yFraction > 8.000021 ){
    log << MSG::WARNING << " Found a point outside the active chambers! " 
        << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<int> tryRegion;
  // again allow about 1% overlap overly paranoid, but nessesary
  if ( 1.01 > xFraction && 1.01 > yFraction ){
    // region 1 is 1/8th in x and y
    tryRegion.push_back(0);
  }
  if (  (2.02 > xFraction && 2.02 > yFraction) &&
        (0.99 < xFraction || 0.99 < yFraction) ){
    // region 2 is 1/4 in x and y (also check not in R1)
    tryRegion.push_back(1);
  }
  if ( (4.04 > xFraction && 4.04 > yFraction) &&
        (1.98 < xFraction || 1.98 < yFraction) ){
    // region 3 is 1/2 in x and y (also check not in R2)
    tryRegion.push_back(2);
  }
  if ( (8.08 > xFraction && 8.08 > yFraction) &&
        (3.96 < xFraction || 3.96 < yFraction) ){
    // region 3 is 1/2 in x and y (also check not in R2)
    tryRegion.push_back(3);    
  }

  log << MSG::DEBUG << " Found " << tryRegion.size() << " regions to search"
      << endreq;

  std::vector<int>::const_iterator iQuarter;
  for(iQuarter = tryQuarter.begin() ; iQuarter != tryQuarter.end() ;
      iQuarter++ ){
    std::vector<int>::const_iterator iRegion;
    for(iRegion = tryRegion.begin() ; iRegion != tryRegion.end() ;
        iRegion++ ){
      log << MSG::DEBUG << " Trying station " << station
          << " region " << *iRegion
          << " quarter " << *iQuarter << endreq;
      StatusCode sc = locateGasGapFromXYZ(station,*iRegion,*iQuarter,
                                           x,y,z,tile,pGasGap);
      if( !sc.isSuccess() ) {
        log << MSG::ERROR << "Problem locating chamber tile" << endreq;
        return sc;
      }
      if( tile.isValid() ){
        log << MSG::DEBUG << " Found tile for chamber " << endreq;
        return StatusCode::SUCCESS;
      }
    }
  }

  log << MSG::WARNING 
      << " Tried every estimated combination and could not locate chamber"
      << endreq;
  return StatusCode::FAILURE;
}

StatusCode 
MuonTileIDXYZ::locateFEFromXYZ(const double& x, 
                               const double& y, 
                               const double& z, 
                               MuonTileID &chamber,
                               std::vector<MuonChannel> &muonChannels,
                               DeMuonGasGap* &pGasGap){
  MsgStream log(msgSvc(), name());

  // locate the chamber first
  StatusCode sc = locateChamberTileAndGap(x,y,z,chamber,pGasGap);
  if(!sc.isSuccess()){
    log << MSG::ERROR << "Could not locate chamberTile or gap" << endreq;
  }
  // now to determine the position of the FE channel within the chamber
  
  double chamX,chamY,chamZ,chamDx,chamDy,chamDz;
  sc = getXYZChamberTile(chamber,chamX,chamDx,chamY,chamDy,chamZ,chamDz);
  
  double localX = x - (chamX - chamDx); // x of hit within chamber
  double localY = y - (chamY - chamDy); // y of hit within chamber

  // FE sizes are attached to regions, two levels above the gasgaps in the TDS
  IDetectorElement *ide = 
    pGasGap->parentIDetectorElement()->parentIDetectorElement();
  DeMuonRegion *region = dynamic_cast<DeMuonRegion*>(ide);
  if(!region){
    log << MSG::ERROR << "Grandparent of " << pGasGap->name()
        << " could not be found " << endreq;
  }


  if(region->FEAnodeX() > 0){
    // there is an Anode readout in this region
    double padSizeX = (2.*chamDx)/(static_cast<double>(region->FEAnodeX()));
    double padSizeY = (2.*chamDy)/(static_cast<double>(region->FEAnodeY()));
    
    int xPos = static_cast<int>(floor(localX/padSizeX));
    int yPos = static_cast<int>(floor(localY/padSizeY));

    muonChannels.push_back(MuonChannel(xPos,yPos,
                                        MuonParameters::Anode));
  }
    
  if(region->FECathodeX() > 0){
    // there is an Cathode readout in this region
    double padSizeX = (2.*chamDx)/(static_cast<double>(region->FECathodeX()));
    double padSizeY = (2.*chamDy)/(static_cast<double>(region->FECathodeY()));

    int xPos = static_cast<int>(floor(localX/padSizeX));
    int yPos = static_cast<int>(floor(localY/padSizeY));

    muonChannels.push_back(MuonChannel(xPos,yPos,
                                        MuonParameters::Anode));
  }
  
  return StatusCode::SUCCESS;
   
}

StatusCode MuonTileIDXYZ::getXYZChamberTile(const MuonTileID& tile, 
                                            double& x, double& deltax,
                                            double& y, double& deltay,
                                            double& z, double& deltaz){
  MsgStream log(msgSvc(), name());
  
  unsigned int chamberNum = getChamberNumber(tile);

  unsigned int station = tile.station();
  unsigned int region  = tile.region();

  StatusCode sc = getXYZChamber(station,region,chamberNum,
                                x,deltax,y,deltay,z,deltaz);
  if(!sc.isSuccess()){
    log << MSG::ERROR << "Failed to get chamber " << chamberNum << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonTileIDXYZ::getXYZChamber(const int& station,
                                        const int& region,
                                        const int& chamberNum,
                                        double& x, double& deltax,
                                        double& y, double& deltay,
                                        double& z, double& deltaz){
  StatusCode sc = getXYZAndCache(station,region,chamberNum,-1,
                                 x,deltax,y,deltay,z,deltaz);
  return sc;
}

StatusCode MuonTileIDXYZ::getXYZGasGap(const int& station,
                                       const int& region,
                                       const int& chamberNum,
                                       const int& gapNum,
                                       double& x, double& deltax,
                                       double& y, double& deltay,
                                       double& z, double& deltaz){
  StatusCode sc = getXYZAndCache(station,region,chamberNum,gapNum,
                                 x,deltax,y,deltay,z,deltaz);
  return sc;
}

StatusCode MuonTileIDXYZ::getXYZAndCache(const int& station,
                                         const int& region,
                                         const int& chamberNum,
                                         const int& gapNum,
                                         double& x, double& deltax,
                                         double& y, double& deltay,
                                         double& z, double& deltaz){
  MsgStream log(msgSvc(), name());

  chamberExtent_ *currCham=0;
  int nGap;
  gapExtent_ *currGapArray=0;

  // Alright I agree this is officially very ugly code
  // I probably should use some stl stuff here somewhere
  // this pointer arimetic is just asking for trouble

  if( 0 == region ){
    currCham = &(chamR1[station][chamberNum-1]);
    nGap = 4;
    currGapArray = gapR1[station][chamberNum-1];
  }else if( 1 == region ){
    currCham = &(chamR2[station][chamberNum-1]);
    nGap = 4;
    currGapArray = gapR2[station][chamberNum-1];
  }else if( 2 == region ){
    currCham = &(chamR3[station][chamberNum-1]);
    if(station < 3){
      nGap = 4;
      currGapArray = gapR3MWPC[station][chamberNum-1];
    }else{
      nGap = 2;
      currGapArray = gapR3RPC[station-3][chamberNum-1];
    }
  }else {
    currCham = &(chamR4[station][chamberNum-1]);
    if(station < 3){
      nGap = 4;
      currGapArray = gapR4MWPC[station][chamberNum-1];
    }else{
      nGap = 2;
      currGapArray = gapR4RPC[station-3][chamberNum-1];
    }
  }


  // test if have information already  
  if( 0. >= currCham->z ) {
    //TDS path t chamber is of the form /dd/Structure/LHCb/Muon/M1/R1/Cham001
    // Name this chamber
    char charPath[80];
    sprintf(charPath,"/dd/Structure/LHCb/Muon/M%1i/R%1i/Cham%03i",
            station+1,region+1,chamberNum);
    std::string chamberString = charPath;

    log << MSG::DEBUG << "asking TDS for " << charPath << endreq;
    SmartDataPtr<DeMuonChamber> muChamber(m_DDS, chamberString);
    if(!muChamber){
      log << MSG::ERROR << "Could not read chamber "
          << chamberString << " from TDS" << endreq;
      return StatusCode::FAILURE;
    }

    currCham->pChamber = muChamber;

    // active volume is defined by the gas gaps, want first and last
    IDetectorElement::IDEContainer::const_iterator itGap;
    int gapIndex;
    for(itGap = muChamber->childBegin(), gapIndex = 0; 
        itGap != muChamber->childEnd();
        itGap++, gapIndex++){
      log << MSG::DEBUG << "asking TDS for " << (*itGap)->name()
          << endreq;    
      SmartDataPtr<DeMuonGasGap> muGap(m_DDS,(*itGap)->name());
      if( !muGap ){
        log << MSG::ERROR << "Could not read gas gaps "
            << (*itGap)->name() << " from TDS" << endreq;
        return StatusCode::FAILURE;
      }
      
      currGapArray[gapIndex].pGasGap = muGap;

      HepTransform3D vTransForm = muGap->geometry()->matrixInv();
      Hep3Vector vtrans = vTransForm.getTranslation();
      
      currGapArray[gapIndex].x = vtrans.x();
      currGapArray[gapIndex].y = vtrans.y();
      currGapArray[gapIndex].z = vtrans.z();

      // get ILVolume pointer
      const ILVolume *logVol = muGap->geometry()->lvolume();
      // Get the solid
      const ISolid *soild = logVol->solid();
      // check these really are boxes (they ought to be!)
      const SolidBox *box = dynamic_cast<const SolidBox *>(soild);
      if( !box ){
        log << MSG::ERROR << "Could not cast gas gap solid to box" 
            << endreq;
        return StatusCode::FAILURE;
      }
    
      currGapArray[gapIndex].dx = box->xHalfLength();
      currGapArray[gapIndex].dy = box->yHalfLength();
      currGapArray[gapIndex].dz = box->zHalfLength();
    }    

    currCham->x = ( currGapArray[0].x + currGapArray[nGap-1].x ) / 2.0;
    currCham->y = ( currGapArray[0].y + currGapArray[nGap-1].y ) / 2.0;
    currCham->z = ( currGapArray[0].z + currGapArray[nGap-1].z ) / 2.0;

    log << MSG::DEBUG 
        << " chamber average of first and last gas gaps reported at x="
        << currCham->x << "mm y= " << currCham->y 
        << "mm z="  << currCham->z << "mm" << endreq;

    currCham->dx = currGapArray[0].dx;
    currCham->dy = currGapArray[0].dy;
    currCham->dz = (fabs( currGapArray[0].z - currGapArray[nGap-1].z ) /2.0) +
      currGapArray[0].dz;
    
    log << MSG::DEBUG 
        << " chamber size x/2=" << currCham->dx << "mm "
        << "y/2 =" << currCham->dy << "mm "
        << "z/2 =" << currCham->dz << "mm " 
        << endreq;
  }
  
  if( -1 == gapNum ){
    x = currCham->x;
    y = currCham->y;
    z = currCham->z;
    deltax = currCham->dx;
    deltay = currCham->dy;
    deltaz = currCham->dz;
  }else{
    x = currGapArray[gapNum].x;
    y = currGapArray[gapNum].y;
    z = currGapArray[gapNum].z;
    deltax = currGapArray[gapNum].dx;
    deltay = currGapArray[gapNum].dy;
    deltaz = currGapArray[gapNum].dz;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode MuonTileIDXYZ::fillStationExtent(){

  MsgStream log(msgSvc(), name());
  
  // So get the TDS representations of the stations
  // to fill stationExtent with the outer edges of the chambers in the
  // corners of region 4
  
  int station;
  for(station = 0 ; station < 5 ; station++){

    double xMax,xMin,yMax,yMin,zMax,zMin;

    double x,y,z;
    double dx,dy,dz;

    int chamCorner;
    for( chamCorner = 0 ; chamCorner < 4 ; chamCorner++ ){
      int nChamber;
      if( 0 == chamCorner ){
        nChamber = MuonGeometry::chamberR4[0][0];
      }else if( 1 == chamCorner ){
        nChamber = MuonGeometry::chamberR4[7][0];
      }else if( 2 == chamCorner ){
        nChamber = MuonGeometry::chamberR4[0][31];
      }else{
        nChamber = MuonGeometry::chamberR4[7][31];
      }

      // all of these are in region 4
      StatusCode sc = getXYZChamber(station,3,nChamber,x,dx,y,dy,z,dz);
      if(!sc.isSuccess()){
        log << MSG::ERROR << "Could not get corner chamber" << endreq;
        return sc;
      }

      if(0 == chamCorner) {
        xMax = x;
        xMin = x;
        yMax = y;
        yMin = y;
        zMax = z;
        zMin = z;
      }else{
        if ( x > xMax ) { xMax = x; }
        if ( x < xMin ) { xMin = x; }
        if ( y > yMax ) { yMax = y; }
        if ( y < yMin ) { yMin = y; }
        if ( z > zMax ) { zMax = z; }
        if ( z < zMin ) { zMin = z; }
      }
    }
    
    stationExtent[station].x = ( xMax + xMin ) / 2.; // all these should
    stationExtent[station].y = ( yMax + yMin ) / 2.; // be zero
    stationExtent[station].z = ( zMax + zMin ) / 2.; // diff due to rounding

    stationExtent[station].dx = (( xMax - xMin ) / 2.) + dx;
    stationExtent[station].dy = (( yMax - yMin ) / 2.) + dy;
    stationExtent[station].dz = (( zMax - zMin ) / 2.) + dz;

    log << MSG::DEBUG 
        << " Station " << station
        << " position x=" << stationExtent[station].x << "mm "
        << "y =" << stationExtent[station].y << "mm "
        << "z =" << stationExtent[station].z << "mm " 
        << endreq;

    log << MSG::DEBUG 
        << " Station " << station
        << " size x/2=" << stationExtent[station].dx << "mm "
        << "y/2 =" << stationExtent[station].dy << "mm "
        << "z/2 =" << stationExtent[station].dz << "mm " 
        << endreq;
  }

  return StatusCode::SUCCESS;
}


int MuonTileIDXYZ::getChamberNumber(const MuonTileID& tile){

  MsgStream log(msgSvc(), name());
  // look-up chamber number from tile
  // use the fact that the chamber numbering is constitent between stations
  
  // quarter counts 
  //      +y          
  //    3 | 0
  //   ---+--- +x
  //    2 | 1
  // in quarter 0 x=x,               y=y
  //            1 x=x                y=Size_y - 1 - y
  //            2 x=Size_x - 1 - x   y=Size_y - 1 - y
  //            3 x=Size_x - 1 - x   y=y
  //
  // size_x = 2*xGrid  (xGrid count across only a twelfth of a region)
  // same for y

  unsigned int chamberNumber; // counts from 1 as per TDS
  // position of chamber number in arrays 
  unsigned int xIndex,yIndex; 
  
  if ( 0 == tile.quarter() ){
    xIndex = (2*tile.layout().xGrid()) + tile.nX();
    yIndex = (2*tile.layout().yGrid()) + tile.nY();
  } else if ( 1 == tile.quarter() ){
    xIndex = (2*tile.layout().xGrid()) + tile.nX();
    yIndex = (2*tile.layout().yGrid()) - (1+tile.nY());
  } else if ( 2 == tile.quarter() ){
    xIndex = (2*tile.layout().xGrid()) - (1+tile.nX());
    yIndex = (2*tile.layout().yGrid()) - (1+tile.nY());
  } else if ( 3 == tile.quarter() ){
    xIndex = (2*tile.layout().xGrid()) - (1+tile.nX());
    yIndex = (2*tile.layout().yGrid()) + tile.nY();
  } 
  
  if( 0 == tile.region() ) {
    chamberNumber = 
      MuonGeometry::chamberR1[xIndex][yIndex];
  }else if ( 1 == tile.region() ){
    chamberNumber = 
      MuonGeometry::chamberR2[xIndex][yIndex];
  }else if ( 2 == tile.region() ){
    chamberNumber = 
      MuonGeometry::chamberR3[xIndex][yIndex];
  }else if ( 3 == tile.region() ){
    chamberNumber = 
      MuonGeometry::chamberR4[xIndex][yIndex];
  }

  log << MSG::DEBUG << "Chamber " << chamberNumber 
      << " in R" << tile.region()+1
      << " xIndex " << xIndex
      << " yIndex " << yIndex
      << " was found for tile " << tile << endreq;
  
  return chamberNumber;
}

StatusCode MuonTileIDXYZ::getXYZPad(const MuonTileID& tile, 
                                      double& x, double& deltax,
                                      double& y, double& deltay,
                                      double& z, double& deltaz){
  MsgStream log(msgSvc(), name());

  // to find the x,y,z of the pad one must first find the chamber
  // to find the chamber one must know the tile of the chamber
  unsigned int station = tile.station();
  unsigned int region  = tile.region();
  
  MuonLayout chambersLayout = 
    m_chamberSystemLayout->regionLayout(station,region);
  // locate the chamber
  MuonTileID chamTile = chambersLayout.contains(tile);

  // chamber size in x and y (z is same as pad)
  double cx,cy,cDeltax,cDeltay;
  
  StatusCode sc = getXYZChamberTile(chamTile,cx,cDeltax,cy,cDeltay,z,deltaz);
  if(!sc.isSuccess()){
    log << MSG::ERROR << "Failed to get chamber for pad tile: " << tile
        << endreq;
    return sc;
  }

  // number of pads in a chamber is : 
  // padLayoutX / chamberLayoutX , padLayoutY / chamberLayoutY
  int xRatio = MuonGeometry::padGridX[station][region] / 
    MuonGeometry::chamberGridX[region];
  int yRatio = MuonGeometry::padGridY[station][region] / 
    MuonGeometry::chamberGridY[region];

  // need to work out offset within chamber
  // initally ignore the "reflectional" nature of the quarters
  int xOffset = tile.nX() - (xRatio * chamTile.nX());
  int yOffset = tile.nY() - (yRatio * chamTile.nY());
  
  // do the reflections of the tileID structure into Cartesian coordinates
  if ( 0 == tile.quarter() ){
    xOffset = xOffset;
    yOffset = yOffset;
  } else if ( 1 == tile.quarter() ){
    xOffset = xOffset;
    yOffset = yRatio - (1 + yOffset);
  } else if ( 2 == tile.quarter() ){
    xOffset = xRatio - (1 + xOffset);
    yOffset = yRatio - (1 + yOffset);
  } else if ( 3 == tile.quarter() ){
    xOffset = xRatio - (1 + xOffset);
    yOffset = yOffset;
  } 

  log << MSG::DEBUG << "Pad xOffset within chamber:" << xOffset 
      << " yOffset within chamber:" << yOffset 
      << " xratio: " << xRatio << " yratio: " << yRatio
      << endreq;
  
  // now make the calculations of x and z (and dx and dy)
  
  x = ( cx - cDeltax ) + 
    ( ( static_cast<double>(xOffset) / static_cast<double>(xRatio) ) *
      (2.0 * cDeltax ) );
  y = ( cy - cDeltay ) + 
    ( ( static_cast<double>(yOffset) / static_cast<double>(yRatio) ) *
      (2.0 * cDeltay ) );
  
  deltax = cDeltax / (static_cast<double>(xRatio));
  deltay = cDeltay / (static_cast<double>(yRatio));

  return StatusCode::SUCCESS;
}  

StatusCode MuonTileIDXYZ::getXYZLogical(const MuonTileID& tile, 
                                          double& x, double& deltax,
                                          double& y, double& deltay,
                                          double& z, double& deltaz){
  MsgStream log(msgSvc(), name());

  // If we get here then the logical strip is potenitally bigger than a chamber
  // if not then we can subcontract to getXYZPad directly

  // to find the x,y,z of the pad one must first find the chamber
  // to find the chamber one must know the tile of the chamber

  unsigned int station = tile.station();
  unsigned int region  = tile.region();
  
  MuonLayout chambers = m_chamberSystemLayout->regionLayout(station,region);

  if( tile.layout().xGrid() >= chambers.xGrid() && 
      tile.layout().yGrid() >= chambers.yGrid() ) {
    // ok logical channels are within a single chamber, delegate to getXYZpad
    StatusCode sc = getXYZPad(tile,x,deltax,y,deltay,z,deltaz);
    if(!sc.isSuccess()){
      log << MSG::ERROR << "Passed a logical channel to getXYZPad: failed"
          << " tile ID=" << tile << endreq;
      return sc;
    }
  }else if(tile.layout().xGrid() >= chambers.xGrid() &&
           tile.layout().yGrid() < chambers.yGrid() ) {
    // This logical channel crosses chambers in y, break it down into 
    // chamber hight pads then combine the chamber extents later
    // number of tiles to make:
    int nTile = chambers.yGrid() / tile.layout().yGrid();
    log << MSG::DEBUG << "Making " << nTile << " temporary MuonTileIDs"
        << " to get all chamber locations"
        << endreq;

    // width of pad (in x) is the same, hight (yGrid) that of the chamber
    MuonLayout tempPadLayout(tile.layout().xGrid(),chambers.yGrid());
    
    std::vector<MuonTileID> tempTiles;

    int i;
    for( i=0 ; i<nTile ; i++ ){
      int yTile = ( tile.nY() * nTile ) + i ;
      MuonTileID tTile(station,0,0,tempPadLayout,region,tile.quarter(),
                     tile.nX(),yTile);
      tempTiles.push_back(tTile);
    }
    
    // loop over the tiles and get max/min x,y,z of the combinations
    double xmin, xmax, ymin, ymax, zmin, zmax;
    double padDx,chamDy,chamDz;   
    std::vector<MuonTileID>::const_iterator iTile;
    for(iTile = tempTiles.begin() ; iTile != tempTiles.end() ; iTile++){
      double lx,ly,lz;
      StatusCode sc = getXYZPad(*iTile,lx,padDx,ly,chamDy,lz,chamDz);
      if(!sc.isSuccess()){
        log << MSG::ERROR << "Passed a temporary channel to getXYZPad: failed"
            << " tile ID=" << *iTile << endreq;
        return sc;
      }      
      if(iTile == tempTiles.begin()){
        xmin = lx;
        xmax = lx;
        ymin = ly;
        ymax = ly;
        zmin = lz;
        zmax = lz;
      }else{
        if( xmax < lx ) { xmax = lx; }
        if( xmin > lx ) { xmin = lx; }
        if( ymax < ly ) { ymax = ly; }
        if( ymin > ly ) { ymin = ly; }
        if( zmax < lz ) { zmax = lz; }
        if( zmin > lz ) { zmin = lz; }
      }
    }
    
    x = (xmax + xmin) / 2.0;
    y = (ymax + ymin) / 2.0;
    z = (zmax + zmin) / 2.0;
    
    deltax = ((xmax - xmin)/2.0) + padDx;
    deltay = ((ymax - ymin)/2.0) + chamDy;
    deltaz = ((zmax - zmin)/2.0) + chamDz;

  }else{
    log << MSG::WARNING 
        << "You requested a logical channel wider than a chamber, failing" 
        << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}  


StatusCode MuonTileIDXYZ::locateGasGapFromXYZ( const int& station,
                                               const int& region,
                                               const int& quarter,
                                               const double& x,
                                               const double& y,
                                               const double& z,
                                               MuonTileID& tile,
                                               DeMuonGasGap* &pGasGap){
  MsgStream log(msgSvc(), name());
  // OK this is the sledgehammer approach (try everything until it works)

  int ixBegin,ixEnd; // loop indices in X
  int iyBegin,iyEnd; // loop indices in Y

  // map to whole station numbering scheme
  if(0 == quarter){
    ixBegin   = 2*MuonGeometry::chamberGridX[region];
    ixEnd     = 4*MuonGeometry::chamberGridX[region];
    iyBegin   = 2*MuonGeometry::chamberGridY[region];
    iyEnd     = 4*MuonGeometry::chamberGridY[region];
  } else if ( 1 == quarter ){
    ixBegin   = 2*MuonGeometry::chamberGridX[region];
    ixEnd     = 4*MuonGeometry::chamberGridX[region];
    iyBegin   = 0;
    iyEnd     = 2*MuonGeometry::chamberGridY[region];
  } else if ( 2 == quarter ){
    ixBegin   = 0;
    ixEnd     = 2*MuonGeometry::chamberGridX[region];
    iyBegin   = 0;
    iyEnd     = 2*MuonGeometry::chamberGridY[region];
  } else if ( 3 == quarter ){
    ixBegin   = 0;
    ixEnd     = 2*MuonGeometry::chamberGridX[region];
    iyBegin   = 2*MuonGeometry::chamberGridY[region];
    iyEnd     = 4*MuonGeometry::chamberGridY[region];
  }  

  // loop over chamber indices
  int ix,iy;
  for(ix = ixBegin; ix < ixEnd ; ix++){
    for(iy= iyBegin; iy < iyEnd ; iy++){
      int nCham;
      if(0 == region){
        nCham = MuonGeometry::chamberR1[ix][iy];
      }else if(1 == region){
        nCham = MuonGeometry::chamberR2[ix][iy];
      }else if(2 == region){
        nCham = MuonGeometry::chamberR3[ix][iy];
      }else{
        nCham = MuonGeometry::chamberR4[ix][iy];
      }

      // check we are not in the hole in the middle of the region
      // MuonGeometry:: has zeros there
      if(!nCham) {
        continue;
      } 
      
      double chamX,chamY,chamZ;
      double chamDx,chamDy,chamDz;
      StatusCode sc = getXYZChamber(station,region,nCham,
                                    chamX,chamDx,chamY,chamDy,chamZ,chamDz);
      if(!sc.isSuccess()){
        log << MSG::ERROR
            << "Could not get chamber: " << nCham 
            << " in station " << station
            << " region " << region
            << endreq;
        return sc;
      }

      if( (fabs(x-chamX) < 0.1 + chamDx) &&
          (fabs(y-chamY) < 0.1 + chamDy) &&
          (fabs(z-chamZ) < 0.1 + chamDz) ) {
        // at last... have the correct chamber, now to make a tile from it

        int xTile,yTile;
        // need to do the reflections to quarter 1 again....
        if(0 == quarter){
          xTile = ix - (2*MuonGeometry::chamberGridX[region]);
          yTile = iy - (2*MuonGeometry::chamberGridY[region]);
        } else if ( 1 == quarter ){
          xTile = ix - (2*MuonGeometry::chamberGridX[region]);
          yTile = (2*MuonGeometry::chamberGridY[region]) - (1+iy);
        } else if ( 2 == quarter ){
          xTile = (2*MuonGeometry::chamberGridX[region]) - (1+ix);
          yTile = (2*MuonGeometry::chamberGridY[region]) - (1+iy);
        } else if ( 3 == quarter ){
          xTile = (2*MuonGeometry::chamberGridX[region]) - (1+ix);
          yTile = iy - (2*MuonGeometry::chamberGridY[region]);
        }  

        // locate the correct GasGap object
        int nGap;
        if( 2 > region ){
          nGap = 4;
        }else {
          if(station < 3){
            nGap = 4;
          }else{
            nGap = 2;
          }
        }
        int iGap;
        for(iGap = 0 ; iGap < nGap ; iGap++){

          double gapX,gapY,gapZ;
          double gapDx,gapDy,gapDz;

          sc = getXYZGasGap(station,region,nCham,iGap,
                            gapX,gapDx,gapY,gapDy,gapZ,gapDz);
          if(!sc.isSuccess()){
            log << MSG::ERROR
                << "Could not get GasGap: " <<iGap 
                << " in chamber: " << nCham 
                << " in station " << station
                << " region " << region
                << endreq;
            return sc;
          }
          if( (fabs(x-gapX) < 0.1 + gapDx) &&
              (fabs(y-gapY) < 0.1 + gapDy) &&
              (fabs(z-gapZ) < 0.1 + gapDz) ) {
            // also have the correct gasGap!

            // copy to pass back to calling routine
            tile = MuonTileID(station,
                              region,
                              iGap,
                              MuonLayout(MuonGeometry::chamberGridX[region],
                                         MuonGeometry::chamberGridY[region]),
                              region,
                              quarter,
                              xTile,
                              yTile);               
            if( 0 == region ){
              pGasGap = gapR1[station][nCham-1][iGap].pGasGap;
            }else if( 1 == region ){
              pGasGap = gapR2[station][nCham-1][iGap].pGasGap;
            }else if( 2 == region ){
              if(station < 3){
                pGasGap = gapR3MWPC[station][nCham-1][iGap].pGasGap;
              }else{
                pGasGap = gapR3RPC[station-3][nCham-1][iGap].pGasGap;
              }
            }else {
              if(station < 3){
                pGasGap = gapR4MWPC[station][nCham-1][iGap].pGasGap;
              }else{
                pGasGap = gapR4RPC[station-3][nCham-1][iGap].pGasGap;
              }
            }
            return StatusCode::SUCCESS;
          }
        }
      }
    }
  }
  // if we got here no chamber was found
  // make an invalid tile to indicate this to the calling routine
  tile = MuonTileID();
  pGasGap = 0;
  return StatusCode::SUCCESS;
}
    

//=============================================================================
