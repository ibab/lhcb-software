// $Id$
// ============================================================================
// Include files 
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleFilter.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/ATypes.h"
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/AuxDTFBase.h"
#include "LoKi/BeamLineFunctions.h"
#include "LoKi/Child.h"
#include "LoKi/ChildSelector.h"
#include "LoKi/CompareParticles.h"
#include "LoKi/DecayChain.h"
#include "LoKi/Decays.h"
#include "LoKi/GetPhysDesktop.h"
#include "LoKi/IDecay.h"
#include "LoKi/ImpactParamTool.h"
#include "LoKi/ImpParBase.h"
#include "LoKi/LoKiPhys.h"
#include "LoKi/NBodyCompare.h"
#include "LoKi/ParticleContextCuts.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/Particles0.h"
#include "LoKi/Particles10.h"
#include "LoKi/Particles11.h"
#include "LoKi/Particles12.h"
#include "LoKi/Particles13.h"
#include "LoKi/Particles14.h"
#include "LoKi/Particles15.h"
#include "LoKi/Particles16.h"
#include "LoKi/Particles17.h"
#include "LoKi/Particles18.h"
#include "LoKi/Particles19.h"
#include "LoKi/Particles1.h"
#include "LoKi/Particles20.h"
#include "LoKi/Particles21.h"
#include "LoKi/Particles22.h"
#include "LoKi/Particles23.h"
#include "LoKi/Particles24.h"
#include "LoKi/Particles25.h"
#include "LoKi/Particles26.h"
#include "LoKi/Particles27.h"
#include "LoKi/Particles28.h"
#include "LoKi/Particles29.h"
#include "LoKi/Particles2.h"
#include "LoKi/Particles30.h"
#include "LoKi/Particles31.h"
#include "LoKi/Particles32.h"
#include "LoKi/Particles33.h"
#include "LoKi/Particles34.h"
#include "LoKi/Particles35.h"
#include "LoKi/Particles36.h"
#include "LoKi/Particles37.h"
#include "LoKi/Particles38.h"
#include "LoKi/Particles39.h"
#include "LoKi/Particles3.h"
#include "LoKi/Particles40.h"
#include "LoKi/Particles41.h"
#include "LoKi/Particles42.h"
#include "LoKi/Particles43.h"
#include "LoKi/Particles44.h"
#include "LoKi/Particles45.h"
#include "LoKi/Particles4.h"
#include "LoKi/Particles5.h"
#include "LoKi/Particles6.h"
#include "LoKi/Particles7.h"
#include "LoKi/Particles8.h"
#include "LoKi/Particles9.h"
#include "LoKi/Particles.h"
#include "LoKi/Photons.h"
#include "LoKi/PhysAlgsDicts.h"
#include "LoKi/PhysAlgs.h"
#include "LoKi/PhysCnv.h"
#include "LoKi/PhysDump.h"
#include "LoKi/PhysExtractDicts.h"
#include "LoKi/PhysExtract.h"
#include "LoKi/PhysHelpers.h"
#include "LoKi/PhysKinematics.h"
#include "LoKi/PhysMoniDicts.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/PhysSinks.h"
#include "LoKi/PhysSources.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/PIDOperators.h"
#include "LoKi/PrintDecay.h"
#include "LoKi/Sections.h"
#include "LoKi/SelectVertex.h"
#include "LoKi/TreeFactory.h"
#include "LoKi/VertexCast.h"
#include "LoKi/VertexContextCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/VertexHolder.h"
#include "LoKi/Vertices0.h"
#include "LoKi/Vertices1.h"
#include "LoKi/Vertices2.h"
#include "LoKi/Vertices3.h"
#include "LoKi/Vertices4.h"
#include "LoKi/Vertices5.h"
#include "LoKi/Vertices.h"
// ============================================================================
/** @file
 *
 *  Implementation file for the package Phys/LoKiPhys
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-28 
 */
// ============================================================================
// The END 
// ============================================================================


