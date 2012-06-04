// $Id: $
#ifndef MUONVARS_H
#define MUONVARS_H 1
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "LoKi/TrackCuts.h"
#include "LoKi/ParticleContextCuts.h"
#include "Kernel/ITriggerTisTos.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/DVAlgorithm.h"
#include <string>
#include <vector>

using namespace LHCb;
using namespace LoKi;
using namespace LoKi::GenTypes;
using namespace LoKi::Types;
using namespace LoKi::Cuts;
using namespace LoKi::Particles;
using namespace LoKi::Tracks;
using namespace LoKi::Vertices;

using namespace std;


void fill_muon_vars(Tuples::Tuple tuple_stdpfjets ,LHCb::Particle::Range stdloosemuons, ITriggerTisTos * m_L0TriggerTisTosTool, ITriggerTisTos * m_TriggerTisTosTool)  ;

#endif /*MUONVARS_H*/ 
