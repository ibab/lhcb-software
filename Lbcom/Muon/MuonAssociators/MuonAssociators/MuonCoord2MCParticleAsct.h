// $Id: MuonCoord2MCParticleAsct.h,v 1.3 2003-01-17 14:14:54 sponce Exp $
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



class MuonCoord2MCParticleAsct: public Associator<MuonCoord,MCParticle>
{

  friend class ToolFactory<MuonCoord2MCParticleAsct>;
  
public:
  // Define data types
  typedef Relation1D<MuonCoord,MCParticle>          Table;
  typedef OwnType                                   Asct;
  typedef FromRange                                 Coords;
  typedef FromIterator                              CoordsIterator;
  typedef ToRange                                   MCParticles;
  typedef ToIterator                                MCParticlesIterator;
    
protected:

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

private:

};
#endif // ASSOCIATORS_MuonCoord2MCParticleASCT_H
