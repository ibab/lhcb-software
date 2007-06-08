// $Id: XmlMuonL1BoardCnv.cpp,v 1.2 2007-06-08 15:34:00 asatta Exp $
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

/** @class XmlMuonL1Cnv
 *
 * XML converter for MuonODE
 *
 * @author Alessia Satta
 */
class XmlMuonL1BoardCnv : public XmlUserConditionCnv<MuonL1Board> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonL1BoardCnv (ISvcLocator* svc);

  /**
   * Default destructor
   */
  ~XmlMuonL1BoardCnv();

protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param dataObj the object to be filled
   * @param address ??
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        MuonL1Board* dataObj,
                                        IOpaqueAddress* address);

private:
StatusCode splitList( std::string &stringList,
                     std::vector<long>& vectorList);
  

private:  
  const XMLCh* L1String;
  const XMLCh* L1NameString;
  const XMLCh* L1NumberString;
  const XMLCh* L1StationString;
  const XMLCh* ODENumberString;
  const XMLCh* ODEListString;
  const XMLCh* ODEReferenceString;
  const XMLCh* ODEConditionString;
  const XMLCh* TSLayoutString;
  const XMLCh* TSLayoutRegionString;
  const XMLCh* TSLayoutXString;
  const XMLCh* TSLayoutYString;


}; 

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlMuonL1BoardCnv)


//-----------------------------------------------------------------------------
// Implementation file for class : XmlMuonMuonL1Cnv
//
// 2004-01-12 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XmlMuonL1BoardCnv::XmlMuonL1BoardCnv(ISvcLocator* svc):
  XmlUserConditionCnv<MuonL1Board> (svc)
{
    L1String= xercesc::XMLString::transcode("L1Board");
    L1NameString= xercesc::XMLString::transcode("L1Name");
    L1NumberString= xercesc::XMLString::transcode("L1SerialNumber");
    L1StationString= xercesc::XMLString::transcode("L1Station");
    ODENumberString= xercesc::XMLString::transcode("ODENumber");
    ODEListString= xercesc::XMLString::transcode("ODEList");
    ODEReferenceString= xercesc::XMLString::transcode("conditionref");
    ODEConditionString= xercesc::XMLString::transcode("href");
    TSLayoutString= xercesc::XMLString::transcode("TSLayout");
    TSLayoutRegionString= xercesc::XMLString::transcode("Region");    
    TSLayoutXString= xercesc::XMLString::transcode("TSLayoutX");    
    TSLayoutYString= xercesc::XMLString::transcode("TSLayoutY");

}

//=============================================================================
// Destructor
//=============================================================================
XmlMuonL1BoardCnv::~XmlMuonL1BoardCnv() {
  xercesc::XMLString::release((XMLCh**)&L1String);
  xercesc::XMLString::release((XMLCh**)&L1NameString);
  xercesc::XMLString::release((XMLCh**)&L1NumberString);
  xercesc::XMLString::release((XMLCh**)&L1StationString);
  xercesc::XMLString::release((XMLCh**)&ODENumberString);
  xercesc::XMLString::release((XMLCh**)&ODEListString);
  xercesc::XMLString::release((XMLCh**)&ODEReferenceString);
  xercesc::XMLString::release((XMLCh**)&ODEConditionString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutRegionString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutXString);
  xercesc::XMLString::release((XMLCh**)&TSLayoutYString);  

}; 


StatusCode 
XmlMuonL1BoardCnv::i_fillSpecificObj(xercesc::DOMElement* childElement,
                                MuonL1Board* dataObj,
                                IOpaqueAddress*          ){
  MsgStream msg(msgSvc(), "XmlMuonL1Cnv");
  const XMLCh* tagName = childElement->getNodeName();
  
  
  
  if (0 == xercesc::XMLString::compareString(L1String, tagName)) {
    const std::string  L1Name =
      dom2Std(childElement->getAttributes()->getNamedItem(L1NameString)
              ->getNodeValue());
    const std::string  L1Number = dom2Std(childElement->
                                          getAttributes()->
                                          getNamedItem(L1NumberString)
                                          ->getNodeValue());
    long L1NumberValue=atol(L1Number.c_str());
    const std::string  L1Station = dom2Std(childElement->
                                           getAttributes()->
                                           getNamedItem(L1StationString)
                                           ->getNodeValue());
    long L1StationValue=atol(L1Station.c_str());
    
    const std::string  ODENumber = dom2Std(childElement->
                                           getAttributes()->
                                           getNamedItem(ODENumberString)
                                           ->getNodeValue());
    long ODENumberValue=atol(ODENumber.c_str());

    std::string  ODEList = dom2Std(childElement->
                                   getAttributes()->
                                   getNamedItem(ODEListString)
                                   ->getNodeValue());
    std::vector<long> ODEListValue;
    StatusCode sc=splitList(ODEList,ODEListValue);
    if(sc.isFailure())return sc;
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
    unsigned int iODE=0;
    
    for(i=0; i < nodeChildren->getLength(); ++i){   
      msg << MSG::VERBOSE << "Processing child "<<
        dom2Std(nodeChildren->item(i)->getNodeName())<<endreq;
      if(dom2Std(nodeChildren->item(i)->getNodeName()) == 
         dom2Std(ODEReferenceString)){
        
        msg << MSG::VERBOSE << "Processing element "
            << dom2Std(ODEReferenceString) <<" "<<iODE<< endreq;
        xercesc::DOMNamedNodeMap* attributes =      
          nodeChildren->item(i)->getAttributes();
        xercesc::DOMNode* odeNode = attributes->
          getNamedItem(ODEConditionString);
        std::string  OdeReference = dom2Std (odeNode->getNodeValue());  
        unsigned int poundPosition = OdeReference.find_last_of('#');
        // std::string entryName = "/" + OdeReference.substr(poundPosition + 1);
        //      std::string location = address->par()[0];
        //      std::string location1 = address->par()[1];
        //      IOpaqueAddress* xmlAddr = 0;      
        //      xmlAddr = createAddressForHref (OdeReference, 5, address);
        //      msg << MSG::VERBOSE << " location : " << xmlAddr->par()[0]
        //          << " entryName : " << xmlAddr->par()[1]
      //          << " isString : " << xmlAddr->ipar()[0] << endreq;      
      //      msg << MSG::DEBUG << "Processing element "
      //   << OdeReference << " "<<entryName<< " "<<location 
      //   <<" "<<location1<<" "<<dataObj->name()<<endreq;
      dataObj->addODE(ODEListValue[iODE],
                           OdeReference.substr(poundPosition + 1));
      msg << MSG::VERBOSE <<ODEListValue[iODE]<<" "<<
        OdeReference.substr(poundPosition + 1)<<endreq;
      iODE++;
      
    }
    
    else if(dom2Std(nodeChildren->item(i)->getNodeName()) == 
            dom2Std(TSLayoutString)){
      msg << MSG::VERBOSE << "Processing element "<<
        dom2Std(TSLayoutString)<<endreq;
      
      xercesc::DOMNamedNodeMap* attributes =      
        nodeChildren->item(i)->getAttributes();
      xercesc::DOMNode* TSRegionNode = attributes->
        getNamedItem(TSLayoutRegionString);
      long  TSRegionValue = atol(dom2Std(TSRegionNode
                                         ->getNodeValue()).c_str());  

      xercesc::DOMNode* TSLayoutXNode = attributes->
        getNamedItem(TSLayoutXString);
      long TSXValue = atol(dom2Std (TSLayoutXNode
                                    ->getNodeValue()).c_str());

   
      xercesc::DOMNode* TSLayoutYNode = attributes->
        getNamedItem(TSLayoutYString);
      long TSYValue = atol(dom2Std (TSLayoutYNode
                                     ->getNodeValue()).c_str()); ;
	msg<<MSG::VERBOSE<<"layout "<<TSXValue<<" "<<TSYValue<<
        " "<<TSRegionValue<<endreq;	
      dataObj->addLayout(TSRegionValue,TSXValue,TSYValue);
      
    }
      
    }
    
    
    
    dataObj->setL1Number(L1NumberValue);
    dataObj->setL1Station(L1StationValue);
    dataObj->setNumberOfODE(ODENumberValue);
  }
  

  
  return StatusCode::SUCCESS;  
}

StatusCode 
XmlMuonL1BoardCnv::splitList( std::string &stringList,
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
