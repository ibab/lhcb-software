
//-----------------------------------------------------------------------------
/** @file RichSmartIDSorter.h
 *
 *  Header file for RichSmartID utility class : RichSmartIDSorter
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-10
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHSMARTIDSORTER_H
#define RICHKERNEL_RICHSMARTIDSORTER_H 1

// STL
#include <functional>

// Kernel
#include "Kernel/RichSmartID.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class SmartIDSorter RichSmartIDSorter.h RichKernel/RichSmartIDSorter.h
   *
   *  Utility class to sort RichSmartIDs according to various criteria
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-01-10
   */
  //-----------------------------------------------------------------------------

  class SmartIDSorter
  {

  private: // utility classes

    /// Functor to sort RichSmartIDs by Rich then panel numbers
    class SortByRegion
      : std::binary_function< const LHCb::RichSmartID&, const LHCb::RichSmartID&, bool >
    {

    public:

      /** Sort operator for the RichSmartIDs
       *
       *  Sorts into order of RICH/Panel etc.
       *
       *  @param p1 First RichSmartID
       *  @param p2 Second RichSmartID
       *
       *  @return bool indicating if p1 should be listed before p2
       */
      inline bool operator() ( const LHCb::RichSmartID& p1, 
                               const LHCb::RichSmartID& p2 ) const
      {
        // Use internal bit packing to sort
        return ( p1.dataBitsOnly().key() < p2.dataBitsOnly().key() );
        // use full class, including data set bits
        //return ( p1.key() < p2.key() );
      }

    };

  public:

    /// Constructor
    SmartIDSorter() { }

    /// Destructor
    ~SmartIDSorter() { }

    /// Sort the list by detector region
    inline static void sortByRegion( LHCb::RichSmartID::Vector& smartIDs )
    {
      std::sort( smartIDs.begin(), smartIDs.end(), SmartIDSorter::SortByRegion() );
    }

  };

}

#endif // RICHKERNEL_RICHSMARTIDSORTER_H
