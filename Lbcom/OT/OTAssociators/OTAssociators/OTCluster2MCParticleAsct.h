// $Id: OTCluster2MCParticleAsct.h,v 1.1.1.1 2002-05-26 22:48:41 jvantilb Exp $
#ifndef ASSOCIATORS_OTCluster2MCParticleASCT_H 
#define ASSOCIATORS_OTCluster2MCParticleASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTCluster.h"
#include "Event/MCParticle.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTCluster2MCParticleLocation = 
                   "Rec/Relations/OTCluster2MCParticle";

/** @class OTCluster2MCParticleAsct OTCluster2MCParticleAsct.h 
 *         Associators/OTCluster2MCParticleAsct.h
 *  
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
#endif // ASSOCIATORS_OTCluster2MCParticleASCT_H
