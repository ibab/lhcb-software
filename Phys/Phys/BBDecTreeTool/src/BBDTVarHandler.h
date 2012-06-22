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
#include "Kernel/IDVAlgorithm.h"
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
  BBDTVarHandler(const IDVAlgorithm* dva, const IDistanceCalculator* dist);
  virtual ~BBDTVarHandler(){}
  void setPIDs(const LoKi::PhysTypes::Cut &cut);
  StatusCode initialize(const std::vector<std::string> &vars);
  StatusCode initialize(const std::vector<bool> &use);
  const double& operator[](unsigned int index) const {
    return m_values[m_map[index]];
  }
  bool set(const LHCb::Particle* p);
  unsigned int numVars() const {
    int size = m_values.size(), num = 0;
    for(int i=0; i < size; i++){
      if(m_use[i]) num++;
    }
    return num;
  }
  double ptSum() const {return m_values[0];}
  double mass() const {return m_values[1];}
  double doca() const {return m_values[2];}
  double ipChi2() const {return m_values[3];}
  double massCor() const {return m_values[4];}
  double ptMin() const {return m_values[5];}
  double fdChi2() const {return m_values[6];}
  double pt() const {return m_values[7];}
  double vChi2DofTot() const {return m_values[8];}
  bool cut(const std::map<std::string,std::pair<double,double> > &cuts) const;

private:

  const IDistanceCalculator* m_dist; ///< LoKi::DistanceCalculator
  const LoKi::Vertices::ImpactParamTool m_ipTool;
  const IDVAlgorithm* m_dva; ///< IDVAlgorithm (to get BPV)

  std::vector<double> m_values; ///< variables
  std::vector<bool> m_use; ///< use this variable?
  std::vector<int> m_map; ///< maps client index to internal index

  LoKi::PhysTypes::Fun m_SUMPT; ///< SUMTREE(PT,[...],0.0)
  LoKi::PhysTypes::Fun m_MINPT; ///< MINTREE([...],PT)
  const LoKi::Cuts::CHI2IP   m_CHI2IP; ///< BPVIPCHI2()
  const LoKi::Cuts::CORRM    m_BPVCORRM; 
  const LoKi::Cuts::VDCHI2   m_BPVVDCHI2;

  std::map<std::string,int> m_indices; ///< var name to (internal) index map
};
// ============================================================================
#endif /* BBDTVarHandler_H */
