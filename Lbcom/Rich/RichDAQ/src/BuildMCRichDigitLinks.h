// $Id: BuildMCRichDigitLinks.h,v 1.1 2003-11-10 14:59:58 jonrob Exp $
#ifndef RICHDAQ_BUILDMCRICHDIGITLINKS_H 
#define RICHDAQ_BUILDMCRICHDIGITLINKS_H 1

// base clase
#include "RichUtils/RichAlgBase.h"

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
