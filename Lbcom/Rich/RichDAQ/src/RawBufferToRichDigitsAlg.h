// $Id: RawBufferToRichDigitsAlg.h,v 1.2 2004-02-02 14:27:39 jonesc Exp $
#ifndef RICHDAQ_RAWBUFFERTORICHDIGITSALG_H 
#define RICHDAQ_RAWBUFFERTORICHDIGITSALG_H 1

// gaudi
#include "GaudiKernel/SmartDataPtr.h"

// base clase
#include "RichUtils/RichAlgBase.h"

// RichDAQ utility classes
#include "RichDAQDefinitions.h"
#include "RichDAQHeaderPD.h"
#include "RichZSHitTriplet.h"
#include "RichDAQLinkNumber.h"
#include "RichNonZeroSuppData.h"

/** @class RawBuffeToRichDigitsAlg RawBufferToRichDigitsAlg.h
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

private: // data

  /// Input location for RawEvent in TES
  std::string m_rawEventLoc;

  /// Output location for RichDigits
  std::string m_richDigitsLoc;

  // Pointer to RichDigit contianer
  RichDigits * m_digits;

};

#endif // RICHDAQ_RAWBUFFERTORICHDIGITSALG_H
