// $Id: DeMuonChamber.cpp,v 1.15 2007-10-17 11:24:04 asatta Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
//
// ============================================================================
#define MUONDET_DEMUONCHAMBER_CPP 1
// ============================================================================

// Include files
#include "MuonDet/DeMuonChamber.h"

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
  : m_StationNumber(0), 
    m_RegionNumber(0),
    m_ChamberNumber(0)
{
}

/// Constructor seting pad sizes and number of gas gaps and chamber number
DeMuonChamber::DeMuonChamber( int nStation,
                              int nRegion,
                              int nChamber)
  : m_StationNumber(nStation), 
    m_RegionNumber(nRegion),
    m_ChamberNumber(nChamber) 
{
}

/// Standard Destructor
DeMuonChamber::~DeMuonChamber()
{
}


StatusCode DeMuonChamber::initialize()  
{
  MsgStream msg( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
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
  //msg<<MSG::INFO<<" test ale "<<name <<" "<<sta<<" "<<reg<<" "<<chm<<endreq;
  return sc;
}

