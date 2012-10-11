// Include files
#include <vector>

#include "DetDescCnv/XmlUserConditionCnv.h"
#include "MuonDet/MuonReadoutCond.h"

#include "GaudiKernel/RegistryEntry.h"
#include <xercesc/dom/DOMNamedNodeMap.hpp>

/** @class XmlMuonReadoutCondCnv
 *
 * XML converter for MuonReadoutCond
 *
 * @author David Hutchcroft
 */
class XmlMuonReadoutCondCnv : public XmlUserConditionCnv<MuonReadoutCond> {

public:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlMuonReadoutCondCnv (ISvcLocator* svc);

  /**
   * Default destructor
   */
  ~XmlMuonReadoutCondCnv();

protected:

  using XmlUserConditionCnv<MuonReadoutCond>::i_fillSpecificObj;
  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param dataObj the object to be filled
   * @param address ??
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        MuonReadoutCond* dataObj,
                                        IOpaqueAddress* address);

private:
  /// fills the X and Y cluster sizes and probabilities
  StatusCode setClusterSizes(MuonReadoutCond* dataObj, const int &index,
                            std::string &clSzX,
                            std::string &clPrX,
                            std::string &clSzY,
                            std::string &clPrY);

  /// filles the time jitter PDF
  StatusCode setJitterVector(MuonReadoutCond* dataObj,
                             const int &index,
                             const std::string &jitterMin,
                             const std::string &jitterInc,
                             std::string &jitterValues);

private:

  // Constant strings for element and parameter names
  const XMLCh* ReadoutString;
  const XMLCh* ReadoutTypeString;
  const XMLCh* EfficiencyString;
  const XMLCh* SyncDriftString;
  const XMLCh* ChamberNoiseString;
  const XMLCh* ElectronicsNoiseString;
  const XMLCh* MeanDeadTimeString;
  const XMLCh* RMSDeadTimeString;
  const XMLCh* TimeGateStartString;
  const XMLCh* PadEdgeSizeXString;
  const XMLCh* PadEdgeSigmaXString;
  const XMLCh* ClusterSizeXString;
  const XMLCh* ClusterProbXString;
  const XMLCh* PadEdgeSizeYString;
  const XMLCh* PadEdgeSigmaYString;
  const XMLCh* ClusterSizeYString;
  const XMLCh* ClusterProbYString;
  const XMLCh* JitterMinString;
  const XMLCh* JitterMaxString;
  const XMLCh* JitterValuesString;

};


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlMuonReadoutCondCnv)


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlMuonReadoutCondCnv::XmlMuonReadoutCondCnv(ISvcLocator* svc) :
  XmlUserConditionCnv<MuonReadoutCond> (svc)
{
MsgStream log (msgSvc(), "XmlMuonReadoutCondCnv");

  ReadoutString = xercesc::XMLString::transcode("Readout");
  ReadoutTypeString = xercesc::XMLString::transcode("ReadoutType");
  EfficiencyString = xercesc::XMLString::transcode("Efficiency");
  SyncDriftString = xercesc::XMLString::transcode("SyncDrift");
  ChamberNoiseString = xercesc::XMLString::transcode("ChamberNoise");
  ElectronicsNoiseString = xercesc::XMLString::transcode("ElectronicsNoise");
  MeanDeadTimeString = xercesc::XMLString::transcode("MeanDeadTime");
  RMSDeadTimeString = xercesc::XMLString::transcode("RMSDeadTime");
  TimeGateStartString = xercesc::XMLString::transcode("TimeGateStart");
  PadEdgeSizeXString = xercesc::XMLString::transcode("PadEdgeSizeX");
  PadEdgeSigmaXString = xercesc::XMLString::transcode("PadEdgeSigmaX");
  ClusterSizeXString = xercesc::XMLString::transcode("ClusterSizeX");
  ClusterProbXString = xercesc::XMLString::transcode("ClusterProbX");
  PadEdgeSizeYString = xercesc::XMLString::transcode("PadEdgeSizeY");
  PadEdgeSigmaYString = xercesc::XMLString::transcode("PadEdgeSigmaY");
  ClusterSizeYString = xercesc::XMLString::transcode("ClusterSizeY");
  ClusterProbYString = xercesc::XMLString::transcode("ClusterProbY");
  JitterMinString = xercesc::XMLString::transcode("JitterMin");
  JitterMaxString = xercesc::XMLString::transcode("JitterMax");
  JitterValuesString = xercesc::XMLString::transcode("JitterValues");
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlMuonReadoutCondCnv::~XmlMuonReadoutCondCnv() {
  xercesc::XMLString::release((XMLCh**)&ReadoutString);
  xercesc::XMLString::release((XMLCh**)&ReadoutTypeString);
  xercesc::XMLString::release((XMLCh**)&EfficiencyString);
  xercesc::XMLString::release((XMLCh**)&SyncDriftString);
  xercesc::XMLString::release((XMLCh**)&ChamberNoiseString);
  xercesc::XMLString::release((XMLCh**)&ElectronicsNoiseString);
  xercesc::XMLString::release((XMLCh**)&MeanDeadTimeString);
  xercesc::XMLString::release((XMLCh**)&RMSDeadTimeString);
  xercesc::XMLString::release((XMLCh**)&TimeGateStartString);
  xercesc::XMLString::release((XMLCh**)&PadEdgeSizeXString);
  xercesc::XMLString::release((XMLCh**)&PadEdgeSigmaXString);
  xercesc::XMLString::release((XMLCh**)&ClusterSizeXString);
  xercesc::XMLString::release((XMLCh**)&ClusterProbXString);
  xercesc::XMLString::release((XMLCh**)&PadEdgeSizeYString);
  xercesc::XMLString::release((XMLCh**)&PadEdgeSigmaYString);
  xercesc::XMLString::release((XMLCh**)&ClusterSizeYString);
  xercesc::XMLString::release((XMLCh**)&ClusterProbYString);
  xercesc::XMLString::release((XMLCh**)&JitterMinString);
  xercesc::XMLString::release((XMLCh**)&JitterMaxString);
  xercesc::XMLString::release((XMLCh**)&JitterValuesString);
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode
XmlMuonReadoutCondCnv::i_fillSpecificObj (xercesc::DOMElement* childElement,
                                          MuonReadoutCond* dataObj,
                                          IOpaqueAddress* /*address*/){
  MsgStream log (msgSvc(), "XmlMuonReadoutCondCnv");

  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();

  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Processing element "
        << dom2Std(tagName) << endmsg;
  /*
        <!--
             ReadoutType      : either Anode or Cathode (must be defined)
             Efficiency       : as a fraction (mean value)
             SyncDrift        : (maximum) synchronization imprecision (ns)
             ChamberNoise     : (counts/sec/cm2)
             ElectronicsNoise : (counts/sec/channel)
             MeanDeadTime     : ns
             RMSDeadTime      : ns
             TimeGateStart    : relative to T0 (ns)
             PadEdgeSizeX     : Size of double hit rate at pad edge in X
             PadEdgeSigmaX    : width (sigma) of pad edge effect in X (cm)
             ClusterSizeX     : 1,2,3 list of cluster sizes in X
             ClusterProbX     : 0.9,0.05,0.05 list of cluster probabilites
             PadEdgeSizeY     : Size of double hit rate at pad edge in Y
             PadEdgeSigmaY    : width (sigma) of pad edge effect in Y
             ClusterSizeY     : 1,2,3 list of cluster sizes in Y
             ClusterProbY     : 0.9,0.05,0.05 list of cluster probabilites
        -->
        <!ELEMENT Readout EMPTY>
        <!ATTLIST Readout ReadoutType   ( Anode | Cathode ) #REQUIRED
                          Efficiency    CDATA "1.0"
                          SyncDrift     CDATA "0.0"
                          ChamberNoise  CDATA "0.0"
                          ElectronicsNoise CDATA "0.0"
                          MeanDeadTime  CDATA "0.0"
                          RMSDeadTime   CDATA "0.0"
                          TimeGateStart CDATA "0.0"
                          PadEdgeSizeX  CDATA "0.0"
                          PadEdgeSigmaX CDATA "1.0"
                          ClusterSizeX  CDATA "1"
                          ClusterProbX  CDATA "1.0"
                          PadEdgeSizeY  CDATA "0.0"
                          PadEdgeSigmaY CDATA "1.0"
                          ClusterSizeY  CDATA "1"
                          ClusterProbY  CDATA "0.0"
                          JitterMin     CDATA #REQUIRED
                          JitterMax     CDATA #REQUIRED
                          JitterValues  CDATA #REQUIRED>
  */

  if (0 == xercesc::XMLString::compareString(ReadoutString, tagName)) {
    const std::string rType =
      dom2Std (childElement->getAttribute (ReadoutTypeString));
    const std::string eff =
      dom2Std (childElement->getAttribute (EfficiencyString));
    const std::string syncD =
      dom2Std (childElement->getAttribute (SyncDriftString));
    const std::string chamNoise =
      dom2Std (childElement->getAttribute (ChamberNoiseString));
    const std::string elecNoise =
      dom2Std (childElement->getAttribute (ElectronicsNoiseString));
    const std::string meanDead =
      dom2Std (childElement->getAttribute (MeanDeadTimeString));
    const std::string rmsDead =
      dom2Std (childElement->getAttribute (RMSDeadTimeString));
    const std::string timeOffset =
      dom2Std (childElement->getAttribute (TimeGateStartString));

    const std::string padESX =
      dom2Std (childElement->getAttribute (PadEdgeSizeXString));
    const std::string padESigX =
      dom2Std (childElement->getAttribute (PadEdgeSigmaXString));
    std::string clSzX =
      dom2Std (childElement->getAttribute (ClusterSizeXString));
    std::string clPrX =
      dom2Std (childElement->getAttribute (ClusterProbXString));

    const std::string padESY =
      dom2Std (childElement->getAttribute (PadEdgeSizeYString));
    const std::string padESigY =
      dom2Std (childElement->getAttribute (PadEdgeSigmaYString));
    std::string clSzY =
      dom2Std (childElement->getAttribute (ClusterSizeYString));
    std::string clPrY =
      dom2Std (childElement->getAttribute (ClusterProbYString));

    const std::string jitterMin =
      dom2Std (childElement->getAttribute (JitterMinString));
    const std::string jitterMax =
      dom2Std (childElement->getAttribute (JitterMaxString));
    std::string jitterValues =
      dom2Std (childElement->getAttribute (JitterValuesString));


    if ( !((rType == "Anode") || (rType == "Cathode")) ) {
      log << MSG::WARNING << "Readout type claimed to be "
          << rType << endmsg;
      return StatusCode::FAILURE;
    }

    if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
      log << MSG::DEBUG << "ReadoutType " << rType <<endmsg;
      log << MSG::DEBUG << "Efficiency " << eff  <<endmsg;
    }

    // index is the number of the readout
    int index;
    StatusCode sc = dataObj->addReadout(rType,index);
    if( sc.isFailure() ){
      log << MSG::WARNING << "Failed to create a new readout" << endmsg;
    }

    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Readout index is :" << index << endmsg;

    dataObj->setEfficiency(  atof(eff.c_str()), index);
    dataObj->setSyncDrift(    atof(syncD.c_str()), index);
    dataObj->setChamberNoise( atof(chamNoise.c_str()), index);
    dataObj->setElectronicsNoise( atof(elecNoise.c_str()), index);
    dataObj->setMeanDeadTime( atof(meanDead.c_str()), index);
    dataObj->setRMSDeadTime(  atof(rmsDead.c_str()), index);
    dataObj->setTimeGateStart(atof(timeOffset.c_str()), index);

    dataObj->setPadEdgeSizeX( atof(padESX.c_str()), index);
    dataObj->setPadEdgeSigmaX(atof(padESigX.c_str()), index);
    dataObj->setPadEdgeSizeY( atof(padESY.c_str()), index);
    dataObj->setPadEdgeSigmaY(atof(padESigY.c_str()), index);

    sc = setClusterSizes(dataObj,index,clSzX,clPrX,clSzY,clPrY);
    if(!sc){
      return sc;
    }

    sc = setJitterVector(dataObj,index,jitterMin,jitterMax,jitterValues);
    if(!sc){
      return sc;
    }

  } else {
    // Unknown tag, a warning message could be issued here
    log << MSG::WARNING << "Can not interpret specific type :"
        << dom2Std(tagName) << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode
XmlMuonReadoutCondCnv::setClusterSizes(MuonReadoutCond* dataObj,
                                      const int &index,
                                      std::string &clSzX,
                                      std::string &clPrX,
                                      std::string &clSzY,
                                      std::string &clPrY){

  MsgStream log (msgSvc(), "XmlMuonReadoutCondCnv");
  // need to split clSzX and clPrX into component parts
  // should be a comma seperated list

  if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
    log << MSG::DEBUG << "Cluster size (X) " << clSzX << endmsg;
    log << MSG::DEBUG << "Cluster Prob (X) " << clPrX << endmsg;
  }

  std::string::size_type cPos = clSzX.find(',');
  std::string sCurr;
  std::vector<int> clSize;
  while( cPos != std::string::npos ){
    sCurr = clSzX.substr( 0, cPos);
    clSize.push_back(atol(sCurr.c_str()));
    clSzX.erase( 0, cPos+1);
    cPos = clSzX.find(',');
  }
  clSize.push_back(atol(clSzX.c_str()));

  cPos = clPrX.find(',');
  std::vector<double> clProb;
  while( cPos != std::string::npos ){
    sCurr = clPrX.substr( 0, cPos);
    clProb.push_back(atof(sCurr.c_str()));
    clPrX.erase( 0, cPos+1);
    cPos = clPrX.find(',');
  }
  clProb.push_back(atof(clPrX.c_str()));

  if(clProb.size() != clSize.size()){
    log << MSG::ERROR << "Found "<< clProb.size()
        << " probabilities and " << clSize.size() << " sizes (X)" << endmsg;
    return StatusCode::FAILURE;
  }
  unsigned int i;
  for(i=0; i < clProb.size(); ++i){
    dataObj->addClusterX(clSize[i],clProb[i],index);
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Clusters (X) " << clSize[i]
          << ","<< clProb[i] << endmsg;
  }

  clSize.clear();
  clProb.clear();

  if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
    log << MSG::DEBUG << "Cluster size (Y) " << clSzY << endmsg;
    log << MSG::DEBUG << "Cluster Prob (Y) " << clPrY << endmsg;
  }

  cPos = clSzY.find(',');
  while( cPos != std::string::npos ){
    sCurr = clSzY.substr( 0, cPos);
    clSize.push_back(atol(sCurr.c_str()));
    clSzY.erase( 0, cPos+1);
    cPos = clSzY.find(',');
  }
  clSize.push_back(atol(clSzY.c_str()));

  cPos = clPrY.find(',');
  while( cPos != std::string::npos ){
    sCurr = clPrY.substr(0,cPos);
    clProb.push_back(atof(sCurr.c_str()));
    clPrY.erase( 0, cPos+1);
    cPos = clPrY.find(',');
  }
  clProb.push_back(atof(clPrY.c_str()));

  if(clProb.size() != clSize.size()){
    log << MSG::ERROR << "Found "<< clProb.size()
        << " probabilities and " << clSize.size() << " sizes (X)" << endmsg;
    return StatusCode::FAILURE;
  }
  for(i=0; i<clProb.size(); ++i){
    dataObj->addClusterY(clSize[i],clProb[i],index);
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Clusters (Y) " << clSize[i]
          << ","<< clProb[i] << endmsg;
  }
  return StatusCode::SUCCESS;
}


StatusCode
XmlMuonReadoutCondCnv::setJitterVector(MuonReadoutCond* dataObj,
                                       const int &index,
                                       const std::string &jitterMin,
                                       const std::string &jitterMax,
                                       std::string &jitterValues){

  MsgStream log (msgSvc(), "XmlMuonReadoutCondCnv");
  // need to split clSzX and clPrX into component parts
  // should be a comma seperated list

  if( UNLIKELY( log.level() <= MSG::DEBUG ) ) {
    log << MSG::DEBUG << "Jitter Min " << jitterMin << endmsg;
    log << MSG::DEBUG << "jitter Max " << jitterMax << endmsg;
  }

  std::string::size_type cPos = jitterValues.find(',');
  std::string sCurr;
  std::vector<double> jitterVector;
  while( cPos != std::string::npos ){ // found a comma in string
    sCurr = jitterValues.substr( 0, cPos); // get up to first comma
    jitterVector.push_back(atof(sCurr.c_str())); // add to vector
    jitterValues.erase( 0, cPos+1); // erase up to and including comma
    cPos = jitterValues.find(','); // find next comma
  }
  jitterVector.push_back(atof(jitterValues.c_str()));

  dataObj->setTimeJitter(jitterVector,atof(jitterMin.c_str()),
                         atof(jitterMax.c_str()),index);

  return StatusCode::SUCCESS;
}

