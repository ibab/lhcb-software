// $Id: DeMuonChamber.cpp,v 1.20 2010-03-17 16:19:06 cattanem Exp $
// ============================================================================
#define MUONDET_DEMUONCHAMBER_CPP 1

// Include files
#include "MuonDet/DeMuonChamber.h"
#include "DetDesc/IGeometryInfo.h"
//Detector descriptions
#include "DetDesc/Condition.h"

/** @file DeMuonChamber.cpp
 * 
 * Implementation of class : DeMuonChamber
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */

/// Standard Constructors
DeMuonChamber::DeMuonChamber()
  : m_msgStream(NULL),
    m_StationNumber(0), 
    m_RegionNumber(0),
    m_ChamberNumber(0)
{
}

/// Constructor seting pad sizes and number of gas gaps and chamber number
DeMuonChamber::DeMuonChamber( int nStation,
                              int nRegion,
                              int nChamber)
  : m_msgStream(NULL),
    m_StationNumber(nStation), 
    m_RegionNumber(nRegion),
    m_ChamberNumber(nChamber) 
{
}

/// Standard Destructor
DeMuonChamber::~DeMuonChamber()
{
  delete m_msgStream; m_msgStream = NULL;
}

StatusCode DeMuonChamber::initialize()  
{
  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msgStream() << MSG::ERROR << "Failure to initialize DetectorElement" << endmsg;
    return sc ; 
  }
  int sta(0),reg(0),chm(0); 
  std::string name=(this->name()).c_str();  
  int len=name.size();
  int start=(DeMuonLocation::Default).size();  
  std::string substring;
  substring.assign(name,start,len);
  char patt[400]; 
  sprintf(patt,"%s",substring.c_str());
  std::string stanum;
  stanum.assign(name,start,3);
  sscanf(stanum.c_str(),"/M%d",&sta);
  std::string regnum;
  regnum.assign(name,start+11,3);
  sscanf(regnum.c_str(),"/R%d",&reg);
  std::string chnum;
  chnum.assign(name,start+19,10);
  sscanf(chnum.c_str(),"/Cham%d",&chm);
  this->setStationNumber(sta-1);
  this->setRegionNumber(reg-1);
  this->setChamberNumber(chm-1);

  // get resolution parameters
  m_chmbGrid     = param<std::string>("Grid");

  // for now with MWPCs and RPCs this is a good formula
  setGridName(m_chmbGrid);
  //msgStream()<<MSG::INFO<<" test ale "<<name <<" "<<sta<<" "<<reg<<" "<<chm<<endmsg;
  return sc;
}

IPVolume* DeMuonChamber::getFirstGasGap(){
  IPVolume* nullPV=NULL;
  ILVolume::PVolumes::const_iterator pvIterator;
//   std::cout<<" debug the ga "<<std::endl;
//   std::cout<<" debug the ga "<<geometry()<<std::endl;
//   std::cout<<" debug the gap "<<geometry()->lvolume()<<std::endl;
//   std::cout<<" debug the gap "<<geometry()->lvolume()->pvBegin()<<std::endl;
//   std::cout<<" debug the gap "<<geometry()->lvolume()->pvEnd()<<std::endl;

  for (pvIterator=((geometry()->lvolume())->pvBegin());
       pvIterator!=(((geometry())->lvolume())->pvEnd());pvIterator++){
    
    const ILVolume*  geoCh=(*pvIterator)->lvolume();
    std::string lvolname=geoCh->name();
    size_t pos;
    pos=lvolname.find_last_of("/");
    std::string mystring=lvolname.substr(pos+1);
 
    //msg<<MSG::ERROR<<"test del nome "<<mystring<<endmsg;
    if(mystring=="lvGasGap"){
      //loop un variys gap layer to get the real gas gap volume
      ILVolume::PVolumes::const_iterator pvGapIterator;
      for (pvGapIterator=(geoCh->pvBegin());pvGapIterator!=(geoCh->pvEnd());pvGapIterator++){
	if(!((*pvGapIterator)->lvolume()->sdName().empty())){
	  return (*pvGapIterator);
	}
      }
    }
  }
  return nullPV;
}
	
IPVolume* DeMuonChamber::getGasGap(int number){
  int loopnumber=0;
  IPVolume* nullPV=NULL;
  ILVolume::PVolumes::const_iterator pvIterator;
  for (pvIterator=((geometry()->lvolume())->pvBegin());
       pvIterator!=(((geometry())->lvolume())->pvEnd());pvIterator++){
    
    const ILVolume*  geoCh=(*pvIterator)->lvolume();
    std::string lvolname=geoCh->name();
    size_t pos;
    pos=lvolname.find_last_of("/");
    std::string mystring=lvolname.substr(pos+1);
    
    //msg<<MSG::ERROR<<"test del nome "<<mystring<<endmsg;
    if(mystring=="lvGasGap"){
      //loop un variys gap layer to get the real gas gap volume
      ILVolume::PVolumes::const_iterator pvGapIterator;
      for (pvGapIterator=(geoCh->pvBegin());pvGapIterator!=(geoCh->pvEnd());pvGapIterator++){
	if(!((*pvGapIterator)->lvolume()->sdName().empty())){
	  if(loopnumber==number) return (*pvGapIterator);
	  loopnumber++;
	 
	}
      }
    }
  }
  return nullPV;
}



IPVolume* DeMuonChamber::getGasGapLayer(int number){
  int loopnumber=0;
  IPVolume* nullPV=NULL;
  ILVolume::PVolumes::const_iterator pvIterator;
  for (pvIterator=((geometry()->lvolume())->pvBegin());
       pvIterator!=(((geometry())->lvolume())->pvEnd());pvIterator++){
    
    const ILVolume*  geoCh=(*pvIterator)->lvolume();
    std::string lvolname=geoCh->name();
    size_t pos;
    pos=lvolname.find_last_of("/");
    std::string mystring=lvolname.substr(pos+1);
    
    //msg<<MSG::ERROR<<"test del nome "<<mystring<<endmsg;
    if(mystring=="lvGasGap"){
      if(loopnumber==number) return (*pvIterator);
      loopnumber++;
	 
    }
    
  }
  return nullPV;
}
	

StatusCode  DeMuonChamber::isPointInGasGap( Gaudi::XYZPoint pointInChamber,
                                            Gaudi::XYZPoint& pointInGap,
                                            int& number,
                                            IPVolume* gasVolume){

 // StatusCode sc;
  int loopnumber=0;
//  IPVolume* nullPV=NULL;
  ILVolume::PVolumes::const_iterator pvIterator;
  for (pvIterator=((geometry()->lvolume())->pvBegin());
       pvIterator!=(((geometry())->lvolume())->pvEnd());pvIterator++){
    
    const ILVolume*  geoCh=(*pvIterator)->lvolume();
    std::string lvolname=geoCh->name();
    size_t pos;
    pos=lvolname.find_last_of("/");
    std::string mystring=lvolname.substr(pos+1);
    
    //msg<<MSG::ERROR<<"test del nome "<<mystring<<endmsg;
    if(mystring=="lvGasGap"){
      //loop un variys gap layer to get the real gas gap volume
      ILVolume::PVolumes::const_iterator pvGapIterator;

  
      bool isIn = (*pvIterator)->isInside(pointInChamber);

      if(isIn){
        Gaudi::XYZPoint myPointInGasFrame=(*pvIterator)->toLocal(pointInChamber);
        for (pvGapIterator=(geoCh->pvBegin());pvGapIterator!=(geoCh->pvEnd());pvGapIterator++){
          if(!((*pvGapIterator)->lvolume()->sdName().empty())){
            bool isInGap = (*pvGapIterator)->isInside(myPointInGasFrame);
            if(isInGap){ 	      
              pointInGap= (*pvGapIterator)->toLocal(myPointInGasFrame);
              gasVolume=(*pvGapIterator);
              number=loopnumber;
              return StatusCode::SUCCESS;
            }else{
              return StatusCode::FAILURE;
            }	    
          }
        }
      }
      loopnumber++;
    }
  }

  return StatusCode::FAILURE;

}

StatusCode  DeMuonChamber::isPointInGasGap(Gaudi::XYZPoint pointInChamber,Gaudi::XYZPoint& pointInGap,IPVolume* gasVolume){
  int number=0;
return isPointInGasGap(pointInChamber,pointInGap,number,gasVolume);
}




int DeMuonChamber::getGasGapNumber(){
  int loopnumber=0;
  //  IPVolume* nullPV=NULL;
  ILVolume::PVolumes::const_iterator pvIterator;
  for (pvIterator=((geometry()->lvolume())->pvBegin());
       pvIterator!=(((geometry())->lvolume())->pvEnd());pvIterator++){
    
    const ILVolume*  geoCh=(*pvIterator)->lvolume();
    std::string lvolname=geoCh->name();
    size_t pos;
    pos=lvolname.find_last_of("/");
    std::string mystring=lvolname.substr(pos+1);
    
    //msg<<MSG::ERROR<<"test del nome "<<mystring<<endmsg;
    if(mystring=="lvGasGap"){
      //loop un variys gap layer to get the real gas gap volume
      ILVolume::PVolumes::const_iterator pvGapIterator;
      for (pvGapIterator=(geoCh->pvBegin());pvGapIterator!=(geoCh->pvEnd());pvGapIterator++){
	if(!((*pvGapIterator)->lvolume()->sdName().empty())){
	 
	  loopnumber++;
	 
	}
      }
    }
  }
  return loopnumber;
}
	
