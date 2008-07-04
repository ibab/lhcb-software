// $Id: STErrorDecoding.h,v 1.2 2008-07-04 15:52:22 mneedham Exp $
#ifndef STERRORDECODING_H 
#define STERRORDECODING_H 1

//===========================================
// Include files
// from Gaudi
#include "STDecodingBaseAlg.h"

#include "Event/RawBank.h"

/** @class STErrorDecoding STErrorDecoding.h public/STErrorDecoding.h
 *   
 *
 *  @author Mathias Knecht, M Needham
 *  @date   2007-09-11 (2008-06)
 */

class STErrorDecoding : public STDecodingBaseAlg {
public: 
  /// Standard constructor
  STErrorDecoding( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STErrorDecoding( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
 
private:


  std::string m_outputLocation;
  
  bool m_PrintErrorInfo;

};
#endif // STERRORDECODING_H
