// $Id: GetMCRichSegmentsAlg.h,v 1.1 2005-12-22 16:42:43 jonrob Exp $
#ifndef SENSDET_GetMCRichSegmentsAlg_H
#define SENSDET_GetMCRichSegmentsAlg_H 1

// base class
#include "GetMCRichInfoBase.h"

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

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string   m_richPhotonsLocation;     ///< Name of TES path for MCRichOpticalPPhotons
  std::string   m_richSegmentsLocation;  /// < Name of TES path for MCRichSegments

  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;
  /// overall count hits in each RICH detector
  std::vector< unsigned long int > m_hitTally;

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
