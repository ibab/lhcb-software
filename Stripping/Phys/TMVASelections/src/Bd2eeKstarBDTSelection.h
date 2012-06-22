// $Id: $
#ifndef BD2EEKSTARBDTSELECTION_H 
#define BD2EEKSTARBDTSELECTION_H 1

#include <vector>
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"

#include "Kernel/IParticleFilter.h"

#include "TMVA/Reader.h"
#include "TString.h"

#include "LoKi/LoKiPhys.h"
#include "LoKi/Child.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"

class IDistanceCalculator;
class DVAlgorithm;

/** @class Bd2eeKstarBDTSelection Bd2eeKstarBDTSelection.h
 *  
 *
 *  @author Jibo He
 *  @date   2012-01-19
 */
class Bd2eeKstarBDTSelection : public extends1<GaudiTool,IParticleFilter>{
  
  friend class ToolFactory<Bd2eeKstarBDTSelection>;

public: 
  /** initialize tool */
  virtual StatusCode initialize();
  
  
  /** finalize tool */
  virtual StatusCode finalize();
  
  
  /** 
   *  @see IParticleFilter 
   */
  virtual bool operator()(const LHCb::Particle* p) const ;
  
protected:
  /// Standard constructor
  Bd2eeKstarBDTSelection( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~Bd2eeKstarBDTSelection( ); ///< Destructor

private:
  
  /// default constructor is disabled 
  Bd2eeKstarBDTSelection();
  
  /// copy constructor is disabled 
  Bd2eeKstarBDTSelection(const Bd2eeKstarBDTSelection&);
  
  /// assignemet operator is disabled 
  Bd2eeKstarBDTSelection& operator=(const Bd2eeKstarBDTSelection&);
  
  bool set(const LHCb::Particle* p) const;

  const IDistanceCalculator* m_dist;
  IDVAlgorithm *m_dva;
  TMVA::Reader *m_BDTReader;

  unsigned int m_nVars;  
  float *m_values;
  
  double m_cut; // BDT cut value
  std::string m_weightsFile; // weights file
  
  const LoKi::Cuts::CHI2IP m_BPVIPCHI2;  
  const LoKi::Cuts::VDCHI2 m_BPVVDCHI2;
  const LoKi::Cuts::DIRA m_BPVDIRA;

};
#endif // BD2EEKSTARBDTSELECTION_H
