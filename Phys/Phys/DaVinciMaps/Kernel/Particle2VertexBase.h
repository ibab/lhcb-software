// $Id: Particle2LHCbIDs.h,v 1.1 2010/08/19 15:09:43 jpalac Exp $
#ifndef PARTICLE2VERTEXBASE_H 
#define PARTICLE2VERTEXBASE_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/SmartRef.h"
#include "Event/VertexBase.h"

#include "Event/Particle.h"

namespace DaVinci{

  namespace Map {
    

    /** @class Particle2VertexBase Particle2VertexBase.h
     *  
     *
     *  @author Juan Palacios
     *  @date   2010-08-25
     */
    class Particle2VertexBase : public DataObject, public  GaudiUtils::VectorMap<SmartRef<LHCb::Particle>, 
                                                                                 SmartRef<LHCb::VertexBase> >
    {

    public:

      typedef GaudiUtils::VectorMap<SmartRef<LHCb::Particle>, 
                                    SmartRef<LHCb::VertexBase> > Map;
      typedef const LHCb::VertexBase* mapped_type;
      typedef const LHCb::Particle* key_type;

    public: 
      /// Standard constructor
      Particle2VertexBase( ); 

      virtual ~Particle2VertexBase( ); ///< Destructor

    public:

      // ========================================================================    
      /// Retrieve pointer to class definition structure     (virtual)
      virtual const CLID& clID() const 
      {
        return Particle2VertexBase::classID() ;
      }
  
      /// Retrieve pointer to class definition structure     (static) 
      static const CLID& classID();
  
      // ========================================================================

    public :

      inline iterator find(const LHCb::Particle* key) const
      {
        return this->find(key);
      }

      inline mapped_type operator()(const LHCb::Particle* key) const
      {
        return this->operator()(key);
      }
  
      inline mapped_type operator[](const LHCb::Particle* key) const
      {
        return this->operator[](key);    
      }

      inline mapped_type at(const LHCb::Particle* key) const
      {
        return this->at(key); 
      }

      inline result_type insert(const LHCb::Particle* key,
                                mapped_type mapped) 
      {
        return this->insert(key, mapped);
      }
  

    };

  } // namespace Map
  
} // namespace DaVinci

  
#endif // PARTICLE2VERTEXBASE_H
