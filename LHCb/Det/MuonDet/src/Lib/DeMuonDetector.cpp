// $Id: DeMuonDetector.cpp,v 1.47 2009-09-09 12:59:56 cattanem Exp $

// Include files
#include "MuonChamberLayout.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonChamberGrid.h"
#include "MuonDet/DeMuonRegion.h"

//Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

/** @file DeMuonDetector.cpp
 * 
 * Implementation of class : DeMuonDetector
 *
 * @author Alessia Satta
 * @author Alessio Sarti, asarti@lnf.infn.it
 *
 */

/// Standard Constructor
DeMuonDetector::DeMuonDetector() {

  bool debug = false;
  if(debug) std::cout<< "Building the Detector !!!" <<std::endl;
  m_detSvc = 0;
  m_stations = 0;
  m_regions = 0;
  m_chamberLayout = 0;
    m_hitNotInGap = 0;

}

/// Standard Destructor
DeMuonDetector::~DeMuonDetector()
{
  if (m_chamberLayout) delete m_chamberLayout; 
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

  m_detSvc = this->dataSvc();

  //Initialize the maximum number of allowed chambers per region
  int myDum[4] = {12,24,48,192};
  for(int dum = 0; dum<4; dum++) {MaxRegions[dum] = myDum[dum];}

  //Initializing the Layout
  MuonLayout R1(1,1), R2(1,2), R3(1,4), R4(2,8); 
  MuonChamberLayout * tLay = new MuonChamberLayout(R1,R2,R3,R4,m_detSvc,msgSvc());
  m_chamberLayout = tLay;

  m_ChmbPtr =  m_chamberLayout->fillChambersVector(m_detSvc);

  //fill geo info
  fillGeoInfo();
  fillGeoArray();

  //Initialize vectors containing Detector informations
  CountDetEls();
  //  delete tLay;  
  updMgrSvc()->registerCondition(&m_daqHelper, 
                                 "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/Cabling", 
                                 &MuonDAQHelper::updateLUT);
  updMgrSvc()->registerCondition(&m_daqHelper,
                                 "/dd/Conditions/ReadoutConf/Muon/Cabling/M2/Cabling",
                                 &MuonDAQHelper::updateLUT);
  updMgrSvc()->registerCondition(&m_daqHelper,
                                 "/dd/Conditions/ReadoutConf/Muon/Cabling/M3/Cabling",
                                 &MuonDAQHelper::updateLUT);
  updMgrSvc()->registerCondition(&m_daqHelper,
                                 "/dd/Conditions/ReadoutConf/Muon/Cabling/M4/Cabling",
                                 &MuonDAQHelper::updateLUT);
  updMgrSvc()->registerCondition(&m_daqHelper,
                                 "/dd/Conditions/ReadoutConf/Muon/Cabling/M5/Cabling",
                                 &MuonDAQHelper::updateLUT);
  
  m_daqHelper.initSvc(dataSvc(),msgSvc());
  sc=m_daqHelper.initDAQMaps();
  if(sc.isFailure())return sc;
  
  return sc;
}



StatusCode DeMuonDetector::Hit2GapNumber(Gaudi::XYZPoint myPoint, 
					 int station, int & gapNumber,
					 int & chamberNumber, int& regNum) const{
  
  //This methods sets the gap = 0 if only the
  //station is provided [HitPoint.z() = 0]
  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; 

  MsgStream msg( msgSvc(), name() );

  sc = Hit2ChamberNumber(myPoint,station,chamberNumber,regNum);
  if(sc.isFailure())return sc;
  
  DeMuonChamber * theChmb = getChmbPtr(station,regNum,chamberNumber);

  //Set SC to failure until gap is found
  sc = StatusCode::FAILURE;

  //Is the chamber returned containing the hit?
  IDetectorElement::IDEContainer::iterator itGap= theChmb->childBegin();
  for(itGap=theChmb->childBegin(); itGap<theChmb->childEnd(); itGap++){
    
    
    //Check the Gas Volume
    IDetectorElement::IDEContainer::iterator itGVol= (*itGap)->childBegin();
    for(itGVol=(*itGap)->childBegin(); itGVol<(*itGap)->childEnd(); itGVol++){
      IGeometryInfo* geoGVol = (*itGVol)->geometry();  
      
      isIn = geoGVol->isInside(myPoint);
      if(isIn) {
        DeMuonGasGap*  myGap =  dynamic_cast<DeMuonGasGap*>( *itGap ) ;
        gapNumber = myGap->gasGapNumber();
        sc = StatusCode::SUCCESS;
        break;
      }
    }
    if(isIn) break;
  }

  if(!isIn) {
    msg << MSG::ERROR << "Gap not found! " <<endreq;
  }
  
  return sc;
}


StatusCode DeMuonDetector::Hit2ChamberNumber(Gaudi::XYZPoint myPoint, 
					     int station, 
					     int & chamberNumber, int& regNum) const{
  
  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; bool debug = false;
  
  MsgStream msg( msgSvc(), name() );

  double x = myPoint.x();  double y = myPoint.y(); double z = myPoint.z();

  //Returning the most likely chamber
  m_chamberLayout->chamberMostLikely((float)x,(float)y,station,chamberNumber,regNum);

  if(regNum>=0&&chamberNumber>=0){
    
    //Providing all 3 numbers identifies a chamber
    IGeometryInfo* geoChm = 
      (getChmbPtr(station,regNum,chamberNumber))->geometry();  
    
    //For calls not providing hit z take the z of chamber center
    if(!z) {myPoint.SetZ(geoChm->toGlobal(Gaudi::XYZPoint(0,0,0)).z());}
    
    //Is the chamber returned containing the hit?
    isIn = geoChm->isInside(myPoint);
    
    if(isIn) {
      sc = StatusCode::SUCCESS;
    } else {
      
      if(debug) std::cout <<"Starting point::  "
      <<station<<" "<<chamberNumber<<" "<<regNum<<std::endl;
      
      //Find the vector of chambers near the one under investigation
      double x_ref(0.),y_ref(0.);
      x_ref = geoChm->toGlobal(Gaudi::XYZPoint(0,0,0)).x();
      y_ref = geoChm->toGlobal(Gaudi::XYZPoint(0,0,0)).y();
      
      int x_sgn(0),y_sgn(0);
      //Avoid unnecessary checks for resolution problems
      if(fabs(x-x_ref)>0.01) {x_sgn = (int)((x-x_ref)/fabs(x-x_ref));}
      if(fabs(y-y_ref)>0.01) {y_sgn = (int)((y-y_ref)/fabs(y-y_ref));}
      
      if(debug) std::cout<< "Hit not found Try to look in corner "
          <<x_ref<< " "<<x<<" "<<x_sgn<<" "<<y_ref<<" "<<y<<" "<<
        y_sgn<<std::endl;    
      
      std::vector<int> regs; std::vector<int> myChams;
      m_chamberLayout->returnChambers(station, (float)x_ref,(float)y_ref,
				      x_sgn,y_sgn,regs, myChams);
      
      std::vector<int>::iterator aChamber;
      //Loops on found chambers
      int idx = 0;
      for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){
        
        int tmpChn = *aChamber;
        int tmpRen = regs.at(idx);
        int tmpStn = station;

        idx++;
        //Accessing Geometry Info
        IGeometryInfo* geoOthChm = (getChmbPtr(tmpStn,tmpRen,tmpChn))
          ->geometry();  
        if(geoOthChm) {
          //For calls not providing hit z take the z of chamber center
          if(!z) {myPoint.SetZ(geoOthChm->toGlobal(Gaudi::XYZPoint(0,0,0)).z());}
	
          isIn = geoOthChm->isInside(myPoint);
          
          if(isIn) {
            sc = StatusCode::SUCCESS;
            //Returns the correct region and chamber number
            chamberNumber = tmpChn;
            regNum = tmpRen;
            break;
          }
        } else {
          msg << MSG::WARNING 
              << "Could not find Geometry info of a given chamber!"
              <<endreq;    
        }
      }
      
      //Debug the chambers returned
      if(debug) {
        std::cout<<"Chmb test n.  :"<<chamberNumber+1<<"  ";
        std::cout<<"Chmb ret.  : ";
        for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){
          std::cout<<*aChamber<<" ";
        }
        std::cout<<" "<<std::endl;
      }
    }
  }

  if(!isIn) {
    msg << MSG::DEBUG << 
      "Smart seek didn't work. Perform loop on all chambers :( !!! " 
        <<endreq;
    int msta(0),mreg(0),mchm(0); 
    //Getting stations
    IDetectorElement::IDEContainer::const_iterator itSt;
    for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
      if(msta == station) { 
        //Getting regions
        IDetectorElement::IDEContainer::iterator itSide=(*itSt)->childBegin();
        for(itSide=(*itSt)->childBegin(); itSide<(*itSt)->childEnd(); itSide++){
          mreg = 0;
          IDetectorElement::IDEContainer::iterator itRg=(*itSide)->childBegin();
          for(itRg=(*itSide)->childBegin(); itRg<(*itSide)->childEnd(); itRg++){
            //Getting chambers
            mchm = 0;
            IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
            for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
              IGeometryInfo* geoAllChm = (*itCh)->geometry();  
              
              //For calls not providing hit z take the z of chamber center
              if(!z) {myPoint.SetZ(geoAllChm->toGlobal(Gaudi::XYZPoint(0,0,0)).z());}
              
              isIn = geoAllChm->isInside(myPoint);
              if(isIn) {
                sc = StatusCode::SUCCESS;
                //Returns the correct region and chamber number
                DeMuonChamber* pCh=dynamic_cast<DeMuonChamber*>(*itCh);
                chamberNumber = pCh->chamberNumber();
                //mchm;  
                regNum = mreg;
                msg << MSG::DEBUG << "Hit found in chamber C: " <<
                  chamberNumber<<" , R: "<<regNum<<" ,S: "<<station<<endreq;
                return sc;
              }
              mchm++;
            }
            //Chamber Loop
            mreg++;
          }//Region Loop
        }//side loop
      }      
      msta++;
    }//Stations Loop
  }
  
  return sc;
}


StatusCode DeMuonDetector::Pos2StChamberNumber(const double x,
					       const double y,
					       int station,
					       int & chamberNumber, int& regNum) const {
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  Gaudi::XYZPoint hitPoint(x,y,0); 

  return Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);
}

StatusCode DeMuonDetector::Pos2StGapNumber(const double x,
                                           const double y,
                                           int station, int & gapNumber,
                                           int & chamberNumber, int& regNum)const {
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  Gaudi::XYZPoint hitPoint(x,y,0); 
  
  return Hit2GapNumber(hitPoint,station,gapNumber,chamberNumber,regNum);
}


StatusCode DeMuonDetector::Pos2StChamberPointer(const double x,
                                                const double y,
                                                int station,
                                                DeMuonChamber* & 
                                                chamberPointer)const{
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  Gaudi::XYZPoint hitPoint(x,y,0); 
  int chamberNumber(-1),regNum(-1);
  StatusCode sc = Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);
  if(sc.isFailure())return sc;
  
  if((regNum > -1) && (chamberNumber>-1)) {
    DeMuonChamber*  myPtr =  getChmbPtr(station,regNum,chamberNumber) ;
    chamberPointer = myPtr;
  }
  
  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberPointer(const double x,
                                              const double y,
                                              const double z,
                                              DeMuonChamber* & chamberPointer)const{
  //Dummy conversion z <-> station  
  return Pos2StChamberPointer(x,y,getStation(z),chamberPointer);
}


StatusCode DeMuonDetector::Pos2ChamberNumber(const double x,
					     const double y,
					     const double z,
					     int & chamberNumber, int& regNum)const{
  MsgStream msg( msgSvc(), name() );

  //Z is know/provided.
  Gaudi::XYZPoint hitPoint(x,y,z);   int sta = getStation(z);

  return Hit2ChamberNumber(hitPoint,sta,chamberNumber,regNum);
}



StatusCode DeMuonDetector::Pos2GapNumber(const double x,
					 const double y,
					 const double z, int & gapNumber,
					 int & chamberNumber, int& regNum)const{

  //Z is know/provided.
  Gaudi::XYZPoint hitPoint(x,y,z);
  int sta = getStation(z);

  return Hit2GapNumber(hitPoint,sta,gapNumber,chamberNumber,regNum);
}


StatusCode DeMuonDetector::Pos2ChamberTile(const double x,
					   const double y,
					   const double z,
					   LHCb::MuonTileID& tile)const{
  int dumChmb(-1), reg(-1);

  //Return the chamber number
  StatusCode sc = Pos2ChamberNumber(x,y,z,dumChmb,reg);

  //Convert chamber number into a tile
  tile = m_chamberLayout->tileChamberNumber(getStation(z),reg,dumChmb);

  return sc;
}

/*IDetectorElement* DeMuonDetector::ReturnADetElement(int lsta, int lreg, 
                                                    int lchm) {
  
  IDetectorElement* myDet = (IDetectorElement*)0;
  int msta(0),mreg(0),mchm(0); 
  
  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    if((msta == lsta) && (lreg == -1) && (lchm == -1)) {
      myDet = *itSt;
      return myDet;
    } 
    //Getting regions
    mreg = 0;
    IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
    for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
      if((msta == lsta) && (mreg == lreg) && (lchm == -1)) {
        myDet = *itRg;
        return myDet;
      } 
      //Getting chambers
      mchm = 0;
      IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
      for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
        
        if((msta == lsta) && (mreg == lreg) && (mchm == lchm)) {
          myDet = *itCh;
          return myDet;
        } 
        mchm++;
      }//Chamber Loop
      mreg++;
    }//Region Loop
    msta++;
  }//Stations Loop
  
  return myDet;
  
}
*/
void DeMuonDetector::CountDetEls() {
  
  IDetectorElement* myDet = (IDetectorElement*)0;
  int msta(0),mreg(0),mallreg(0); 

  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    IDetectorElement::IDEContainer::iterator itSide=(*itSt)->childBegin();
    myDet = *itSt;
    if(myDet) msta++;

    //Getting regions
    mreg = 0;
    IDetectorElement::IDEContainer::iterator itRg=(*itSide)->childBegin();
    for(itRg=(*itSide)->childBegin(); itRg<(*itSide)->childEnd(); itRg++){

      myDet = *itRg;
      if(myDet) {
        mreg++;
        mallreg++;
      }

    }//Region Loop

    m_regsperSta[msta-1] = mreg;

  }//Stations Loop

  m_stations = msta;
  m_regions = mallreg;

  return;
  
}

DeMuonChamber* DeMuonDetector::getChmbPtr(const int station, const int region,
			    const int chmb) const {
  DeMuonChamber* myPtr;
  int encode, re(0);
  encode = 276*station+chmb;
  if(region) {
    re = region;
    while(re >= 1) {encode += MaxRegions[re-1]; re--;}
  }
  myPtr = m_ChmbPtr.at(encode);
  return myPtr;
}

/*const DeMuonChamber* DeMuonDetector::getChmbPtr(const int station, 
                                                const int region,
                                                const int chmb) const {
  DeMuonChamber* myPtr;
  int encode, re(0);
  encode = 276*station+chmb;
  if(region) {
    re = region;
    while(re >= 1) {encode += MaxRegions[re-1]; re--;}
  }
  myPtr = m_ChmbPtr.at(encode);
  return myPtr;
}
*/

/*void DeMuonDetector::fillChmbPtr() {
  
  int encode(0);
  bool debug = false;
  //Stations
  for(int iS = 0; iS<5; iS++){ 
    //Regions
    for(int iR = 0; iR<4; iR++){ 
      //Chambers
      for(int iC = 0; iC<MaxRegions[iR]; iC++){ 
        
        IDetectorElement* det  = ReturnADetElement(iS,iR,iC);
        DeMuonChamber*  myPtr =  dynamic_cast<DeMuonChamber*>( det ) ;
        
        if(debug) std::cout<<"Filling chamber Pointer: "<<encode<<" "
                           <<iS<<" "<<MaxRegions[iR]<<" "<<iR<<" "<<iC<<
                    std::endl;
        if(myPtr) {
          m_ChmbPtr.at(encode) = myPtr;
        } else {
          m_ChmbPtr.at(encode) = (DeMuonChamber*)0;
        }
        encode++;
      }
    }
  }
  if(debug)  std::cout<<" Filled the chamber pointer "<<std::endl;
  return;
}
*/

std::vector< std::pair<MuonFrontEndID, std::vector<float> > > 
DeMuonDetector::listOfPhysChannels(Gaudi::XYZPoint my_entry, Gaudi::XYZPoint my_exit, 
                                   int region, int chamber) {
  
  //Pair vector;  
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > tmpPair;
  
  int regNum(0),chamberNumber(0);
  int station = getStation(my_entry.z());
  int regNum1(0),chamberNumber1(0);
  int station1 = getStation(my_exit.z());
  
  MsgStream msg( msgSvc(), name() );
  
  if((region == -1) || (chamber == -1)) {
    //Hit entry
    Hit2ChamberNumber(my_entry,station,chamberNumber,regNum);
    
    //Hit exit
    Hit2ChamberNumber(my_exit,station1,chamberNumber1,regNum1);
    
    if(chamberNumber != chamberNumber1 || regNum != regNum1) { 
      msg << MSG::ERROR <<
        "Hit entry and exit are in different chambers! Returning a void list."
          <<endreq; 
      return tmpPair;
    }
  } else {
    //If given.. assign region and chamber number
    regNum = region; chamberNumber = chamber;
  }
  
  //Getting the chamber pointer.
  DeMuonChamber*  myChPtr =  getChmbPtr(station,regNum,chamberNumber) ;
  
  bool isIn = false;
  //Getting a gap [gaps in same chamber have same dimensions]
  DeMuonGasGap*  myGap = (DeMuonGasGap*) 0; int gapCnt(0);
  IDetectorElement::IDEContainer::iterator itGap=myChPtr->childBegin();
  for(itGap=myChPtr->childBegin(); itGap<myChPtr->childEnd(); itGap++){
    myGap =  dynamic_cast<DeMuonGasGap*>( *itGap ) ;
    Gaudi::XYZPoint aleq = myGap->geometry()->toGlobal(Gaudi::XYZPoint(0.0,0.0,0.0));
    //    msg<<MSG::INFO<<" test "<<aleq.x()<<" "<<aleq.y()<<" "<<aleq.z()<<endreq;
    
    //Check the Gas Volume
    IDetectorElement::IDEContainer::iterator itGVol= (*itGap)->childBegin();
    for(itGVol=(*itGap)->childBegin(); itGVol<(*itGap)->childEnd(); itGVol++){
      IGeometryInfo* geoGVol = (*itGVol)->geometry();  
      
      
      Gaudi::XYZPoint mid(((my_entry.x()+my_exit.x())/2),((my_entry.y()+my_exit.y())/2),((my_entry.z()+my_exit.z())/2));
      
      
      isIn = geoGVol->isInside(mid);
      
      Gaudi::XYZPoint aleqq = geoGVol->toGlobal(Gaudi::XYZPoint(0.0,0.0,0.0));
      //      msg<<MSG::INFO<<" test "<<aleqq.x()<<" "<<aleqq.y()<<" "<<aleqq.z()
      //     <<endreq;    
      if(isIn) break;
      //      msg<<MSG::INFO<<" is in "<<isIn<<endreq;
    }
    if(isIn) break;
    
    gapCnt++;
  }
  

  //  if(!myGap) {
  if(!isIn) {
    msg << MSG::DEBUG <<"Could not find the gap. Returning a void list."<<endreq;
    m_hitNotInGap++;	 
    return tmpPair;
  }
  
  //Gap Geometry info 
  //This is OK ONLY if you want to access x and y informations.
  //Otherwise you need to go down to the volume of gas gap inside 
  IGeometryInfo*  geoCh=myGap->geometry();

  //Retrieve the chamber box dimensions  
  const SolidBox *box = dynamic_cast<const SolidBox *>
    (geoCh->lvolume()->solid());
  double dx = box->xHalfLength();  double dy = box->yHalfLength();


  //  msg<<MSG::INFO<<" pre gap "<<gapCnt<<" "<<my_entry.x()<<" "<<
  //  my_entry.y()<<" "<<my_entry.z()<<endreq;
  //msg<<MSG::INFO<<" pre gap "<<gapCnt<<" "<<my_exit.x()<<" "<<
  //  my_exit.y()<<" "<<my_exit.z()<<endreq;

  //Refer the distances to Local system [should be the gap]
  Gaudi::XYZPoint new_entry = geoCh->toLocal(my_entry);
  Gaudi::XYZPoint new_exit  = geoCh->toLocal(my_exit);

  //msg<<MSG::INFO<<" gap "<<gapCnt<<" "<<new_entry.x()<<" "<<
  //  new_entry.y()<<" "<<new_entry.z()<<endreq;
  //msg<<MSG::INFO<<" gap "<<gapCnt<<" "<<new_exit.x()<<" "<<
  //  new_exit.y()<<" "<<new_exit.z()<<endreq;

  Gaudi::XYZPoint LL(-dx,-dy,0);
  Gaudi::XYZPoint LR(dx,-dy,0);
  Gaudi::XYZPoint UL(dx,dy,0);
  //  Gaudi::XYZPoint UR(-dx,dy,0);
  
  Gaudi::XYZPoint lowerleft = geoCh->toGlobal(LL);
  Gaudi::XYZPoint lowerright  = geoCh->toGlobal(LR);
  Gaudi::XYZPoint upperleft = geoCh->toGlobal(UL);
  //  Gaudi::XYZPoint upperright  = geoCh->toGlobal(UR);


  //Define relative dimensions
  double mod_xen(0), mod_yen(0), mod_xex(0), mod_yex(0);
  if( dx && dy ) {
    if((lowerleft.x()<lowerright.x())&&(lowerleft.y()<upperleft.y())){
      mod_xen = (new_entry.x()+dx)/(2*dx);
      mod_yen = (new_entry.y()+dy)/(2*dy);
      mod_xex = (new_exit.x()+dx)/(2*dx);
      mod_yex = (new_exit.y()+dy)/(2*dy);
    }else if(lowerleft.x()<lowerright.x()&&(lowerleft.y()>upperleft.y())){
      mod_xen = (new_entry.x()+dx)/(2*dx);
      mod_yen = (-new_entry.y()+dy)/(2*dy);
      mod_xex = (new_exit.x()+dx)/(2*dx);
      mod_yex = (-new_exit.y()+dy)/(2*dy);
    }else if(lowerleft.x()>lowerright.x()&&(lowerleft.y()>upperleft.y())){
      mod_xen = (-new_entry.x()+dx)/(2*dx);
      mod_yen = (-new_entry.y()+dy)/(2*dy);
      mod_xex = (-new_exit.x()+dx)/(2*dx);
      mod_yex = (-new_exit.y()+dy)/(2*dy);
       msg<<MSG::INFO<<" Should never enter here "<<endreq;
    }else if(lowerleft.x()>lowerright.x()&&(lowerleft.y()<upperleft.y())){
      mod_xen = (-new_entry.x()+dx)/(2*dx);
      mod_yen = (new_entry.y()+dy)/(2*dy);
      mod_xex = (-new_exit.x()+dx)/(2*dx);
      mod_yex = (new_exit.y()+dy)/(2*dy);  
      msg<<MSG::INFO<<" Should never enter here "<<endreq;
    }
  } else {
    msg << MSG::ERROR <<"Null chamber dimensions. Returning a void list."<<
      endreq; 
    return tmpPair;
  }

  //Getting the grid pointer
  Condition* aGrid = myChPtr->condition(myChPtr->getGridName());
  MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(aGrid);

  //Convert relative distances into absolute ones
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > myPair;
  std::vector< std::pair< MuonFrontEndID,std::vector<float> > >::iterator 
    tmpPair_it;
  std::vector<float> myVec; MuonFrontEndID myFE;
  if(theGrid) {
    //Gets list of channels
    tmpPair = theGrid->listOfPhysChannels(mod_xen,mod_yen,mod_xex,mod_yex);
    
    tmpPair_it = tmpPair.begin();
    for(tmpPair_it = tmpPair.begin();tmpPair_it<tmpPair.end(); tmpPair_it++){
      myFE  = tmpPair_it->first;
      myVec = tmpPair_it->second;
      myFE.setLayer(gapCnt/2);

      for(int iDm = 0; iDm<4; iDm++){  
        myVec.at(iDm) = (float)(iDm%2 ? myVec.at(iDm)*2*dy : myVec.at(iDm)*2*dx);
        //Added resolution effect
        if(fabs(myVec.at(iDm)) < 0.0001) myVec.at(iDm) = 0;
      }
      myPair.push_back(std::pair< MuonFrontEndID,std::vector<float> >
                       (myFE,myVec));
    }
  } else {
    msg << MSG::ERROR <<"No grid found. Returning a void list."<<endreq; 
    return tmpPair;
  }
  return myPair;
}

StatusCode DeMuonDetector::Tile2XYZ(LHCb::MuonTileID tile, 
				    double & x, double & dx,
				    double & y, double & dy, 
				    double & z, double & dz){
  
  StatusCode sc = StatusCode::FAILURE;

  //Ask the chamber Layout about the tile.
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG <<"Calling Tile2XYZpos method!"<<endreq; 

  sc = m_chamberLayout->Tile2XYZpos(tile,x,dx,y,dy,z,dz);

  return sc;
}

StatusCode DeMuonDetector::getPCCenter(MuonFrontEndID fe,int chamber,
                                       int station,int region,
                                       double& xcenter, double& ycenter,
                                       double& zcenter)
{
  //  MsgStream msg( msgSvc(), name() );

  DeMuonChamber*  myChPtr =  getChmbPtr(station,region,chamber) ;

  DeMuonGasGap*  myGap = NULL;
  IDetectorElement::IDEContainer::iterator itGap=myChPtr->childBegin();
    myGap= dynamic_cast<DeMuonGasGap*>(*itGap);  
  //Gap Geometry info  
  IGeometryInfo*  geoCh=myGap->geometry();
  //Retrieve the chamber box dimensions  
  const SolidBox *box = dynamic_cast<const SolidBox *>
    (geoCh->lvolume()->solid());
  double dx = box->xHalfLength();  
  double dy = box->yHalfLength();
  Condition* aGrid = 
  myChPtr->condition(myChPtr->getGridName());
  MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(aGrid);
  double xcenter_norma=-1;
  double ycenter_norma=-1;  
  StatusCode sc=theGrid->getPCCenter(fe,xcenter_norma,ycenter_norma);
  if(sc.isFailure())return StatusCode::FAILURE;
  double xcenter_gap=xcenter_norma*2*dx-dx;
  double ycenter_gap=ycenter_norma*2*dy-dy;
  //  unsigned int layer=fe.getLayer();
  Gaudi::XYZPoint loc(xcenter_gap,ycenter_gap,0);   
  Gaudi::XYZPoint glob= geoCh->toGlobal(loc);
  xcenter=glob.x();
  ycenter=glob.y();
  zcenter=glob.z();
  return StatusCode::SUCCESS;
  
}

StatusCode  DeMuonDetector::Chamber2Tile(int  chaNum, int station, int region, 
                                         LHCb::MuonTileID& tile) const
{
  StatusCode sc = StatusCode::SUCCESS; 
  //Convert chamber number into a tile
  tile = m_chamberLayout->tileChamberNumber(station,region,chaNum);  
  return sc;
};


void DeMuonDetector::fillGeoInfo()
{
  MsgStream msg( msgSvc(), name() );
  bool debug=false;
  
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  int station=0; 
  int Side=0;
  int region=0;
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    
    IGeometryInfo*  geoSt=(*itSt)->geometry();
    Gaudi::XYZPoint globSt= geoSt->toGlobal(Gaudi::XYZPoint(0,0,0));
    m_stationZ[station] = globSt.z();    
    IDetectorElement::IDEContainer::iterator itSide=(*itSt)->childBegin();
    Side=0;
    for(itSide=(*itSt)->childBegin(); itSide<(*itSt)->childEnd(); itSide++){
      
      IDetectorElement::IDEContainer::iterator itRg=(*itSide)->childBegin();
      region=0;      
      if(debug)msg<<MSG::INFO<<" station "<<station<<endreq;      
      for(itRg=(*itSide)->childBegin(); itRg<(*itSide)->childEnd(); itRg++){
        if(debug)msg<<MSG::INFO<<" region "<<region<<endreq;      
        IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
        //        DeMuonRegion* reg=dynamic_cast<DeMuonRegion*> (*itRg);
        for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
          DeMuonChamber* chPt=dynamic_cast<DeMuonChamber*> (*itCh);          
          IDetectorElement::IDEContainer::iterator itGap=(*itCh)->childBegin();
          int gaps=0;          
          double area=0;          
          DeMuonGasGap* 
            myGap= dynamic_cast<DeMuonGasGap*>(*((*itCh)->childBegin()));  
          //Gap Geometry info  
          IGeometryInfo*  geoCh=myGap->geometry();
          
          //Retrieve the chamber box dimensions  
          const SolidBox *box = dynamic_cast<const SolidBox *>
            (geoCh->lvolume()->solid());
          double dx = box->xHalfLength();
          double dy = box->yHalfLength();
          double dz = box->zHalfLength();
          m_sensitiveAreaX[station*4+region]=(float)(2.*dx);
          m_sensitiveAreaY[station*4+region]=(float)(2.*dy);
          m_sensitiveAreaZ[station*4+region]=(float)(2.*dz);
          area=4*dx*dy;
          m_areaChamber[station*4+region]=area;          
          for(itGap=(*itCh)->childBegin(); itGap<(*itCh)->childEnd(); itGap++){
            gaps++;            
          }
          m_gapPerRegion[station*4+region]=gaps;
          m_gapPerFE[station*4+region]=gaps/2; 
          
          
        
          Condition* bGrid = (chPt)->condition(chPt->getGridName());
          MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(bGrid);
          
          int nreadout=1;
          if(theGrid->getGrid2SizeY()>1)nreadout=2;
          m_readoutNumber[station*4+region]=nreadout;
          for( int i = 0; i<nreadout;i++){
            if(i==0){
              m_phChannelNX[i][station*4+region]=theGrid->getGrid1SizeX();
              m_phChannelNY[i][station*4+region]=theGrid->getGrid1SizeY();
              m_readoutType[i][station*4+region]=
                (theGrid->getReadoutGrid())[i];             
            }            
            if(i==1){
              m_phChannelNX[i][station*4+region]=theGrid->getGrid2SizeX();
              m_phChannelNY[i][station*4+region]=theGrid->getGrid2SizeY();
              m_readoutType[i][station*4+region]=
                (theGrid->getReadoutGrid())[i];
            }
          }
          int maps=(theGrid->getMapGrid()).size()/2;
          m_LogMapPerRegion[station*4+region]=maps;
          if(debug)msg<<MSG::INFO<<" red and maps "<<nreadout<<" "<<maps<<endreq;    
          if(nreadout==1)
          {
            for( int i = 0; i<maps;i++){
              m_LogMapRType[i][station*4+region]=
                m_readoutType[0][station*4+region];
              m_LogMapMergex[i][station*4+region]=
                (theGrid->getMapGrid())[i*2];
              m_LogMapMergey[i][station*4+region]=
                (theGrid->getMapGrid())[i*2+1];
              if(debug)msg<<MSG::INFO<<" red and maps "<<i<<" "<< m_LogMapRType[i][station*4+region]<<" "<<
                         m_LogMapMergex[i][station*4+region]<<" "<<m_LogMapMergey[i][station*4+region]<<endreq;    
            }            
          }else if(nreadout==2){ 
            for( int i = 0; i<maps;i++){
              m_LogMapRType[i][station*4+region]=
                m_readoutType[i][station*4+region];
              m_LogMapMergex[i][station*4+region]=
                (theGrid->getMapGrid())[i*2];
              m_LogMapMergey[i][station*4+region]=
                (theGrid->getMapGrid())[i*2+1];
              if(debug)msg<<MSG::INFO<<" red and maps "<<i<<" "<< m_LogMapRType[i][station*4+region]<<" "<<
                         m_LogMapMergex[i][station*4+region]<<" "<<m_LogMapMergey[i][station*4+region]<<endreq;  
            }            
          }
          
          break;          
        }        
        int chamber=0;        
        for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
          chamber++;          
        }m_chamberPerRegion[station*4+region]=chamber*2;        
        region++;
      }
      Side++;
    }    
    station++;      
  }
      
  // initialization by hand of the logical layout in the different regions
  m_layoutX[0][0]=24;  
  m_layoutX[0][1]=24;  
  m_layoutX[0][2]=24;  
  m_layoutX[0][3]=24;
  m_layoutX[0][4]=48; 
  m_layoutX[0][5]=48;  
  m_layoutX[0][6]=48;  
  m_layoutX[0][7]=48;
  m_layoutX[1][4]=8; 
  m_layoutX[1][5]=4;  
  m_layoutX[1][6]=2;  
  m_layoutX[1][7]=2;
  m_layoutX[0][8]=48;  
  m_layoutX[0][9]=48;  
  m_layoutX[0][10]=48;  
  m_layoutX[0][11]=48;
  m_layoutX[1][8]=8; 
  m_layoutX[1][9]=4;  
  m_layoutX[1][10]=2;  
  m_layoutX[1][11]=2;
  m_layoutX[0][12]=12;  
  m_layoutX[0][13]=12;  
  m_layoutX[0][14]=12;  
  m_layoutX[0][15]=12;  
  m_layoutX[1][13]=4;  
  m_layoutX[1][14]=2;  
  m_layoutX[1][15]=2;
  m_layoutX[0][16]=12;  
  m_layoutX[0][17]=12;  
  m_layoutX[0][18]=12;  
  m_layoutX[0][19]=12;
  m_layoutX[1][17]=4;  
  m_layoutX[1][18]=2;  
  m_layoutX[1][19]=2;  
  
  m_layoutY[0][0]=8;  
  m_layoutY[0][1]=8;  
  m_layoutY[0][2]=8;  
  m_layoutY[0][3]=8;
  m_layoutY[0][4]=1; 
  m_layoutY[0][5]=2;  
  m_layoutY[0][6]=2;  
  m_layoutY[0][7]=2;
  m_layoutY[1][4]=8; 
  m_layoutY[1][5]=8;  
  m_layoutY[1][6]=8;  
  m_layoutY[1][7]=8;
  m_layoutY[0][8]=1;  
  m_layoutY[0][9]=2;  
  m_layoutY[0][10]=2;  
  m_layoutY[0][11]=2;
  m_layoutY[1][8]=8; 
  m_layoutY[1][9]=8;  
  m_layoutY[1][10]=8;  
  m_layoutY[1][11]=8;
  m_layoutY[0][12]=8;  
  m_layoutY[0][13]=2;  
  m_layoutY[0][14]=2;  
  m_layoutY[0][15]=2;  
  m_layoutY[1][13]=8;  
  m_layoutY[1][14]=8;  
  m_layoutY[1][15]=8;
  m_layoutY[0][16]=8;  
  m_layoutY[0][17]=2;  
  m_layoutY[0][18]=2;  
  m_layoutY[0][19]=2;
  m_layoutY[1][17]=8;  
  m_layoutY[1][18]=8;  
  m_layoutY[1][19]=8;

  // fill pad sizes
  
  for(int stat=0;stat<5;stat++){
    for(int reg=0;reg<4;reg++){
      unsigned int part=stat*4+reg;
      // one readout
      if( m_readoutNumber[part]==1){
        //1 map 
        if(m_LogMapPerRegion[part]==1){
          //already pads...
          m_padSizeX[part]=(m_sensitiveAreaX[part]/m_phChannelNX[0][part])* m_LogMapMergex[0][part];
          m_padSizeY[part]=(m_sensitiveAreaY[part]/m_phChannelNY[0][part])* m_LogMapMergey[0][part];
          
        }else if(m_LogMapPerRegion[part]==2){
          int mgx=0;          
          int mgy=0;
          mgx = (m_LogMapMergex[0][part]>m_LogMapMergex[1][part]) ? m_LogMapMergex[1][part] : m_LogMapMergex[0][part];
          mgy = (m_LogMapMergey[0][part]>m_LogMapMergey[1][part]) ? m_LogMapMergey[1][part] : m_LogMapMergey[0][part];  
          m_padSizeX[part]=(m_sensitiveAreaX[part]/m_phChannelNX[0][part])* mgx;
          m_padSizeY[part]=(m_sensitiveAreaY[part]/m_phChannelNY[0][part])* mgy;           
        }        
      }else  if( m_readoutNumber[part]==2){
        //the convention is always anode first...
        int mgx=0;          
        int mgy=0;
        mgx = m_LogMapMergex[0][part];
        mgy = m_LogMapMergey[1][part] ;          
        m_padSizeX[part]=(m_sensitiveAreaX[part]/m_phChannelNX[0][part])* mgx;
        m_padSizeY[part]=(m_sensitiveAreaY[part]/m_phChannelNY[0][part])* mgy;       
      }
    }    
  }
  //initializatuon by hand of the OR inside the same Cardiac
  m_phCardiacORNX[1][0]=1;
  m_phCardiacORNX[1][1]=1;
  m_phCardiacORNX[1][2]=2;
  m_phCardiacORNX[0][3]=2;
  m_phCardiacORNY[1][0]=1;
  m_phCardiacORNY[1][1]=2;
  m_phCardiacORNY[1][2]=1;
  m_phCardiacORNY[0][3]=1;
  
  m_phCardiacORNX[0][4]=1;
  m_phCardiacORNX[1][4]=1;
  m_phCardiacORNX[0][5]=1;
  m_phCardiacORNX[1][5]=2;
  m_phCardiacORNX[1][6]=1;
  m_phCardiacORNX[0][7]=1;

  m_phCardiacORNY[0][4]=1;
  m_phCardiacORNY[1][4]=1;
  m_phCardiacORNY[0][5]=1;
  m_phCardiacORNY[1][5]=2;
  m_phCardiacORNY[1][6]=1;
  m_phCardiacORNY[0][7]=1;

  m_phCardiacORNX[0][8]=1;
  m_phCardiacORNX[1][8]=1;
  m_phCardiacORNX[0][9]=1;
  m_phCardiacORNX[1][9]=2;
  m_phCardiacORNX[1][10]=1;
  m_phCardiacORNX[0][11]=1;

  m_phCardiacORNY[0][8]=1;
  m_phCardiacORNY[1][8]=1;
  m_phCardiacORNY[0][9]=1;
  m_phCardiacORNY[1][9]=2;
  m_phCardiacORNY[1][10]=1;
  m_phCardiacORNY[0][11]=1;

  m_phCardiacORNX[1][12]=1;
  m_phCardiacORNX[1][13]=1;
  m_phCardiacORNX[1][14]=2;
  m_phCardiacORNX[0][15]=4;

  m_phCardiacORNY[1][12]=1;
  m_phCardiacORNY[1][13]=1;
  m_phCardiacORNY[1][14]=1;
  m_phCardiacORNY[0][15]=1;

  m_phCardiacORNX[1][16]=1;
  m_phCardiacORNX[1][17]=1;
  m_phCardiacORNX[1][18]=2;
  m_phCardiacORNX[0][19]=4;

  m_phCardiacORNY[1][16]=1;
  m_phCardiacORNY[1][17]=1;
  m_phCardiacORNY[1][18]=1;
  m_phCardiacORNY[0][19]=1;

  return;
};

void DeMuonDetector::fillGeoArray()
{
  MsgStream msg( msgSvc(), name() );
  bool debug=false;
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  
  MuonLayout layoutInner=m_chamberLayout->layout(0);
  MuonLayout layoutOuter=m_chamberLayout->layout(3);
  int station=0;
  msg<<MSG::DEBUG<< "layout inner "<<
    layoutInner.xGrid()<<" "<<  layoutInner.yGrid()<<endreq;

  msg<<MSG::DEBUG<< "layout outer "<<
    layoutOuter.xGrid()<<" "<<  layoutOuter.yGrid()<<endreq;
  
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
      //get the dimensions of the inner rectangular 
    if(debug)msg<<MSG::INFO<<" inside loop "<<endreq;
    double minX=100000;
    double minY=100000;
    for(unsigned int nx=0;nx<layoutInner.xGrid();nx++){
      for(int qua=0;qua<4;qua++){          
        LHCb::MuonTileID chTile(station,layoutInner,0,qua,nx,
                                layoutInner.yGrid());
        int chNumber=m_chamberLayout->getChamberNumber(chTile);
        DeMuonChamber* chP=getChmbPtr(station,0,chNumber);
        DeMuonGasGap* myGap= dynamic_cast<DeMuonGasGap*>
          (*(chP->childBegin()));
        IGeometryInfo*  geoCh=myGap->geometry();
        
          //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (geoCh->lvolume()->solid());
        double dx = box->xHalfLength();
        double dy = box->yHalfLength();
        Gaudi::XYZPoint glob1= geoCh->toGlobal(Gaudi::XYZPoint(-dx,-dy,0));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(Gaudi::XYZPoint(-dx,dy,0));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(Gaudi::XYZPoint(dx,-dy,0));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(Gaudi::XYZPoint(dx,dy,0));
        if(fabs(glob1.y())<minY)minY=fabs(glob1.y());      
        if(fabs(glob2.y())<minY)minY=fabs(glob2.y());      
        if(fabs(glob3.y())<minY)minY=fabs(glob3.y());      
        if(fabs(glob4.y())<minY)minY=fabs(glob4.y());      
      }      
    }

    for(unsigned int ny=0;ny<layoutInner.yGrid();ny++){
      for(int qua=0;qua<4;qua++){          
        LHCb::MuonTileID chTile(station,layoutInner,0,qua,layoutInner.xGrid(),
                                ny);
        int chNumber=m_chamberLayout->getChamberNumber(chTile);
        DeMuonChamber* chP=getChmbPtr(station,0,chNumber);
        DeMuonGasGap* myGap= dynamic_cast<DeMuonGasGap*>
          (*(chP->childBegin()));
        IGeometryInfo*  geoCh=myGap->geometry();
        
          //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (geoCh->lvolume()->solid());
        double dx = box->xHalfLength();
        double dy = box->yHalfLength();
        Gaudi::XYZPoint glob1= geoCh->toGlobal(Gaudi::XYZPoint(-dx,-dy,0));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(Gaudi::XYZPoint(-dx,dy,0));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(Gaudi::XYZPoint(dx,-dy,0));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(Gaudi::XYZPoint(dx,dy,0));
        if(fabs(glob1.x())<minX)minX=fabs(glob1.x());      
        if(fabs(glob2.x())<minX)minX=fabs(glob2.x());      
        if(fabs(glob3.x())<minX)minX=fabs(glob3.x());      
        if(fabs(glob4.x())<minX)minX=fabs(glob4.x());      
      }      
    }
    m_stationBox[station][0]=minX;
    m_stationBox[station][1]=minY;
    //now the dimsnion of the outer parr...
    if(debug)msg<<MSG::INFO<<" min size "<<minX<<" "<<minY<<endreq;
    double maxX=0;
    double maxY=0;
    for(unsigned int nx=0;nx<2*layoutOuter.xGrid();nx++){
      for(int qua=0;qua<4;qua++){          
        LHCb::MuonTileID chTile(station,layoutOuter,3,qua,nx,
                                2*layoutOuter.yGrid()-1);
        int chNumber=m_chamberLayout->getChamberNumber(chTile);   
        if(debug)msg<<MSG::INFO<<" chNumber "<<chNumber <<endreq;
        DeMuonChamber* chP=getChmbPtr(station,3,chNumber);
        DeMuonGasGap* myGap= dynamic_cast<DeMuonGasGap*>
          (*(chP->childBegin()));
        IGeometryInfo*  geoCh=myGap->geometry();
        
          //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (geoCh->lvolume()->solid());
        double dx = box->xHalfLength();
        double dy = box->yHalfLength();
        Gaudi::XYZPoint glob1= geoCh->toGlobal(Gaudi::XYZPoint(-dx,-dy,0));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(Gaudi::XYZPoint(-dx,dy,0));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(Gaudi::XYZPoint(dx,-dy,0));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(Gaudi::XYZPoint(dx,dy,0));
        if(fabs(glob1.y())>maxY)maxY=fabs(glob1.y());      
        if(fabs(glob2.y())>maxY)maxY=fabs(glob2.y());      
        if(fabs(glob3.y())>maxY)maxY=fabs(glob3.y());      
        if(fabs(glob4.y())>maxY)maxY=fabs(glob4.y());      
      }      
    }

    for(unsigned int ny=0;ny<2*layoutOuter.yGrid();ny++){
      for(int qua=0;qua<4;qua++){          
        LHCb::MuonTileID chTile(station,layoutOuter,3,qua,
                                2*layoutOuter.xGrid()-1,
                                ny);
        int chNumber=m_chamberLayout->getChamberNumber(chTile);
        DeMuonChamber* chP=getChmbPtr(station,3,chNumber);
        DeMuonGasGap* myGap= dynamic_cast<DeMuonGasGap*>
          (*(chP->childBegin()));
        IGeometryInfo*  geoCh=myGap->geometry();
        
          //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (geoCh->lvolume()->solid());
        double dx = box->xHalfLength();
        double dy = box->yHalfLength();
        Gaudi::XYZPoint glob1= geoCh->toGlobal(Gaudi::XYZPoint(-dx,-dy,0));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(Gaudi::XYZPoint(-dx,dy,0));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(Gaudi::XYZPoint(dx,-dy,0));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(Gaudi::XYZPoint(dx,dy,0));
        if(fabs(glob1.x())>maxX)maxX=fabs(glob1.x());      
        if(fabs(glob2.x())>maxX)maxX=fabs(glob2.x());      
        if(fabs(glob3.x())>maxX)maxX=fabs(glob3.x());      
        if(fabs(glob4.x())>maxX)maxX=fabs(glob4.x());      
      }      
    }
    m_stationBox[station][2]=maxX;
    m_stationBox[station][3]=maxY;


    msg<<MSG::DEBUG<<" station  inner "<<station<<" "<< 
      m_stationBox[station][0]<<
      " "<<m_stationBox[station][1]<<
      " station  outer "<<station<<" "<< 
      m_stationBox[station][2]<<
      " "<<m_stationBox[station][3]<<endreq;
    
    station++;      
  }
  
  return;
}



int DeMuonDetector::sensitiveVolumeID(const Gaudi::XYZPoint &myPoint) const
{
  int nsta =0;
  int nreg =0;   
  int nchm =0;  
  int ngap =0;
  unsigned int nqua =0;
  
  // retrieve station,region,chamber,gap:
  StatusCode sc = StatusCode::FAILURE;
  nsta=getStation(myPoint.z());
  
  
  sc = Hit2GapNumber(myPoint,nsta,ngap,nchm,nreg);
  if(sc.isFailure())return -1;
  
  //retrieve the quadrant:
  double xPoi = myPoint.x();
  double yPoi = myPoint.y();
  double zPoi = myPoint.z();
  LHCb::MuonTileID tile;
  sc = Pos2ChamberTile(xPoi,yPoi,zPoi,tile);
  if(sc.isFailure())return -1;
  nqua = tile.quarter();
    // pack the integer:
  int id = (nqua<<PackMCMuonHit::shiftQuadrantID) |
           (nsta<<PackMCMuonHit::shiftStationID ) |
           (nreg<<PackMCMuonHit::shiftRegionID)   |
           (nchm<<PackMCMuonHit::shiftChamberID)  |
           (ngap<<PackMCMuonHit::shiftGapID);
  return id;

}


DetectorElement* DeMuonDetector::Tile2Station(LHCb::MuonTileID aTile)
{
  char stationName[10];
  MsgStream msg( msgSvc(), name() );
  
  unsigned int istat = aTile.station();
  sprintf(stationName,"/M%d",istat+1);
  std::string stationPath = DeMuonLocation::Default+stationName;

  SmartDataPtr<DetectorElement> station(m_detSvc, stationPath);
  if( !station ) {
    msg << MSG::INFO << "Error in retrieving DetectorElement!" << endreq;
  }
  return station;
}


std::vector<DeMuonChamber*> DeMuonDetector::Tile2Chamber(LHCb::MuonTileID aTile)
{ 
  MsgStream msg( msgSvc(), name() );
  unsigned int istat = aTile.station();
  unsigned int ireg =  aTile.region();
  unsigned int chamNum;

  std::vector<DeMuonChamber*> ChmbPtr;
  std::vector<unsigned int> chamberVector=m_chamberLayout->Tile2ChamberNum(aTile);
  int vdim = chamberVector.size();
  
  // std::cout << "chamberNumVect--> size: "<< vdim << std::endl;
  for(int i=0; i<vdim; i++) {
    chamNum = chamberVector.at(i);
    ChmbPtr.push_back(getChmbPtr(istat,ireg,chamNum));
  }
  return ChmbPtr;
}


DetectorElement* DeMuonDetector::Hit2Station(Gaudi::XYZPoint myPoint)
{ 
  char stationName[10];
  MsgStream msg(msgSvc(),name());
  
  unsigned int istat = getStation(myPoint.z());
  sprintf(stationName,"/M%d",istat+1);
  std::string stationPath = DeMuonLocation::Default+stationName;

  SmartDataPtr<DetectorElement> station(m_detSvc, stationPath);
  if( !station ) {
    msg << MSG::INFO << "Error in retrieving DetectorElement!" << endreq;
  }
  return station;
}


DeMuonChamber* DeMuonDetector::Hit2Chamber(Gaudi::XYZPoint myPoint)
{ 
  MsgStream msg(msgSvc(),name());
  DeMuonChamber* ChmbPtr;
  unsigned int istat = getStation(myPoint.z());
  
  StatusCode sc = this->Pos2StChamberPointer(myPoint.x(),
                                             myPoint.y(),
                                             istat,
                                             ChmbPtr);  
  if(sc!=StatusCode::SUCCESS) {
    msg << MSG::ERROR << "Error calling Pos2StChamberPointer function!" << endreq;
  }
  return ChmbPtr;
}

