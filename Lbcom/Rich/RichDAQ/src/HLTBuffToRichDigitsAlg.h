// $Id: HLTBuffToRichDigitsAlg.h,v 1.1.1.1 2003-11-08 14:26:20 jonesc Exp $
#ifndef RICHDAQ_HLTBUFFTORICHDIGITSALG_H 
#define RICHDAQ_HLTBUFFTORICHDIGITSALG_H 1

// from STL
#include <string>

// gaudi
#include "GaudiKernel/SmartDataPtr.h"

// base clase
#include "RichUtils/RichAlgBase.h"

// Event
#include "Event/DAQTypes.h"
#include "Event/HltEvent.h"
#include "Event/RichDigit.h"

// RichDAQ utility classes
#include "RichDAQHeaderPD.h"
#include "RichZSHitTriplet.h"
#include "RichDAQLinkNumber.h"
#include "RichNonZeroSuppData.h"

/** @class HLTBuffToRichDigitsAlg HLTBuffToRichDigitsAlg.h
 *  
 *  Algorithm to create RichDigits from HLTEvent object
 * 
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

class HLTBuffToRichDigitsAlg : public RichAlgBase {

public:

  /// Standard constructor
  HLTBuffToRichDigitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HLTBuffToRichDigitsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // definitions

  typedef std::vector<HltBank> RichHltBanks;

private: // methods

  /// Decode a zero suppress data block
  unsigned int decodeZeroSuppressedBank( const HltBank & bank ); 

  /// Decode a non-zero suppress data block
  unsigned int decodeNonZeroSuppressedBank( const HltBank & bank ); 

  // temporary
  StatusCode createHltEvent();

private: // data

  /// Input location for HltEvent in TES
  std::string m_hltEventLoc;

  /// Output location for RichDigits
  std::string m_richDigitsLoc;

  // Pointer to RichDigit contianer
  RichDigits * m_digits;

};
#endif // RICHDAQ_HLTBUFFTORICHDIGITSALG_H
