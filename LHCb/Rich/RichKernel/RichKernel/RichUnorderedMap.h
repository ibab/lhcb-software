
//--------------------------------------------------------------------------------
/** @file RichUnorderedMap.h
 *
 *  Header file for utility has map for the RICH : Rich::Map
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHUNORDEREDMAP_H
#define RICHKERNEL_RICHUNORDEREDMAP_H 1

// STL
#include <unordered_map>
#include <ostream>

namespace Rich
{

  //--------------------------------------------------------------------------------
  /** @class UnorderedMap RichKernel/RichUnorderedMap.h
   *
   *  A utility class providing a standard std::unordered_map like object.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2015-09-22
   */
  //--------------------------------------------------------------------------------

  template < typename KEY, 
             typename VALUE, 
             typename HASH  = std::hash<KEY>,
             typename EQUAL = std::equal_to<KEY>,
             typename ALLOC = std::allocator<std::pair<const KEY,VALUE> > 
             >
  class UnorderedMap : public std::unordered_map < KEY , VALUE , HASH , EQUAL , ALLOC >
  {
  public:
    /// Operator overloading for ostream
    friend inline std::ostream& operator << ( std::ostream& str ,
                                              const UnorderedMap<KEY,VALUE,HASH,EQUAL,ALLOC> & m )
    {
      str << "[";
      for ( const auto& i : m ) { str << " (" << i.first << "," << i.second << ")"; }
      return str << " ]";
    }
  };

}

#endif // RICHKERNEL_RICHUNORDEREDMAP_H
