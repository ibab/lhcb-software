// $Id: XmlMuonRegionCnv.cpp,v 1.4 2002-02-21 16:38:44 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/02/01 18:02:14  dhcroft
// Fixed overrun errors in sprintf, thanks to Pere
//
// Revision 1.2  2002/01/31 10:00:10  dhcroft
// Moved CLIDs to seperate files for Visual C linker
//
// ============================================================================

// Include files
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "DetDesc/XmlUserDetElemCnv.h"
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/DeMuonGasGap.h"

/// Gaudi interfaces
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IDataProviderSvc.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/GenericAddress.h"
#include <dom/DOM_NamedNodeMap.hpp>

/** @class XmlMuonRegionCnv
 *
 * XML converter for MuonRegion
 *
 * @author David Hutchcroft
 */
class XmlMuonRegionCnv : public XmlUserDetElemCnv<DeMuonRegion> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonRegionCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  ~XmlMuonRegionCnv() {}

  virtual StatusCode initialize();
  
protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DeMuonRegion* dataObj,
                                        IOpaqueAddress* address);
  
private:

  // calls to break up the code into more managable blocks.
  StatusCode chamberRead (DOM_Element &childElement, 
                          DeMuonRegion* dataObj,
                          IOpaqueAddress* address);

  StatusCode chamberChildrenRead(DOM_Element &childElement,
                                 std::string &readout,
                                 int &gasGapNumber, int &gasGapOffset,
                                 std::string &gasGapLogvol,
                                 std::string &gasGapSupport);

  StatusCode makeChamberObjects(DOM_Element &childElement,
                                DeMuonRegion *dataObj,
                                IOpaqueAddress* address,
                                double padx, double pady,
                                std::string &readout,
                                int &gasGapNumber, int &gasGapOffset,
                                std::string &gasGapLogvol,
                                std::string &gasGapSupport);
  
  StatusCode makeGasGapObjects(DataSvcHelpers::RegistryEntry* chamPath,
                               std::string &chamberName, 
                               int &statNum, int &regNum, int &ChamNum,
                               int &gasGapNumber, int &gasGapOffset,
                               std::string &gasGapLogvol,
                               std::string &gasGapSupport);
  
  ISvcLocator *m_pSvcLocator;

  IDataProviderSvc *m_pDetDataSvc;
};


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlMuonRegionCnv> muonreg_factory;
const ICnvFactory& XmlMuonRegionCnvFactory = muonreg_factory;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlMuonRegionCnv::XmlMuonRegionCnv(ISvcLocator* svc) :
  XmlUserDetElemCnv<DeMuonRegion> (svc),
  m_pSvcLocator(svc),
  m_pDetDataSvc(0)
{
}

StatusCode XmlMuonRegionCnv::initialize(){
  StatusCode sc = XmlBaseDetElemCnv::initialize();
  if(sc.isSuccess()){
    MsgStream log (msgSvc(), "XmlDetElemCnv");
    log << MSG::VERBOSE << "Initializing" << endreq;
    
    // taken from Algorithm.cpp 
    sc = m_pSvcLocator->getService("DetectorDataSvc", 
                                    IID_IDataProviderSvc, 
                                    (IInterface*&)m_pDetDataSvc);
    if( !sc.isSuccess() ){
      log << MSG::ERROR << "Failed to get DetectorDataSvc" << endreq;
          return StatusCode::FAILURE;
    }
  }
  return sc;
}

// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode XmlMuonRegionCnv::i_fillSpecificObj (DOM_Element childElement,
                                                DeMuonRegion* dataObj,
                                                IOpaqueAddress* address) {
  MsgStream log (msgSvc(), "XmlMuonRegionCnv");
  StatusCode sc;
  
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());

  log << MSG::DEBUG << "Processing element "
      << tagName << endreq;
  

  /*
    <!-- Number of Chambers in region -->
    <!-- logical pad sizes in mm (be explicit) -->
    <!ELEMENT Chambers (GasGap,readoutinfo)>
    <!ATTLIST Chambers 
              Number CDATA #REQUIRED
              logvol CDATA #REQUIRED
              support CDATA #REQUIRED
              padx CDATA #REQUIRED
              pady CDATA #REQUIRED>
    
    <!-- Information on the logical location of the gas gap -->
    <!ELEMENT GasGap EMPTY>
    <!ATTLIST GasGap 
              Number CDATA #REQUIRED
              offset CDATA #REQUIRED
              logvol CDATA #REQUIRED>
  */


  if ("Chambers" == tagName) {
    sc = chamberRead(childElement,dataObj,address);
    if(!sc.isSuccess()){
      log << MSG::WARNING << "Failed to read chamber" << endreq;
      return sc;
    }
  } else {
    // Unknown tag, a warning message could be issued here
    log << MSG::WARNING << "Can not interpret specific type :" 
        << tagName << endreq;
  }
  return StatusCode::SUCCESS;
}


StatusCode XmlMuonRegionCnv::chamberRead (DOM_Element &childElement,
                                          DeMuonRegion* dataObj,
                                          IOpaqueAddress* address ) {
  MsgStream log (msgSvc(), "XmlMuonRegionCnv");
  StatusCode sc;

  // get a value of the 'padx' attribute
  const std::string padx = dom2Std (childElement.getAttribute ("padx"));
  const std::string pady = dom2Std (childElement.getAttribute ("pady"));

  double evalPadx;
  double evalPady;

  if(!padx.empty() && !pady.empty()){
    evalPadx=xmlSvc()->eval(padx.c_str());
    evalPady=xmlSvc()->eval(pady.c_str());
    log << MSG::DEBUG 
        << "Pad size padx="  << evalPadx 
        << "mm pady=" << evalPady << "mm" 
        << endreq;
  } else {
    log << MSG::WARNING << "Failed to find padx and pady" << endreq;
    return StatusCode::FAILURE;
  }

  // look for children: 
  // both GasGap and Readout should be here

  if(!childElement.hasChildNodes()){
    log << MSG::DEBUG << "No readout/gasgap associated to this region" 
        << endreq;
    return StatusCode::FAILURE;
  }

  std::string readout;
  int gasGapNumber=0;
  int gasGapOffset=0;
  std::string gasGapLogvol;
  std::string gasGapSupport;
  sc=chamberChildrenRead(childElement,readout,
                         gasGapNumber,gasGapOffset,
                         gasGapLogvol,gasGapSupport);
  if(!sc.isSuccess()){
    log << MSG::WARNING << "Failed to read chamber children" << endreq;
    return sc;
  }
  
  // get a value of the 'Number' attribute
  const std::string Number = dom2Std (childElement.getAttribute ("Number"));

  if (Number.empty()) {
    log <<MSG::WARNING << "Chamber object did not have number attribute" 
        << endreq;
    return StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << "Number has converted value : " <<
    atol(Number.c_str()) << endreq;
  
  dataObj->setchamberNum (atol(Number.c_str()));
  
  sc=makeChamberObjects(childElement,dataObj,address,
                        evalPadx,evalPady,readout,
                        gasGapNumber,gasGapOffset,
                        gasGapLogvol,gasGapSupport);
  if(!sc.isSuccess()){
    log << MSG::WARNING << "Failed to make chamber objects" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode 
XmlMuonRegionCnv::chamberChildrenRead(DOM_Element &childElement,
                                      std::string &readout,
                                      int &gasGapNumber, int &gasGapOffset,
                                      std::string &gasGapLogvol,
                                      std::string &gasGapSupport){
  
  MsgStream log (msgSvc(), "XmlMuonRegionCnv");
  
  DOM_NodeList nodeChildren = childElement.getChildNodes();
  unsigned int i;
  for(i=0; i < nodeChildren.getLength(); ++i){
        
    // get child and type it

    if ( nodeChildren.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
      log << MSG::WARNING 
          << "Non-element in the chambers tag" << endreq;
      return StatusCode::FAILURE;
    } 

    if(dom2Std(nodeChildren.item(i).getNodeName()) == "readoutinfo"){

      DOM_NamedNodeMap attributes = nodeChildren.item(i).getAttributes();
      DOM_Node readoutNode = attributes.getNamedItem("condition");

      readout = dom2Std(readoutNode.getNodeValue());
      log << MSG::DEBUG << "Readout is "
          << readout << endreq;

    } else if(dom2Std(nodeChildren.item(i).getNodeName()) == "GasGap"){

      DOM_NamedNodeMap attributes = nodeChildren.item(i).getAttributes();

      DOM_Node ggNumberNode = attributes.getNamedItem("Number");
      gasGapNumber = 
        atol(dom2Std(ggNumberNode.getNodeValue()).c_str());

      DOM_Node ggOffsetNode = attributes.getNamedItem("offset");
      gasGapOffset = 
        atol(dom2Std(ggOffsetNode.getNodeValue()).c_str());
        
      DOM_Node ggLogvol = attributes.getNamedItem("logvol");
      gasGapLogvol = dom2Std(ggLogvol.getNodeValue());

      DOM_Node ggSupport = attributes.getNamedItem("support");
      gasGapSupport = dom2Std(ggSupport.getNodeValue());

      log << MSG::DEBUG << "GasGap parameters: "
          << "Number " << gasGapNumber 
          << " offset " << gasGapOffset
          << " logvol " << gasGapLogvol
          << " support " << gasGapSupport 
          << endreq;
    } else {
      log << MSG::WARNING 
          << "Problems in the chambers tag: what is a " 
          << dom2Std(nodeChildren.item(i).getNodeName()) << "?"
          << endreq;
      return StatusCode::FAILURE;
    }

  }
  return StatusCode::SUCCESS;
}

StatusCode 
XmlMuonRegionCnv::makeChamberObjects(DOM_Element &childElement,
                                     DeMuonRegion *dataObj,
                                     IOpaqueAddress* address,
                                     double padx, double pady,
                                     std::string &readout,
                                     int &gasGapNumber, int &gasGapOffset,
                                     std::string &gasGapLogvol,
                                     std::string &gasGapSupport){

  MsgStream log (msgSvc(), "XmlMuonRegionCnv");
  StatusCode sc;

  // Use the TDS tree structure to determine the station and region numbers
  DataSvcHelpers::RegistryEntry* regionEntry = 
    dynamic_cast<DataSvcHelpers::RegistryEntry*> (address->registry()); 
  DataSvcHelpers::RegistryEntry* stationEntry = 
    dynamic_cast<DataSvcHelpers::RegistryEntry*> (regionEntry->parent());
  DataSvcHelpers::RegistryEntry* muonEntry = 
    dynamic_cast<DataSvcHelpers::RegistryEntry*> (stationEntry->parent());

  DataSvcHelpers::RegistryEntry::Iterator itEntry = muonEntry->begin();
  int statNum=1;
  while( (*itEntry) != stationEntry && itEntry != muonEntry->end() ) {
    itEntry++;
    statNum++; 
  }
  if(itEntry == muonEntry->end()){
    log << MSG::ERROR << "Could not find stationEntry from muonEntry"
        << endreq;
    return StatusCode::FAILURE;
  }

  itEntry = stationEntry->begin();
  int regNum=1;
  while( (*itEntry) != regionEntry && itEntry != stationEntry->end() ) {
    itEntry++;
    regNum++; 
  }
  if(itEntry == muonEntry->end()){
    log << MSG::ERROR << "Could not find regionEntry from stationEntry"
        << endreq;
    return StatusCode::FAILURE;
  }

  // Now create chamberNum identical chambers, differing only in position
  int chamNum;
  for(chamNum=1;chamNum<=dataObj->chamberNum();chamNum++){
    // NOTE: this is a "safe" new as this is passed to the transisent store
    DeMuonChamber *cChamber = 
      new DeMuonChamber(statNum,regNum,chamNum,padx,pady,gasGapNumber);

    log << MSG::DEBUG << "Set padx=" << cChamber->padx()
        << " and pady=" << cChamber->pady() << endreq;
    
    // add the readout to the chamber
    cChamber->createReadOut(readout);
    
    // the position of this chamber is determined by a logical volume
    // in logvol, a support in support and is number (rpath) i
    // with the path npath="pvM5R4Cham190"
    // from the logical volume associated with 
    // support="/dd/Structure/LHCb/Muon/M5/R4"
    std::string logVolName = dom2Std (childElement.getAttribute ("logvol"));
    std::string support = dom2Std (childElement.getAttribute ("support"));
    ILVolume::ReplicaPath repPath;
    repPath.push_back(static_cast<unsigned int>(chamNum-1));
	
    log << MSG::DEBUG << "GI Logical volume : " << logVolName  << endreq;
    log << MSG::DEBUG << "GI Support        : " << support     << endreq;
    log << MSG::DEBUG << "GI rpath  : " << (chamNum-1) << endreq;
    // add the geometry information
    cChamber->createGeometryInfo (logVolName,support,repPath);
	  	
    // Name this chamber
    char Cham000[8];
    sprintf(Cham000,"Cham%03i",chamNum);
    
    std::string chamberName = Cham000;

    // And register it to current data object directory
    log << MSG::DEBUG << "Registering chamber " << chamberName 
        <<	  endreq;

    // pass to tranisent store
    sc = regionEntry->add(chamberName,cChamber);    
    if(StatusCode::SUCCESS != sc) {
      log << MSG::WARNING << "The store rejected chamber "
          << chamberName << endreq;
      return StatusCode::FAILURE;
    }

    //get path of chamber
    DataSvcHelpers::RegistryEntry* chamPath = 
      dynamic_cast<DataSvcHelpers::RegistryEntry*> (cChamber->registry()); 

    sc= makeGasGapObjects(chamPath,chamberName,
                          statNum,regNum,chamNum,
                          gasGapNumber,gasGapOffset,
                          gasGapLogvol,gasGapSupport);
    if(!sc.isSuccess()){
      log << MSG::WARNING << "Failed to make gasgap objects" << endreq;
      return sc;
    }    
  }
  return StatusCode::SUCCESS;
}

StatusCode 
XmlMuonRegionCnv::makeGasGapObjects(DataSvcHelpers::RegistryEntry* chamPath,
                                    std::string &chamberName,
                                    int &statNum, int &regNum,
                                    int &chamNum, 
                                    int &gasGapNumber, int &gasGapOffset,
                                    std::string &gasGapLogvol,
                                    std::string &gasGapSupport){

  MsgStream log (msgSvc(), "XmlMuonRegionCnv");
  StatusCode sc;
  // Now make the gas gaps to go with the chambers
  
  int gapNum;
  for(gapNum=1; gapNum <= gasGapNumber; gapNum++){
    DeMuonGasGap *cGap = new DeMuonGasGap(statNum,regNum,chamNum,gapNum);
    // now this gets very ugly.
    // for MWPC M1R1 chamber 1 the gas gaps are in
    // logvol=/dd/Geometry/Muon/M1R1Cham/lvGasGapLayer5
    // support=/dd/Structure/LHCb/Muon/M1/R1/Cham001 
    // rpath=0/4, 1/4, 2/4 and 3/4

    std::string gapSupport = gasGapSupport + '/' + chamberName;

    ILVolume::ReplicaPath ggRepPath;
    ggRepPath.push_back(static_cast<unsigned int>(gapNum-1));
    ggRepPath.push_back(static_cast<unsigned int>(gasGapOffset-1));

    log << MSG::DEBUG 
        << "GasGap Logvol " <<  gasGapLogvol
        << " support " <<  gapSupport
        << " repPath " <<  gapNum-1 << "/" << gasGapOffset-1
        << endreq;

    // add the geometry information
    cGap->createGeometryInfo (gasGapLogvol,gapSupport,
                              ggRepPath);
    char Gap0[5];
    sprintf(Gap0,"Gap%1i",gapNum);
    std::string gapName = Gap0;

    // And register it to current data object directory
    log << MSG::DEBUG << "Registering gap " 
        << gapName 
        <<	  endreq;
    
    // pass to tranisent store
    sc = chamPath->add(gapName,cGap);
    if(!sc.isSuccess()) {
      log << MSG::WARNING << "The store rejected gap "
          << gapName << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}
