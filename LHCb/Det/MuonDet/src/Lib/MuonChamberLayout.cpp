// Include files

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
//Muon
#include "MuonChamberLayout.h"
#include "MuonDet/MuonChamberGrid.h"
#include "MuonDet/DeMuonDetector.h"

//Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/SolidBox.h"

//standard
#include <iostream>
#include <vector>

#ifdef __INTEL_COMPILER       // Disable ICC remark from ROOT
  #pragma warning(disable:68) // Integer conversion resulted in a change of sign 
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : MuonChamberLayout
//
// 2004-01-07 : Alessio Sarti
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonChamberLayout::MuonChamberLayout()
  : m_msgStream(NULL), m_msgSvc(NULL),  m_detSvc(NULL), m_isM1defined(false)
{

  MuonLayout R1(1,1);
  MuonLayout R2(1,2);
  MuonLayout R3(1,4);
  MuonLayout R4(2,8);

  //When initialized in this way needs un update for the Layout
  MuonChamberLayout(R1,R2,R3,R4,this->dataSvc(),this->msgSvc());
}


MuonChamberLayout::MuonChamberLayout(MuonLayout R1,
                                     MuonLayout R2,
                                     MuonLayout R3,
                                     MuonLayout R4,
                                     IDataProviderSvc* detSvc,
                                     IMessageSvc *msgSvc)
  : m_msgStream(NULL)
{


  //Setting up the layout for the various regions
  m_layout[0] = R1;
  m_layout[1] = R2;
  m_layout[2] = R3;
  m_layout[3] = R4;
  m_detSvc = detSvc;
  m_msgSvc =msgSvc;
  //Performing vectors and grid initialization
  initialize();
}

StatusCode MuonChamberLayout::initialize( ) {

  bool debug = false;
  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) {
    if(debug) msgStream()<<MSG::ERROR << "Failure to initialize DetectorElement" <<
      endmsg;
    return sc ;
  }

  SmartDataPtr<DeMuonDetector> 
    MuonDe(m_detSvc,DeMuonLocation::Default);
  m_isM1defined = MuonDe->isM1defined();
  if (debug)
    msgStream()<<MSG::INFO<< "Retrieved M1 definition status: " << m_isM1defined <<endmsg;


  //Grid initialization
  int iDd;
  unsigned int cgX[4] = {1,1,1,2}; m_cgX.resize(4);
  for(iDd = 0; iDd<4; iDd++) { m_cgX.at(iDd) = cgX[iDd]; }
  unsigned int cgY[4] = {1,2,4,8}; m_cgY.resize(4);
  for(iDd = 0; iDd<4; iDd++) { m_cgY.at(iDd) = cgY[iDd]; }

  if(debug) {
    msgStream()<<MSG::DEBUG<< " Building a Grid inside MuonLayout"<<endmsg;
  }

  //Offset: needs to change?
  int myoffset[4] = {0,16,48,112}; m_offSet.resize(4);
  for(iDd = 0; iDd<4; iDd++) { m_offSet.at(iDd) = myoffset[iDd]; }

  //Initializing the chamber grid with detector elements
  m_chamberGrid.resize(368);
  for(int iD = 0; iD<368; iD++) { m_chamberGrid.at(iD) = -1; }

  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
MuonChamberLayout::~MuonChamberLayout() {delete m_msgStream; m_msgStream = NULL;}

void MuonChamberLayout::Copy(MuonChamberLayout &lay) {
  //Copy constructor
  bool debug = false;
  for(int iL = 0; iL<4; iL++) {
    if(debug){
      msgStream()<<MSG::DEBUG<<"My Copied layout: "<<iL<<endmsg;
    }

    lay.setLayout(iL,this->layout(iL));
  }
  lay.setDataProvider(this->DataProvider());
}



std::vector<int> MuonChamberLayout::
neighborChambers(float myX, float myY, int stat, int x_direction, int y_direction) const{

  std::vector<int> myChambers;
  int sC_idY(0),sC_idX(0),reg(0);
  bool debug = false;

  gridPosition(myX,myY,stat,sC_idX,sC_idY,reg);

  if(debug){
    msgStream()<<MSG::DEBUG<<"Position in the grid "<<sC_idX<<" "<<sC_idY<<" "<<
      reg<<" "<<x_direction<<" "<<y_direction<<endmsg;
  }

  //Deciding the corner-side
  int scX(0),scY(0);
  if(x_direction != 0) scX++;
  if(y_direction != 0) scY++;

  for(int ix = 0; ix <= scX; ix++) {
    for(int iy = 0; iy <= scY; iy++) {
      //Do the computation only for OTHER chambers
      if(ix || iy) {
        chamberXY(sC_idX,sC_idY,ix*x_direction,iy*y_direction,reg,myChambers);
      }
    }
  }

  if(debug) {
    //   using GaudiUtils::operator<<; // for streaming std::vector
    msgStream()<<MSG::DEBUG<<"Returning chambers: "<<myChambers<<endmsg;
  }

  return myChambers;

}

void MuonChamberLayout::returnChambers(int sta, float st_x, float st_y, int x_dir,
                                       int y_dir, std::vector<int>& regs, std::vector<int>& chs) const {

  std::vector<int> myChambers;
  myChambers = neighborChambers(st_x,st_y,sta,x_dir,y_dir);

  //Now get the chambers
  std::vector<int>::iterator idTile;
  bool debug = false;
  for(idTile = myChambers.begin(); idTile<myChambers.end(); idTile++){
    const long int cTile = *idTile;
    //Added protection against non existing chamber
    if(cTile>0) {
      int region = findRegion(cTile);
      if(debug)  {
        msgStream()<<MSG::DEBUG<<"Returned chambers:: "<<sta+1<<" "
                   <<region+1<<" "<<cTile-m_offSet.at(region)<<
          endmsg;
      }

      regs.push_back(region);  chs.push_back(cTile-m_offSet.at(region)-1);
    }
  }
  if(debug) {
    msgStream()<<MSG::DEBUG<<"Exiting from chamber creation "<<endmsg;
  }


  return;
}

void MuonChamberLayout::chamberXY(int sx, int sy, int shx, int shy,
                                  int reg, std::vector<int> &chamberNumber)const {

  int vSize(0); int chN = -1;  bool debug = false;
  int fx = sx + shx;
  int fy = sy + shy;

  if(debug){
    msgStream()<<MSG::INFO<<" Chamber XY. Reg: "<<reg<<" "<<m_cgX.at(reg)<<
      " "<<m_cgY.at(reg)<<" "<<fx<<" "<<fy<<" "<<sx<<" "<<sy<<
      " "<<shx<<" "<<shy<<endmsg;
  }


  //Protect against chambers outside the current region grid
  if(fx<0 || fy<0 || fx >= 4*(int)m_cgX.at(reg)
     || fy >= 4*(int)m_cgY.at(reg)) {

    if(debug) msgStream()<<MSG::INFO<<" Inside Protection IF "<<chN<<endmsg;
    //Starting point in the new (larger) grid
    sy = sy + 2*m_cgY.at(reg);
    if(reg<2) {
      if(sx < 2*(int)m_cgX.at(reg)) { sx = 1; }
      else { sx = 2;}
    }

    //Goes to other region
    if(reg+1<4) {
      sx = (sx / (2/m_cgX.at(reg+1)))+m_cgX.at(reg+1);
      //Checks in larger grid (higher region)
      chamberXY(sx,sy,shx,shy,reg+1,chamberNumber);
    } else {
      //Exits [chamber outside the R4 grid]
      if(debug){
        msgStream()<<MSG::INFO<<" Chamber not Found (>3) "<<chN<<endmsg;
      }

      chamberNumber.push_back(-1);
      return;
    }
    //Fix the chamber number for chambers in region != from starting one
    vSize = chamberNumber.size();
    if(vSize>0) {
      if(debug){
        msgStream()<<MSG::INFO<<" Encode and chamber in first call: "<<chN<<
          " :: "<<chamberNumber.at(vSize-1)<<" "<<chamberNumber.size()
                   <<" "<<reg+1<<endmsg;
      }


      if(chamberNumber.at(vSize-1)>-1) return;
    }
  } else {
    //Look inside the grid

    int enc = fx+4*m_cgX.at(reg)*fy+m_offSet.at(reg);
    chN = m_chamberGrid.at(enc);
    if(debug) {
      msgStream()<<MSG::INFO<<" Encode and chamber in first call: "<<chN<<" "<<
        enc<<" "<<reg<<" "<<fx<<" "<<fy<<endmsg;
    }

  }

  //If a chamber number is <0 this means that
  //-> The chamber is in a different region
  //-> The chamber has NOT been turned on in the xml
  if(chN<0 && shouldLowReg(fx,fy,reg)) {
    //Found a -1 chamber: need to low
    //the region number (if > 1) to look in inside inner region
    if (debug)
      msgStream()<<MSG::INFO<<" chamber number is <0  , REGION : "<< reg<< endmsg;

    if(reg-1>=0) {
      //When dropping down the region number
      if(reg < 3){
        if(fx < 2*(int)m_cgX.at(reg)) { shx = -1; }
        else { shx = 1; }
      } else {
        shx = -2;
      }
      shy = m_cgY.at(reg);
      if(debug){
        msgStream()<<MSG::DEBUG<<" Subcall 2: "<<fx<<" "<<fy<<" "<<shx<<" "<<-shy<<
          " "<<reg-1<<endmsg;
      }

      chamberXY(fx,fy,shx,-shy,reg-1,chamberNumber);
    } else {
      //lowest (inner) region. Exiting
      if(debug){
        msgStream()<<MSG::INFO<<" Chamber not Found (<0) "<<chN<<endmsg;
      }

      chamberNumber.push_back(-1);
      return;
    }
    vSize = chamberNumber.size();
    if (debug) msgStream()<<MSG::INFO<<"chamberStack size: "<< vSize <<endmsg;
    if(vSize>0) {
      if(debug) {
        msgStream()<<MSG::INFO<<" Encode and chamber in second call: "<<chN<<" :: "
                   <<chamberNumber.at(vSize-1)<<" "<<chamberNumber.size()
                   <<" "<<endmsg;
      }

      if(chamberNumber.at(vSize-1)>-1) {
        chN = chamberNumber.at(vSize-1);
      }
    }



    //Second chamber needed for regions 0,1,2
    if(reg <3) {
      if((sy/m_cgY.at(reg) < 1) || (sy/m_cgY.at(reg) > 2)) {
        if(reg-1>0) {
          if(debug) {
            msgStream()<<MSG::INFO<<" Third call "<<endmsg;
          }
          chamberXY(fx,fy,0,0-m_cgY.at(reg),reg-1,chamberNumber);
        } else {
          if(debug){
            msgStream()<<MSG::INFO<<" Chamber not Found "<<endmsg;
          }

          chamberNumber.push_back(-1);
          return;
        }
        vSize = chamberNumber.size();
        if(vSize) {
          if(debug) {
            msgStream()<<MSG::INFO<<" Encode and chamber in third call: "<<chN<<
              " :: "<<chamberNumber.at(vSize-1)<<" "<<
              chamberNumber.size()<<" "<<endmsg;
          }

          if(chamberNumber.at(vSize-1)>-1) return;
        }
      }
    }
  } else if(chN != -1) {
    //Need a +1 to avoid skipping chamber n.1 (idx 0)
    chamberNumber.push_back(chN+m_offSet.at(reg)+1);
    if(debug){
      msgStream()<<MSG::INFO<< "Closing Chamber " << chN
                 << " in R" << reg
                 << " xIndex " << fx
                 << " yIndex " << fy <<endmsg;
    }



  } else {
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
      msgStream()<<MSG::DEBUG<< "Chamber " << chN
                 << " in R" << reg
                 << " xIndex " << fx
                 << " yIndex " << fy
                 << " is not in TES/xml. " <<endmsg;
  }

  return;
}

int MuonChamberLayout::findRegion(int chamber) const {
  int reg(0);
  int offset[4] = {16,48,112,368};
  for(reg=0; reg<4; reg++) {
    if(chamber < offset[reg]) break;
  }
  if(reg>3){
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
      msgStream()<<MSG::DEBUG<<"Region not found for chamber: "<<chamber<<
        ". Go back and check the code!"<<endmsg;
    reg = -1;
  }
  return reg;
}

void MuonChamberLayout::chamberMostLikely(float x,float y, int station,
                                          int& chmb, int& reg)const {

  int myReg(-1);
  int fx(-1),fy(-1);
  if(m_xS.at(station) && m_yS.at(station)) {
    gridPosition(x,y,station,fx,fy,myReg);
  } else {
    msgStream()<<MSG::INFO << "Null chamber dimensions"<< endmsg;
  }
  if(myReg<0){
    reg=-1;
    chmb=-1;
    return;
  }
  //Here's the chamber!!!!
  if(fx > -1 && fy > -1) {
    //Look inside the grid
    int enc = fx+4*m_cgX.at(myReg)*fy+m_offSet.at(myReg);
    chmb = m_chamberGrid.at(enc);
    reg = myReg;
  } else {
    msgStream()<<MSG::INFO <<"Matrix Index problem!!!!"<<endmsg;
  }

  return;
}

//Returns the Tile for a given chamber
LHCb::MuonTileID MuonChamberLayout::tileChamber(DeMuonChamber* chmb)const{

  bool debug = false;
  LHCb::MuonTileID myTile;

  //Region and station got from chamber
  myTile.setStation(chmb->stationNumber());
  unsigned int reg = chmb->regionNumber();
  myTile.setRegion(reg);

  //Gap is not used here
  //myTile.setLayer(0);

  myTile.setLayout(MuonLayout(m_cgX.at(reg),m_cgY.at(reg)));
  if(debug){
    msgStream()<<MSG::INFO<<"TileChamber:: "<<m_cgX.at(reg)<<" "<<m_cgY.at(reg)<<endmsg;
  }

  //Chamber number is needed to find x,y
  unsigned int chN = chmb->chamberNumber();
  unsigned int mychN;
  unsigned int fx(0),fy(0),enc(0);
  for(fx = 0; fx<4*m_cgX.at(reg); fx++){
    for(fy = 0; fy<4*m_cgY.at(reg); fy++){
      enc = fx+4*m_cgX.at(reg)*fy+m_offSet.at(reg);
      mychN = m_chamberGrid.at(enc);
      if(chN == mychN) {
        // quarter definition according to D.Hutchcroft
        //      +y
        //    3 | 0
        //   ---+--- +x
        //    2 | 1

        int myQuarter = -1;
        if(fx >= m_cgX.at(reg)*2) {
          myQuarter = 1;
          if(fy >= m_cgY.at(reg)*2) {
            myQuarter = 0;
          }
        } else {
          myQuarter = 2;
          if(fy >= m_cgY.at(reg)*2) {
            myQuarter = 3;
          }
        }

        if(debug) {
          msgStream()<<MSG::INFO<<"X, Y " <<fx<<" "<<fy<<
            " for chamber "<<chN<<" "<<reg<<endmsg;
        }

        if( myQuarter == 0){
          myTile.setX(fx - m_cgX.at(reg)*2);
          myTile.setY(fy - m_cgY.at(reg)*2);
        }else if ( myQuarter == 1){
          myTile.setX(fx - m_cgX.at(reg)*2);
          myTile.setY(-1 -fy + m_cgY.at(reg)*2);
        }else if ( myQuarter == 2){
          myTile.setX(-1 -fx + m_cgX.at(reg)*2);
          myTile.setY(-1 -fy + m_cgY.at(reg)*2);
        }else if ( myQuarter == 3){
          myTile.setX(-1-fx + m_cgX.at(reg)*2);
          myTile.setY(fy - m_cgY.at(reg)*2);
        }
        myTile.setQuarter(myQuarter);

        if(debug) {
          msgStream()<<MSG::INFO<<"X, Y and Q " <<myTile.nX()<<" "<<
            myTile.nY()<<" "<<myQuarter<<
            " for chamber "<<chN<<" "<<reg<<endmsg;
        }

        return myTile;
      }
    }
  }

  msgStream()<<MSG::INFO<<"X, Y and Q not found! Check for grid problems" <<endmsg;
  return myTile;
}


//Returns the Tile for a given chamber number
LHCb::MuonTileID MuonChamberLayout::tileChamberNumber(int sta, int reg, int chmbNum)const{



  int  encode = 276*sta+chmbNum;
  if(reg==1)encode=encode+12;
  if(reg==2)encode=encode+12+24;
  if(reg==3)encode=encode+12+24+48;
  LHCb::MuonTileID myTile;

  DeMuonChamber* deChmb=m_ChVec[encode];
  if(deChmb) {
    myTile = tileChamber(deChmb);
  }   else {
    msgStream()<<MSG::INFO<<" Could not find the chamber "<<chmbNum<<
      " in the TES. Check the configuration!"<<endmsg;
  }
  return myTile;
}


void MuonChamberLayout::gridPosition(float x, float y, int iS, int &idx,
                                     int &idy, int &reg)const {

  int myIdx(0),myIdy(0),myReg(-1);
  float rX(0),rY(0);
  bool debug = false;
  float xs = m_xS.at(iS);   float ys = m_yS.at(iS);
  //Extracting chamber indexes in the finest grid.
  myIdx = (int)(x/xs);  myIdy = (int)(y/ys);
  rX = x-xs*myIdx; rY = y-ys*myIdy;
  if(debug) {
    msgStream()<<MSG::INFO<< "Rest: "<<rX<<" "<<rY<<" "<<xs<<" "<<ys<<" "
               <<x<<" "<<y<< endmsg;
  }


  for(int iRx = 0; iRx<4; iRx++) {
    if(debug) {
      msgStream()<<MSG::INFO<< "Idx in loop grid: "<<myIdx+2*(int)m_cgX.at(iRx)<<
        " "<<myIdy+2*(int)m_cgY.at(iRx)<<" for region: "<<iRx<<
        endmsg;

    }

    //To be checked fator 2 in last gridX
    if(abs(myIdx)<2*(int)m_cgX.at(iRx) && abs(myIdy)<2*(int)m_cgY.at(iRx)){
      myReg = iRx;
      break;
    } else {
      if(iRx < 2) myIdx /= 2;
    }
  }
  if(myIdy<=0 && rY<0) myIdy--;
  if(myIdx<=0 && rX<0) myIdx--;
  if(myReg>-1){
    idx = myIdx+2*(int)m_cgX.at(myReg);
    idy = myIdy+2*(int)m_cgY.at(myReg);
    reg = myReg;
    if(debug)  {
      msgStream()<<MSG::INFO << "Final Idxs grid: "<<idx<<" "<<idy<<
        " for region: "<<myReg<<endmsg;
    }

  }
  return;
}

void MuonChamberLayout::setGridStep(){
  float dimX[5] = {240.f,301.363f,325.155f,348.947f,372.739f};
  float dimY[5] = {200.f,251.136f,270.962f,290.789f,310.615f};
  for(int iDum = (m_isM1defined?0:1); iDum<5; iDum++) {
    m_xS.push_back(dimX[iDum]);  m_yS.push_back(dimY[iDum]);
  }
}


std::vector<DeMuonChamber*>  MuonChamberLayout::fillChambersVector(IDataProviderSvc* detSvc) {

  int idx(-1),idy(-1),reg(-1);
  bool debug = false;

  int MaxRegions[4] = {12,24,48,192};
  m_ChVec.resize(1380);

  StatusCode sc = StatusCode::SUCCESS;

  SmartDataPtr<DetectorElement> muonSys (detSvc,DeMuonLocation::Default);

  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=muonSys->childBegin();

  //Reset the cache information
  m_logHorizGridX.resize(20);  m_logHorizGridY.resize(20);
  m_logVertGridX.resize(20);   m_logVertGridY.resize(20);
  m_padGridX.resize(20);       m_padGridY.resize(20);
  for(int ir = 0; ir<20; ir++) {
    m_logHorizGridX.at(ir) =   m_logHorizGridY.at(ir) =
      m_logVertGridX.at(ir) =  m_logVertGridY.at(ir) =
      m_padGridX.at(ir) =      m_padGridY.at(ir) = 0;
  }

  //Set the grid steps for the various stations
  setGridStep();
  int vIdx(-1),myvIdx(-1);
  int iS(0),obtIS(0); int encode(0);
  for(itSt=muonSys->childBegin(); itSt<muonSys->childEnd(); itSt++){

    //Are there any void Stations?
    std::string name=((*itSt)->name()).c_str();

    if (name.find("/MF") != name.npos) //if it finds a filter, it skips to
      continue;                           // the next element

    int len=name.size();
    int start=(DeMuonLocation::Default).size();
    std::string substring;
    substring.assign(name,start,len);

    sscanf(substring.c_str(),"/M%d",&obtIS);
    if(debug){
      msgStream()<<MSG::INFO <<"Station Name: "<<(*itSt)->name()<<" ::  "<<obtIS<<endmsg;
    }


    while(iS != obtIS-(m_isM1defined?1:2)) {
      msgStream() << MSG::INFO << "iS number " << iS << " obtIS number " <<
        obtIS << " isM1defined: " << m_isM1defined << endmsg;
      msgStream()<<MSG::WARNING <<"There is/are void stations! "<<endmsg;
      for(int ire = 0; ire<4; ire++) {
        for(int ich = 0; ich<MaxRegions[ire]; ich++) {
          m_ChVec.at(encode) = (DeMuonChamber*)0;
          encode++;
        }
      }
      iS++;
    }
    //getting station side (A or C)
    IDetectorElement::IDEContainer::iterator itSide=(*itSt)->childBegin();
    int iSide=0;
    for(itSide=(*itSt)->childBegin(); itSide<(*itSt)->childEnd(); itSide++){

      //Getting regions
      IDetectorElement::IDEContainer::iterator itRg=(*itSide)->childBegin();
      int iR(0),obtIR(0);
      for(itRg=(*itSide)->childBegin(); itRg<(*itSide)->childEnd();
          itRg++){

        //Are there any void Regions?
        name=((*itRg)->name()).c_str();
        start=(DeMuonLocation::Default).size();
        std::string substringSta;
        substringSta.assign(name,start,3);
        std::string substringReg;
        substringReg.assign(name,start+11,3);
        if(debug){
          msgStream()<<MSG::INFO <<" station path "<<substringSta<<endmsg;
          msgStream()<<MSG::INFO<<" reg path "<<substringReg<<endmsg;
        }

        sscanf(substringSta.c_str(),"/M%d",&obtIS);

        sscanf(substringReg.c_str(),"/R%d",&obtIR);
        if(debug){
          msgStream()<<MSG::INFO <<"Region Name: "<<(*itRg)->name()<<
            " ::  "<<obtIR<<endmsg;
        }

        while(iR != obtIR-1) {
          msgStream()<<MSG::INFO<<"There is/are void regions. "<<endmsg;
          for(int ich = 0+iSide*MaxRegions[iR]/2;
              ich<(iSide+1)*MaxRegions[iR]/2; ich++) {
            //int countCh=276*iS+m_offSet[iR]+
            m_ChVec.at(encode) = (DeMuonChamber*)0;
            encode++;
          }
          iR++;
        }

        //Index needed to fill the cache information
        //There are 4 regions per station: this is not going to change!
        vIdx = iR+iS*4;
        reg=iR;
        //Getting chambers
        int chamCnt = 0;
        IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
        for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){

          DeMuonChamber*  deChmb =  dynamic_cast<DeMuonChamber*>( *itCh
                                                                  ) ;
          int countCh=276*iS+deChmb->chamberNumber();
          if(iR==1)countCh=countCh+12;
          if(iR==2)countCh=countCh+12+24;
          if(iR==3)countCh=countCh+12+24+48;
          if(debug){
            msgStream()<<MSG::INFO<<" ch position "<<countCh<<" "<<iS<<" "<<iR<<" "
                       <<deChmb->chamberNumber()<<endmsg;
          }

          //   if(deChmb->chamberNumber()==4)
          //     std::cout<<" ch position "<<countCh<<" "<<iS<<" "<<iR<<" "<<deChmb->chamberNumber()<<endmsg;
          //    if(deChmb) {
          //      m_ChVec.at(encode) = deChmb;
          //    } else {
          //      m_ChVec.at(encode) = (DeMuonChamber*)0;
          //    }
          if(deChmb) {
            m_ChVec.at(countCh) = deChmb;
          } else {
            m_ChVec.at(countCh) = (DeMuonChamber*)0;
          }

          encode++;

          double myX = (deChmb->geometry())->toGlobal(Gaudi::XYZPoint(0,0,0)).x();
          double myY = (deChmb->geometry())->toGlobal(Gaudi::XYZPoint(0,0,0)).y();
          gridPosition((float)myX,(float)myY,iS,idx,idy,reg);

          int enc = idx+4*m_cgX.at(reg)*idy+m_offSet.at(reg);
          m_chamberGrid.at(enc) = deChmb->chamberNumber();

          if(debug)  {
            msgStream()<<MSG::INFO<<"Chamber initialization: "<<enc<<
              " "<<deChmb->chamberNumber()<<" "<<idx<<" "<<idy<<
              " "<<m_cgX.at(reg)<<" "<<m_offSet.at(reg)<<" "<<myX<<" "
                       <<myY<<endmsg;
          }

          //Try to fill also the other relevant quantities
          //Only when the region changes
          if(vIdx != myvIdx) {
            myvIdx = vIdx;
            sc = fillSystemGrids(deChmb,vIdx,reg);
            if(sc.isFailure()) {
              msgStream()<<MSG::INFO<<
                "Failed to fill the system grid for chamber "
                         <<deChmb->chamberNumber()<<
                " in region "<<reg<<endmsg;}
          }
          chamCnt++;

        }
        //next region
        iR++;
      }
      //next side
    }

    //next station
    iS++;
  }
  if(debug){
    msgStream()<<MSG::INFO<<"Filled chamber vector of size: "<<encode<<endmsg;
  }

  return m_ChVec;
}

bool MuonChamberLayout::shouldLowReg(int idX, int idY, int reg) const {
  bool lower = true;
  bool iXnz = (((idX <  (int)m_cgX.at(reg))&&(idX>0)) ||
               ((idX>=3*(int)m_cgX.at(reg))&&(idX<4*(int)m_cgX.at(reg))));
  bool iYnz = (((idY <  (int)m_cgY.at(reg))&&(idY>0)) ||
               ((idY>=4*(int)m_cgY.at(reg))&&(idX<4*(int)m_cgY.at(reg))));
  if(iXnz && iYnz) {lower = false;}
  return lower;
}


StatusCode MuonChamberLayout::Tile2XYZpos(const LHCb::MuonTileID& tile,
                                          double& x, double& deltax,
                                          double& y, double& deltay,
                                          double& z, double& deltaz){

  bool m_debug = false;

  if( 0 == m_logVertGridX.size() ){
    msgStream()<<MSG::INFO<<
      " The channel / pad grids have not been initialized!!!! Why? "
               <<endmsg;
    fillChambersVector(this->dataSvc());
    msgStream()<<MSG::INFO <<" Called initialization "<<endmsg;
    if( 0 == m_logVertGridX.size() ){
      msgStream()<<MSG::INFO<<" Initialization failed!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }

  // OK how to make this work.....
  // first locate the station and region from the tile
  unsigned int station = tile.station();
  unsigned int region  = tile.region();

  if(m_debug) {
    msgStream()<<MSG::INFO<< "Tile det: " <<tile.layout().xGrid()<<", "
               <<tile.layout().yGrid()<<" in station "<<station<<" and region "
               <<region<< ";Grid details. Reg "<<m_cgX.at(region)<<", "
               <<m_cgY.at(region)<< " ; Pad "<<m_padGridX[station*4 + region]
               <<", "<<m_padGridY[station*4 + region]<< " ; LogH "
               <<m_logHorizGridX[station*4 + region]<<", "
               <<m_logHorizGridY[station*4 + region]<< " ; LogV "
               <<m_logVertGridX[station*4 + region]<<", "
               <<m_logVertGridY[station*4 + region]<< endmsg;
  }

  // now compare the layout parameter to possible "levels"
  // currently chamber, logical channel, pad
  if( m_cgX.at(region) ==  tile.layout().xGrid() &&
      m_cgY.at(region) ==  tile.layout().yGrid() ){

    // chambers
    StatusCode sc = getXYZChamberTile(tile,x,deltax,y,deltay,z,deltaz,true);
    if(!sc.isSuccess()){
      msgStream() << MSG::ERROR << "Failed to get xyz from chamber" << endmsg;
      return sc;
    }
  }
  else if( m_padGridX[station*4 + region] ==
           tile.layout().xGrid() &&
           m_padGridY[station*4 + region] ==
           tile.layout().yGrid() ){

    // muon pads and logical channels with a 1:1 mapping to pads
    if ( m_debug ) {
      msgStream() << MSG::DEBUG
                  << "Found a tile laying out pads"
                  << endmsg;
    }

    StatusCode sc = getXYZPad(tile,x,deltax,y,deltay,z,deltaz);

    if(!sc.isSuccess()){
      msgStream() << MSG::ERROR << "Failed to get xyz from chamber" << endmsg;
      return sc;
    }
  }else if( m_logHorizGridX[station*4 + region] ==
            tile.layout().xGrid() &&
            m_logHorizGridY[station*4 + region] ==
            tile.layout().yGrid() ){

    // horizontal logical channels

    if ( m_debug ) {
      msgStream() << MSG::DEBUG
                  << "Found a tile laying out horizontal logical channels"
                  << endmsg;
    }

    StatusCode sc = getXYZLogical(tile,x,deltax,y,deltay,z,deltaz);

    if(!sc.isSuccess()){
      msgStream() << MSG::ERROR << "Failed to get xyz from chamber" << endmsg;
      return sc;
    }
  }else if( m_logVertGridX[station*4 + region] ==
            tile.layout().xGrid() &&
            m_logVertGridY[station*4 + region] ==
            tile.layout().yGrid() ){

    // vertical logical channels

    if ( m_debug ) {
      msgStream() << MSG::DEBUG
                  << "Found a tile laying out vertical logical channels"
                  << endmsg;
    }

    StatusCode sc = getXYZLogical(tile,x,deltax,y,deltay,z,deltaz);

    if(!sc.isSuccess()){
      msgStream() << MSG::ERROR << "Failed to get xyz from chamber" << endmsg;
      return sc;
    }

  } else if( 1 == tile.layout().xGrid() && 1 == tile.layout().yGrid() ) {

    if ( m_debug ) {
      msgStream() << MSG::DEBUG
                  << "Found a tile laying out Twelfths" << endmsg;
    }

    StatusCode sc = getXYZTwelfth(tile,x,deltax,y,deltay,z,deltaz);
    if(!sc.isSuccess()){
      msgStream() << MSG::ERROR << "Failed to get xyz from twelfth" << endmsg;
      return sc;
    }

  } else {
    msgStream() << MSG::ERROR
                << "Did not understand the LHCb::MuonTileID encoding"
                << " xGrid=" << tile.layout().xGrid()
                << " yGrid=" << tile.layout().yGrid()
                <<endmsg;
    return StatusCode::FAILURE;
  }

  if(m_debug)  msgStream() << MSG::DEBUG << "Output " <<x<<" "<<deltax<<" "<<y<<" "<<deltay<<" "<<z<<" "<<deltaz<<endmsg;

  return StatusCode::SUCCESS;

}


StatusCode MuonChamberLayout::fillSystemGrids(DeMuonChamber *deChmb,
                                              int vIdx, int reg){

  bool debug = false;

  //Getting the grid pointer
  Condition* aGrid = deChmb->condition(deChmb->getGridName());
  MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(aGrid);

  if(!theGrid) {return StatusCode::FAILURE;}

  std::vector<int> readoutType = theGrid->getReadoutGrid();
  std::vector<int> mapType = theGrid->getMapGrid();

  //Phys Channels Vectors
  int grX  =  m_cgX.at(reg)*theGrid->getGrid1SizeX();
  int grY  =  m_cgY.at(reg)*theGrid->getGrid1SizeY();
  int SgrX =  m_cgX.at(reg)*theGrid->getGrid2SizeX();
  int SgrY =  m_cgY.at(reg)*theGrid->getGrid2SizeY();

  //Log channels multipl
  int xm0(0),xm1(0),ym0(0),ym1(0);
  xm0 = mapType.at(0);   ym0 = mapType.at(1);
  if(mapType.size()==4) {
    xm1 = mapType.at(2);   ym1 = mapType.at(3);
  }

  if(debug) {
    msgStream() << MSG::DEBUG << "Grid "<< deChmb->getGridName() 
                <<"  data:: "<<grX/m_cgX.at(reg)<<" "<<grY/m_cgY.at(reg)
                <<" "<<SgrX/m_cgX.at(reg)<<" "<<SgrY/m_cgY.at(reg)
                <<" ; Map:: "<<xm0<<" "<<ym0<<" "<<xm1<<" "<<ym1<<endmsg;
  }

  //Service variables
  int axm0(0),axm1(0),aym0(0),aym1(0);
  int agrX(0),agrY(0),aSgrX(0),aSgrY(0);

  //Only one readout
  if(readoutType.size() == 1) {
    // and only one map
    if( mapType.size() == 2 ){
      // so horiz, vert and pad all same
      m_logHorizGridX.at(vIdx) = grX / xm0;
      m_logHorizGridY.at(vIdx) = grY / ym0;
      m_logVertGridX.at(vIdx) = grX / xm0;
      m_logVertGridY.at(vIdx) = grY / ym0;
      m_padGridX.at(vIdx) = grX / xm0;
      m_padGridY.at(vIdx) = grY / ym0;
    } else {
      //If two maps to be crossed
      if(xm0 < xm1){
        // xm0 is the vertical map, xm1 is the horizontal map
        m_logVertGridX.at(vIdx) = grX / xm0;
        m_logVertGridY.at(vIdx) = grY / ym0;

        m_logHorizGridX.at(vIdx) = grX / xm1;
        m_logHorizGridY.at(vIdx) = grY / ym1;

        m_padGridX.at(vIdx) = grX / xm0;
        m_padGridY.at(vIdx) = grY / ym1;
      } else {
        // xm1 is the vertical map, xm0 is the horizontal map
        m_logVertGridX.at(vIdx) = grX / xm1;
        m_logVertGridY.at(vIdx) = grY / ym1;

        m_logHorizGridX.at(vIdx) = grX / xm0;
        m_logHorizGridY.at(vIdx) = grY / ym0;

        m_padGridX.at(vIdx) = grX / xm1;
        m_padGridY.at(vIdx) = grY / ym0;
      }
    }
  } else if(readoutType.size() == 2) {
    //There are 2 different readouts
    if( !readoutType.at(0) ){
      //First readout is anode readout xm0
      // must be two maps for Andode and cathode seperatly
      if ( grX / xm0 > SgrX / xm1 ){
        axm0 = xm0; axm1 = xm1; aym0 = ym0; aym1 = ym1;
        agrX = grX; agrY = grY; aSgrX = SgrX; aSgrY = SgrY;
      } else {
        axm0 = xm1; axm1 = xm0; aym0 = ym1; aym1 = ym0;
        agrX = SgrX; agrY = SgrY; aSgrX = grX; aSgrY = grY;
      }
    } else {
      //map over was reversed, otherwise the same
      if ( grX / xm1 > xm1 / xm0 ){
        axm0 = xm1; axm1 = xm0; aym0 = ym1; aym1 = ym0;
        agrX = grX; agrY = grY; aSgrX = SgrX; aSgrY = SgrY;
      } else {
        axm0 = xm0; axm1 = xm1; aym0 = ym0; aym1 = ym1;
        agrX = SgrX; agrY = SgrY; aSgrX = grX; aSgrY = grY;
      }
    }

    m_logVertGridX.at(vIdx) = agrX / axm0;
    m_logVertGridY.at(vIdx) = agrY / aym0;

    m_logHorizGridX.at(vIdx) = aSgrX / axm1;
    m_logHorizGridY.at(vIdx) = aSgrY / aym1;

    m_padGridX.at(vIdx) =  agrX / axm0;
    m_padGridY.at(vIdx) = aSgrY / aym1;

  } else {
    msgStream() << MSG::ERROR <<"MuonChamberLayouty                             ERROR  Wrong size readout vector:: "
                <<readoutType.size()<<" != from 1 or 2!!!!"<<endmsg;
  }
  //End of FE work
  return StatusCode::SUCCESS;
}


StatusCode MuonChamberLayout::getXYZChamberTile(const LHCb::MuonTileID& tile,
                                                double& x, double& deltax,
                                                double& y, double& deltay,
                                                double& z, double& deltaz,
                                                bool toGlob){

  unsigned int chamberNum = getChamberNumber(tile);

  unsigned int station = tile.station();
  unsigned int region  = tile.region();

  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
    msgStream() << MSG::DEBUG<<" Get XYZ Chamber Tile " <<chamberNum<<" "<<station<<" "<<region<<endmsg;

  StatusCode sc = getXYZChamber(station,region,chamberNum,
                                x,deltax,y,deltay,z,deltaz,toGlob);
  if(!sc.isSuccess()){
    msgStream() << MSG::ERROR << "Failed to get chamber " << chamberNum << endmsg;
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonChamberLayout::getXYZChamber(const int& station,
                                            const int& region,
                                            const int& chamberNum,
                                            double& x, double& deltax,
                                            double& y, double& deltay,
                                            double& z, double& deltaz,
                                            bool toGlob){
  StatusCode sc = getXYZ(station,region,chamberNum,-1,
                         x,deltax,y,deltay,z,deltaz,toGlob);
  return sc;
}

StatusCode MuonChamberLayout::getXYZ(const int& station,
                                     const int& region,
                                     const int& chamberNum,
                                     const int& gapNum,
                                     double& x, double& deltax,
                                     double& y, double& deltay,
                                     double& z, double& deltaz,
                                     bool toGlob){

  int encode, re(0);   DeMuonChamber* muChamber;
  encode = 276*station+chamberNum;
  int MaxRegions[4] = {12,24,48,192};
  if(region) {
    re = region;
    while(re >= 1) {encode += MaxRegions[re-1]; re--;}
  }
  muChamber = m_ChVec.at(encode);
  
  double Dx(0.),Dy(0.),Dz(0.);
  
  if( -1 == gapNum ){
    
    //I'm intrested in the Chamber, not the gaps
    IGeometryInfo* cInfo = muChamber->geometry();
    Gaudi::Transform3D vTransForm = cInfo->toGlobalMatrix();
    
    Gaudi::XYZVector vtrans;    Gaudi::Rotation3D vrota;
    vTransForm.GetDecomposition(vrota,vtrans);
    
    x = vtrans.x();
    y = vtrans.y();
    z = vtrans.z();
    
    // get ILVolume pointer
    const ILVolume *logVol = cInfo->lvolume();
    // Get the solid
    const ISolid *solid = logVol->solid();
    // check these really are boxes (they ought to be!)
    const SolidBox *box = dynamic_cast<const SolidBox *>(solid);
    if( !box ){
      msgStream() << MSG::ERROR << "Could not cast gas gap solid to box"
                  << endmsg;
      return StatusCode::FAILURE;
    }

    
    const ISolid *GVso = muChamber->getFirstGasGap()->lvolume()->solid();
    // check these really are boxes (they ought to be!)
    const SolidBox *GVbox = dynamic_cast<const SolidBox *>(GVso);
    if( !GVbox ){
      msgStream() << MSG::ERROR << "Could not cast gas gap solid to box"
                  << endmsg;
      return StatusCode::FAILURE;
    }
    
    //Take the deltax and deltay from
    //Gas volume dimension
    Dx   = GVbox->xHalfLength();
    Dy   = GVbox->yHalfLength();
    //While take the deltaz from the whole chamber dimensions
    Dz   = box->zHalfLength();
    
    Gaudi::XYZPoint cnt(0,0,0);
    Gaudi::XYZPoint crn(Dx,Dy,Dz);
    if(toGlob) {
      localToglobal(cInfo,cnt,crn,deltax,deltay,deltaz);
    } else {
      x = y = z = 0;
      deltax = Dx;      deltay = Dy;      deltaz = Dz;
    }
  } else {
    
    // Going down to the needed gap
    // active volume is defined by the gas gaps, want first and last
    IGeometryInfo* cInfo = muChamber->geometry();
    IPVolume* gap=muChamber->getGasGap(gapNum);
    IPVolume* gapLayer=muChamber->getGasGapLayer(gapNum);
    //    bool FoundGap = false;

    if( !gap ){

      msgStream() << MSG::ERROR << "Could not read gas gaps  from TDS" 
                  << endmsg;
      return StatusCode::FAILURE;
    }

    //Found the needed gap
    //FoundGap = true;  
    const ISolid *solid = gap->lvolume()->solid();
    const SolidBox *box = dynamic_cast<const SolidBox *>(solid);
    if( !box ){
      msgStream() << MSG::ERROR << "Could not cast gas gap solid to box"
                  << endmsg;
      return StatusCode::FAILURE;
    }
    
    
    
    //Take the deltax and deltay from
    //Gas volume dimension
    Dx   = box->xHalfLength();
    Dy   = box->yHalfLength();
    //While take the deltaz from the whole chamber dimensions
    Dz   = box->zHalfLength();

    Gaudi::XYZPoint cnt(0,0,0);
    Gaudi::XYZPoint crn(Dx,Dy,Dz);
    
    if(toGlob) {
      Gaudi::XYZPoint pointInCh=gapLayer->toMother(Gaudi::XYZPoint(0,0,0));
      Gaudi::XYZPoint pointInGlobal=muChamber->geometry()->toGlobal(pointInCh);
      Gaudi::XYZPoint crnInCh=gapLayer->toMother(crn);
      x=pointInGlobal.x();
      y=pointInGlobal.y();
      z=pointInGlobal.z();
	
      localToglobal(cInfo,pointInCh,crnInCh,deltax,deltay,deltaz);
    } else {
      x = y = z = 0;
      deltax = Dx;      deltay = Dy;      deltaz = Dz;
    }    
  }
  return StatusCode::SUCCESS;
}


StatusCode MuonChamberLayout::getXYZPad(const LHCb::MuonTileID& tile,
                                        double& x, double& deltax,
                                        double& y, double& deltay,
                                        double& z, double& deltaz){

  // to find the x,y,z of the pad one must first find the chamber
  // to find the chamber one must know the tile of the chamber
  unsigned int station = tile.station();
  unsigned int region  = tile.region();

  bool m_debug = false;

  // locate the chamber
  LHCb::MuonTileID chamTile = m_layout[region].contains(tile);
  unsigned int chamb = getChamberNumber(chamTile);


  // chamber size in x and y (z is same as pad)
  double cx,cy,cDeltax,cDeltay;
  double Dx(0.),Dy(0.),Dz(0.);

  // number of pads or logical channels in a chamber is :
  // layout.xGrid() / chamberLayoutX , padLayoutY / chamberLayoutY
  int xRatio = tile.layout().xGrid() / m_cgX.at(region);
  int yRatio = tile.layout().yGrid() / m_cgY.at(region);

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

  if ( m_debug ) {
    msgStream() << MSG::DEBUG << "Pad xOffset within chamber:" << xOffset
                << " yOffset within chamber:" << yOffset
                << " xratio: " << xRatio << " yratio: " << yRatio
                << endmsg;
  }


  //gap num??
  int gapNum = -1;
  //Here only gets the right deltas and offsets to current PAD.
  StatusCode sc = getXYZ(station,region,chamb,gapNum,cx,cDeltax,cy,cDeltay,z,Dz,false);

  if(!sc.isSuccess()){
    msgStream() << MSG::ERROR << "Failed to get chamber for pad tile: " << tile
                << endmsg;
    return sc;
  }


  // now make the calculations of x,y and z (and dx, dy and dz)
  // in chamber reference frame.
  x = ( cx - cDeltax ) +
    ( ( static_cast<double>(xOffset) / static_cast<double>(xRatio) ) *
      (2.0 * cDeltax ) );
  y = ( cy - cDeltay ) +
    ( ( static_cast<double>(yOffset) / static_cast<double>(yRatio) ) *
      (2.0 * cDeltay ) );

  if ( m_debug ) 
    msgStream() << MSG::DEBUG <<" getXYZPad:: XY tem info  "
                <<xOffset<<" "<<yOffset <<" "<<cx<<" "<<cDeltax<<" "
                <<cy<<" "<<cDeltay<<" "<<x<<" "<<y<<endmsg;

  Dx = cDeltax / (static_cast<double>(xRatio));
  Dy = cDeltay / (static_cast<double>(yRatio));

  // correct to the center of the pad
  x = x + Dx;
  y = y + Dy;

  if ( m_debug )
    msgStream() << MSG::DEBUG <<" getXYZPad:: XY info  "
                <<x<<" "<<Dx<<" "<<y<<" "<<Dy<<endmsg;

  Gaudi::XYZPoint cnt(x,y,z);
  Gaudi::XYZPoint crn(x+Dx,y+Dy,z+Dz);

  std::vector<LHCb::MuonTileID> mytiles;  mytiles.push_back(chamTile);
  std::vector<DeMuonChamber*> myChs = createChambersFromTile(mytiles);

  //Correct XYZ and Deltas for local to Global transformation
  deltax = Dx;  deltay = Dy;  deltaz = Dz;
  if(!myChs.empty()) {

    DeMuonChamber* muChamber = myChs.at(0);
    IGeometryInfo* cInfo = muChamber->geometry();
    //Those are not used in deltas computation
    x = cInfo->toGlobal(cnt).x();
    y = cInfo->toGlobal(cnt).y();
    z = cInfo->toGlobal(cnt).z();

    localToglobal(cInfo,cnt,crn,deltax,deltay,deltaz);
  }

  if ( m_debug )
    msgStream() << MSG::DEBUG <<" getXYZPad:: loc to glob "
                <<Dx<<" "<<Dy<<" "<<Dz<<" "<<myChs.at(0)->name()<<" "
                <<deltax<<" "<<deltay<<" "<<deltaz<<endmsg;

  return StatusCode::SUCCESS;
}


StatusCode MuonChamberLayout::getXYZLogical(const LHCb::MuonTileID& tile,
                                            double& x, double& deltax,
                                            double& y, double& deltay,
                                            double& z, double& deltaz){

  // If we get here then the logical strip is potenitally bigger than a chamber
  // if not then we can subcontract to getXYZPad directly

  // to find the x,y,z of the pad one must first find the chamber
  // to find the chamber one must know the tile of the chamber

  unsigned int station = tile.station();
  unsigned int region  = tile.region();
  bool m_debug = false;

  if( tile.layout().xGrid() >= m_layout[region].xGrid() &&
      tile.layout().yGrid() >= m_layout[region].yGrid() ) {
    // ok logical channels are within a single chamber, delegate to getXYZpad
    StatusCode sc = getXYZPad(tile,x,deltax,y,deltay,z,deltaz);
    if(!sc.isSuccess()){
      msgStream() << MSG::ERROR << "Passed a logical channel to getXYZPad: failed"
                  << " tile ID=" << tile << endmsg;
      return sc;
    }
  }else if(tile.layout().xGrid() >= m_layout[region].xGrid() &&
           tile.layout().yGrid() < m_layout[region].yGrid() ) {
    // This logical channel crosses chambers in y, break it down into
    // chamber hight pads then combine the chamber extents later
    // number of tiles to make:
    int nTile = m_layout[region].yGrid() / tile.layout().yGrid();
    if ( m_debug ) {
      msgStream() << MSG::DEBUG << "Making " << nTile << " temporary LHCb::MuonTileIDs"
                  << " to get all chamber locations"
                  << endmsg;
    }

    // width of pad (in x) is the same, hight (yGrid) that of the chamber
    MuonLayout tempPadLayout(tile.layout().xGrid(),m_layout[region].yGrid());

    std::vector<LHCb::MuonTileID> tempTiles;

    int i;
    for( i=0 ; i<nTile ; i++ ){
      int yTile = ( tile.nY() * nTile ) + i ;
      LHCb::MuonTileID tTile(station,tempPadLayout,region,tile.quarter(),
                             tile.nX(),yTile);
      tempTiles.push_back(tTile);
    }

    // loop over the tiles and get max/min x,y,z of the combinations
    double xmin = 0.0;
    double xmax = 0.0;
    double ymin = 0.0;
    double ymax = 0.0;
    double zmin = 0.0;
    double zmax = 0.0;
    double padDx,chamDy,chamDz;
    std::vector<LHCb::MuonTileID>::const_iterator iTile;
    for(iTile = tempTiles.begin() ; iTile != tempTiles.end() ; iTile++){
      double lx,ly,lz;
      StatusCode sc = getXYZPad(*iTile,lx,padDx,ly,chamDy,lz,chamDz);
      if(!sc.isSuccess()){
        msgStream() << MSG::ERROR << "Passed a temporary channel to getXYZPad: failed"
                    << " tile ID=" << *iTile << endmsg;
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
    msgStream() << MSG::WARNING
                << "You requested a logical channel wider than a chamber, failing"
                << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}



StatusCode MuonChamberLayout::getXYZTwelfth(const LHCb::MuonTileID& tile,
                                            double& x, double& deltax,
                                            double& y, double& deltay,
                                            double& z, double& deltaz){
  unsigned int station = tile.station();
  unsigned int region  = tile.region();
  unsigned int quarter = tile.quarter();

  unsigned int twelfth;
  if(0 == quarter){
    if(1 == tile.nX() && 0 == tile.nY()){
      twelfth = 0;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 1;
    }else{
      twelfth = 2;
    }
  }else if(1 == quarter){
    if(0 == tile.nX() && 1 == tile.nY()){
      twelfth = 3;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 4;
    }else{
      twelfth = 5;
    }
  }else if(2 == quarter){
    if(1 == tile.nX() && 0 == tile.nY()){
      twelfth = 6;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 7;
    }else{
      twelfth = 8;
    }
  }else{
    if(0 == tile.nX() && 1 == tile.nY()){
      twelfth = 9;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 10;
    }else{
      twelfth = 11;
    }
  }

  double xMax(0.0),xMin(0.0),yMax(0.0),yMin(0.0),zMax(0.0),zMin(0.0);

  int chamCorner; // only need 3 corners here
  for( chamCorner = 0 ; chamCorner < 4 ; chamCorner++ ){
    int nChamber = getTwelfthCorner(region,twelfth,chamCorner);
    // get the chamber
    StatusCode sc = getXYZChamber(station,region,nChamber,
                                  x,deltax,y,deltay,z,deltaz,true);
    if(!sc.isSuccess()){
      msgStream() << MSG::ERROR << "Could not get corner chamber" << endmsg;
      return sc;
    }

    if(0 == chamCorner) {
      xMax = x; xMin = x; yMax = y; yMin = y; zMax = z; zMin = z;
    }else{
      if ( x > xMax ) { xMax = x; }
      if ( x < xMin ) { xMin = x; }
      if ( y > yMax ) { yMax = y; }
      if ( y < yMin ) { yMin = y; }
      if ( z > zMax ) { zMax = z; }
      if ( z < zMin ) { zMin = z; }
    }
  } // end loop over chamCorner

  // strore the extent of the twelfth
  x =  ( xMax + xMin ) / 2.;
  y =  ( yMax + yMin ) / 2.;
  z =  ( zMax + zMin ) / 2.;

  deltax = (( xMax - xMin ) / 2.) + deltax;
  deltay = (( yMax - yMin ) / 2.) + deltay;
  deltaz = (( zMax - zMin ) / 2.) + deltaz;

  return StatusCode::SUCCESS;
}

int MuonChamberLayout::getChamberNumber(const LHCb::MuonTileID& tile){

  // look-up chamber number from tile
  // use the fact that the chamber numbering is constitent between stations

  unsigned int chamberNumber = 0; // counts from 1 as per TDS

  bool m_debug = false;

  //Look inside the grid
  if(m_debug)
    msgStream() << MSG::DEBUG <<" getChamberNumber:: Bef corr.   "
                <<tile.region()<<" "<<tile.nX()<<" "<<tile.nY()<<" "
                <<m_cgX.at(tile.region())<<" "<<m_offSet.at(tile.region())<<endmsg;
  int fx(0),fy(0);
  //Correct for quarter information
  if( tile.quarter() == 0){
    fx = tile.nX() + m_cgX.at(tile.region())*2;
    fy = tile.nY() + m_cgY.at(tile.region())*2;
  } else if ( tile.quarter() == 1){
    fx = tile.nX() + m_cgX.at(tile.region())*2;
    fy = - 1 - tile.nY() + m_cgY.at(tile.region())*2;
  } else if ( tile.quarter() == 2){
    fx = - 1 - tile.nX() + m_cgX.at(tile.region())*2;
    fy = - 1 - tile.nY() + m_cgY.at(tile.region())*2;
  } else if ( tile.quarter() == 3){
    fx = - 1 - tile.nX() + m_cgX.at(tile.region())*2;
    fy = tile.nY() + m_cgY.at(tile.region())*2;
  }

  int enc = fx+4*m_cgX.at(tile.region())*fy+m_offSet.at(tile.region());
  chamberNumber = m_chamberGrid.at(enc);

  if ( m_debug ) {
    msgStream() << MSG::DEBUG <<" getChamberNumber:: chNum  "
                <<chamberNumber<<" "<<enc<<" "<<fx<<" "<<fy<<" "
                <<m_cgX.at(tile.region())<<" "<<m_offSet.at(tile.region())<<endmsg;

    msgStream() << MSG::DEBUG << "Chamber " << chamberNumber
                << " in R" << tile.region()+1
                << " xIndex " << tile.nX()
                << " yIndex " << tile.nY()
                << " was found for tile " << tile << endmsg;
  }
  if(chamberNumber>1380){
    msgStream() << MSG::ERROR << "Chamber " << chamberNumber
                << " in R" << tile.region()+1
                << " xIndex " << tile.nX()
                << " yIndex " << tile.nY()
                << " was found for tile " << tile << endmsg;
    chamberNumber = 0;
  }
  return chamberNumber;
}

int MuonChamberLayout::getTwelfthCorner(const int& region,
                                        const int& twelfth,
                                        const int& chamberNum){
  int xPos=0;  int yPos=0;
  getTwelfthCornerIndex(region,twelfth,chamberNum,xPos,yPos);

  // get the actual chamber number
  std::vector<int> chamberNumber;
  chamberXY(xPos,yPos,0,0, region, chamberNumber);
  int ActChamber = chamberNumber.at(0);

  return ActChamber;

}

void MuonChamberLayout::getTwelfthCornerIndex(const int& region,
                                              const int& twelfth,
                                              const int& chamberNum,
                                              int &xPos, int &yPos){
  // chambers defining the corners of the twelfths
  // here the indexing in region, quater, twelfth
  // numbering clockwise:
  //
  //  +-------+ +-------+           ChamberNum
  //  | 4   3 | | 2   1 |           counts (no particular reason)
  //  |   +---+ +---+   |           +------+
  //  | 5 |         | 0 |           | 2  3 |
  //  +---+         +---+           | 0  1 |
  //  +---+         +---+           +------+
  //  | 6 |         |11 |
  //  |   +---+ +---+   |
  //  | 7   8 | | 9  10 |
  //  +-------+ +-------+
  // map to whole station numbering scheme

  if(0 == twelfth){
    xPos   = 3*m_cgX.at(region);
    yPos   = 2*m_cgY.at(region);
  }else if(1 == twelfth){
    xPos   = 3*m_cgX.at(region);
    yPos   = 3*m_cgY.at(region);
  }else if(2 == twelfth){
    xPos   = 2*m_cgX.at(region);
    yPos   = 3*m_cgY.at(region);
  }else if(3 == twelfth){
    xPos   = 1*m_cgX.at(region);
    yPos   = 3*m_cgY.at(region);
  }else if(4 == twelfth){
    xPos   = 0;
    yPos   = 3*m_cgY.at(region);
  }else if(5 == twelfth){
    xPos   = 0;
    yPos   = 2*m_cgY.at(region);
  }else if(6 == twelfth){
    xPos   = 0;
    yPos   = 1*m_cgY.at(region);
  }else if(7 == twelfth){
    xPos   = 0;
    yPos   = 0;
  }else if(8 == twelfth){
    xPos   = 1*m_cgX.at(region);
    yPos   = 0;
  }else if(9 == twelfth){
    xPos   = 2*m_cgX.at(region);
    yPos   = 0;
  }else if(10 == twelfth){
    xPos   = 3*m_cgX.at(region);
    yPos   = 0;
  }else{
    xPos   = 3*m_cgX.at(region);
    yPos   = 1*m_cgY.at(region);
  }

  // correct to positions of 1,2,3
  if(1 == chamberNum){
    xPos = xPos + (m_cgX.at(region)-1);
  }else if(2 == chamberNum){
    yPos = yPos + (m_cgY.at(region)-1);
  }else if(3 == chamberNum){
    xPos = xPos + (m_cgX.at(region)-1);
    yPos = yPos + (m_cgY.at(region)-1);
  }
}


void MuonChamberLayout::localToglobal(IGeometryInfo* gInfo,
                                      Gaudi::XYZPoint cent, Gaudi::XYZPoint corn,
                                      double &dx, double &dy, double &dz){

  double GctrX(0.),GctrY(0.),GctrZ(0.),GcrnX(0.),GcrnY(0.),GcrnZ(0.);

  GctrX = gInfo->toGlobal(cent).x();
  GctrY = gInfo->toGlobal(cent).y();
  GctrZ = gInfo->toGlobal(cent).z();

  GcrnX = gInfo->toGlobal(corn).x();
  GcrnY = gInfo->toGlobal(corn).y();
  GcrnZ = gInfo->toGlobal(corn).z();

  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
    msgStream() << MSG::DEBUG
                << "Local to Global: gCtr:: "<<GctrX<<" "<<GctrY<<" "<<GctrZ
                <<" ; gCrn:: "<<GcrnX<<" "<<GcrnY<<" "<<GcrnZ<<endmsg;

  dx = fabs(GctrX - GcrnX);
  dy = fabs(GctrY - GcrnY);
  dz = fabs(GctrZ - GcrnZ);

  return;

}

std::vector<DeMuonChamber*> MuonChamberLayout::
createChambersFromTile(std::vector<LHCb::MuonTileID> mytiles){

  std::vector<LHCb::MuonTileID>::iterator idTile;
  std::vector<DeMuonChamber*> myChambers;
  int station, region, chamb;
  bool debug = false;
  int MaxRegions[4] = {12,24,48,192};
  int encode(0), re(0);

  for(idTile = mytiles.begin(); idTile<mytiles.end(); idTile++){
    const long int cTile = *idTile;
    station = idTile->station();
    region = idTile->region();
    chamb = getChamberNumber(*idTile);
    //Added protection against non existing chamber
    if(cTile>0) {
      if(debug)
        msgStream() << MSG::DEBUG <<"Returned chambers:: "<<station
                    <<" "<<region
                    <<" "<<chamb<<endmsg;
      encode = 276*station+chamb;
      if(region) {
        re = region;
        while(re >= 1) {encode += MaxRegions[re-1]; re--;}
      }

      DeMuonChamber * myChmb = m_ChVec.at(encode);
      myChambers.push_back(myChmb);
    }
  }

  return myChambers;
}


std::vector<unsigned int> MuonChamberLayout::
Tile2ChamberNum(const LHCb::MuonTileID& tile){

  bool m_debug = false;

  m_chaVect.resize(1);

  if( 0 == m_logVertGridX.size() ){
    msgStream() << MSG::INFO <<" The channel / pad grids have not been initialized!!!! Why? "<<endmsg;
    fillChambersVector(this->dataSvc());
    msgStream() << MSG::INFO <<" Called initialization "<<endmsg;
    if( 0 == m_logVertGridX.size() ){
      msgStream() << MSG::INFO <<" Initialization failed!"<<endmsg;
      return m_chaVect;
    }
  }

  // OK how to make this work.....
  // first locate the station and region from the tile
  unsigned int station = tile.station();
  unsigned int region  = tile.region();

  if(m_debug)
    msgStream() << MSG::DEBUG <<"Tile det: " <<tile.layout().xGrid()
                <<", "<<tile.layout().yGrid()
                <<" in station "<<station
                <<" and region "<<region
                <<";Grid details. Reg "<<m_cgX.at(region)
                <<", "<<m_cgY.at(region)
                <<" ; Pad "<<m_padGridX[station*4 + region]
                <<", "<<m_padGridY[station*4 + region]
                <<" ; LogH "<<m_logHorizGridX[station*4 + region]
                <<", "<<m_logHorizGridY[station*4 + region]
                <<" ; LogV "<<m_logVertGridX[station*4 + region]
                <<", "<<m_logVertGridY[station*4 + region]<<endmsg;

  // now compare the layout parameter to possible "levels"
  // currently chamber, logical channel, pad
  if( m_cgX.at(region) ==  tile.layout().xGrid() &&
      m_cgY.at(region) ==  tile.layout().yGrid() ){

    // chambers

    if ( m_debug )
      msgStream() << MSG::DEBUG << "Found a tile covering a chamber" << endmsg;
    m_chaVect.at(0) = getChamberNumber(tile);
    ///    StatusCode sc = getXYZChamberTile(tile,x,deltax,y,deltay,z,deltaz,true);
  }
  else if( m_padGridX[station*4 + region] == tile.layout().xGrid() &&
           m_padGridY[station*4 + region] == tile.layout().yGrid() ){

    // muon pads and logical channels with a 1:1 mapping to pads

    if ( m_debug )
      msgStream() << MSG::DEBUG << "Found a tile laying out pads" <<endmsg;

    LHCb::MuonTileID chamTile = m_layout[region].contains(tile);
    m_chaVect.at(0) = getChamberNumber(chamTile);
    ///    StatusCode sc = getXYZPad(tile,x,deltax,y,deltay,z,deltaz);
  }
  else if( m_logHorizGridX[station*4 + region] == tile.layout().xGrid() &&
           m_logHorizGridY[station*4 + region] == tile.layout().yGrid() ){

    // horizontal logical channels

    m_chaVect = Logical2ChamberNum(tile);
  }
  else if( m_logVertGridX[station*4 + region] == tile.layout().xGrid() &&
           m_logVertGridY[station*4 + region] == tile.layout().yGrid() ){

    // vertical logical channels

    if ( m_debug ) {
      msgStream() << MSG::DEBUG << "Found a tile laying out vertical logical channels" <<endmsg;
    }
    m_chaVect = Logical2ChamberNum(tile);
  }
  else if( 1 == tile.layout().xGrid() && 1 == tile.layout().yGrid() ) {

    // tile laying out Twelfths

    if ( m_debug )
      msgStream() << MSG::DEBUG << "Found a tile laying out Twelfths" <<endmsg;

    m_chaVect = Twelfth2ChamberNum(tile);
  }
  else {
    msgStream() << MSG::ERROR
                << "Did not understand the LHCb::MuonTileID encoding"
                << " xGrid=" << tile.layout().xGrid()
                << " yGrid=" << tile.layout().yGrid()
                <<endmsg;
  }

  return m_chaVect;
}


std::vector<unsigned int> MuonChamberLayout::
Logical2ChamberNum(const LHCb::MuonTileID& tile){

  // If we get here then the logical strip is potenitally bigger than a chamber
  // if not then we can subcontract to getXYZPad directly

  // to find the x,y,z of the pad one must first find the chamber
  // to find the chamber one must know the tile of the chamber

  std::vector<unsigned int> chamberVect;

  unsigned int station = tile.station();
  unsigned int region  = tile.region();
  bool m_debug = false;

  if( tile.layout().xGrid() >= m_layout[region].xGrid() &&
      tile.layout().yGrid() >= m_layout[region].yGrid() ) {

    // ok logical channels are within a single chamber
    LHCb::MuonTileID chamTile = m_layout[region].contains(tile);
    unsigned int chamb = getChamberNumber(chamTile);
    chamberVect.resize(1);
    chamberVect.at(0)=chamb;
  }
  else if(tile.layout().xGrid() >= m_layout[region].xGrid() &&
          tile.layout().yGrid() < m_layout[region].yGrid() ) {
    // This logical channel crosses chambers in y, break it down into
    // chamber hight pads then combine the chamber extents later
    // number of tiles to make:
    int nTile = m_layout[region].yGrid() / tile.layout().yGrid(); // number of chamber crossed

    // width of pad (in x) is the same, hight (yGrid) that of the chamber
    MuonLayout tempPadLayout(tile.layout().xGrid(),m_layout[region].yGrid());

    std::vector<LHCb::MuonTileID> tempTiles;

    int i;
    for( i=0 ; i<nTile ; i++ ){
      int yTile = ( tile.nY() * nTile ) + i ;
      LHCb::MuonTileID tTile(station,tempPadLayout,region,tile.quarter(),tile.nX(),yTile);
      if(m_debug) {
        msgStream() << MSG::DEBUG << "i: " << i
                    << "  nX: " << tile.nX()
                    << "  nY: " << yTile << endmsg;
      }
      tempTiles.push_back(tTile);
    }

    // loop over the tiles and get the their chamber numbers

    std::vector<LHCb::MuonTileID>::const_iterator iTile;
    for(iTile = tempTiles.begin() ; iTile != tempTiles.end() ; iTile++){
      LHCb::MuonTileID chamTile = m_layout[region].contains(*iTile);
      unsigned int chamb = getChamberNumber(chamTile);
      chamberVect.push_back(chamb);
    }
  }
  else{
    msgStream() << MSG::WARNING
                << "You requested a logical channel wider than a chamber, failing"
                << endmsg;
  }
  return chamberVect;
}


std::vector<unsigned int> MuonChamberLayout::
Twelfth2ChamberNum(const LHCb::MuonTileID& tile){
  unsigned int region  = tile.region();
  unsigned int quarter = tile.quarter();
  std::vector<unsigned int> chamberVect;

  unsigned int twelfth;
  if(0 == quarter){
    if(1 == tile.nX() && 0 == tile.nY()){
      twelfth = 0;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 1;
    }else{
      twelfth = 2;
    }
  }else if(1 == quarter){
    if(0 == tile.nX() && 1 == tile.nY()){
      twelfth = 3;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 4;
    }else{
      twelfth = 5;
    }
  }else if(2 == quarter){
    if(1 == tile.nX() && 0 == tile.nY()){
      twelfth = 6;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 7;
    }else{
      twelfth = 8;
    }
  }else{
    if(0 == tile.nX() && 1 == tile.nY()){
      twelfth = 9;
    }else if(1 == tile.nX() && 1 == tile.nY()){
      twelfth = 10;
    }else{
      twelfth = 11;
    }
  }

  int chamCorner; // only need 3 corners here
  int nChamber;
  unsigned int vdim=0;
  for( chamCorner = 0 ; chamCorner < 4 ; chamCorner++ ){
    nChamber = getTwelfthCorner(region,twelfth,chamCorner);
    // get the chamber
    vdim++;
    chamberVect.resize(vdim);
    chamberVect.at(vdim)=nChamber;
  }
  return chamberVect;
}

