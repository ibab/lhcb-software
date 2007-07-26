// $Id: FilterMCParticleArrayByDecay.h,v 1.2 2007-07-26 14:22:36 jpalac Exp $
#ifndef FILTERMCPARTICLEARRAYBYDECAY_H 
#define FILTERMCPARTICLEARRAYBYDECAY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleArrayFilter.h"            // Interface


/** @class FilterMCParticleArrayByDecay FilterMCParticleArrayByDecay.h
 *  
 *  
 *  
 *  @author Juan Palacios
 *  @date   2007-07-25
 */

class IMCDecayFinder;

class FilterMCParticleArrayByDecay : public GaudiTool, 
                                     virtual public IMCParticleArrayFilter {
public: 
  /// Standard constructor
  FilterMCParticleArrayByDecay( const std::string& type, 
                                const std::string& name,
                                const IInterface* parent);

  virtual ~FilterMCParticleArrayByDecay( ); ///< Destructor

  StatusCode initialize();
  StatusCode finalize();
  

  /// Filter and put results into new array
  virtual StatusCode filter( const LHCb::MCParticle::ConstVector&, 
                             LHCb::MCParticle::ConstVector& ) const;
  /// Filter and remove elements that do not pass filter from array
  virtual StatusCode filter( LHCb::MCParticle::ConstVector& ) const;


protected:

private:

  void findDecayHeads( const LHCb::MCParticle::ConstVector& in, 
                       LHCb::MCParticle::ConstVector& heads) const;
  void findAllDecay( const LHCb::MCParticle::ConstVector& heads, 
                     LHCb::MCParticle::ConstVector& decay) const;

private:
  std::string m_decayFinderName;
  IMCDecayFinder* m_decayFinder;

};
#endif // FILTERMCPARTICLEARRAYBYDECAY_H
