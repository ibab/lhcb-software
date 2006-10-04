
//-----------------------------------------------------------------------------
/** @file RichDAQ_load.cpp
 *
 *  Declaration file for objects in component library RichDAQ
 *
 *  CVS Log :-
 *  $Id: RichDAQ_load.cpp,v 1.12 2006-10-04 10:46:47 jonrob Exp $
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

  // Algorithm to simply load the RawEvent into memory
  DECLARE_ALGORITHM( RichLoadRawEvent );

  // Tool to decode the Raw buffer into RichSmartIDs
  DECLARE_TOOL( RichRawBufferToSmartIDsTool );

  // Tool to create HPD bank bank objects from Raw Buffer
  DECLARE_TOOL( RichRawDataFormatTool );

  // Pixel suppression tools
  DECLARE_TOOL( RichHighOccHPDSuppressionTool );
  DECLARE_TOOL( RichHPDPixelClusterSuppressionTool );

}
