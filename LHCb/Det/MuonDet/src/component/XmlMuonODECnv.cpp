// $Id: XmlMuonODECnv.cpp,v 1.4 2004-04-23 12:08:00 cattanem Exp $
// Include files 

#include <vector>

#include "DetDescCnv/XmlUserConditionCnv.h"
#include "MuonDet/MuonReadoutCond.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "GaudiKernel/RegistryEntry.h"

#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>   
#include <xercesc/dom/DOMNode.hpp>   

#include "GaudiKernel/IOpaqueAddress.h"

// local

/** @class XmlMuonODECnv
 *
 * XML converter for MuonODE
 *
 * @author Alessia Satta
 */
class XmlMuonODECnv : public XmlUserConditionCnv<MuonODEBoard> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonODECnv (ISvcLocator* svc);

  /**
   * Default destructor
   */
  ~XmlMuonODECnv();

protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param dataObj the object to be filled
   * @param address ??
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        MuonODEBoard* dataObj,
                                        IOpaqueAddress* address);

private:
StatusCode splitList( std::string &stringList,
                     std::vector<long>& vectorList);
  

private:
 const XMLCh* ODEString;
 const XMLCh* ODENumberString;
 const XMLCh* RegionString;
 const XMLCh* TSLayoutXString;
 const XMLCh* TSLayoutYString;
 const XMLCh* TSNumberString;
 const XMLCh* TSGridXListString;
 const XMLCh* TSGridYListString;
 const XMLCh* TSQuadrantListString;
 const XMLCh* TSMapString;
 const XMLCh* TSMapRefString;            
}; 

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlMuonODECnv> muonode_factory;
const ICnvFactory& XmlMuonODECnvFactory = muonode_factory;


//-----------------------------------------------------------------------------
// Implementation file for class : XmlMuonMuonODECnv
//
// 2004-01-12 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XmlMuonODECnv::XmlMuonODECnv(ISvcLocator* svc):
  XmlUserConditionCnv<MuonODEBoard> (svc)
{

ODEString=xercesc::XMLString::transcode("ODEBoard");             
ODENumberString=xercesc::XMLString::transcode("ODESerialNumber");       
RegionString=xercesc::XMLString::transcode("RegionNumber"); 
TSLayoutXString=xercesc::XMLString::transcode("TSLayoutX");       
TSLayoutYString=xercesc::XMLString::transcode("TSLayoutY");       
TSNumberString=xercesc::XMLString::transcode("TSNumber");        
TSGridXListString=xercesc::XMLString::transcode("TSGridXList");     
TSGridYListString=xercesc::XMLString::transcode("TSGridYList");     
TSQuadrantListString=xercesc::XMLString::transcode("TSQuadrantList");  
TSMapString=xercesc::XMLString::transcode("conditionref");           
TSMapRefString=xercesc::XMLString::transcode("href");
}

//=============================================================================
// Destructor
//=============================================================================
XmlMuonODECnv::~XmlMuonODECnv() {
  xercesc::XMLString::release((XMLCh**)&ODEString);           
  xercesc::XMLString::release((XMLCh**)&RegionString); 
  xercesc::XMLString::release((XMLCh**)&ODENumberString);     
  xercesc::XMLString::release((XMLCh**)&TSLayoutXString);     
  xercesc::XMLString::release((XMLCh**)&TSLayoutYString);     
  xercesc::XMLString::release((XMLCh**)&TSNumberString);      
  xercesc::XMLString::release((XMLCh**)&TSGridXListString);   
  xercesc::XMLString::release((XMLCh**)&TSGridYListString);   
  xercesc::XMLString::release((XMLCh**)&TSQuadrantListString);
  xercesc::XMLString::release((XMLCh**)&TSMapString);         
  xercesc::XMLString::release((XMLCh**)&TSMapRefString);
}; 


StatusCode 
XmlMuonODECnv::i_fillSpecificObj(xercesc::DOMElement* childElement,
                                          MuonODEBoard* dataObj,
                                          IOpaqueAddress*          ){
  MsgStream msg(msgSvc(), "XmlMuonODECnv");
  const XMLCh* tagName = childElement->getNodeName();

  if (0 == xercesc::XMLString::compareString(ODEString, tagName)) {
    const std::string odeNumberString =
      dom2Std (childElement->getAttribute (ODENumberString));
    long ODENumberValue=atol(odeNumberString.c_str());
    const std::string regionNumberString =
      dom2Std (childElement->getAttribute (RegionString));
    long regionNumberValue=atol(regionNumberString.c_str());
    dataObj->initialize(ODENumberValue,regionNumberValue);    

    const std::string tsLayoutXString =
      dom2Std (childElement->getAttribute (TSLayoutXString));
    long tsLayoutXValue=atol(tsLayoutXString.c_str());
    const std::string tsLayoutYString =
      dom2Std (childElement->getAttribute (TSLayoutYString));
    long tsLayoutYValue=atol(tsLayoutYString.c_str());
    const std::string tsNumberString =
      dom2Std (childElement->getAttribute (TSNumberString));
    long tsNumberValue=atol(tsNumberString.c_str());
    std::string tsGridXListString =
      dom2Std (childElement->getAttribute (TSGridXListString));
    std::vector<long> tsGridXValue;
    splitList(tsGridXListString,tsGridXValue);
    std::string tsGridYListString =
      dom2Std (childElement->getAttribute (TSGridYListString));
    std::vector<long> tsGridYValue;
    splitList(tsGridYListString,tsGridYValue);
    std::string tsQuadrantListString =
      dom2Std (childElement->getAttribute (TSQuadrantListString));
    std::vector<long> tsQuadrantValue;
    splitList(tsQuadrantListString,tsQuadrantValue);

    dataObj->update(tsLayoutXValue,tsLayoutYValue,tsNumberValue, 
                    tsGridXValue, tsGridYValue,
                    tsQuadrantValue);
	dataObj->setQuadrants();
    
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
    for(i=0; i < nodeChildren->getLength(); ++i){
      if(dom2Std(nodeChildren->item(i)->getNodeName()) == 
         dom2Std(TSMapString)){
        xercesc::DOMNamedNodeMap* attributes =      
          nodeChildren->item(i)->getAttributes();
        xercesc::DOMNode* TSNode = attributes->
          getNamedItem(TSMapRefString);
        std::string  TSReference = dom2Std (TSNode->getNodeValue());  
        unsigned int poundPosition = TSReference.find_last_of('#');
        std::string entryName = "/" + TSReference.substr(poundPosition + 1);
        dataObj->addTSName(TSReference.substr(poundPosition + 1));    
        
      }      
    }    
  }
  return StatusCode::SUCCESS;  
}

StatusCode 
XmlMuonODECnv::splitList( std::string &stringList,
                             std::vector<long>& vectorList){
  
  std::string::size_type cPos = stringList.find(',');
  std::string sCurr;
  while( cPos != std::string::npos ){ // found a comma in string
    sCurr = stringList.substr( 0, cPos); // get up to first comma
    vectorList.push_back(atol(sCurr.c_str())); // add to vector
    stringList.erase( 0, cPos+1); // erase up to and including comma
    cPos = stringList.find(','); // find next comma
  }
  vectorList.push_back(atol(stringList.c_str())); 
return StatusCode::SUCCESS;
}

//=============================================================================
