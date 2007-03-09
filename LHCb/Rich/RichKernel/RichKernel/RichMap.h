
//--------------------------------------------------------------------------------
/** @file RichMap.h
 *
 *  Header file for utility has map for the RICH : Rich::Map
 *
 *  CVS Log :-
 *  $Id: RichMap.h,v 1.6 2007-03-09 17:58:13 jonrob Exp $
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

  template < class KEY, class VALUE >
  class Map : public std::map < KEY , VALUE >
  {
  public:
    /// Operator overloading for ostream
    friend inline std::ostream& operator << ( std::ostream& str ,
                                              const Map<KEY,VALUE> & map )
    {
      str << "[";
      for ( typename Map<KEY,VALUE>::const_iterator iM = map.begin(); iM != map.end(); ++iM )
      {
        str << " (" << iM->first << "," << iM->second << ")";
      }
      return str << " ]";
    }
  };

}

#endif // RICHKERNEL_RICHMAP_H
