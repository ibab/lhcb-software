// $Id: OTCluster2MCParticleAsct.h,v 1.3 2002-08-07 15:55:06 jvantilb Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCPARTICLEASCT_H
#define OTASSOCIATORS_OTCLUSTER2MCPARTICLEASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTCluster.h"
#include "Event/MCParticle.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTCluster2MCParticleLocation = 
                   "Rec/Relations/OTClusters2MCParticles";

/** @class OTCluster2MCParticleAsct OTCluster2MCParticleAsct.h \
 *         "OTAssociators/OTCluster2MCParticleAsct.h"
 *  
 *  Associator for the OTClusters with the corresponding MCParticles. 
 *  Makes use of the associator with the MCHits. By default OTClusters 
 *  coming from spillover are not associated.
 *
 *  @author J. van Tilburg
 *  @date   14/05/2002
 */


class OTCluster;
class MCParticle;

class OTCluster2MCParticleAsct: public Associator<OTCluster,MCParticle>
{
  friend ToolFactory<OTCluster2MCParticleAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTCluster,MCParticle>          Table;
  typedef OwnType                                   Asct;
  typedef FromRange                                 Clusters;
  typedef FromIterator                              ClustersIterator;
  typedef ToRange                                   MCParticles;
  typedef ToIterator                                MCParticlesIterator;
    
private:

  /// Standard constructor
  OTCluster2MCParticleAsct(const std::string& type,
                           const std::string& name,
                           const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", OTCluster2MCParticleLocation );
    setProperty( "AlgorithmType", "OTCluster2MCParticleAlg" );
    setProperty( "AlgorithmName", "OTCluster2MCParticle" );
  }; 

  /// Destructor
  virtual ~OTCluster2MCParticleAsct() {}; ///< Destructor


};
#endif // OTASSOCIATORS_OTCLUSTER2MCPARTICLEASCT_H
