// $Id: RichDigitCreator.h,v 1.1.1.1 2003-06-19 11:22:54 jonesc Exp $
#ifndef RICHREADOUT_RICHDIGITCREATOR_H
#define RICHREADOUT_RICHDIGITCREATOR_H 1

// Fram Gaudi framework
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// From Rich Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCTruth.h"

#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichDigitCreator RichDigitCreator.h
 *
 *  Algorithm for forming RichDigits from MCRichDigits, a one to one copy.
 *
 *  @author Christopher.Rob.Jones@cern.ch
 *  created Fri Mar 15 11:10:17 2002
 *
 */

class RichDigitCreator : public Algorithm {

public:

  /// Standard algorithm constructor
  RichDigitCreator (const std::string &name,
                    ISvcLocator *svcLoc);

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
