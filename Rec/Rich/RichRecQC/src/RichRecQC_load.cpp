
/** @file RichRecQC_load.cpp
 *
 *  Declaration of objects in component library RichRecQC
 *
 *  CVS Log :-
 *  $Id: RichRecQC_load.cpp,v 1.7 2006-09-25 06:24:47 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichRecQC )
{
  DECLARE_ALGORITHM( RichPIDQC        );
  DECLARE_ALGORITHM( RichRecoQC       );
  DECLARE_ALGORITHM( RichHPDHitsMoni  );
  DECLARE_ALGORITHM( RichRecPixelQC   );
  DECLARE_ALGORITHM( RichRecSummaryQC );
}
