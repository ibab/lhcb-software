// $Id: TrackAssociator.h,v 1.3 2005-10-17 16:30:21 ebos Exp $
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

  typedef ITCluster2MCParticleAsct::IAsct    ITClusAsct ;
  typedef OTTime2MCParticleAsct::IAsct       OTTimAsct ;
  typedef VeloCluster2MCParticleAsct::IAsct  VeloClusAsct ;

private:

  void countMCPart( const MCParticle* part, 
                    double incVelo, 
                    double incTT1,
                    double incSeed );
    
  std::string m_inputContainer;   ///< Name of the track container
  std::string m_outputTable;      ///< Name of the association table
  double      m_minimalZ;         ///< minimal Z for IT/OT, to ignore TT1
  double      m_fractionOK;       ///< minimal good matching fraction
  bool        m_makeLinker;       ///< Option to construct the linker relations



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
