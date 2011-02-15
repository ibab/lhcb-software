// $Id: $  -*- C++ -*-
#ifndef BBDTVarHandler_H
#define BBDTVarHandler_H
// ============================================================================
// Include files 
// Include files 
#include <vector>
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
#include "Kernel/IDistanceCalculator.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/AParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"
// ============================================================================
/** @class BBDTVarHandler
 *  Utility class used by BBDecTreeTool to obtain variable values.
 * 
 * @author Mike Williams
 * @date 2011-02-15 
 *                    $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 * 
 */
class BBDTVarHandler {

public:
  BBDTVarHandler();
  virtual ~BBDTVarHandler(){}
  const double& operator[](unsigned int index) const {return m_values[index];}
  bool set(const LHCb::Particle* p, const DVAlgorithm *dvalg, 
	   const IDistanceCalculator* dist);
  unsigned int numVars() const {return m_values.size();}
  double ptSum() const {return m_values[0];}
  double mass() const {return m_values[1];}
  double doca() const {return m_values[2];}
  double ipChi2() const {return m_values[3];}
  double massCor() const {return m_values[4];}
  double ptMin() const {return m_values[5];}
  double fdChi2() const {return m_values[6];}

private:
  std::vector<double> m_values; ///< variables
  const LoKi::PhysTypes::Fun m_SUMPT; ///< SUMTREE(PT,'K+'==ABSID,0.0)
  const LoKi::PhysTypes::Fun m_MINPT; ///< MINTREE('K+'==ABSID,PT)
  const LoKi::Cuts::BPVIPCHI2 m_BPVIPCHI2; ///< BPVIPCHI2()
};
// ============================================================================
#endif /* BBDTVarHandler_H */
