// $Id: MCRichDigitsToRawBufferAlg.h,v 1.3 2004-03-16 13:37:37 jonesc Exp $
#ifndef RICHDAQ_MCRICHDIGITSTORAWBUFFERALG_H
#define RICHDAQ_MCRICHDIGITSTORAWBUFFERALG_H 1

// from STL
#include <string>

// gaudi
#include "GaudiKernel/SmartDataPtr.h"

// base clase
#include "RichUtils/RichAlgBase.h"

// Event Model
#include "Event/DAQTypes.h"
#include "Event/RawBuffer.h"
#include "Event/RichDigit.h"

// RichDAQ utility classes
#include "RichDAQHeaderPD.h"
#include "RichZSHitTriplet.h"
#include "RichDAQLinkNumber.h"
#include "RichNonZeroSuppData.h"

/** @class MCRichDigitsToRawBufferAlg MCRichDigitsToRawBufferAlg.h
 *
 *  Algorithm to fill the Raw buffer with RICH information from MCRichDigits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

class MCRichDigitsToRawBufferAlg : public RichAlgBase {

public:

  /// Standard constructor
  MCRichDigitsToRawBufferAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MCRichDigitsToRawBufferAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Fill the data bank for zero suppressed HPDs
  void fillZeroSuppressed    ( RichSmartID pdID,
                               RichDAQ::RAWBank & dataBank,
                               const MCRichDigitVector & pdHits ) const;

  /// Fill the data bank for non zero suppressed HPDs
  void fillNonZeroSuppressed ( RichSmartID pdID,
                               RichDAQ::RAWBank & dataBank,
                               const MCRichDigitVector & pdHits ) const;

private: // data

  /// Location of Raw buffer in TES
  std::string m_rawBuffLoc;

  /// Location of input MCRichDigits in TES
  std::string m_digitsLoc;

  /// The number of hits marking the transistion between zero and non-zero suppressed data
  unsigned int m_zeroSuppresCut;

};
#endif // RICHDAQ_RICHDIGITSTORAWBUFFERALG_H
