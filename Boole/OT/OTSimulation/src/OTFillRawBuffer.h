// $Id: OTFillRawBuffer.h,v 1.7 2008-05-28 20:05:04 janos Exp $
#ifndef OTFILLRAWBUFFER_H 
#define OTFILLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from OTDAQ
#include "OTDAQ/IOTRawBankEncoder.h"

/** @class OTFillRawBuffer OTFillRawBuffer.h
 *   
 *
 *  @author Jan Amoraal
 *  @date   2008-05-23
 */

class OTFillRawBuffer : public GaudiAlgorithm {

public: 
  /// Standard constructor
  OTFillRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTFillRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  
  IOTRawBankEncoder* m_encoder;

};
#endif // OTFILLRAWBUFFER_H
