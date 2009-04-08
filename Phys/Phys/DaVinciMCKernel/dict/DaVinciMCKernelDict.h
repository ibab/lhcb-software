// $Id: DaVinciMCKernelDict.h,v 1.10 2009-04-08 12:34:13 jpalac Exp $
#ifndef DICT_DAVINCIMCKERNELDICT_H 
#define DICT_DAVINCIMCKERNELDICT_H 1

// Include files

/** @file DaVinciMCKernelDict DaVinciMCKernelDict.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-10-02
 */
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/ICheatedLifetimeFitter.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "Kernel/IFilterMCParticles.h"
#include "Kernel/IHepMC2MC.h"
#include "Kernel/IMCParticleArrayFilter.h"
#include "Kernel/IMC2Collision.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/IParticle2MCWeightedAssociator.h"
#include "Kernel/IP2VVMCPartAngleCalculator.h"
#include "Kernel/IPrintDecayTreeTool.h"
#include "Kernel/IPV2MC.h"
#include "Kernel/MCAssociation.h"
namespace
{
  std::vector<MCAssociation> __stdvector_MCAssoc;
}

#endif // DICT_DAVINCIMCKERNELDICT_H
