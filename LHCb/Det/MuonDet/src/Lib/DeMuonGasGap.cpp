// $Id: DeMuonGasGap.cpp,v 1.14 2008-04-02 11:47:48 asatta Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================

// Include files
#include "MuonDet/DeMuonGasGap.h"

/** @file DeMuonGasGap.cpp
 * 
 * Implementation of class : DeMuonChamber
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */

DeMuonGasGap::DeMuonGasGap() :
  m_StationNumber(0),
  m_RegionNumber(0),
  m_ChamberNumber(0),
  m_GasGapNumber(0)
{
}

DeMuonGasGap::DeMuonGasGap(int nStation, int nRegion, 
                           int nChamber, int nGasGap) :
  m_StationNumber(nStation),
  m_RegionNumber(nRegion),
  m_ChamberNumber(nChamber),
  m_GasGapNumber(nGasGap)
{
}

DeMuonGasGap::~DeMuonGasGap()
{
}


StatusCode DeMuonGasGap::initialize()  
{
  MsgStream msg( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  int sta(0),reg(0),chm(0),gap(0);   
  std::string name=(this->name()).c_str();  
  int len=name.size();
  int start=(DeMuonLocation::Default).size();  
  std::string substring;
  substring.assign(name,start,len);
//  char patt[400]; 
  std::string stanum;
  stanum.assign(name,start,3);
  sscanf(stanum.c_str(),"/M%d",&sta);
  std::string regnum;
  regnum.assign(name,start+11,3);
  sscanf(regnum.c_str(),"/R%d",&reg);
  std::string chnum;
  chnum.assign(name,start+19,10);
  sscanf(chnum.c_str(),"/Cham%d",&chm);
  std::string gapnum;
  gapnum.assign(name,start+27,10);
  sscanf(gapnum.c_str(),"/Gap%d",&gap);

  this->setStationNumber(sta-1);
  this->setRegionNumber(reg-1);
  this->setChamberNumber(chm-1);
  this->setGasGapNumber(gap-1);

  //  msg << MSG::INFO << "Gap INFO? " << name<<" "<<sta <<" "<< reg<<" "<< chm<<" "<<gap<<endreq;

  return sc;
}
