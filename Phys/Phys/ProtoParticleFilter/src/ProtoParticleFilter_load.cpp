// $Id: ProtoParticleFilter_load.cpp,v 1.2 2006-11-20 15:59:49 jonrob Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( ProtoParticleFilter ) 
{
  DECLARE_TOOL ( ProtoParticleDLLFilter        );
  DECLARE_TOOL ( ChargedProtoParticleDLLFilter );
  DECLARE_TOOL ( ProtoParticleMUONFilter       );
  DECLARE_TOOL ( ProtoParticleCALOFilter       );
  DECLARE_TOOL ( ProtoParticleRICHFilter       );
}
