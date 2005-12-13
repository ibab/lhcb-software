
//-----------------------------------------------------------------------------
/** @file RichSmartIDSorter.h
 *
 *  Header file for RichSmartID utility class : RichSmartIDSorter
 *
 *  $Id: RichSmartIDSorter.h,v 1.9 2005-12-13 17:23:28 papanest Exp $
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

//-----------------------------------------------------------------------------
/** @class RichSmartIDSorter RichSmartIDSorter.h RichKernel/RichSmartIDSorter.h
 *
 *  Utility class to sort RichSmartIDs according to various criteria
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-10
 */
//-----------------------------------------------------------------------------

class RichSmartIDSorter 
{

private: // utility classes

  /// Functor to sort RichSmartIDs by Rich then panel numbers
  class SortByRegion
    : std::binary_function< const LHCb::RichSmartID, const LHCb::RichSmartID, bool >
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
    inline bool operator() ( const LHCb::RichSmartID p1, const LHCb::RichSmartID p2 ) const
    {
      // Use internal bit packing to sort
      return ( p1.dataBitsOnly().key() < p2.dataBitsOnly().key() );
      // Use explicit field comparision. Slower but assumes nothing about bit packing
      // Fixed so that PD col is lesser to PD Row, for compatibility with RTTC data
      // This is opposite to that with the above fast sorting
      //return ( 100000000*p1.rich() + 10000000*p1.panel() +
      //         1000000*p1.pdRow() + 10000*p1.pdCol() +
      //         100*p1.pixelCol() + p1.pixelRow()  <
      //         100000000*p2.rich() + 10000000*p2.panel() +
      //         1000000*p2.pdRow() + 10000*p2.pdCol() +
      //         100*p2.pixelCol() + p2.pixelRow() );
    }

  };

public:

  /// Constructor
  RichSmartIDSorter() {};

  /// Destructor
  ~RichSmartIDSorter() {};

  /// Sort the list by detector region
  inline void sortByRegion( LHCb::RichSmartID::Vector& smartIDs ) const
  {
    std::sort( smartIDs.begin(), smartIDs.end(), RichSmartIDSorter::SortByRegion() );
  }

};

#endif // RICHKERNEL_RICHSMARTIDSORTER_H
