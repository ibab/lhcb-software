
//--------------------------------------------------------------------------
/** @file RichGlobalPIDDigitSel.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDDigitSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDDigitSel.h,v 1.7 2006-12-19 09:06:20 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H
#define RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H 1

// Base
#include "RichGlobalPIDAlgBase.h"
#include "RichRecBase/RichRecProcCode.h"

// Event
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"

//--------------------------------------------------------------------------
/** @class RichGlobalPIDDigitSel RichGlobalPIDDigitSel.h
 *
 *  RichDigit selection algorithm for Rich Global PID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

class RichGlobalPIDDigitSel : public RichGlobalPIDAlgBase
{

public:

  /// Standard constructor
  RichGlobalPIDDigitSel( const std::string& name,
                         ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDDigitSel();   ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // private data

  /// Maximum number of usable pixels
  int m_maxUsedPixels;

  /// Location of processing status object in TES
  std::string m_procStatLocation;

};

#endif // RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H
