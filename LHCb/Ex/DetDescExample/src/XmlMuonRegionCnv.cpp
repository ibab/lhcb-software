// $Id: XmlMuonRegionCnv.cpp,v 1.3 2003-01-17 14:16:22 sponce Exp $

// Include files

#include "DetDesc/XmlUserDetElemCnv.h"
#include "DeMuonRegion.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

#include <dom/DOM_NamedNodeMap.hpp>

/** @class XmlMuonRegionCnv
 *
 * simplified XML converter for MuonRegion
 * See whole implementation by David in package Det/MuonDet
 *
 * @author David Hutchcroft
 * @author Sebastien Ponce
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
  XmlUserDetElemCnv<DeMuonRegion> (svc) {}


// -----------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------------
StatusCode XmlMuonRegionCnv::initialize(){
  StatusCode sc = XmlBaseDetElemCnv::initialize();
  if(sc.isSuccess()){
    MsgStream log (msgSvc(), "XmlDetElemCnv");
    log << MSG::VERBOSE << "Initializing" << endmsg;    
  }
  return sc;
}

// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode XmlMuonRegionCnv::i_fillSpecificObj (DOM_Element childElement,
                                                DeMuonRegion* dataObj,
                                                IOpaqueAddress* /*address*/) {
  MsgStream log (msgSvc(), "XmlMuonRegionCnv");
  
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());

  log << MSG::DEBUG << "Processing element " << tagName << endmsg;
  
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

    // checks there are children
    if(!childElement.hasChildNodes()){
      log << MSG::DEBUG << "No readout/gasgap associated to this region" 
          << endmsg;
      return StatusCode::FAILURE;
    }

    // look for children of type GasGap and retrieve the number of gasGap
    int gasGapNumber=0;
    DOM_NodeList nodeChildren = childElement.getChildNodes();
    unsigned int i;
    for (i=0; i < nodeChildren.getLength(); ++i) {
      // get child and type it
      if (nodeChildren.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
        log << MSG::WARNING 
            << "Non-element in the chambers tag" << endmsg;
        return StatusCode::FAILURE;
      }

      if(dom2Std(nodeChildren.item(i).getNodeName()) == "GasGap") {
        
        DOM_NamedNodeMap attributes = nodeChildren.item(i).getAttributes();
        DOM_Node ggNumberNode = attributes.getNamedItem("Number");
        const std::string gasGapNumberStr =
          dom2Std(ggNumberNode.getNodeValue());
        gasGapNumber = (int) xmlSvc()->eval(gasGapNumberStr.c_str(), false);
        
      }
    }
    
    // for now with MWPCs and RPCs this is a good formula
    dataObj->setGapsPerFE(gasGapNumber / 2);

    // get a value of the 'Number' attribute
    const std::string Number = dom2Std (childElement.getAttribute ("Number"));
    if (Number.empty()) {
      log <<MSG::WARNING << "Chamber object did not have number attribute" 
          << endmsg;
      return StatusCode::FAILURE;
    }
    dataObj->setchamberNum ((int) xmlSvc()->eval(Number.c_str(), false));

  } else if("ReadoutMap" == tagName){

    const std::string ReadoutType = 
      dom2Std (childElement.getAttribute ("ReadoutType"));
    const std::string NFEChamberX = 
      dom2Std (childElement.getAttribute ("NFEChamberX"));
    const std::string NFEChamberY = 
      dom2Std (childElement.getAttribute ("NFEChamberY"));

    if(!("Anode" == ReadoutType || "Cathode" == ReadoutType)){
      log << MSG::WARNING << "Was given a readout type :" 
          << ReadoutType << endmsg;
      return StatusCode::FAILURE;
    }
    if("Anode" == ReadoutType){
      dataObj->setFEAnodeX ((int) xmlSvc()->eval(NFEChamberX.c_str(), false));
      dataObj->setFEAnodeY ((int) xmlSvc()->eval(NFEChamberY.c_str(), false));
    }else{
      dataObj->setFECathodeX ((int) xmlSvc()->eval(NFEChamberX.c_str(), false));
      dataObj->setFECathodeY ((int) xmlSvc()->eval(NFEChamberY.c_str(), false));
    }      

  }else {
    // Unknown tag, a warning message could be issued here
    log << MSG::WARNING << "Can not interpret specific type :" 
        << tagName << endmsg;
  }
  return StatusCode::SUCCESS;
}
