// $Id: OTCluster2MCHitAsct.h,v 1.2 2002-05-27 11:38:34 cattanem Exp $
#ifndef ASSOCIATORS_OTCluster2MCHitASCT_H 
#define ASSOCIATORS_OTCluster2MCHitASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTCluster.h"
#include "Event/MCHit.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTCluster2MCHitLocation =
                       "Rec/Relations/OTClusters2MCHits";

/** @class OTCluster2MCHitAsct OTCluster2MCHitAsct.h 
 *         Associators/OTCluster2MCHitAsct.h
 *  
 *
 *  @author J. van Tilburg
 *  @date   14/05/2002
 */

class OTCluster2MCHitAsct: public Associator<OTCluster,MCHit>
{
  friend ToolFactory<OTCluster2MCHitAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTCluster,MCHit>               Table;
  typedef OwnType                                   Asct;
  typedef FromRange                                 Clusters;
  typedef FromIterator                              ClustersIterator;
  typedef ToRange                                   MCHits;
  typedef ToIterator                                MCHitsIterator;
    
private:

  /// Standard constructor
  OTCluster2MCHitAsct(const std::string& type,
                      const std::string& name,
                      const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", OTCluster2MCHitLocation);
    setProperty( "AlgorithmType", "OTCluster2MCHitAlg" );
    setProperty( "AlgorithmName", "OTCluster2MCHit" );
  }; 

  /// Destructor
  virtual ~OTCluster2MCHitAsct() {}; ///< Destructor


};
#endif // ASSOCIATORS_OTCluster2MCHitASCT_H

