// $Id: FilterMCParticlesBase.h,v 1.1 2007-07-24 12:09:22 jpalac Exp $
#ifndef KERNEL_FILTERMCPARTICLESBASE_H 
#define KERNEL_FILTERMCPARTICLESBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IFilterMCParticles.h"            // Interface


/** @class FilterMCParticlesBase FilterMCParticlesBase.h Kernel/FilterMCParticlesBase.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2007-07-20
 */
class FilterMCParticlesBase : public GaudiTool, 
                              virtual public IFilterMCParticles {
public: 
  /// Standard constructor
  FilterMCParticlesBase( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~FilterMCParticlesBase( ); ///< Destructor

  /// Test if filter is satisfied on ensemble of MCParticles
  virtual bool isSatisfied( const LHCb::MCParticle::ConstVector& ) const;
  /// Test if filter is satisfied on ensemble of MCParticles
  virtual bool operator()( const LHCb::MCParticle::ConstVector& ) const;
protected:

private:

};
#endif // KERNEL_FILTERMCPARTICLESBASE_H
