
/** @file RichRecMCTools_load.cpp
 *
 *  Declaration of objects in component library RichRecMCTools
 *
 *  CVS Log :-
 *  $Id: RichRecMCTools_load.cpp,v 1.4 2004-10-13 09:37:27 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2004/07/27 16:14:11  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichRecMCTools ) 
{

  // Rich reconstruction working object creators that use cheated MC 
  // information in one way or another
  DECLARE_TOOL( RichPixelCreatorFromCheatedRichDigits        );
  DECLARE_TOOL( RichPixelCreatorFromSignalRichDigits         );
  DECLARE_TOOL( RichPixelCreatorFromMCRichHits               );
  DECLARE_TOOL( RichPixelCreatorFromRichDigitsWithBg         );
  DECLARE_TOOL( RichPhotonCreatorFromMCRichOpticalPhotons    );
  DECLARE_TOOL( RichPhotonPredictorUsingMCRichOpticalPhotons );

  // MC truth interrogation tools
  DECLARE_TOOL( RichRecMCTruthTool );

};
