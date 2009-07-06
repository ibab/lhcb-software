
//-----------------------------------------------------------------------------
/** @file ITIsolatedTrackSelector.h
 *
 *  Header file for reconstruction tool : ITIsolatedTrackSelector
 *
 *  CVS Log :-
 *  $Id: ITIsolatedTrackSelector.h,v 1.2 2009-07-06 18:29:28 jonrob Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#ifndef TRACKTOOLS_ITIsolatedTrackSelector_H
#define TRACKTOOLS_ITIsolatedTrackSelector_H

//-----------------------------------------------------------------------------
/** @class ITIsolatedTrackSelector ITIsolatedTrackSelector.h
 *
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiAlg/GaudiHistoTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <string>

// GaudiKernel
#include "GaudiKernel/HashMap.h"

#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

class ISTClusterCollector;



class ITIsolatedTrackSelector : public GaudiHistoTool,
                      virtual public ITrackSelector
{

public:

  /// constructer
  ITIsolatedTrackSelector( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  virtual ~ITIsolatedTrackSelector();

  /// Tool initialization
  virtual StatusCode initialize();

  /** Returns if the given track is selected or not
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  virtual bool accept ( const LHCb::Track& aTrack ) const;



  enum Category  {CSide = 1 , ASide =2, Bottom = 3, Top =4 , Mixed =5 };

private:


 Category ITCategory(const std::vector<LHCb::LHCbID>& ids) const;

 std::vector< ISTClusterCollector* > m_collectors;

 unsigned int m_maxHitNbr, m_minNumITHits;
  
};

#endif // TRACKTOOLS_ITIsolatedTrackSelector_H
