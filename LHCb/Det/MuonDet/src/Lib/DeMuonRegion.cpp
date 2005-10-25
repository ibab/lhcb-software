// $Id: DeMuonRegion.cpp,v 1.7 2005-10-25 06:59:08 asarti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================

// Include files
#include "MuonDet/DeMuonRegion.h"

/** @file DeMuonRegion.cpp
 * 
 * Implementation of class : DeMuonRegion
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef XmlUserDetElemCnv<DeMuonRegion>       XmlDeMuonRegion;
static CnvFactory<XmlDeMuonRegion>           s_XmlDeMuonRegionFactory ;
const ICnvFactory&  XmlDeMuonRegionFactory = s_XmlDeMuonRegionFactory ;

/// Standard Constructor
DeMuonRegion::DeMuonRegion()
  : m_chamberNum(0), m_GapsPerFE(0), 
    m_FEAnodeX(0), m_FEAnodeY(0), 
    m_FECathodeX(0), m_FECathodeY(0), m_logMap()
{
}

/// Standard Destructor
DeMuonRegion::~DeMuonRegion()
{
}
  
StatusCode DeMuonRegion::initialize()  
{
  MsgStream msg( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  // get resolution parameters
  m_GapsPerFE     = param<int>("gapsPerFe");
  m_FEAnodeX      = param<int>("feAnodeX");
  m_FEAnodeY      = param<int>("feAnodeY");
  m_chamberNum    = param<int>("chmbNum");

  // for now with MWPCs and RPCs this is a good formula
  setGapsPerFE(m_GapsPerFE);
  setFEAnodeX(m_FEAnodeX);

  //  setFEAnodeY(1);
  addLogicalMap(MuonParameters::Anode, 1, 1);
  //  setFECathodeX( atol(NFEChamberX.c_str()) );
  //  setFECathodeY( atol(NFEChamberY.c_str()) );
  //  setchamberNum(gasGapChamber);


  return sc;
}


  
/// Add a logical mapping between FE readout channels and 
/// logical channels (or pads if not crossed)
void DeMuonRegion::addLogicalMap( const MuonParameters::ReadoutType &type, 
                                  const int &xMap, const int &yMap){
  m_logMap.push_back(logMap_(type,xMap,yMap));
}

/// Get the logical mappings beween FE readout channels and 
/// logical channels (or pads if not crossed)
StatusCode DeMuonRegion::logicalMap(const unsigned int& mapNum,
                                    MuonParameters::ReadoutType &rType,
                                    int &xMap, int &yMap) const {
  if(mapNum >= m_logMap.size()){
    return StatusCode::FAILURE;
  }
  xMap = m_logMap[mapNum].m_xMap;
  yMap = m_logMap[mapNum].m_yMap;
  rType = m_logMap[mapNum].m_type;
  return StatusCode::SUCCESS;
}
