
//-----------------------------------------------------------------------------
/** @file OTHitCleaner.h
 *
 *  Header file for class : Tf::OTHitCleaner
 *
 *  $Id: OTHitCleaner.h,v 1.3 2007-08-28 14:41:23 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-10
 */
//-----------------------------------------------------------------------------

#ifndef TFTOOLS_OTHitCleaner
#define TFTOOLS_OTHitCleaner 1

// STL
#include <iterator>
#include <functional>
#include <map>
//#include <sstream>

// From gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"

// Tf
#include "TfKernel/IOTHitCleaner.h"

namespace Tf
{

  //-----------------------------------------------------------------------------
  /** @class OTHitCleaner OTHitCleaner.h
   *
   *  OTHit cleaner tool
   *
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *
   *  @date   2007-06-01
   *
   *  @todo Is it possible to have a common interface for OT and ST (other?) cleaning tools
   **/
  //-----------------------------------------------------------------------------
  class OTHitCleaner : public GaudiTool,
                       virtual public IOTHitCleaner
  {

  private:

    /// Max cluster size job option
    int m_maxClusterSize;

    /// Max module occupancy
    double m_maxModuleOcc;

  public:

    /// Standard Constructor
    OTHitCleaner( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

    /// Destructor
    ~OTHitCleaner( );

    /// Tool initialization
    StatusCode initialize ( );

  public:

    // Clean the given range of hits
    unsigned int cleanHits( const OTHits::const_iterator begin,
                            const OTHits::const_iterator end,
                            OTHits& output ) const;

  private:

    /// remove clusters
    void removeClusters( const OTHits::const_iterator begin,
                         const OTHits::const_iterator end,
                         OTHits& output ) const;

    /// remove hot modules
    void removeHotModules( const OTHits::const_iterator begin,
                           const OTHits::const_iterator end,
                           OTHits& output ) const;

  }; // OTHitCleaner

} // namespace

#endif


