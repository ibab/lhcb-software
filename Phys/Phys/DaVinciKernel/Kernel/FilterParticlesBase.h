// $Id: FilterParticlesBase.h,v 1.1 2007-07-20 14:55:13 jpalac Exp $
#ifndef KERNEL_FILTERPARTICLESBASE_H 
#define KERNEL_FILTERPARTICLESBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IFilterParticles.h"            // Interface


/** @class FilterParticlesBase FilterParticlesBase.h Kernel/FilterParticlesBase.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2007-07-20
 */
class FilterParticlesBase : public GaudiTool, 
                            virtual public IFilterParticles {
public: 
  /// Standard constructor
  FilterParticlesBase( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~FilterParticlesBase( ); ///< Destructor

  /// Test if filter is satisfied on ensemble of particles
  virtual bool isSatisfied( const LHCb::Particle::ConstVector& ) const;
  /// Test if filter is satisfied on ensemble of particles
  virtual bool operator()( const LHCb::Particle::ConstVector& ) const;
protected:

private:

};
#endif // KERNEL_FILTERPARTICLESBASE_H
