// $Id: MCRichDigitsToHLTBuffAlg.h,v 1.2 2003-11-09 12:39:28 jonrob Exp $
#ifndef RICHDAQ_MCRICHDIGITSTOHLTBUFFALG_H
#define RICHDAQ_MCRICHDIGITSTOHLTBUFFALG_H 1

// from STL
#include <string>

// gaudi
#include "GaudiKernel/SmartDataPtr.h"

// base clase
#include "RichUtils/RichAlgBase.h"

// Event Model
#include "Event/DAQTypes.h"
#include "Event/HltBuffer.h"
#include "Event/RichDigit.h"

// RichDAQ utility classes
#include "RichDAQHeaderPD.h"
#include "RichZSHitTriplet.h"
#include "RichDAQLinkNumber.h"
#include "RichNonZeroSuppData.h"

/** @class MCRichDigitsToHLTBuffAlg MCRichDigitsToHLTBuffAlg.h
 *
 *  Algorithm to fill the HLT buffer with RICH information from MCRichDigits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

class MCRichDigitsToHLTBuffAlg : public RichAlgBase {

public:

  /// Standard constructor
  MCRichDigitsToHLTBuffAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCRichDigitsToHLTBuffAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  void fillZeroSuppressed    ( RichSmartID pdID,
                               Rich::HLTBank & dataBank,
                               const MCRichDigitVector & pdHits );
  void fillNonZeroSuppressed ( RichSmartID pdID,
                               Rich::HLTBank & dataBank,
                               const MCRichDigitVector & pdHits );

private: // data

  /// Location of HLT buffer in TES
  std::string m_hltBuffLoc;

  /// Location of input MCRichDigits in TES
  std::string m_digitsLoc;

  /// The number of hits marking the transistion between zero and non-zero suppressed data
  unsigned int m_zeroSuppresCut;

};
#endif // RICHDAQ_RICHDIGITSTOHLTBUFFALG_H
