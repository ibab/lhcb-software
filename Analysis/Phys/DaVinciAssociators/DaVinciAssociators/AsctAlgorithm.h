// $Id: AsctAlgorithm.h,v 1.1 2002-07-18 12:35:40 phicharp Exp $
#ifndef ASCTALGORITHM_H 
#define ASCTALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


/** @class AsctAlgorithm AsctAlgorithm.h
 *  
 *  Base class for algorithms creating Association tables
 *  @author Philippe Charpentier
 *  @date   2002-07-16
 */
class AsctAlgorithm : public Algorithm {
public:
  /// Standard constructor
  AsctAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AsctAlgorithm( ); ///< Destructor

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; };

  /// Set up an associator internally used by this algorithm
  template <class T>
  StatusCode retrievePrivateAsct( std::string asctType, std::string extName,
                                  bool sameInput, T*& asct) ;

protected:

  std::vector<std::string> m_inputData;  ///< location of input data in the TES
  std::string m_outputTable; ///< location of relations table  

};

template <class T>
StatusCode AsctAlgorithm::retrievePrivateAsct( std::string asctType,
                                               std::string extName,
                                               bool sameInput,
                                               T*& asct) {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> retrievePrivateAsct" << endreq;

  // Internally used associator
  std::string myAsctName = name() + extName  + "Asct";
  std::string myAlgName  = name() + extName + "Alg" ;
  std::string myLocation = outputTable() + extName + "Temp" ;
  
  StatusCode sc = toolSvc()->retrieveTool( asctType,  myAsctName, asct);
  if( sc.isFailure() || 0 == asct) {
    log << MSG::FATAL << "    Unable to retrieve " 
        << myAsctName << " Associator tool" 
        << endreq;
    return sc;
  }
  IProperty* prop = 
    dynamic_cast<IProperty*>( asct );
  if( prop ) {
    sc = prop->setProperty( "Location", myLocation);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property Location set to " << myLocation
          << " in Asct " << myAsctName << endreq;
    }
    sc = prop->setProperty( "AlgorithmName", myAlgName);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property AlgorithmName set to " << myAlgName
          << " in Asct " << myAsctName << endreq;
    }
  }
  
  // Now set the auxuiliary algorithm's InputData property as the local one
  //  ONLY if requested...
  prop = dynamic_cast<IProperty*>( asct->algorithm() );
  if( sameInput && prop ) {
    std::string propString ;
    std::string sep = "\"";
    propString = "[";
    for( std::vector<std::string>::iterator inp = m_inputData.begin();
         m_inputData.end() != inp; inp++ ) {
      propString = propString + sep + *inp ;
      sep = "\",\"";
    }
    propString = propString + "\"]";
    sc = prop->setProperty( "InputData", propString);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property InputData set to "
          << propString << " in algo " << myAlgName << endreq;
    }
    else {
      log << MSG::DEBUG << " **** Error setting Property InputData of "
          << myAlgName << " to " << propString << endreq;
    }
  }
  return StatusCode::SUCCESS;
};
//=============================================================================
#endif // ASCTALGORITHM_H
