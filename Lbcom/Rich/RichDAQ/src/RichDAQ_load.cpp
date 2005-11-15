
//-----------------------------------------------------------------------------
/** @file RichDAQ_load.cpp
 *
 *  Declaration file for objects in component library RichDAQ
 *
 *  CVS Log :-
 *  $Id: RichDAQ_load.cpp,v 1.9 2005-11-15 12:57:47 jonrob Exp $
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

  // Tool giving information on the varius numbering scehemes
  DECLARE_TOOL( RichDetNumberingTool );

}
