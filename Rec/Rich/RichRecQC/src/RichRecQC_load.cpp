
/** @file RichRecQC_load.cpp
 *
 *  Declaration of objects in component library RichRecQC
 *
 *  CVS Log :-
 *  $Id: RichRecQC_load.cpp,v 1.8 2007-02-02 10:08:36 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichRecQC )
{
  DECLARE_ALGORITHM( PIDQC        );
  DECLARE_ALGORITHM( RecoQC       );
  DECLARE_ALGORITHM( HPDHitsMoni  );
  DECLARE_ALGORITHM( PixelQC      );
  DECLARE_ALGORITHM( SummaryQC    );
}
