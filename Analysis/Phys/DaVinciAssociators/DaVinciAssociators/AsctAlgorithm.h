// $Id: AsctAlgorithm.h,v 1.4 2002-10-10 18:42:36 gcorti Exp $
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
StatusCode retrievePrivateAsct( std::string asctType,
                                bool sameInput, T*& asct) {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> entering retrievePrivateAsct" << endreq;

  // Internally used associator
  std::string myLocation = outputTable() +"AuxTable" ;
  StringProperty myAlgName;
  
  StatusCode sc = toolSvc()->retrieveTool( asctType, asct, this);
  if( sc.isFailure() || 0 == asct) {
    log << MSG::FATAL << "    Unable to retrieve " 
        << asctType << " Associator tool" 
        << endreq;
    return sc;
  }
  IProperty* prop = dynamic_cast<IProperty*>( asct );
  if( prop ) {
    sc = prop->setProperty( "Location", myLocation);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property Location set to " << myLocation
          << " in Asct " << asctType << endreq;
    }
    StringProperty myAlgType;
    myAlgType.assign( prop->getProperty( "AlgorithmType" ) );
    log << MSG::DEBUG << "AlgorithmType is : " << myAlgType << endreq;
    myAlgName = name() + "." + (std::string)myAlgType;
    sc = prop->setProperty( "AlgorithmName", myAlgName);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property AlgorithmName set to " 
          << myAlgName << " in Asct " << asctType << endreq;
    }
  }
  if( !sameInput) return StatusCode::SUCCESS;
  
  
  // Now set the auxiliary algorithm's InputData property as the local one
  //  ONLY if requested...
  prop = dynamic_cast<IProperty*>( asct->algorithm() );
  if( prop ) {
    std::string propString ;
    std::string sep = "\"";
    propString = "[";
    for( std::vector<std::string>::iterator inp = m_inputData.begin();
         m_inputData.end() != inp; inp++ ) {
      propString = propString + sep + *inp ;
      sep = "\",\"";
    }
    propString = propString + "\"]";
    log << MSG::DEBUG << "InputData property is : " << propString << endreq;
  
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

protected:

  std::vector<std::string> m_inputData;  ///< location of input data in the TES
  std::string m_outputTable; ///< location of relations table  

};

//=============================================================================
#endif // ASCTALGORITHM_H
