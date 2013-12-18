// $Id: $
#ifndef BC2JPSIHBDTSELECTION_H 
#define BC2JPSIHBDTSELECTION_H 1

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

/** @class Bc2JpsiHBDTSelection Bc2JpsiHBDTSelection.h
 *  
 *
 *  @author Jibo He
 *  @date   2013-11-01
 */
class Bc2JpsiHBDTSelection : public extends1<GaudiTool,IParticleFilter>{
  
  friend class ToolFactory<Bc2JpsiHBDTSelection>;

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
  Bc2JpsiHBDTSelection( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);
  
  virtual ~Bc2JpsiHBDTSelection( ); ///< Destructor

private:
  
  /// default constructor is disabled 
  Bc2JpsiHBDTSelection();
  
  /// copy constructor is disabled 
  Bc2JpsiHBDTSelection(const Bc2JpsiHBDTSelection&);
  
  /// assignemet operator is disabled 
  Bc2JpsiHBDTSelection& operator=(const Bc2JpsiHBDTSelection&);
  
  bool set(const LHCb::Particle* p) const;

  const IDistanceCalculator* m_dist;
  IDVAlgorithm *m_dva;
  TMVA::Reader *m_BDTReader;

  unsigned int m_nVars;  
  float *m_values;
  
  double m_cut; // BDT cut value
  std::string m_weightsFile; // weights file
  
  const LoKi::Cuts::CHI2IP m_BPVIPCHI2;  

};
#endif // BD2EEKSTARBDTSELECTION_H
