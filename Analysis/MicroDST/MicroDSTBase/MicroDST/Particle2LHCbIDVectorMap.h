// $Id: Particle2LHCbIDVectorMap.h,v 1.1 2010-08-19 15:09:43 jpalac Exp $
#ifndef PARTICLE2LHCBIDVECTORMAP_H 
#define PARTICLE2LHCBIDVECTORMAP_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/SmartRef.h"
#include "Kernel/LHCbID.h"

namespace LHCb {
  class Particle;
  class LHCbID;
}


/** @class Particle2LHCbIDVectorMap Particle2LHCbIDVectorMap.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2010-08-18
 */
class Particle2LHCbIDVectorMap : public DataObject,
                                 
                                 public GaudiUtils::VectorMap<SmartRef<LHCb::Particle>, 
                                                              std::vector<LHCb::LHCbID> >
{

public:
  typedef  GaudiUtils::VectorMap<SmartRef<LHCb::Particle>, std::vector<LHCb::LHCbID> > _Base;

public: 
  /// Standard constructor
  Particle2LHCbIDVectorMap( ); 

  virtual ~Particle2LHCbIDVectorMap( ); ///< Destructor

public:

  // ========================================================================    
  /// Retrieve pointer to class definition structure     (virtual)
  virtual const CLID& clID() const 
  {
    return Particle2LHCbIDVectorMap::classID() ;
  }
  
  /// Retrieve pointer to class definition structure     (static) 
  static const CLID& classID();
  
  // ========================================================================

};
#endif // PARTICLE2LHCBIDVECTORMAP_H
