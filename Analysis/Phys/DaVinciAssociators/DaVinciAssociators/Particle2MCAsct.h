// $Id: Particle2MCAsct.h,v 1.3 2003-04-17 09:58:25 phicharp Exp $
#ifndef ASSOCIATORS_Particle2MCASCT_H 
#define ASSOCIATORS_Particle2MCASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Event
#include "Event/MCParticle.h"
#include "Event/Particle.h"
// Associators
#include "Relations/Associator.h"

static const std::string& 
Particle2MCAsctLocation = "Phys/Relations/Particle2MC";

/** @class Particle2MCAsct Particle2MCAsct.h Associators/Particle2MCAsct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   05/04/2002
 */

class Particle2MCAsct : public Associator<Particle,MCParticle>
{
  friend ToolFactory<Particle2MCAsct>;
  
public:
  // Define data types
  typedef Relation1D<Particle,MCParticle>       Table;
  typedef OwnType                               Asct;
  
protected:
  /// Standard constructor
  Particle2MCAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : Asct( type, name, parent) {

    setProperty( "Location", Particle2MCAsctLocation );
    setProperty( "AlgorithmType", "Particle2MCChi2" );
    setProperty( "AlgorithmName", "Particle2MCChi2" );
    
  }; 

  virtual ~Particle2MCAsct() { }; ///< Destructor

private:
    bool hasTable;
};

typedef Particle2MCAsct::FromRange                 ParticlesToMC;
typedef Particle2MCAsct::FromIterator              ParticlesToMCIterator;
typedef Particle2MCAsct::ToRange                   MCsFromParticle;
typedef Particle2MCAsct::ToIterator                MCsFromParticleIterator;
typedef Particle2MCAsct::Table                     Particle2MCTable;
  
#endif // ASSOCIATORS_Particle2MCASCT_H
