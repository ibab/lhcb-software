
//===============================================================================
/** @file MCRichDigitsToRawBufferAlg.h
 *
 *  Header file for RICH DAQ algorithm : MCRichDigitsToRawBufferAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitsToRawBufferAlg.h,v 1.2 2005-06-23 15:10:12 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//===============================================================================

#ifndef RICHDAQ_MCRICHDIGITSTORAWBUFFERALG_H
#define RICHDAQ_MCRICHDIGITSTORAWBUFFERALG_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// base class
#include "RichKernel/RichAlgBase.h"

// Event Model
#include "Event/MCRichDigit.h"

// Interfaces
#include "RichKernel/IRichRawDataFormatTool.h"
#include "RichKernel/IRichHPDToLevel1Tool.h"

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

class MCRichDigitsToRawBufferAlg : public RichAlgBase {

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

  /// Pointer to RICH Level1 tool
  const IRichHPDToLevel1Tool * m_level1;

  /// Location of input MCRichDigits in TES
  std::string m_digitsLoc;

  /// Data Format version
  unsigned int m_version;

};

#endif // RICHDAQ_RICHDIGITSTORAWBUFFERALG_H
