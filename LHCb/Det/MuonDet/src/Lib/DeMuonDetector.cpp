// $Id: DeMuonDetector.cpp,v 1.1 2005-10-25 06:54:28 asarti Exp $

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
  std::cout << "Initializing the detector" <<std::endl;

  MsgStream msg( msgSvc(), name() );
  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  //Initializing the Layout
  m_chamberLayout.initialize();

  //  m_chamberLayout.setDataProvider(m_detSvc);

  m_chamberLayout.fillChambersVector(m_detSvc);
  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberNumber(const double x,
					     const double y,
					     const double z,
					     int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::SUCCESS;

  //Returning the most likely chamber
  m_chamberLayout.chamberMostLikely(x,y,getStation(z),chamberNumber,regNum);

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
  //  tile = m_chamberLayout.tileChamberNumber(getStation(z),reg,dumChmb);

  return sc;
}

