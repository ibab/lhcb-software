// Include files
#include "MuonChamberLayout.h"
#include "MuonDet/DeMuonDetector.h"
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

  //Maximum number of chambers allowed per region
  //Initialize the maximum number of allowed chambers per region
static constexpr const std::array<int,4> MaxRegions = {12,24,48,192}; // this can go to DDDB

const CLID& DeMuonDetector::clID () const
{
  return DeMuonDetector::classID() ;
}

DeMuonDetector::DeMuonDetector() = default;
DeMuonDetector::~DeMuonDetector() = default;

StatusCode DeMuonDetector::initialize()
{

  if( UNLIKELY( msgStream().level() <= MSG::VERBOSE ) ) 
    msgStream() << MSG::VERBOSE << "Initializing the detector" <<endmsg;
  
  m_detSvc = this->dataSvc();

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) {
    msgStream() << MSG::ERROR << "Failure to initialize DetectorElement" << endmsg;
    return sc ;
  }

  SmartDataPtr<DetectorElement> muonSys (m_detSvc,DeMuonLocation::Default);
  if(!muonSys){
    msgStream()<< MSG::ERROR << "Error retrieving the muon detector "<<endmsg;
    return StatusCode::FAILURE;
  }




  //Getting cabling
  m_isM1defined = false;
  IDetectorElement::IDEContainer::iterator itSt=muonSys->childBegin();

  
  
  for(itSt=muonSys->childBegin(); itSt<muonSys->childEnd(); itSt++){
    if(testForFilter(*itSt)) continue; // skip muon filters

    std::string name=((*itSt)->name()).c_str();
    // Test if M1 is defined in the DB
    m_isM1defined |= (name.find("/M1") != name.npos);
    
    std::string base = DeMuonLocation::Cabling;
    std::string sName, path;
    path = base+"/"+sName.assign(name.end()-2, name.end())+"/Cabling";

    updMgrSvc()->registerCondition(&m_daqHelper, path,
				   &MuonDAQHelper::updateLUT);
      /*
    if(m_isM1defined) {
      updMgrSvc()->registerCondition(&m_daqHelper,
				     "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/Cabling",
				     &MuonDAQHelper::updateLUT);
    }
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
      */
  }
  
  //Initializing the Layout in chamber coordinates
  MuonLayout R1(1,1), R2(1,2), R3(1,4), R4(2,8); // also this can go to DDDB
  m_chamberLayout.reset( new MuonChamberLayout(R1,R2,R3,R4,m_detSvc,msgSvc()) );
  
  m_ChmbPtr =  m_chamberLayout->fillChambersVector(m_detSvc);
  
  //Initialize vectors containing Detector informations
  CountDetEls();

  //fill geo info
  fillGeoInfo();
  fillGeoArray();
  if( UNLIKELY( msgStream().level() <= MSG::VERBOSE ) ) 
    msgStream()<<MSG::VERBOSE<<" ecco qui 111 "<<
      m_stationBox[0][0]<<" "<<
      m_stationBox[0][1]<<" "<<
      m_stationBox[0][2]<<" "<<
      m_stationBox[0][3]<<" "<<endmsg;

  
  m_daqHelper.initSvc(dataSvc(),msgSvc());
  return m_daqHelper.initDAQMaps();
}



StatusCode DeMuonDetector::Hit2GapNumber(Gaudi::XYZPoint myPoint,
                                         int station, int & gapNumber,
                                         int & chamberNumber, int& regNum) const{

  //This methods sets the gap = 0 if only the
  //station is provided [HitPoint.z() = 0]
  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false;

  sc = Hit2ChamberNumber(myPoint,station,chamberNumber,regNum);
  if(sc.isFailure())return sc;

  
  DeMuonChamber * theChmb = getChmbPtr(station,regNum,chamberNumber);
  Gaudi::XYZPoint myPointInChFrame=theChmb ->geometry()->toLocal(myPoint);
  //Set SC to failure until gap is found
  sc = StatusCode::FAILURE;
  //  int gasGapNumber= theChmb->getGasGapNumber();
  //Is the chamber returned containing the hit?
  Gaudi::XYZPoint pointInGap;
  
  IPVolume* gasVolume=NULL;
  StatusCode scGap=  theChmb ->isPointInGasGap(myPointInChFrame,
					       pointInGap,gapNumber,gasVolume);
  if(scGap.isSuccess()){
    isIn=true;
    sc = StatusCode::SUCCESS;
  }



  if(!isIn) { 
    msgStream() << MSG::ERROR << "Gap not found! " <<endmsg;
    
  }
  return sc;
}


StatusCode DeMuonDetector::Hit2ChamberNumber(Gaudi::XYZPoint myPoint,
                                             int station,
                                             int & chamberNumber, int& regNum) const{

  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; bool debug = false;

  double x = myPoint.x();  double y = myPoint.y(); double z = myPoint.z();

  //Returning the most likely chamber
  m_chamberLayout->chamberMostLikely((float)x,(float)y,station,chamberNumber,regNum);
if(debug)std::cout<<chamberNumber<<" "<<regNum<<std::endl;
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

      //Loops on found chambers
      int idx = 0;
      for(auto aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){

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
          msgStream() << MSG::WARNING
                      << "Could not find Geometry info of a given chamber!"
                      <<endmsg;
        }
      }

      //Debug the chambers returned
      if(debug) {
        std::cout<<"Chmb test n.  :"<<chamberNumber+1<<"  ";
        std::cout<<"Chmb ret.  : ";
        for(const auto&  aChamber:myChams) {
          std::cout<<aChamber<<" ";
        }
        std::cout<<" "<<std::endl;
      }
    }
  }

  if(!isIn) {
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
      msgStream() << MSG::DEBUG <<
        "Smart seek didn't work. Perform loop on all chambers :( !!! "
                  <<endmsg;
    int msta(0),mreg(0),mchm(0);
    //Getting stations
    for(auto itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
      if (testForFilter(*itSt)) continue;

      if(msta == station) {
        //Getting regions
        for(auto itSide=(*itSt)->childBegin(); itSide<(*itSt)->childEnd(); itSide++){
          mreg = 0;
          for(auto itRg=(*itSide)->childBegin(); itRg<(*itSide)->childEnd(); itRg++){
            //Getting chambers
            mchm = 0;
            for(auto itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
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
                if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
                  msgStream() << MSG::DEBUG << "Hit found in chamber C: " <<
                    chamberNumber<<" , R: "<<regNum<<" ,S: "<<station<<endmsg;
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
                                                DeMuonChamber* & chamberPointer)const{
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  Gaudi::XYZPoint hitPoint(x,y,0);
  int chamberNumber(-1),regNum(-1);
  StatusCode sc = Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);
  if(sc.isFailure()) return sc;

  if((regNum > -1) && (chamberNumber>-1)) {
    chamberPointer = getChmbPtr(station,regNum,chamberNumber) ;
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

  //Z is know/provided.
  Gaudi::XYZPoint hitPoint(x,y,z);
  int sta = getStation(z);
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

void DeMuonDetector::CountDetEls() {

  int msta(0),mallreg(0);

  //Getting stations
  for(auto itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    if (testForFilter(*itSt)) continue;
    if(*itSt) msta++;

    //Getting regions
    auto itSide=(*itSt)->childBegin();
    auto mreg = std::count_if((*itSide)->childBegin(), (*itSide)->childEnd(),
                              [](const IDetectorElement* rg) -> bool 
                              { return rg; } );
    m_regsperSta[msta-1] = mreg;
    mallreg += mreg;

  }//Stations Loop

  m_stations = msta;
  // these are the so-called partitions i.e. regions summed over stations
  m_regions = mallreg;
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

  if (!myPtr)
    msgStream() << MSG::WARNING << "DeMuonDetector::getChmbPtr"
      "is returning NULL pointer" << endmsg;

  return myPtr;
}

std::vector< std::pair<MuonFrontEndID, std::vector<float> > >
DeMuonDetector::listOfPhysChannels(Gaudi::XYZPoint my_entry, Gaudi::XYZPoint my_exit,
                                   int region, int chamber) {

  //Pair vector;
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > tmpPair;

  int regNum(0),chamberNumber(0);
  int station = getStation(my_entry.z());
  int regNum1(0),chamberNumber1(0);
  int station1 = getStation(my_exit.z());
  int gapCnt=0;
  if((region == -1) || (chamber == -1)) {
    //Hit entry
    Hit2ChamberNumber(my_entry,station,chamberNumber,regNum);

    //Hit exit
    Hit2ChamberNumber(my_exit,station1,chamberNumber1,regNum1);

    if(chamberNumber != chamberNumber1 || regNum != regNum1) {
      msgStream() << MSG::ERROR <<
        "Hit entry and exit are in different chambers! Returning a void list."
                  <<endmsg;
      return tmpPair;
    }
  } else {
    //If given.. assign region and chamber number
    regNum = region; chamberNumber = chamber;
  }

  //Getting the chamber pointer.
  DeMuonChamber*  myChPtr =  getChmbPtr(station,regNum,chamberNumber); 
  //check if the retrieved chamber is OK
  Gaudi::XYZPoint midInCh= (myChPtr ->geometry())->toLocal(Gaudi::XYZPoint(
                              ((my_entry.x()+my_exit.x())/2),
                              ((my_entry.y()+my_exit.y())/2),
                              ((my_entry.z()+my_exit.z())/2)));
  Gaudi::XYZPoint entryInCh= (myChPtr ->geometry())->toLocal(my_entry);
  Gaudi::XYZPoint exitInCh= (myChPtr ->geometry())->toLocal(my_exit);
  //  int gapsInCh=myChPtr->getGasGapNumber();
  bool isIn = false;
  // int GapCnt=0;
  Gaudi::XYZPoint pointInGap;
  int gapNumber=-1;
  IPVolume* gasVolume=NULL;
  StatusCode inGap=myChPtr-> isPointInGasGap(midInCh,pointInGap,gapNumber,gasVolume);
  if(inGap.isSuccess()){
    isIn=true;
    gapCnt=gapNumber;
  }

 //  if(!myGap) {
  if(!isIn) {
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
      msgStream() << MSG::DEBUG <<"Could not find the gap. Returning a void list."<<endmsg;
    m_hitNotInGap++;	 
    return tmpPair;
  }
  
  //Gap Geometry info 
  //This is OK ONLY if you want to access x and y informations.
  //Otherwise you need to go down to the volume of gas gap inside 
  IPVolume* gasLayer=myChPtr->getGasGapLayer(gapCnt);
 

  //Retrieve the chamber box dimensions  
  const SolidBox *box = dynamic_cast<const SolidBox *>
    (gasLayer->lvolume()->solid());
  double dx = box->xHalfLength();  double dy = box->yHalfLength();


  //Refer the distances to Local system [should be the gap]
  Gaudi::XYZPoint new_entry = gasLayer->toLocal(entryInCh);
  Gaudi::XYZPoint new_exit  = gasLayer->toLocal(exitInCh);


  Gaudi::XYZPoint LL(-dx,-dy,0);
  Gaudi::XYZPoint LR(dx,-dy,0);
  Gaudi::XYZPoint UL(dx,dy,0);
  //  Gaudi::XYZPoint UR(-dx,dy,0);
  


									
  Gaudi::XYZPoint lowerleft =(myChPtr ->geometry())->toGlobal( gasLayer->toMother(LL));
  Gaudi::XYZPoint lowerright  = (myChPtr ->geometry())->toGlobal(gasLayer->toMother(LR));
  Gaudi::XYZPoint upperleft = (myChPtr ->geometry())->toGlobal( gasLayer->toMother(UL));
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
       msgStream()<<MSG::INFO<<" Should never enter here "<<endmsg;
    }else if(lowerleft.x()>lowerright.x()&&(lowerleft.y()<upperleft.y())){
      mod_xen = (-new_entry.x()+dx)/(2*dx);
      mod_yen = (new_entry.y()+dy)/(2*dy);
      mod_xex = (-new_exit.x()+dx)/(2*dx);
      mod_yex = (new_exit.y()+dy)/(2*dy);  
      msgStream()<<MSG::INFO<<" Should never enter here "<<endmsg;
    }
  } else {
  msgStream()  << MSG::ERROR <<"Null chamber dimensions. Returning a void list."<<
      endmsg; 
    return tmpPair;
  }

  //Getting the grid pointer
  Condition* aGrid = myChPtr->condition(myChPtr->getGridName());
  MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(aGrid);

  //Convert relative distances into absolute ones
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > myPair;
  std::vector<float> myVec; MuonFrontEndID myFE;
  if(theGrid) {
    //Gets list of channels

    tmpPair = theGrid->listOfPhysChannels(mod_xen,mod_yen,mod_xex,mod_yex);
    for(auto tmpPair_it = tmpPair.begin();tmpPair_it<tmpPair.end(); tmpPair_it++){
      myFE  = tmpPair_it->first;
      myVec = tmpPair_it->second;
      myFE.setLayer(gapCnt/2);

      for(int iDm = 0; iDm<4; iDm++){  
        myVec.at(iDm) = iDm%2 ? myVec.at(iDm)*2*(float)dy : myVec.at(iDm)*2*(float)dx;
        //Added resolution effect
        if(fabs(myVec.at(iDm)) < 0.0001) myVec.at(iDm) = 0;
      }
      myPair.push_back(std::pair< MuonFrontEndID,std::vector<float> >
                       (myFE,myVec));
    }
  } else {
      msgStream()<<  MSG::ERROR <<
	"No grid found. Returning a void list."<<endmsg; 
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
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) 
    msgStream() << MSG::DEBUG <<"Calling Tile2XYZpos method!"<<endmsg;

  sc = m_chamberLayout->Tile2XYZpos(tile,x,dx,y,dy,z,dz);

  return sc;
}

StatusCode DeMuonDetector::getPCCenter(MuonFrontEndID fe,int chamber,
                                       int station,int region,
                                       double& xcenter, double& ycenter,
                                       double& zcenter)
{

  DeMuonChamber*  myChPtr =  getChmbPtr(station,region,chamber) ;

  IPVolume*  myGap=myChPtr-> getGasGapLayer(0);

  const SolidBox *box = dynamic_cast<const SolidBox *>
    (myGap->lvolume()->solid());
  double dx = box->xHalfLength();  
  double dy = box->yHalfLength();
  Condition* aGrid = 
  myChPtr->condition(myChPtr->getGridName());
  MuonChamberGrid* theGrid = static_cast<MuonChamberGrid*>(aGrid);
  double xcenter_norma=-1;
  double ycenter_norma=-1;  
  StatusCode sc=theGrid->getPCCenter(fe,xcenter_norma,ycenter_norma);
  if(sc.isFailure())return StatusCode::FAILURE;
  double xcenter_gap=xcenter_norma*2*dx-dx;
  double ycenter_gap=ycenter_norma*2*dy-dy;

  Gaudi::XYZPoint loc(xcenter_gap,ycenter_gap,0);   
  Gaudi::XYZPoint refInCh= myGap->toMother(loc);
  Gaudi::XYZPoint glob= myChPtr->geometry()->toGlobal(refInCh);
 
  xcenter=glob.x();
  ycenter=glob.y();
  zcenter=glob.z();
  return StatusCode::SUCCESS;
}


StatusCode  DeMuonDetector::Chamber2Tile(int  chaNum, int station, int region,
                                         LHCb::MuonTileID& tile) const
{
  //Convert chamber number into a tile
  tile = m_chamberLayout->tileChamberNumber(station,region,chaNum);
  return StatusCode::SUCCESS;
}


void DeMuonDetector::fillGeoInfo()
{
  //    bool debug=false;
  //  bool debug=true;

  int station=0;
  int Side=0;
  int region=0;

  for(auto itSt = this->childBegin(); itSt<this->childEnd(); itSt++){ // loop over detector elements

    //Check if the "station" isn't actually a filter
    if (testForFilter(*itSt)) continue;

    // get station z-coordinates
    IGeometryInfo*  geoSt=(*itSt)->geometry();
    Gaudi::XYZPoint globSt= geoSt->toGlobal(Gaudi::XYZPoint(0,0,0));
    //    m_stationZ[station] = globSt.z();
        m_stationZ.push_back(globSt.z());
	//    msgStream()<<MSG::INFO<<"STATION: "<<station << " --> z: " <<
        //    m_stationZ.at(station) <<endmsg;
    //    m_stationZ[station]  <<endmsg;
    //

    // now crawl down to the chambers

    Side=0;

    
    for(auto itSide=(*itSt)->childBegin(); itSide<(*itSt)->childEnd(); itSide++){ // loop over sides

      region=0;
      //      if(debug)
      // msgStream()<<MSG::INFO<<" station "<<station<<endmsg;

      for(auto itRg=(*itSide)->childBegin(); itRg<(*itSide)->childEnd(); itRg++){ // loop over regions

        auto itCh=(*itRg)->childBegin();

	// apparently it loops only over the first chamber to get general information
	// can it be made equivalently using begin() iterator ?
	//        for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){ // loop over chambers
	//    msgStream()<<MSG::INFO<<"DET ELEM cham: "<<(*itCh)->name()<<endmsg;
          DeMuonChamber* chPt=dynamic_cast<DeMuonChamber*> (*itCh);
          int gaps=0;
          double area=0;

          for (auto pvIterator=((((*itCh)->geometry())->lvolume())->pvBegin());
               pvIterator!=((((*itCh)->geometry())->lvolume())->pvEnd());
               pvIterator++){
            
            const ILVolume*  geoCh=(*pvIterator)->lvolume();
            std::string lvolname=geoCh->name();
            size_t pos;
            pos=lvolname.find_last_of("/");
            std::string mystring=lvolname.substr(pos+1);
	    //    msgStream()<<MSG::INFO<<"VOL NAME: "<<lvolname<<endmsg;
            
	    
    if(mystring=="lvGasGap"){ // these are the gas gaps
	    
              for (auto pvGapIterator=(geoCh->pvBegin());
                   pvGapIterator!=(geoCh->pvEnd());pvGapIterator++){

		//		msgStream()<<MSG::INFO<<"Gap Volume Name: "
		//   <<(*pvGapIterator)->lvolume()->sdName()<<endmsg;

                if( !((*pvGapIterator)->lvolume()->sdName().empty()) ){
		  //                  if(debug)msgStream()<<MSG::ERROR<<

                  const ILVolume* geoGap=(*pvGapIterator)->lvolume();
                  //Retrieve the chamber box dimensions  
                  const SolidBox *box = dynamic_cast<const SolidBox *>
                    (geoGap->solid());
                  float dx = box->xHalfLength();
                  float dy = box->yHalfLength();
                  float dz = box->zHalfLength();
                  m_sensitiveAreaX[station*4+region]=2*dx;
                  m_sensitiveAreaY[station*4+region]=2*dy;
                  m_sensitiveAreaZ[station*4+region]=2*dz;
                  area=4*dx*dy;
                  m_areaChamber[station*4+region]=area;
                  gaps++;
                }
              }
            }
          }          
          
          
          m_gapPerRegion[station*4+region]=gaps;
          m_gapPerFE[station*4+region]=gaps/2;

          Condition* bGrid = (chPt)->condition(chPt->getGridName());
          MuonChamberGrid* theGrid = static_cast<MuonChamberGrid*>(bGrid);
          
	  // the grid condition is the size of physical readout channels for a certain 
	  // readout type (i.e. wires or cathode pads) normalized to the 
	  // number of physical channels of the same type in the chamber
	  // By convention 0=wires, 1=cathode pads

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
	  //          if(debug)msgStream()<<MSG::INFO<<" red and maps "<<nreadout<<" "<<maps<<endmsg;
          if(nreadout==1)
          {
            for( int i = 0; i<maps;i++){
              m_LogMapRType[i][station*4+region]=
                m_readoutType[0][station*4+region];
              m_LogMapMergex[i][station*4+region]=
                (theGrid->getMapGrid())[i*2]; // these are the log OR of the physical channels
              m_LogMapMergey[i][station*4+region]=
                (theGrid->getMapGrid())[i*2+1]; // these are the log OR of the physical channels
	      //              if(debug)msgStream()<<MSG::INFO<<" red and maps "<<i<<" "<< m_LogMapRType[i][station*4+region]<<" "<<
	      //                m_LogMapMergex[i][station*4+region]<<" "<<m_LogMapMergey[i][station*4+region]<<endmsg;
            }
          }else if(nreadout==2){
            for( int i = 0; i<maps;i++){
              m_LogMapRType[i][station*4+region]=
                m_readoutType[i][station*4+region];
              m_LogMapMergex[i][station*4+region]=
                (theGrid->getMapGrid())[i*2];
              m_LogMapMergey[i][station*4+region]=
                (theGrid->getMapGrid())[i*2+1];
	      //              if(debug)msgStream()<<MSG::INFO<<" red and maps "<<i<<" "<< m_LogMapRType[i][station*4+region]<<" "<<
	      //   m_LogMapMergex[i][station*4+region]<<" "<<m_LogMapMergey[i][station*4+region]<<endmsg;
            }
          }

	  //          break;
	  //        }
        int chamber=0;
        for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
          chamber++;
        }m_chamberPerRegion[station*4+region]=chamber*2;
        region++;
      }
      Side++;
    }
    station++;
  } // end of loop over detector elements

  // initialization by hand of the logical layout in the different regions
  // che orrore...

  if(m_isM1defined) {
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
    
  } else {
    //    m_layoutX[0][0]=24;  //M1
    //    m_layoutX[0][1]=24;  //M1
    //    m_layoutX[0][2]=24;  //M1
    //    m_layoutX[0][3]=24;  //M1
    m_layoutX[0][ 0]=48; //M2 vstrips
    m_layoutX[0][ 1]=48; //M2 vstrips
    m_layoutX[0][ 2]=48; //M2 vstrips
    m_layoutX[0][ 3]=48; //M2 vstrips
    m_layoutX[1][ 0]=8;  //M2 hstrips
    m_layoutX[1][ 1]=4;  //M2 hstrips
    m_layoutX[1][ 2]=2;  //M2 hstrips
    m_layoutX[1][ 3]=2;  //M2 hstrips
    m_layoutX[0][ 4]=48; //M3 vstrips
    m_layoutX[0][ 5]=48; //M3 vstrips
    m_layoutX[0][ 6]=48; //M3 vstrips
    m_layoutX[0][ 7]=48; //M3 vstrips
    m_layoutX[1][ 4]=8;  //M3 hstrips
    m_layoutX[1][ 5]=4;  //M3 hstrips
    m_layoutX[1][ 6]=2;  //M3 hstrips
    m_layoutX[1][ 7]=2;  //M3 hstrips
    m_layoutX[0][ 8]=12; //M4 vstrips
    m_layoutX[0][ 9]=12; //M4 vstrips
    m_layoutX[0][10]=12; //M4 vstrips
    m_layoutX[0][11]=12; //M4 vstrips
    m_layoutX[1][ 9]=4;  //M4 hstrips
    m_layoutX[1][10]=2;  //M4 hstrips
    m_layoutX[1][11]=2;  //M4 hstrips
    m_layoutX[0][12]=12; //M5 vstrips
    m_layoutX[0][13]=12; //M5 vstrips
    m_layoutX[0][14]=12; //M5 vstrips
    m_layoutX[0][15]=12; //M5 vstrips
    m_layoutX[1][13]=4;  //M5 hstrips
    m_layoutX[1][14]=2;  //M5 hstrips
    m_layoutX[1][15]=2;  //M5 hstrips
 
    //    m_layoutY[0][0]=8; //M1
    //    m_layoutY[0][1]=8; //M1
    //    m_layoutY[0][2]=8; //M1
    //    m_layoutY[0][3]=8; //M1
    m_layoutY[0][ 0]=1;  //M2 vstrips
    m_layoutY[0][ 1]=2;	//M2 vstrips
    m_layoutY[0][ 2]=2;	//M2 vstrips
    m_layoutY[0][ 3]=2;	//M2 vstrips
    m_layoutY[1][ 0]=8;	//M2 hstrips
    m_layoutY[1][ 1]=8;	//M2 hstrips
    m_layoutY[1][ 2]=8;	//M2 hstrips
    m_layoutY[1][ 3]=8;	//M2 hstrips
    m_layoutY[0][ 4]=1;	//M3 vstrips
    m_layoutY[0][ 5]=2;	//M3 vstrips
    m_layoutY[0][ 6]=2;	//M3 vstrips
    m_layoutY[0][ 7]=2;	//M3 vstrips
    m_layoutY[1][ 4]=8;	//M3 hstrips
    m_layoutY[1][ 5]=8;	//M3 hstrips
    m_layoutY[1][ 6]=8;	//M3 hstrips
    m_layoutY[1][ 7]=8;	//M3 hstrips
    m_layoutY[0][ 8]=8;	//M4 vstrips
    m_layoutY[0][ 9]=2;	//M4 vstrips
    m_layoutY[0][10]=2;	//M4 vstrips
    m_layoutY[0][11]=2;	//M4 vstrips
    m_layoutY[1][ 9]=8;	//M4 hstrips
    m_layoutY[1][10]=8;	//M4 hstrips
    m_layoutY[1][11]=8;	//M4 hstrips
    m_layoutY[0][12]=8;	//M5 vstrips
    m_layoutY[0][13]=2;	//M5 vstrips
    m_layoutY[0][14]=2;	//M5 vstrips
    m_layoutY[0][15]=2;	//M5 vstrips
    m_layoutY[1][13]=8;	//M5 hstrips
    m_layoutY[1][14]=8;	//M5 hstrips
    m_layoutY[1][15]=8;	//M5 hstrips
  //initializatuon by hand of the OR inside the same Cardiac
    /*
    m_phCardiacORNX[1][0]=1;
    m_phCardiacORNX[1][1]=1;
    m_phCardiacORNX[1][2]=2;
    m_phCardiacORNX[0][3]=2;

    m_phCardiacORNY[1][0]=1;
    m_phCardiacORNY[1][1]=2;
    m_phCardiacORNY[1][2]=1;
    m_phCardiacORNY[0][3]=1;
    */    
    m_phCardiacORNX[0][0]=1;
    m_phCardiacORNX[1][0]=1;
    m_phCardiacORNX[0][1]=1;
    m_phCardiacORNX[1][1]=2;
    m_phCardiacORNX[1][2]=1;
    m_phCardiacORNX[0][3]=1;
    
    m_phCardiacORNY[0][0]=1;
    m_phCardiacORNY[1][0]=1;
    m_phCardiacORNY[0][1]=1;
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
    
    m_phCardiacORNX[1][8]=1;
    m_phCardiacORNX[1][9]=1;
    m_phCardiacORNX[1][10]=2;
    m_phCardiacORNX[0][11]=4;
    
    m_phCardiacORNY[1][8]=1;
    m_phCardiacORNY[1][9]=1;
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
  }
  /*
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
  */
  // fill pad sizes

  //  msgStream()<<MSG::INFO<<"Stations/regions in this setup: "<<station<<"/"<<region<<endmsg;

  for(int stat=0;stat<m_stations;stat++){
    for(int reg=0;reg<m_regsperSta[stat];reg++){
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
        m_padSizeY[part]=(m_sensitiveAreaY[part]/m_phChannelNY[1][part])* mgy;
      }
    }
  }
}

void DeMuonDetector::fillGeoArray()
{
  bool debug=false;
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();

  MuonLayout layoutInner=m_chamberLayout->layout(0);
  MuonLayout layoutOuter=m_chamberLayout->layout(3);
  int station=0;
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) {
    msgStream()<<MSG::DEBUG<< "layout inner "<<
      layoutInner.xGrid()<<" "<<  layoutInner.yGrid()<<endmsg;
    msgStream()<<MSG::DEBUG<< "layout outer "<<
      layoutOuter.xGrid()<<" "<<  layoutOuter.yGrid()<<endmsg;
  }

  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    //get the dimensions of the inner rectangular

    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
      msgStream()<<MSG::DEBUG<<"DeMuonDetector::fillGeoArray station: " << station <<endmsg;
    if (testForFilter(*itSt) == true)
      continue;
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
      msgStream()<<MSG::DEBUG<<"DeMuonDetector::fillGeoArray station: " << station <<endmsg;

    double minX=100000;
    double minY=100000;
    for(unsigned int nx=0;nx<layoutInner.xGrid();nx++){
      for(int qua=0;qua<4;qua++){
        LHCb::MuonTileID chTile(station,layoutInner,0,qua,nx,
                                layoutInner.yGrid());
        int chNumber=m_chamberLayout->getChamberNumber(chTile);
        DeMuonChamber* chP=getChmbPtr(station,0,chNumber);
        IPVolume* myGapVol=chP->getGasGapLayer(0);

        IGeometryInfo*  geoCh=chP->geometry();
        
          //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (myGapVol->lvolume()->solid());
        double dx = box->xHalfLength();
        double dy = box->yHalfLength(); 
        Gaudi::XYZPoint glob1= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,-dy,0)));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,dy,0)));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,-dy,0)));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,dy,0)));
	if(debug)msgStream()<<MSG::ERROR<< 
myGapVol->toMother(Gaudi::XYZPoint(-dx,-dy,0))<<endmsg;
	
if(debug)msgStream()<<MSG::ERROR<<geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,-dy,0)))<<endmsg;
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
        IPVolume* myGapVol=chP->getGasGapLayer(0);
        
        
        IGeometryInfo*  geoCh=chP->geometry();
        
        //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (myGapVol->lvolume()->solid());
        
        double dx = box->xHalfLength();
        double dy = box->yHalfLength();
        Gaudi::XYZPoint glob1= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,-dy,0)));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,dy,0)));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,-dy,0)));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,dy,0)));
        if(fabs(glob1.x())<minX)minX=fabs(glob1.x());      
        if(fabs(glob2.x())<minX)minX=fabs(glob2.x());      
        if(fabs(glob3.x())<minX)minX=fabs(glob3.x());      
        if(fabs(glob4.x())<minX)minX=fabs(glob4.x());      
      }      
    }
    
    m_stationBox[station][0]=minX;
    m_stationBox[station][1]=minY;
    //now the dimsnion of the outer parr...
    if(debug)msgStream()<<MSG::DEBUG<<" min size "<<minX<<" "<<minY<<endmsg;
    double maxX=0;
    double maxY=0;
    for(unsigned int nx=0;nx<2*layoutOuter.xGrid();nx++){
      for(int qua=0;qua<4;qua++){          


        LHCb::MuonTileID chTile(station,layoutOuter,3,qua,nx,
                                2*layoutOuter.yGrid()-1);
        int chNumber=m_chamberLayout->getChamberNumber(chTile);        
        DeMuonChamber* chP=getChmbPtr(station,3,chNumber);
        IPVolume* myGapVol=chP->getGasGapLayer(0);

        
        IGeometryInfo*  geoCh=chP->geometry();
        
          //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (myGapVol->lvolume()->solid());

        double dx = box->xHalfLength();
        double dy = box->yHalfLength();
        Gaudi::XYZPoint glob1= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,-dy,0)));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,dy,0)));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,-dy,0)));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,dy,0)));
        

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
        IPVolume* myGapVol=chP->getGasGapLayer(0);

        
        IGeometryInfo*  geoCh=chP->geometry();
        
          //Retrieve the chamber box dimensions  
        const SolidBox *box = dynamic_cast<const SolidBox *>
          (myGapVol->lvolume()->solid());

        double dx = box->xHalfLength();
        double dy = box->yHalfLength();
        Gaudi::XYZPoint glob1= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,-dy,0)));
        Gaudi::XYZPoint glob2= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(-dx,dy,0)));
        Gaudi::XYZPoint glob3= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,-dy,0)));
        Gaudi::XYZPoint glob4= geoCh->toGlobal(myGapVol->toMother(Gaudi::XYZPoint(dx,dy,0)));
        if(fabs(glob1.x())>maxX)maxX=fabs(glob1.x());      
        if(fabs(glob2.x())>maxX)maxX=fabs(glob2.x());      
        if(fabs(glob3.x())>maxX)maxX=fabs(glob3.x());      
        if(fabs(glob4.x())>maxX)maxX=fabs(glob4.x());      
      }      
    }

    m_stationBox[station][2]=maxX;
    m_stationBox[station][3]=maxY;


    if( UNLIKELY( msgStream().level() <= MSG::VERBOSE ) ) 
      msgStream()<<MSG::VERBOSE<<" station  inner "<<station<<" "<<
        m_stationBox[station][0]<<
        " "<<m_stationBox[station][1]<<
        " station  outer "<<station<<" "<<
        m_stationBox[station][2]<<
        " "<<m_stationBox[station][3]<<endmsg;

    station++;
  }
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

  unsigned int istat = aTile.station();
  sprintf(stationName,"/M%u",istat+1);
  std::string stationPath = DeMuonLocation::Default+stationName;

  SmartDataPtr<DetectorElement> station(m_detSvc, stationPath);
  if( !station ) {
    msgStream() << MSG::ERROR << "Error in retrieving DetectorElement!" << endmsg;
  }
  return station;
}


std::vector<DeMuonChamber*> DeMuonDetector::Tile2Chamber(LHCb::MuonTileID aTile)
{
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
  unsigned int istat = getStation(myPoint.z());
  auto stationName = DeMuonLocation::Default+ "/M" + std::to_string( istat+(m_isM1defined?1:2));

  SmartDataPtr<DetectorElement> station(m_detSvc, stationName );
  if( !station ) {
    msgStream() << MSG::ERROR << "Error in retrieving DetectorElement!" << endmsg;
  }
  return station;
}


DeMuonChamber* DeMuonDetector::Hit2Chamber(Gaudi::XYZPoint myPoint)
{
  DeMuonChamber* ChmbPtr;
  unsigned int istat = getStation(myPoint.z());

  StatusCode sc = this->Pos2StChamberPointer(myPoint.x(),
                                             myPoint.y(),
                                             istat,
                                             ChmbPtr);
  if(sc!=StatusCode::SUCCESS) {
    msgStream() << MSG::ERROR << "Error calling Pos2StChamberPointer function!" << endmsg;
  }
  return ChmbPtr;
}

