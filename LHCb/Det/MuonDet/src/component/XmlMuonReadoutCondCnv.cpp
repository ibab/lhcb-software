// $Id: XmlMuonReadoutCondCnv.cpp,v 1.4 2002-08-05 18:14:09 asatta Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/06/04 16:08:37  dhcroft
// Added time jitter pdfs to the readouts
//
// Revision 1.2  2002/01/31 10:00:10  dhcroft
// Moved CLIDs to seperate files for Visual C linker
//
// ============================================================================

// Include files
#include <vector>

#include "DetDesc/XmlUserConditionCnv.h"
#include "MuonDet/MuonReadoutCond.h"

#include "GaudiKernel/RegistryEntry.h"
#include <dom/DOM_NamedNodeMap.hpp>

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
  ~XmlMuonReadoutCondCnv() {}


protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
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

};


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlMuonReadoutCondCnv> muonreadoutcond_factory;
const ICnvFactory& XmlMuonReadoutCondCnvFactory = muonreadoutcond_factory;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlMuonReadoutCondCnv::XmlMuonReadoutCondCnv(ISvcLocator* svc) :
  XmlUserConditionCnv<MuonReadoutCond> (svc)
{
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child element
// ------------------------------------------------------------------------
StatusCode XmlMuonReadoutCondCnv::i_fillSpecificObj (DOM_Element childElement,
                                                     MuonReadoutCond* dataObj,
                                                 IOpaqueAddress* /*address*/){
  MsgStream log (msgSvc(), "XmlMuonReadoutCondCnv");

  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());

  log << MSG::DEBUG << "Processing element "
      << tagName << endreq;
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

  if ("Readout" == tagName) {
    const std::string rType =
      dom2Std (childElement.getAttribute ("ReadoutType"));
    const std::string eff =
      dom2Std (childElement.getAttribute ("Efficiency"));
    const std::string syncD =
      dom2Std (childElement.getAttribute ("SyncDrift"));
    const std::string chamNoise =
      dom2Std (childElement.getAttribute ("ChamberNoise"));
    const std::string elecNoise =
      dom2Std (childElement.getAttribute ("ElectronicsNoise"));
    const std::string meanDead =
      dom2Std (childElement.getAttribute ("MeanDeadTime"));
    const std::string rmsDead =
      dom2Std (childElement.getAttribute ("RMSDeadTime"));
    const std::string timeOffset =
      dom2Std (childElement.getAttribute ("TimeGateStart"));

    const std::string padESX =
      dom2Std (childElement.getAttribute ("PadEdgeSizeX"));
    const std::string padESigX =
      dom2Std (childElement.getAttribute ("PadEdgeSigmaX"));
    std::string clSzX =
      dom2Std (childElement.getAttribute ("ClusterSizeX"));
    std::string clPrX =
      dom2Std (childElement.getAttribute ("ClusterProbX"));

    const std::string padESY =
      dom2Std (childElement.getAttribute ("PadEdgeSizeY"));
    const std::string padESigY =
      dom2Std (childElement.getAttribute ("PadEdgeSigmaY"));
    std::string clSzY =
      dom2Std (childElement.getAttribute ("ClusterSizeY"));
    std::string clPrY =
      dom2Std (childElement.getAttribute ("ClusterProbY"));

    const std::string jitterMin =
      dom2Std (childElement.getAttribute ("JitterMin"));
    const std::string jitterMax =
      dom2Std (childElement.getAttribute ("JitterMax"));
    std::string jitterValues =
      dom2Std (childElement.getAttribute ("JitterValues"));
    
    
    if( !(rType == "Anode" || rType == "Cathode") ){
      log << MSG::WARNING << "Readout type claimed to be "
          << rType << endreq;
      return StatusCode::FAILURE;
    }

    log << MSG::DEBUG << "ReadoutType "
        << rType <<endreq;
    log << MSG::DEBUG << "Efficiency "
        << eff  <<endreq;

    // index is the number of the readout
    int index;
    StatusCode sc = dataObj->addReadout(rType,index);
    if( sc.isFailure() ){
      log << MSG::WARNING << "Failed to create a new readout" << endreq;
    }

    log << MSG::DEBUG << "Readout index is :" << index << endreq;

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
        << tagName << endreq;
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
  
  log << MSG::DEBUG << "Cluster size (X) " << clSzX << endreq;
  log << MSG::DEBUG << "Cluster Prob (X) " << clPrX << endreq;
  
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
        << " probabilities and " << clSize.size() << " sizes (X)" << endreq;
    return StatusCode::FAILURE;
  }
  unsigned int i;
  for(i=0; i < clProb.size(); ++i){
    dataObj->addClusterX(clSize[i],clProb[i],index);
    log << MSG::DEBUG << "Clusters (X) " << clSize[i]
        << ","<< clProb[i] << endreq;
  }
  
  clSize.clear();
  clProb.clear();

  log << MSG::DEBUG << "Cluster size (Y) " << clSzY << endreq;
  log << MSG::DEBUG << "Cluster Prob (Y) " << clPrY << endreq;
  
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
        << " probabilities and " << clSize.size() << " sizes (X)" << endreq;
    return StatusCode::FAILURE;
  }
  for(i=0; i<clProb.size(); ++i){
    dataObj->addClusterY(clSize[i],clProb[i],index);
    log << MSG::DEBUG << "Clusters (Y) " << clSize[i]
        << ","<< clProb[i] << endreq;
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
  
  log << MSG::DEBUG << "Jitter Min " << jitterMin << endreq;
  log << MSG::DEBUG << "jitter Max " << jitterMax << endreq;
  
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

