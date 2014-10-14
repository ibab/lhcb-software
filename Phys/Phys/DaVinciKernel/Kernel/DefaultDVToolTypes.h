#ifndef KERNEL_DEFAULTTOOLTYPES_H 
#define KERNEL_DEFAULTTOOLTYPES_H 1

#include <string>

namespace DaVinci
{
  namespace DefaultTools
  {
    static const std::string Distance         = "LoKi::DistanceCalculator:PUBLIC";
    static const std::string VertexFitter     = "LoKi::VertexFitter:PUBLIC";
    static const std::string ParticleCombiner = "LoKi::VertexFitter:PUBLIC";
    static const std::string PVRelator = "GenericParticle2PVRelator<_p2PVWithIPChi2, OfflineDistanceCalculatorName>/P2PVWithIPChi2:PUBLIC";
    static const std::string ParticleReFitter = "LoKi::VertexFitter:PUBLIC";
  }
}

#endif // KERNEL_DEFAULTTOOLTYPES_H
