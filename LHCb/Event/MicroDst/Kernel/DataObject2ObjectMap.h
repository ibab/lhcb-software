// $Id: DataObject2ObjectVector.h,v 1.1 2010/08/19 15:09:43 jpalac Exp $
#ifndef DATAOBJECT2OBJECTMAP_H 
#define DATAOBJECT2OBJECTMAP_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StatusCode.h"
#include "Event/Particle.h"
#include "Kernel/LHCbID.h"

namespace DaVinci{

  namespace Map {
    

    /** @class DataObject2ObjectMap DataObject2ObjectMap.h
     *  
     *
     *  @author Juan Palacios
     *  @date   2010-08-18
     */

    template <class From, class To>
    class DataObject2ObjectMap : public DataObject

    {

    public:

      typedef GaudiUtils::VectorMap<SmartRef<From>, To > Map;
      typedef typename Map::iterator iterator;
      typedef typename Map::const_iterator const_iterator;
      typedef typename Map::reverse_iterator reverse_iterator;
      typedef typename Map::const_reverse_iterator const_reverse_iterator;
      typedef typename Map::mapped_type mapped_type;
      typedef typename Map::result_type result_type;
      typedef typename Map::value_type value_type;
      typedef typename Map::size_type size_type;
      typedef const From* key_type;
  

    public: 
      /// Standard constructor
      DataObject2ObjectMap( )
        :
        DataObject(),
        m_map()
      {
      }

      /// Standard constructor
      DataObject2ObjectMap(const DataObject2ObjectMap& rhs )
        :
        DataObject(),
        m_map(rhs.m_map)
      {
      }


      template <class INPUT>
      DataObject2ObjectMap(INPUT first,
                           INPUT last)
        :
        DataObject(),
        m_map(first, last)
      {
      }
      
        ~DataObject2ObjectMap( ) 
      {
        m_map.clear();
      }
      
    public :

      inline iterator find(key_type key) const
      {
        return m_map.find(key);
      }

      inline const mapped_type& operator()(key_type key) const
      {
        return m_map(key);
      }
  
      inline const mapped_type& operator[](key_type key) const
      {
        return m_map[key];    
      }

      inline const mapped_type& at(key_type key) const
      {
        return m_map.at(key); 
      }

      inline size_type count(const key_type key) const 
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

      inline const mapped_type& value_at(const size_t index) const 
      {
        return m_map.value_at(index);
      }

      virtual StatusCode update() 
      {
        const Map tmp(m_map.begin(), m_map.end());
        m_map = tmp;
        return StatusCode::SUCCESS;
      }
      

    private :
      Map m_map;

    };

  } // namespace Map
  
} // namespace DaVinci

  
#endif // DATAOBJECT2OBJECTMAP_H
