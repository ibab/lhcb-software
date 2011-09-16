
#ifndef DIRECTIONFITTER_H
#define DIRECTIONFITTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDirectionFit.h"            // Interface

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

using namespace LHCb;

typedef ROOT::Math::SMatrix<double, 10, 10,
                            ROOT::Math::MatRepSym<double,10> > SymMatrix10x10;

/** @class DirectionFitter DirectionFitter.h
 *  
 *  reimplementation of the DirectionFitter for DC06
 *
 *  @author Yuehong Xie
 *  @date   2006-06-07
 */

class DirectionFitter : public GaudiTool, virtual public IDirectionFit {
public: 
  /// Standard constructor
  DirectionFitter( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  StatusCode initialize();

  /// inputs: Vertex corresponding to the assumed production point
  ///         LHCb::Particle itself
  /// output: modified particle

  virtual StatusCode fit( const LHCb::VertexBase&,
                          LHCb::Particle& ) const ;

  virtual StatusCode reFit( LHCb::Particle& ) const {
    Error("reFit is not implemented for DirectionFitter");
    return StatusCode::FAILURE;
  }

  virtual ~DirectionFitter( ); ///< Destructor

private:
      
  LHCb::IParticlePropertySvc* m_ppSvc;

  bool m_applyBMassConstraint;

  int m_maxIter;
  double m_maxDeltaChi2;

};
#endif // DIRECTIONFITTER_H
