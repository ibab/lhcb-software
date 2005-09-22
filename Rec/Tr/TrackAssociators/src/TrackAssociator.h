// $Id: TrackAssociator.h,v 1.1.1.1 2005-09-22 08:27:06 erodrigu Exp $
#ifndef TRACKASSOCIATORS_TRACKASSOCIATOR_H 
#define TRACKASSOCIATORS_TRACKASSOCIATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from XxxAssociators
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"
#include "ITAssociators/ITCluster2MCParticleAsct.h"
#include "OTAssociators/OTTime2MCParticleAsct.h"

// Forward declarations
class MCParticle;
class ITCluster;
class OTTime;
class VeloCluster;

/** @class TrackAssociator TrackAssociator.h
 *
 *  This algorithm computes the relation between a Track and a MCParticle.
 *  The requirement is a match of both the Velo and the Seed part of the 
 *  Track. If there are not enough coordinates, the match is assumed so that
 *  a Velo only or a Seed only are matched properly.
 *  The required fraction of hits is a jobOption 'FractionOK', default 0.70
 *  Another option is 'MinimalZ' which allows to ignore IT/OT hits in TT1.
 *
 *  Adapted to the new track event model.
 *  @author Edwin Bos
 *  @date   2005-09-12
 *
 *  @author Olivier Callot
 *  @date   2002-07-01
 */

class TrackAssociator : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackAssociator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackAssociator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  typedef ITCluster2MCParticleAsct::IAsct    ITClusAsct ;
  typedef OTTime2MCParticleAsct::IAsct       OTTimAsct ;
  typedef VeloCluster2MCParticleAsct::IAsct  VeloClusAsct ;

protected:

// Adjust the counters for this particle, create one if needed 
  void countMCPart( const MCParticle* part, 
                    double incVelo, 
                    double incTT1,
                    double incSeed ) {
    bool found = false;
    for ( unsigned int jj = 0 ; m_parts.size() > jj ; jj++  ) {
      if ( m_parts[jj] == part ) {
        m_nVelo[jj] += incVelo;
        m_nTT1[jj]  += incTT1 ;
        m_nSeed[jj] += incSeed;
        found = true;
        break;
      }
    }
    if ( !found ) {
      m_parts.push_back( part );
      m_nVelo.push_back( incVelo );
      m_nTT1.push_back(  incTT1  );
      m_nSeed.push_back( incSeed );
    }
  }

private:

  std::string m_inputContainer;   ///< Name of the track container
  std::string m_outputTable;      ///< Name of the association table
  double      m_minimalZ;         ///< minimal Z for IT/OT, to ignore TT1
  double      m_fractionOK;       ///< minimal good matching fraction

  VeloClusAsct* m_veloClusToMCP;
  ITClusAsct*   m_itClusToMCP;
  OTTimAsct*    m_otTimToMCP;

  std::vector<const MCParticle*> m_parts;
  std::vector<double> m_nVelo;
  std::vector<double> m_nTT1;
  std::vector<double> m_nSeed;

  double m_nTotVelo;
  double m_nTotTT1;
  double m_nTotSeed;

};
#endif // TRACKASSOCIATORS_TRACKASSOCIATOR_H
