// $Id: RawBufferToRichDigitsAlg.h,v 1.4 2004-06-17 12:00:04 cattanem Exp $
#ifndef RICHDAQ_RAWBUFFERTORICHDIGITSALG_H 
#define RICHDAQ_RAWBUFFERTORICHDIGITSALG_H 1

// gaudi
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichKernel/RichAlgBase.h"

// RichDAQ utility classes
#include "RichDAQDefinitions.h"
#include "RichDAQHeaderPD.h"
#include "RichZSHitTriplet.h"
#include "RichDAQLinkNumber.h"
#include "RichNonZeroSuppData.h"

/** @class RawBufferToRichDigitsAlg RawBufferToRichDigitsAlg.h
 *  
 *  Algorithm to create RichDigits from RawEvent object
 * 
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

class RawBufferToRichDigitsAlg : public RichAlgBase {

public:

  /// Standard constructor
  RawBufferToRichDigitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBufferToRichDigitsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Decode a zero suppress data block
  unsigned int decodeZeroSuppressedBank( const RawBank & bank ) const; 

  /// Decode a non-zero suppress data block
  unsigned int decodeNonZeroSuppressedBank( const RawBank & bank ) const; 

  /// Retrieves the raw event. If not available tries to build one from RawBuffer
  RawEvent * getRawEvent();

private: // data

  /// Input location for RawEvent in TES
  std::string m_rawEventLoc;

  /// Output location for RichDigits
  std::string m_richDigitsLoc;

  // Pointer to RichDigit contianer
  RichDigits * m_digits;

};

#endif // RICHDAQ_RAWBUFFERTORICHDIGITSALG_H
