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
    class Particle2VertexBase : public DataObject 
    {

    public:
      typedef  GaudiUtils::VectorMap<SmartRef<LHCb::Particle>, 
                                     SmartRef<LHCb::VertexBase> > Map;
      typedef Map::iterator iterator;
      typedef Map::const_iterator const_iterator;
      typedef Map::reverse_iterator reverse_iterator;
      typedef Map::const_reverse_iterator const_reverse_iterator;
      //      typedef Map::mapped_type mapped_type;
      typedef const LHCb::VertexBase* mapped_type;
      typedef Map::result_type result_type;
      typedef Map::value_type value_type;
      typedef Map::size_type size_type;
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

      inline iterator find(key_type key) const
      {
        return m_map.find(key);
      }

      inline mapped_type operator()(key_type key) const
      {
        return m_map(key);
      }
  
      inline mapped_type operator[](key_type key) const
      {
        return m_map[key];    
      }

      inline mapped_type at(key_type key) const
      {
        return m_map.at(key); 
      }

      inline size_type count(key_type key) const 
      {
        return m_map.count(key);
      }

      inline result_type insert(key_type key,
                                mapped_type mapped) 
      {
        return m_map.insert(key, mapped);
      }
  
      inline iterator begin  () const 
      {
        return m_map.begin  () ;
      }
  
      inline iterator end() const 
      {
        return m_map.end    () ;
      }

      inline reverse_iterator rbegin  () const
      {
        
        return m_map.rbegin  () ;
    
      }

      inline reverse_iterator rend() const
      { 
        return m_map.rend    () ; 
      }
  
      inline size_type size() const
      {
        return m_map.size();
      }
  
      inline bool empty() const 
      {
        return m_map.empty();
      }
  
      inline key_type key_at(const size_t index) const 
      {
        return m_map.key_at(index);
      }

      inline mapped_type value_at(const size_t index) const 
      {
        return m_map.value_at(index);
      }

    private :
      Map m_map;

    };

  } // namespace Map
  
} // namespace DaVinci

  
#endif // PARTICLE2VERTEXBASE_H
