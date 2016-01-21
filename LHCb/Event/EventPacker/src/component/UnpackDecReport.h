// $Id: $
#ifndef UNPACKDECREPORT_H 
#define UNPACKDECREPORT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class UnpackDecReport UnpackDecReport.h
 *  
 *  Unpacks DecReports
 *  
 *  @author Olivier Callot
 *  @date   2012-01-23
 */
class UnpackDecReport : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  UnpackDecReport( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackDecReport(); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_outputName;

  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing

};

#endif // UNPACKDECREPORT_H
