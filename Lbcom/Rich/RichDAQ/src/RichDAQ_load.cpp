
/** @file RichDAQ_load.cpp
 *
 *  Declaration file for objects in component library RichDAQ
 *
 *  CVS Log :-
 *  $Id: RichDAQ_load.cpp,v 1.5 2004-07-27 13:46:07 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichDAQ )
{

  // Algorithm to convert RichDigits to Raw Buffer
  DECLARE_ALGORITHM( MCRichDigitsToRawBufferAlg );

  // Algorithm to create RichDigits from Raw Buffer
  DECLARE_ALGORITHM( RawBufferToRichDigitsAlg );

  // Create links between RichDigits and MCRichDigits
  DECLARE_ALGORITHM( BuildMCRichDigitLinks );

}
