// $Id: RichDigitCreator.h,v 1.4 2004-06-17 14:53:02 jonesc Exp $
#ifndef RICHREADOUT_RICHDIGITCREATOR_H
#define RICHREADOUT_RICHDIGITCREATOR_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Gaudi framework
#include "GaudiKernel/SmartDataPtr.h"

// From Rich Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCTruth.h"

// units
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichDigitCreator RichDigitCreator.h
 *
 *  Algorithm for forming RichDigits from MCRichDigits, a one to one copy.
 *
 *  @author Christopher.Rob.Jones@cern.ch
 *  created Fri Mar 15 11:10:17 2002
 *
 */

class RichDigitCreator : public RichAlgBase {

public:

  /// Standard algorithm constructor
  RichDigitCreator ( const std::string &name,
                     ISvcLocator *svcLoc );

  /// Default destructor
  ~RichDigitCreator();

  /// Gaudi framework initialize method
  virtual StatusCode initialize ();

  /// Gaudi framework execute method
  virtual StatusCode execute ();

  /// Gaudi framework finalize method
  virtual StatusCode finalize ();

private:

  /// Default constructor
  RichDigitCreator();

  /// String containing input MCRichDigits location in TDS
  std::string m_mcRichDigitsLocation;

  /// String containing output RichDigits location in TDS
  std::string m_richDigitsLocation;

};

#endif // RICHREADOUT_RICHDIGITCREATOR_H
