// $Id: DeMuonDetector.cpp,v 1.2 2005-10-28 09:55:34 asarti Exp $

// Include files
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/MuonChamberLayout.h"

//Detector description
#include "DetDesc/IGeometryInfo.h"

//Muon Kernel
#include "MuonKernel/MuonTileID.h"

/** @file DeMuonDetector.cpp
 * 
 * Implementation of class : DeMuonDetector
 *
 * @author Alessia Satta
 * @author Alessio Sarti, asarti@lnf.infn.it
 *
 */

#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef XmlUserDetElemCnv<DeMuonDetector>       XmlDeMuonDetector;
static CnvFactory<XmlDeMuonDetector>           s_XmlDeMuonDetectorFactory;
const ICnvFactory&  XmlDeMuonDetectorFactory = s_XmlDeMuonDetectorFactory;

/// Standard Constructor
DeMuonDetector::DeMuonDetector() {

  bool debug = false;
  if(debug) std::cout<< "Building the Detector !!!" <<std::endl;

  m_detSvc = this->dataSvc();
}

/// Standard Destructor
DeMuonDetector::~DeMuonDetector()
{
}
  
const CLID& DeMuonDetector::clID () const 
{ 
  return DeMuonDetector::classID() ; 
}

StatusCode DeMuonDetector::initialize()  
{

  MsgStream msg( msgSvc(), name() );
  msg << MSG::VERBOSE << "Initializing the detector" <<endreq;

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  //Initializing the Layout
  m_chamberLayout.initialize();

  m_chamberLayout.setDataProvider(m_detSvc);

  m_chamberLayout.fillChambersVector(m_detSvc);
  return sc;
}


StatusCode DeMuonDetector::Hit2ChamberNumber(HepPoint3D myPoint, 
					     int station, 
					     int & chamberNumber, int& regNum){

  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; bool debug = false;

  MsgStream msg( msgSvc(), name() );
  double x = myPoint.x();  double y = myPoint.y(); const double z = myPoint.z();
  //Returning the most likely chamber
  m_chamberLayout.chamberMostLikely(x,y,station,chamberNumber,regNum);

  //Providing all 3 numbers identifies a chamber
  IGeometryInfo* geoChm = (this->ReturnADetElement(station,regNum,chamberNumber))->geometry();  

  //For calls not providing hit z take the z of chamber center
  if(!z) {myPoint.setZ(geoChm->toGlobal(HepPoint3D(0,0,0)).z());}

  //Is the chamber returned containing the hit?
  isIn = geoChm->isInside(myPoint);

  if(isIn) {
    msg << MSG::DEBUG << "Hit found" <<endreq;
    sc = StatusCode::SUCCESS;
  } else {
    //Find the vector of chambers near the one under investigation
    double x_ref(0),y_ref(0); std::vector<DeMuonChamber*> myChams;
    x_ref = geoChm->toGlobal(HepPoint3D(0,0,0)).x();
    y_ref = geoChm->toGlobal(HepPoint3D(0,0,0)).y();

    int x_sgn(0),y_sgn(0);
    //Avoid unnecessary checks for resolution problems
    if(fabs(x-x_ref)>0.01) {x_sgn = (x-x_ref)/fabs(x-x_ref);}
    if(fabs(y-y_ref)>0.01) {y_sgn = (y-y_ref)/fabs(y-y_ref);}

    msg << MSG::DEBUG << "Hit found Try to look in corner "<<x_ref<< " "<<x<<" "<<x_sgn<<" "<<y_ref<<" "<<y<<" "<<y_sgn<<endreq;    

    myChams = m_chamberLayout.neighborChambers(chamberNumber,station,regNum,x_sgn,y_sgn);
    std::vector<DeMuonChamber*>::iterator aChamber;

    //Loops on found chambers
    for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){
      IGeometryInfo* geoOthChm = (*aChamber)->geometry();

      //For calls not providing hit z take the z of chamber center
      if(!z) {myPoint.setZ(geoOthChm->toGlobal(HepPoint3D(0,0,0)).z());}

      isIn = geoOthChm->isInside(myPoint);
      if(isIn) {
	sc = StatusCode::SUCCESS;
	//Returns the correct region and chamber number
	chamberNumber = (*aChamber)->chamberNumber();
	regNum = (*aChamber)->regionNumber();
	break;
      }
    }

    //Debug the chambers returned
    if(debug) {
      std::cout<<"Chmb test n.  :"<<chamberNumber+1<<"  ";
      std::cout<<"Chmb ret.  : ";
      for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){
	std::cout<<(*aChamber)->chamberNumber()+1<<" ";
      }
      std::cout<<" "<<std::endl;
    }
  }
  if(!isIn) {
    msg << MSG::WARNING << "Smart chamber seeking didn't work. Go on and perform full loop on all chambers :( !!! " <<endreq;
    int msta(0),mreg(0),mchm(0); 
    //Getting stations
    IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
    for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
      if(msta == station) {
	//Getting regions
	mreg = 0;
	IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
	for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
	  //Getting chambers
	  mchm = 0;
	  IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
	  for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
	    IGeometryInfo* geoAllChm = (*itCh)->geometry();  

	    //For calls not providing hit z take the z of chamber center
	    if(!z) {myPoint.setZ(geoAllChm->toGlobal(HepPoint3D(0,0,0)).z());}

	    isIn = geoAllChm->isInside(myPoint);
	    if(isIn) {
	      sc = StatusCode::SUCCESS;
	      //Returns the correct region and chamber number
	      chamberNumber = mchm;  regNum = mreg;
	      goto endFloop;
	    }
	    mchm++;
	  }//Chamber Loop
	  mreg++;
	}//Region Loop
      }
      msta++;
    }//Stations Loop
  endFloop:
    msg << MSG::WARNING << "Hit found in  chamber: " <<chamberNumber<<" , R: "<<regNum<<endreq;
  }  
  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberNumber(const double x,
					     const double y,
					     int station,
					     int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  HepPoint3D hitPoint(x,y,0); 

  sc = Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);

  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberNumber(const double x,
					     const double y,
					     const double z,
					     int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Z is know/provided.
  HepPoint3D hitPoint(x,y,z);   int sta = getStation(z);

  sc = Hit2ChamberNumber(hitPoint,sta,chamberNumber,regNum);

  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberTile(const double x,
					   const double y,
					   const double z,
					   MuonTileID& tile){
  StatusCode sc = StatusCode::SUCCESS;
  int dumChmb(-1), reg(-1);

  //Return the chamber number
  sc = Pos2ChamberNumber(x,y,z,dumChmb,reg);

  //Convert chamber number into a tile
  tile = m_chamberLayout.tileChamberNumber(getStation(z),reg,dumChmb);

  return sc;
}

IDetectorElement* DeMuonDetector::ReturnADetElement(int lsta, int lreg, int lchm) {
  
  IDetectorElement* myDet;
  int msta(0),mreg(0),mchm(0); 

  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    if((msta == lsta) && (lreg == -1) && (lchm == -1)) {
      myDet = *itSt;
      goto exit;
    } 
    //Getting regions
    mreg = 0;
    IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
    for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
      if((msta == lsta) && (mreg == lreg) && (lchm == -1)) {
	myDet = *itRg;
	goto exit;
      } 
      //Getting chambers
      mchm = 0;
      IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
      for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
	
	if((msta == lsta) && (mreg == lreg) && (mchm == lchm)) {
	  myDet = *itCh;
	  goto exit;
	} 
	mchm++;
      }//Chamber Loop
      mreg++;
    }//Region Loop
    msta++;
  }//Stations Loop
  
 exit:
  
  return myDet;
  
}
