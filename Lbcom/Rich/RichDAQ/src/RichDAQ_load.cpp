
//-----------------------------------------------------------------------------
/** @file RichDAQ_load.cpp
 *
 *  Declaration file for objects in component library RichDAQ
 *
 *  CVS Log :-
 *  $Id: RichDAQ_load.cpp,v 1.7 2005-01-07 12:35:59 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.6  2004/10/30 19:13:05  jonrob
 *  Reworking RawBuffer decoding as a tool, to allow reconstruction 
 *  to skip RichDigit creation
 *
 *  Revision 1.5  2004/07/27 13:46:07  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichDAQ )
{

  // Algorithm to create RichDigits from RichSmartIDs
  DECLARE_ALGORITHM( RawBufferToRichDigitsAlg );

  // Tool to decode the Raw buffer into RichSmartIDs
  DECLARE_TOOL( RichRawBufferToSmartIDsTool );

  // Tool to create HPD bank bank objects from Raw Buffer
  DECLARE_TOOL( RichRawDataFormatTool );

  // Tool to convert between HPD and L1 identifiers
  DECLARE_TOOL( RichHPDToLevel1Tool );

  // Tool to convert between HPD hardware and software identifiers
  DECLARE_TOOL( RichHPDIDTool );

}
