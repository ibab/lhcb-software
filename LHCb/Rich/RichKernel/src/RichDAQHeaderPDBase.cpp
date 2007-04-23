
//=============================================================================================
/** @file RichDAQHeaderPDBase.cpp
 *
 *  Implementation file for RICH DAQ utility class : Rich::DAQ::HeaderPDBase
 *
 *  CVS Log :-
 *  $Id: RichDAQHeaderPDBase.cpp,v 1.1 2007-04-23 12:44:04 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/04/2007
 */
//=============================================================================================

// local
#include "RichKernel/RichDAQHeaderPDBase.h"

using namespace Rich::DAQ;

HeaderPDBase::~HeaderPDBase() { }

void HeaderPDBase::readFromDataStream( const LongType *& data )
{
  for ( unsigned int i = 0; i < nHeaderWords(); ++i )
  {
    headerWords()[i] = *(data++);
  }
}
