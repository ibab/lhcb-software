
//-----------------------------------------------------------------------------
/** @file STHitCleaner.h
 *
 *  Header file for class : Tf::STHitCleaner
 *
 *  $Id: STHitCleaner.h,v 1.2 2007-08-28 14:41:23 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-10
 */
//-----------------------------------------------------------------------------

#ifndef TFTOOLS_STHitCleaner
#define TFTOOLS_STHitCleaner 1

// STL
#include <iterator>
#include <functional>
#include <map>

// From gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"

// Tf
#include "TfKernel/ISTHitCleaner.h"

namespace Tf
{

  //-----------------------------------------------------------------------------
  /** @class STHitCleaner STHitCleaner.h
   *
   *  STHit cleaner tool
   *
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *
   *  @date   2007-06-01
   *
   *  @todo Is it possible to have a common interface for OT and ST (other?) cleaning tools
   **/
  //-----------------------------------------------------------------------------
  class STHitCleaner : public GaudiTool,
                       virtual public ISTHitCleaner
  {

  private:

    /// Max beetle occupancy job option
    unsigned int m_maxBeetleOcc;

  public:

    /// Standard Constructor
    STHitCleaner( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

    /// Destructor
    ~STHitCleaner( );

    /// Tool initialization
    StatusCode initialize ( );

  public:

    // Clean the given range of hits
    unsigned int cleanHits( const STHits::const_iterator begin,
                            const STHits::const_iterator end,
                            STHits& output ) const;

  private:

    // Clean out hot beetles
    void removeHotBeetles( const STHits::const_iterator begin,
                           const STHits::const_iterator end,
                           STHits& output ) const;

  }; // STHitCleaner

} // namespace

#endif

