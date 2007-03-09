
//=========================================================================
/** @file ProtoParticleFilter_load.cpp
 *
 *  Declaration of objects in the component library ProtoParticleFilter
 *
 *  CVS Log :-
 *  $Id: ProtoParticleFilter_load.cpp,v 1.3 2007-03-09 18:12:26 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */
//=========================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( ProtoParticleFilter ) 
{
  DECLARE_TOOL ( ProtoParticleDLLFilter        );
  DECLARE_TOOL ( ChargedProtoParticleDLLFilter );
  DECLARE_TOOL ( ProtoParticleMUONFilter       );
  DECLARE_TOOL ( ProtoParticleCALOFilter       );
  DECLARE_TOOL ( ProtoParticleRICHFilter       );
}
