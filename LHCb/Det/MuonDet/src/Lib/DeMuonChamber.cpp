// $Id: DeMuonChamber.cpp,v 1.5 2005-10-25 06:59:08 asarti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
//
// ============================================================================
#define MUONDET_DEMUONCHAMBER_CPP 1
// ============================================================================

// Include files
#include "MuonDet/DeMuonChamber.h"

/** @file DeMuonChamber.cpp
 * 
 * Implementation of class : DeMuonChamber
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */

#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef XmlUserDetElemCnv<DeMuonChamber>       XmlDeMuonChamber;
static CnvFactory<XmlDeMuonChamber>           s_XmlDeMuonChamberFactory ;
const ICnvFactory&  XmlDeMuonChamberFactory = s_XmlDeMuonChamberFactory ;

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
  char patt[400]; 
  sprintf(patt,"%s",(this->name()).data());
  sscanf(patt,"/dd/Structure/LHCb/Muon/M%d/R%d/Cham%d",&sta,&reg,&chm);

  this->setStationNumber(sta-1);
  this->setRegionNumber(reg-1);
  this->setChamberNumber(chm-1);

  //  msg << MSG::INFO << "Chamber name? " << sta <<" "<< reg<<" "<< chm<<endreq;
  // get resolution parameters
  m_chmbGrid     = param<std::string>("Grid");

  // for now with MWPCs and RPCs this is a good formula
  setchmbGrid(m_chmbGrid);
  return sc;
}
