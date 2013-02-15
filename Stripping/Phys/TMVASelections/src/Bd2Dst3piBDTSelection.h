// $Id: $
#ifndef BD2DST3PIBDTSELECTION_H 
#define BD2DST3PIBDTSELECTION_H 1

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
#include "LoKi/AParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"
#include "LoKi/CoreCuts.h"

class IDistanceCalculator;
class DVAlgorithm;

/** @class BD2DST3PIBDTSelection Bd2Dst3piBDTSelection.h
 *  
 *
 *  @author Conor Fitzpatrick
 *  @date   2013-2-13
 */
class Bd2Dst3piBDTSelection : public extends1<GaudiTool,IParticleFilter>{
  
  friend class ToolFactory<Bd2Dst3piBDTSelection>;

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
  Bd2Dst3piBDTSelection( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~Bd2Dst3piBDTSelection( ); ///< Destructor

private:
  
  /// default constructor is disabled 
  Bd2Dst3piBDTSelection();
  
  /// copy constructor is disabled 
  Bd2Dst3piBDTSelection(const Bd2Dst3piBDTSelection&);
  
  /// assignemet operator is disabled 
  Bd2Dst3piBDTSelection& operator=(const Bd2Dst3piBDTSelection&);
  
  bool set(const LHCb::Particle* p) const;

private:

  const IDistanceCalculator* m_dist;
  IDVAlgorithm *m_dva;
  TMVA::Reader *m_BDTReader;

  unsigned int m_nVars;  
  float *m_values;
  
  double m_cut; // BDT cut value
  std::string m_weightsFile; // weights file

};
#endif // BD2DST3PIBDTSELECTION_H
