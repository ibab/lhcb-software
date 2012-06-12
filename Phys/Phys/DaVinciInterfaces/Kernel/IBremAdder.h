// $Id: IBremAdder.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_IBREMADDER_H 
#define KERNEL_IBREMADDER_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from LHCb
#include "Event/Particle.h"
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloMomentum.h" 

/** @class IBremAdder IBremAdder.h Kernel/IBremAdder.h
 *  
 * Interface for BremStrahlung correction to electron particle
 *
 *  @author Olivier Deschamps
 *  @date   2006-10-25
 */
class GAUDI_API IBremAdder : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IBremAdder, 4, 0);

  // Add Brem
  virtual  bool addBrem( LHCb::Particle* particle , bool force=false) = 0 ;
  // Remove Brem
  virtual  bool removeBrem( LHCb::Particle* particle , bool force=false) = 0 ;
  // Add Brem on particle pair (removing overlap)
  virtual  bool addBrem2Pair( LHCb::Particle* p1, LHCb::Particle* p2 , bool force=false) = 0 ;

  // helper methods
  virtual  bool hasBrem(const LHCb::Particle* particle)=0;
  virtual const LHCb::CaloMomentum bremMomentum(const LHCb::Particle* particle,std::string flag="")=0;
  virtual const std::pair<LHCb::CaloMomentum,LHCb::CaloMomentum> bremMomenta(const LHCb::Particle* p1,
                                                                             const LHCb::Particle*p2,
                                                                             std::string flag="")=0;
protected:
  virtual ~IBremAdder() ;               // virtual and protected destructors
private:
};
#endif // KERNEL_IBREMADDER_H
