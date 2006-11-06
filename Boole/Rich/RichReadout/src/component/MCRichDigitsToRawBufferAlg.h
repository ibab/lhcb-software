
//===============================================================================
/** @file MCRichDigitsToRawBufferAlg.h
 *
 *  Header file for RICH DAQ algorithm : MCRichDigitsToRawBufferAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitsToRawBufferAlg.h,v 1.7 2006-11-06 09:41:56 cattanem Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//===============================================================================

#ifndef RICHDAQ_MCRICHDIGITSTORAWBUFFERALG_H
#define RICHDAQ_MCRICHDIGITSTORAWBUFFERALG_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Event Model
#include "Event/MCRichDigit.h"

// Interfaces
#include "RichKernel/IRichRawDataFormatTool.h"

//===============================================================================
/** @class MCRichDigitsToRawBufferAlg MCRichDigitsToRawBufferAlg.h
 *
 *  Algorithm to fill the Raw buffer with RICH information from MCRichDigits.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 *
 *  @todo Remove DC04 hacks when no longer needed
 */
//===============================================================================

class MCRichDigitsToRawBufferAlg : public RichAlgBase
{

public:

  /// Standard constructor
  MCRichDigitsToRawBufferAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MCRichDigitsToRawBufferAlg( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  /// Pointer to RICH raw data format tool
  const IRichRawDataFormatTool * m_rawFormatT;

  /// Location of input MCRichDigits in TES
  std::string m_digitsLoc;

  /// Data Format version
  unsigned int m_version;

};

#endif // RICHDAQ_RICHDIGITSTORAWBUFFERALG_H
