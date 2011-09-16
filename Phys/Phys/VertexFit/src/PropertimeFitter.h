
#ifndef PROPERTIMEFITTER_H 
#define PROPERTIMEFITTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ILifetimeFitter.h"            // Interface

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

using namespace LHCb;

typedef ROOT::Math::SMatrix<double, 10, 10,
                            ROOT::Math::MatRepSym<double,10> > SymMatrix10x10;

/** @class PropertimeFitter PropertimeFitter.h
 *  
 *  reimplementation of the LifetimeFitter for DC06
 *
 *  @author Yuehong Xie
 *  @date   2006-06-07
 */

class PropertimeFitter : public GaudiTool, virtual public ILifetimeFitter {
public: 
  /// Standard constructor
  PropertimeFitter( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  StatusCode initialize();

  /// Method to get propertime
  /// inputs: Vertex corresponding to the assumed production point
  ///         LHCb::Particle itself
  /// output: resulting propertime and error, chisq.

  virtual StatusCode fit( const LHCb::VertexBase&, const LHCb::Particle&,
                          double& propertime, double& error,
                          double& chisq) const ;


  virtual ~PropertimeFitter( ); ///< Destructor
 
private:
      
  LHCb::IParticlePropertySvc* m_ppSvc;

  bool m_applyBMassConstraint;

  int m_maxIter;
  double m_maxDeltaChi2;

};
#endif // PROPERTIMEFITTER_H
