// $Id: Particle2MCWithChi2Asct.h,v 1.7 2004-06-16 16:16:25 pkoppenb Exp $
#ifndef Particle2MCWithChi2ASCT_H 
#define Particle2MCWithChi2ASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Associators
#include "Relations/AssociatorWeighted.h"

static const std::string& 
Particle2MCWithChi2AsctLocation = "Phys/Relations/Particle2MCWithChi2";

/** @class Particle2MCWithChi2Asct Particle2MCWithChi2Asct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   17/05/2002
 */

#include "Event/Particle.h"
#include "Event/MCParticle.h"

class Particle2MCWithChi2Asct : 
  public AssociatorWeighted<Particle,MCParticle,double>
{
  friend class ToolFactory<Particle2MCWithChi2Asct>;
public:
  // Define data types
  typedef RelationWeighted1D<Particle,MCParticle,double>       Table;
  typedef OwnType                               Asct;

  /// Standard constructor
  Particle2MCWithChi2Asct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : Asct( type, name, parent) {

    setProperty( "Location", Particle2MCWithChi2AsctLocation );
    setProperty( "AlgorithmType", "Particle2MCWithChi2" );
    setProperty( "AlgorithmName", "Particle2MCWithChi2" );
    
  }; 

  virtual ~Particle2MCWithChi2Asct( ){}; ///< Destructor

protected:

private:

};

typedef Particle2MCWithChi2Asct::FromRange      ParticlesToMCChi2;
typedef Particle2MCWithChi2Asct::FromIterator   ParticlesToMCChi2Iterator;
typedef Particle2MCWithChi2Asct::ToRange        MCsFromParticleChi2;
typedef Particle2MCWithChi2Asct::ToIterator     MCsFromParticleChi2Iterator;
typedef Particle2MCWithChi2Asct::Table          Particle2MCWithChi2Table;
  
#endif // Particle2MCWithChi2ASCT_H
