// $Id: XmlMuonTSMapCnv.cpp,v 1.8 2008-07-12 06:15:40 asatta Exp $
// Include files

#include <vector>

#include "DetDescCnv/XmlUserConditionCnv.h"
//#include "MuonDet/MuonReadoutCond.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "GaudiKernel/RegistryEntry.h"

#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include "GaudiKernel/IOpaqueAddress.h"

// local

/** @class XmlMuonTSMapCnv
 *
 * XML converter for MuonODE
 *
 * @author Alessia Satta
 */
class XmlMuonTSMapCnv : public XmlUserConditionCnv<MuonTSMap> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonTSMapCnv (ISvcLocator* svc);

  /**
   * Default destructor
   */
  ~XmlMuonTSMapCnv();

protected:

  using XmlUserConditionCnv<MuonTSMap>::i_fillSpecificObj;
  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param dataObj the object to be filled
   * @param address ??
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        MuonTSMap* dataObj,
                                        IOpaqueAddress* address);

private:
StatusCode splitList( std::string &stringList,
                     std::vector<long>& vectorList);
private:
  const XMLCh* TSString;
  const XMLCh* LogLayoutNumberString;
  const XMLCh* GridXString;
  const XMLCh* GridYString;
  const XMLCh* OutputSignalString;
  const XMLCh* OutputLayoutSeqString;
  const XMLCh* OutputGridXSeqString;
  const XMLCh* OutputGridYSeqString;
  const XMLCh* OutputSynchSeqString;
};

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlMuonTSMapCnv)

//-----------------------------------------------------------------------------
// Implementation file for class : XmlMuonMuonTSMapCnv
//
// 2004-01-12 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XmlMuonTSMapCnv::XmlMuonTSMapCnv(ISvcLocator* svc):
  XmlUserConditionCnv<MuonTSMap> (svc)
{
      MsgStream msg(msgSvc(), "XmlMuonTSMapCnv");

TSString=xercesc::XMLString::transcode("TSMap");
LogLayoutNumberString=xercesc::XMLString::transcode("NumberLogLayout");
GridXString=xercesc::XMLString::transcode("GridXLayout");
GridYString=xercesc::XMLString::transcode("GridYLayout");
OutputSignalString=xercesc::XMLString::transcode("OutputSignal");
OutputLayoutSeqString=xercesc::XMLString::transcode("OutputLayoutSeq");
OutputGridXSeqString=xercesc::XMLString::transcode("OutputGridXSeq");
OutputGridYSeqString=xercesc::XMLString::transcode("OutputGridYSeq");
OutputSynchSeqString=xercesc::XMLString::transcode("OutputSynchCh");
}

//=============================================================================
// Destructor
//=============================================================================
XmlMuonTSMapCnv::~XmlMuonTSMapCnv() {
  xercesc::XMLString::release((XMLCh**)&TSString);
  xercesc::XMLString::release((XMLCh**)&LogLayoutNumberString);
  xercesc::XMLString::release((XMLCh**)&GridXString);
  xercesc::XMLString::release((XMLCh**)&GridYString);
  xercesc::XMLString::release((XMLCh**)&OutputSignalString);
  xercesc::XMLString::release((XMLCh**)&OutputLayoutSeqString);
  xercesc::XMLString::release((XMLCh**)&OutputGridXSeqString);
  xercesc::XMLString::release((XMLCh**)&OutputGridYSeqString);
  xercesc::XMLString::release((XMLCh**)&OutputSynchSeqString);
}


StatusCode
XmlMuonTSMapCnv::i_fillSpecificObj(xercesc::DOMElement* childElement,
                                          MuonTSMap* dataObj,
                                          IOpaqueAddress*         ){
  MsgStream msg(msgSvc(), "XmlMuonTSMapCnv");
  const XMLCh* tagName = childElement->getNodeName();

  if (0 == xercesc::XMLString::compareString(TSString, tagName)) {
    const std::string layoutNumberString =
      dom2Std (childElement->getAttribute (LogLayoutNumberString));
    long LogLayoutNumberValue=atol(layoutNumberString.c_str());

    std::string gridX =
      dom2Std (childElement->getAttribute (GridXString));
    std::vector<long> gridXValue;
     StatusCode sc=splitList(gridX,gridXValue);
     if(sc.isFailure())return sc;
     std::string gridY =
      dom2Std (childElement->getAttribute (GridYString));
    std::vector<long> gridYValue;
     sc=splitList(gridY,gridYValue);
     if(sc.isFailure())return sc;
     long tsgx[2];
     long tsgy[2];
     for(int i=0; i<LogLayoutNumberValue; i++){
       tsgx[i]=gridXValue[i];
       tsgy[i]=gridYValue[i];
     }
     sc=dataObj->initialize(LogLayoutNumberValue,tsgx,tsgy);
     if(sc.isFailure())return sc;
     std::string outputSignalString =
       dom2Std (childElement->getAttribute (OutputSignalString));
     long outputSignalValue=atol(outputSignalString.c_str());
     std::string outputLayoutSeqString =
       dom2Std (childElement->getAttribute (OutputLayoutSeqString));
     std::vector<long> outputLayoutSeq;
     sc=splitList(outputLayoutSeqString,outputLayoutSeq);
     if(sc.isFailure())return sc;
     std::string outputGridXSeqString =
       dom2Std (childElement->getAttribute (OutputGridXSeqString));
     std::vector<long> outputGridXSeq;
     sc=splitList(outputGridXSeqString,outputGridXSeq);
     if(sc.isFailure())return sc;
     std::string outputGridYSeqString =
       dom2Std (childElement->getAttribute (OutputGridYSeqString));
     std::vector<long> outputGridYSeq;
     sc=splitList(outputGridYSeqString,outputGridYSeq);
     std::string outputSynchSeqString =
       dom2Std (childElement->getAttribute (OutputSynchSeqString));
     std::vector<long> outputSynchSeq;
     sc=splitList(outputSynchSeqString,outputSynchSeq);
     sc=dataObj->update(outputSignalValue,outputLayoutSeq,
			outputGridXSeq,outputGridYSeq,outputSynchSeq);
     if(sc.isFailure())return sc;

  }
  return StatusCode::SUCCESS;
}


StatusCode
XmlMuonTSMapCnv::splitList( std::string &stringList,
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









