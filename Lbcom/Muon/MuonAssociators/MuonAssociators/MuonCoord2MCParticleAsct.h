// $Id: MuonCoord2MCParticleAsct.h,v 1.1.1.1 2002-06-28 09:59:33 dhcroft Exp $
#ifndef ASSOCIATORS_MuonCoord2MCParticleASCT_H 
#define ASSOCIATORS_MuonCoord2MCParticleASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/MuonCoord.h"
#include "Event/MCParticle.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& MuonCoord2MCParticleLocation = 
                   "Rec/Relations/MuonCoords2MCParticles";

/** @class MuonCoord2MCParticleAsct MuonCoord2MCParticleAsct.h 
 *         Associators/MuonCoord2MCParticleAsct.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   12/06/2002
 */


class MuonCoord;
class MCParticle;

class MuonCoord2MCParticleAsct: public Associator<MuonCoord,MCParticle>
{
  friend ToolFactory<MuonCoord2MCParticleAsct>;
  
public:
  // Define data types
  typedef Relation1D<MuonCoord,MCParticle>          Table;
  typedef OwnType                                   Asct;
  typedef FromRange                                 Coords;
  typedef FromIterator                              CoordsIterator;
  typedef ToRange                                   MCParticles;
  typedef ToIterator                                MCParticlesIterator;
    
private:

  /// Standard constructor
  MuonCoord2MCParticleAsct(const std::string& type,
                           const std::string& name,
                           const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", MuonCoord2MCParticleLocation );
    setProperty( "AlgorithmType", "MuonCoord2MCParticleAlg" );
    setProperty( "AlgorithmName", "MuonCoord2MCParticle" );
  }; 

  /// Destructor
  virtual ~MuonCoord2MCParticleAsct() {}; ///< Destructor


};
#endif // ASSOCIATORS_MuonCoord2MCParticleASCT_H
