// $Id: Particle2MCWithChi2Asct.h,v 1.3 2002-10-02 07:06:26 phicharp Exp $
#ifndef Particle2MCWithChi2ASCT_H 
#define Particle2MCWithChi2ASCT_H 1

// Associators
#include "Particle2MCWeightedAsct.h"


/** @class Particle2MCWithChi2Asct Particle2MCWithChi2Asct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   17/05/2002
 */

typedef Particle2MCWeightedAsct Particle2MCWithChi2Asct;

typedef Particle2MCWithChi2Asct::FromRange      ParticlesToMCChi2;
typedef Particle2MCWithChi2Asct::FromIterator   ParticlesToMCChi2Iterator;
typedef Particle2MCWithChi2Asct::ToRange        MCsFromParticleChi2;
typedef Particle2MCWithChi2Asct::ToIterator     MCsFromParticleChi2Iterator;
typedef Particle2MCWithChi2Asct::Table          Particle2MCWithChi2Table;
  
#endif // Particle2MCWithChi2ASCT_H
