// $Id: GetMCRichSegmentsAlg.h,v 1.4 2009-07-17 13:46:13 jonrob Exp $
#ifndef SENSDET_GetMCRichSegmentsAlg_H
#define SENSDET_GetMCRichSegmentsAlg_H 1

// base class
#include "GaussRICH/GetMCRichInfoBase.h"

// rich kernel
#include "RichKernel/RichMap.h"

/** @class GetMCRichSegmentsAlg GetMCRichSegmentsAlg.h
 *
 *  Algorithm to create MCRichSegment objects from Gauss G4 information
 *
 *  @author Sajan EASO
 *  @date   2005-12-06
 */

class GetMCRichSegmentsAlg : public GetMCRichInfoBase
{

public:

  /// Standard constructor
  GetMCRichSegmentsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCRichSegmentsAlg( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string m_richPhotonsLocation; ///< Name of TES path for MCRichOpticalPhotons
  std::string m_richHitsLocation;    ///< Name of TES path for MCRichOpticalHits

  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;

  /// map for photon counting
  typedef Rich::Map< const Rich::RadiatorType, unsigned long int > RadMap;
  /// count overall number of photons in each RICH radiator medium
  RadMap m_hitTally;

private:
  // sorting functions

  /// Pair for position and direction at that point
  typedef std::pair< Gaudi::XYZVector, Gaudi::XYZPoint > MomentumAtZ;
  /// Momentum sorting 
  struct SortMomentumAtZ
  {
    bool operator() ( const MomentumAtZ & p1, const MomentumAtZ & p2 ) const
    {
      return ( p1.second.z() < p2.second.z() );
    }
  };
  /// Sorted set for position and momentum data
  typedef std::set< MomentumAtZ, SortMomentumAtZ > MomentaAtZ;

};

#endif // SENSDET_GetMCRichSegmentsAlg_H
