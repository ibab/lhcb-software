
//-----------------------------------------------------------------------------
/** @file RichDAQ_load.cpp
 *
 *  Declaration file for objects in component library RichDAQ
 *
 *  CVS Log :-
 *  $Id: RichDAQ_load.cpp,v 1.13 2007-02-01 17:42:29 jonrob Exp $
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
  DECLARE_ALGORITHM( LoadRawEvent );

  // Tool to decode the Raw buffer into RichSmartIDs
  DECLARE_TOOL( RawBufferToSmartIDsTool );

  // Tool to create HPD bank bank objects from Raw Buffer
  DECLARE_TOOL( RawDataFormatTool );

  // Pixel suppression tools
  DECLARE_TOOL( HighOccHPDSuppressionTool );
  DECLARE_TOOL( HPDPixelClusterSuppressionTool );

}

