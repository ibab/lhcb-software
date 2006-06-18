// $Id: ProtoParticleFilter_load.cpp,v 1.1.1.1 2006-06-18 14:23:44 jonrob Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( ProtoParticleFilter ) 
{
  DECLARE_TOOL ( ProtoParticleDLLFilter        );
  DECLARE_TOOL ( ChargedProtoParticleDLLFilter );
  DECLARE_TOOL ( ProtoParticleMUONFilter       );
  DECLARE_TOOL ( ProtoParticleCALOFilter       );
}
