// $Id: BuildMCRichDigitLinks.h,v 1.2 2004-06-17 12:00:04 cattanem Exp $
#ifndef RICHDAQ_BUILDMCRICHDIGITLINKS_H 
#define RICHDAQ_BUILDMCRICHDIGITLINKS_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Gaudi framework
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCTruth.h"

/** @class BuildMCRichDigitLinks BuildMCRichDigitLinks.h
 *  
 *  Builds the links between MCRichDigits and RichDigits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

class BuildMCRichDigitLinks : public RichAlgBase {

public:

  /// Standard constructor
  BuildMCRichDigitLinks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BuildMCRichDigitLinks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// String containing input MCRichDigits location in TDS
  std::string m_mcRichDigitsLocation;

  /// String containing input RichDigits location in TDS
  std::string m_richDigitsLocation;

};
#endif // RICHDAQ_BUILDMCRICHDIGITLINKS_H
