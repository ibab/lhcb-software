
//------------------------------------------------------------------------------------
/** @file RichDigiQC_load.cpp
 *
 *  Implementation file for classes in the RichDigiQC Component Library
 *
 *  CVS Log :-
 *  $Id: RichDigiQC_load.cpp,v 1.3 2005-02-20 18:36:41 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//------------------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichDigiQC )
{
  DECLARE_ALGORITHM( RichDigitQC );
  DECLARE_ALGORITHM( RichDigiDataObjVerifier );
}
