
//------------------------------------------------------------------------------------
/** @file RichDigiQC_load.cpp
 *
 *  Implementation file for classes in the RichDigiQC Component Library
 *
 *  CVS Log :-
 *  $Id: RichDigiQC_load.cpp,v 1.4 2007-02-02 10:12:44 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//------------------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichDigiQC )
{
  DECLARE_ALGORITHM( DigitQC );
  DECLARE_ALGORITHM( DataObjVerifier );
}
