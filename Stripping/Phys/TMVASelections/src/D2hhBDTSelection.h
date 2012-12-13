// $Id: $
#ifndef D2HHBDTSELECTION_H 
#define D2HHBDTSELECTION_H 1

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
#include "LoKi/CoreCuts.h"

class IDistanceCalculator;
class DVAlgorithm;

/** @class D2HHBDTSelection D2hhBDTSelection.h
 *  
 *
 *  @author Stefano Perazzini
 *  @date   2012-11-11
 */
class D2hhBDTSelection : public extends1<GaudiTool,IParticleFilter>{
  
  friend class ToolFactory<D2hhBDTSelection>;

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
  D2hhBDTSelection( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~D2hhBDTSelection( ); ///< Destructor

private:
  
  /// default constructor is disabled 
  D2hhBDTSelection();
  
  /// copy constructor is disabled 
  D2hhBDTSelection(const D2hhBDTSelection&);
  
  /// assignemet operator is disabled 
  D2hhBDTSelection& operator=(const D2hhBDTSelection&);
  
  bool set(const LHCb::Particle* p) const;

private:

  const IDistanceCalculator* m_dist;
  IDVAlgorithm *m_dva;
  TMVA::Reader *m_BDTReader;

  unsigned int m_nVars;  
  float *m_values;
  
  double m_cut; // BDT cut value
  std::string m_weightsFile; // weights file

  const LoKi::Cuts::MIPDV     m_MIPDV;
  const LoKi::Cuts::BPVIP     m_BPVIP;  
  const LoKi::Cuts::VD        m_BPVVD;

};
#endif // D2HHBDTSELECTION_H
