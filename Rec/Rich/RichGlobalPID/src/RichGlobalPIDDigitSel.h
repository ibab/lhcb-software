// $Id: RichGlobalPIDDigitSel.h,v 1.2 2003-07-02 09:02:59 jonrob Exp $
#ifndef RICHRECALGS_RICHGLOBALPIDDIGITSEL_H
#define RICHRECALGS_RICHGLOBALPIDDIGITSEL_H 1

// Base
#include "RichGlobalPIDAlgBase.h"
#include "RichRecBase/RichRecProcCode.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"

// interfaces
#include "RichRecBase/IRichPixelCreator.h"

/** @class RichGlobalPIDDigitSel RichGlobalPIDDigitSel.h
 *
 *  RichDigit selection algorithm for Rich Global PID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

class RichGlobalPIDDigitSel : public RichGlobalPIDAlgBase {

public:

  /// Standard constructor
  RichGlobalPIDDigitSel( const std::string& name,
                         ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDDigitSel();   ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // private data

  // tool pointers
  IRichPixelCreator * m_pixelCr;

  /// Maximum number of usable pixels
  int m_maxUsedPixels;

  /// Location of processing status object in TES
  std::string m_procStatLocation;
  
};

#endif // RICHRECALGS_RICHGLOBALPIDDIGITSEL_H
