// $Id: XmlMuonRegionCnv.cpp,v 1.9 2002-09-27 13:59:41 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/04/25 10:16:31  dhcroft
// Fix to reading Cathode channels
//
// Revision 1.7  2002/04/22 07:42:15  dhcroft
// Fixed a bug in setting the number of Anode and Cathoed FE channels per chamber
//
// Revision 1.6  2002/04/08 14:55:00  dhcroft
// Changed code to calculate Gaps per FE channel rather than read from XML
//
// Revision 1.5  2002/03/20 16:43:40  dhcroft
// Added the size of the Front end channels to the XML and the mapping from FE to logical channels
//
// Revision 1.4  2002/02/21 16:38:44  dhcroft
// Added methods to retrieve the number of the station, region, chamber and gas gap to DeMuonChamber and
// DeMuonGasGap objects. Modified XmlMuonRegionCnv to fill these parameters when making the objects.
//
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
              support CDATA #REQUIRED>
    
    <!-- Information on the logical location of the gas gap -->
    <!ELEMENT GasGap EMPTY>
    <!ATTLIST GasGap 
              Number CDATA #REQUIRED
              offset CDATA #REQUIRED
              logvol CDATA #REQUIRED>
    <!-- Information on the mapping of FE channels to logical channels -->
    <!-- NFEChamber(X/Y) are the number of channels in x and y
         across a chamber -->
    <!-- MergeLogical are the information on which FE channels to merge 
         into either one or two logical strip (or pads if not crosses) 
         sets (-1 is no mapping)-->
    <!ELEMENT ReadoutMap EMPTY>
    <!ATTLIST ReadoutMap 
              ReadoutType ( Anode | Cathode ) #REQUIRED
              NFEChamberX  CDATA #REQUIRED
              NFEChamberY  CDATA #REQUIRED
              MergeLogicalX1 CDATA #REQUIRED
              MergeLogicalY1 CDATA #REQUIRED
              MergeLogicalX2 CDATA "-1"
              MergeLogicalY2 CDATA "-1">                  
  */


  if ("Chambers" == tagName) {
    sc = chamberRead(childElement,dataObj,address);
    if(!sc.isSuccess()){
      log << MSG::WARNING << "Failed to read chamber" << endreq;
      return sc;
    }    
  } else if("ReadoutMap" == tagName){

    const std::string ReadoutType = 
      dom2Std (childElement.getAttribute ("ReadoutType"));
    const std::string NFEChamberX = 
      dom2Std (childElement.getAttribute ("NFEChamberX"));
    const std::string NFEChamberY = 
      dom2Std (childElement.getAttribute ("NFEChamberY"));
    const std::string MergeLogicalX1 = 
      dom2Std (childElement.getAttribute ("MergeLogicalX1"));
    const std::string MergeLogicalY1 = 
      dom2Std (childElement.getAttribute ("MergeLogicalY1"));
    const std::string MergeLogicalX2 = 
      dom2Std (childElement.getAttribute ("MergeLogicalX2"));
    const std::string MergeLogicalY2 = 
      dom2Std (childElement.getAttribute ("MergeLogicalY2"));

    if(!("Anode" == ReadoutType || "Cathode" == ReadoutType)){
      log << MSG::WARNING << "Was given a readout type :" 
          << ReadoutType << endreq;
      return StatusCode::FAILURE;
    }
    if("Anode" == ReadoutType){
      dataObj->setFEAnodeX( atol(NFEChamberX.c_str()) );
      dataObj->setFEAnodeY( atol(NFEChamberY.c_str()) );
      dataObj->addLogicalMap(MuonParameters::Anode, 
                             atol(MergeLogicalX1.c_str()),
                             atol(MergeLogicalY1.c_str()));
      if( 0 < atol(MergeLogicalX2.c_str()) ){
        dataObj->addLogicalMap(MuonParameters::Anode, 
                               atol(MergeLogicalX2.c_str()),
                               atol(MergeLogicalY2.c_str()));
      }
    }else{
      dataObj->setFECathodeX( atol(NFEChamberX.c_str()) );
      dataObj->setFECathodeY( atol(NFEChamberY.c_str()) );
      dataObj->addLogicalMap(MuonParameters::Cathode, 
                             atol(MergeLogicalX1.c_str()),
                             atol(MergeLogicalY1.c_str()));
      if( 0 < atol(MergeLogicalX2.c_str()) ){
        dataObj->addLogicalMap(MuonParameters::Cathode, 
                               atol(MergeLogicalX2.c_str()),
                               atol(MergeLogicalY2.c_str()));
      }
    }      

  }else {
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

  // for now with MWPCs and RPCs this is a good formula
  int numberGapsPerFE = gasGapNumber / 2 ; 
    
  dataObj->setGapsPerFE(numberGapsPerFE);

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
  
  sc=makeChamberObjects(childElement,dataObj,address,readout,
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
      new DeMuonChamber(statNum,regNum,chamNum,gasGapNumber);
    
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
