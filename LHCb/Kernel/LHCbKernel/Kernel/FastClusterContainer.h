// $Id:$ 
// ============================================================================
#ifndef GAUDIROOT_FASTCLUSTERCONTAINER_H
#define GAUDIROOT_FASTCLUSTERCONTAINER_H
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <stdexcept>
#include <vector>
#include <algorithm>
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectContainerBase.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @class FastClusterContainer FastClusterContainer.h <dir>/FastClusterContainer.h
 *
 * Fast containers can only be used for containees not having a virtual table.
 * Though the container behaves like a vector and allows the access of objects
 * of type VISIBLE, the internal representation is of type INTERNAL.
 * This avoids unnecessary calls to the constructor of the VISIBLE provided of 
 * course that the INTERNAL type maps ideally to a primitive.
 * The size of the INTERNAL and the VISIBLE type must be identical.
 *
 * @author  M.Frank
 * @version 1.0
 *
 */
template <typename VISIBLE, typename INTERNAL> 
class FastClusterContainer : public ObjectContainerBase 
{
private:
  // ==========================================================================
  /// static compile-time assertion 
  BOOST_STATIC_ASSERT( sizeof(VISIBLE) == sizeof(INTERNAL) ) ;
  // ==========================================================================  
private:
  typedef typename std::vector<VISIBLE>       VD;
public:
  typedef typename VD::value_type             value_type;
  typedef typename VD::iterator               iterator;
  typedef typename VD::const_iterator         const_iterator;
  typedef typename VD::reverse_iterator       reverse_iterator;
  typedef typename VD::const_reverse_iterator const_reverse_iterator;
  typedef typename VD::size_type              size_type;
  typedef typename VD::difference_type        difference_type;
  typedef typename VD::pointer                pointer;
  typedef typename VD::const_pointer          const_pointer;
  typedef typename VD::reference              reference;
  typedef typename VD::const_reference        const_reference;
  typedef typename VISIBLE::chan_type         chan_type;
  
private:
  
  typedef std::vector<INTERNAL> InternalData;
  /// Data holder
  InternalData m_data;
  union DataRep {
    std::vector<INTERNAL>* internal;
    std::vector<VISIBLE>*  ext;
  };
  DataRep         m_r;   //! Transient

public:
  /// Standard constructor
  FastClusterContainer() : ObjectContainerBase() 
  {
    m_r.internal = &m_data;
    if ( sizeof(VISIBLE) != sizeof(INTERNAL) )   {
      throw std::runtime_error("Cannot remap data of different size!");
    }
  }

  /// Standard destructor
  virtual ~FastClusterContainer() {}
  /// return iterator for beginning of mutable sequence
  iterator begin()                           { return m_r.ext->begin();       }
  /// return iterator for end of mutable sequence
  iterator end()                             { return m_r.ext->end();         }
  /// return iterator for beginning of nonmutable sequence
  const_iterator begin() const               { return m_r.ext->begin();       }
  /// return iterator for end of nonmutable sequence
  const_iterator end() const                 { return m_r.ext->end();         }
  /// return iterator for beginning of reversed mutable sequence
  reverse_iterator rbegin()                  { return m_r.ext->rbegin();      }
  /// return iterator for beginning of reversed nonmutable sequence
  const_reverse_iterator rbegin() const      { return m_r.ext->rbegin();      }
  /// return iterator for end of reversed mutable sequence
  reverse_iterator rend()                    { return m_r.ext->rend();        }
  /// return iterator for end of reversed nonmutable sequence
  const_reverse_iterator rend() const        { return m_r.ext->rend();        }
  /// subscript mutable sequence
  reference operator[](size_type i)          { return m_r.ext->operator[](i); }
  /// subscript nonmutable sequence
  const_reference operator[](size_type i)const{return m_r.ext->operator[](i); }
  /// subscript mutable sequence with checking
  reference at(size_type i)                  { return m_r.ext->at(i);         }
  /// subscript nonmutable sequence with checking
  const_reference at(size_type i) const      { return m_r.ext->at(i);         }
  /// test if sequence is empty
  bool empty() const                         { return m_data.empty();         }
  /// return first element of mutable sequence
  reference front()                          { return m_r.ext->front();       }
  /// return first element of nonmutable sequence
  const_reference front() const              { return m_r.ext->front();       }
  /// return last element of mutable sequence
  reference back()                           { return m_r.ext->back();        }
  /// return last element of nonmutable sequence
  const_reference back() const               { return m_r.ext->back();        }
  /// insert element at end
  void push_back(const VISIBLE& val)         { return m_r.ext->push_back(val);}
  /// erase element at end
  void pop_back()                            { return m_data.pop_back();      }

  /// Return current length of allocated storage
  size_type size() const                     { return m_data.size();          } 
  /// Return current length of allocated storage
  size_type capacity() const                 { return m_data.capacity();      } 
  /// Clear container. Note: No drestructors may be called !
  void clear()                               { return m_data.clear();         }
  /// Reserve container space
  void reserve(size_type len)                { return m_data.reserve(len);    }
  /// Resize container entries
  void resize(size_type len )                { return m_data.resize(len);     }
  /// Resize container entries
  void resize(size_type len, const VISIBLE& def )
  {  return m_data.resize(len, *(const INTERNAL*)&def);                       }

  /// find method
  template <class findPolicy> 
    const_iterator find(const typename findPolicy::comp_type& value) const{
    std::pair< const_iterator, const_iterator> iterP = std::equal_range(begin(),end(),value,findPolicy());
    return (iterP.first!=iterP.second) ? iterP.first : end();
  }
  
  /// object method, adding a direct access by key using a customised lower_bound (assumes list sorted)
  const value_type * object( const chan_type & id ) const{
    const_iterator it = find<typename VISIBLE::findPolicy>( id );
    if( it == end() ){
      return 0; // not found
    }
    return &(*it);
  }
  
  // ==========================================================================
public: // fake methods form ObjectContainerBase 
  // ==========================================================================
  /// Distance of a given object from the beginning of its container
  virtual long index( const ContainedObject* /* obj */ ) const { return -1 ; }
  /// Pointer to an object of a given distance
  virtual ContainedObject* containedObject( long /* dist */ ) const { return 0 ; }
  /// Number of objects in the container
  virtual size_type numberOfObjects() const { return this->size() ; }
  /// Add an object to the container
  virtual long add    ( ContainedObject* /* obj */ ) { return -1 ; }
  /// Release object from the container
  virtual long remove ( ContainedObject* /* obj */ ) { return -1 ; }
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // GAUDIROOT_FASTCLUSTERCONTAINER_H
// ============================================================================
