// $Id: STErrorDecoding.h,v 1.1 2008-06-25 06:55:15 mneedham Exp $
#ifndef STERRORDECODING_H 
#define STERRORDECODING_H 1

//===========================================
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawBank.h"

/** @class STErrorDecoding STErrorDecoding.h public/STErrorDecoding.h
 *   
 *
 *  @author Mathias Knecht, M Needham
 *  @date   2007-09-11 (2008-06)
 */

class STErrorDecoding : public GaudiAlgorithm {
public: 
  /// Standard constructor
  STErrorDecoding( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STErrorDecoding( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
 
private:

  LHCb::RawBank::BankType m_bankType;  
  
  std::string m_detType;
  std::string m_outputLocation;
  std::string m_ErrorLocation;  
  std::string m_typeString;
  
  bool m_PrintErrorInfo;

};
#endif // STERRORDECODING_H
