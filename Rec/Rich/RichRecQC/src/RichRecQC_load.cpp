
/** @file RichRecQC_load.cpp
 *
 *  Declaration of objects in component library RichRecQC
 *
 *  CVS Log :-
 *  $Id: RichRecQC_load.cpp,v 1.3 2004-07-28 11:58:23 jonesc Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2004/07/27 13:56:30  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichRecQC )
{

  DECLARE_ALGORITHM( RichPIDQC );
  DECLARE_ALGORITHM( RichRecoQC );

}
