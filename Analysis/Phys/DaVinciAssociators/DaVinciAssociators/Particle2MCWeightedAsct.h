// $Id: Particle2MCWeightedAsct.h,v 1.1 2002-10-02 07:06:26 phicharp Exp $
#ifndef Particle2MCWeightedASCT_H 
#define Particle2MCWeightedASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Event
#include "Event/MCParticle.h"
#include "Event/Particle.h"
// Associators
#include "Relations/AssociatorWeighted.h"

static const std::string& 
Particle2MCWithChi2AsctLocation = "Phys/Relations/Particle2MCWithChi2";

/** @class Particle2MCWeightedAsct Particle2MCWeightedAsct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   17/05/2002
 */
class Particle2MCWeightedAsct : 
  public AssociatorWeighted<Particle,MCParticle,double>
{
public:
  // Define data types
  typedef RelationWeighted1D<Particle,MCParticle,double>       Table;
  typedef OwnType                               Asct;

  /// Standard constructor
  Particle2MCWeightedAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : Asct( type, name, parent) {

    setProperty( "Location", Particle2MCWithChi2AsctLocation );
    setProperty( "AlgorithmType", "Particle2MCWithChi2" );
    setProperty( "AlgorithmName", "Particle2MCWithChi2" );
    
  }; 

  virtual ~Particle2MCWeightedAsct( ){}; ///< Destructor

protected:

private:

};

typedef Particle2MCWeightedAsct::FromRange      ParticlesToMCLinks;
typedef Particle2MCWeightedAsct::FromIterator   ParticlesToMCLinksIterator;
typedef Particle2MCWeightedAsct::ToRange        MCsFromParticleLinks;
typedef Particle2MCWeightedAsct::ToIterator     MCsFromParticleLinksIterator;
typedef Particle2MCWeightedAsct::Table          Particle2MCWeightedTable;
  
#endif // Particle2MCWeightedASCT_H
