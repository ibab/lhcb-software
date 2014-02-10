
//--------------------------------------------------------------------------------
/** @file RichMap.h
 *
 *  Header file for utility has map for the RICH : Rich::Map
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHMAP_H
#define RICHKERNEL_RICHMAP_H 1

// STL
#include <map>
#include <ostream>

namespace Rich
{

  //--------------------------------------------------------------------------------
  /** @class Map RichMap.h RichKernel/RichMap.h
   *
   *  A utility class providing a standard std::map like object.
   *
   *  Provides a convenient single point of definition for optimisation studies.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-01-11
   */
  //--------------------------------------------------------------------------------

  template < typename KEY, 
             typename VALUE, 
             typename COMPARE = std::less<KEY>,
             typename ALLOC   = std::allocator<std::pair<const KEY,VALUE> > >
  class Map : public std::map < KEY , VALUE , COMPARE , ALLOC >
  {
  public:
    /// Operator overloading for ostream
    friend inline std::ostream& operator << ( std::ostream& str ,
                                              const Map<KEY,VALUE,COMPARE,ALLOC> & map )
    {
      str << "[";
      for ( typename Map<KEY,VALUE,COMPARE,ALLOC>::const_iterator iM = map.begin(); 
            iM != map.end(); ++iM )
      {
        str << " (" << iM->first << "," << iM->second << ")";
      }
      return str << " ]";
    }
  };

}

#endif // RICHKERNEL_RICHMAP_H
