// $Id: ProtoParticle2MCAsct.h,v 1.1 2002-07-12 15:24:46 phicharp Exp $
#ifndef ASSOCIATORS_ProtoParticle2MCASCT_H 
#define ASSOCIATORS_ProtoParticle2MCASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Event
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
// Associators
#include "Relations/Associator.h"

static const std::string& 
ProtoParticle2MCAsctLocation = "Rec/Relations/ProtoParticle2MC";

/** @class ProtoParticle2MCAsct DaVinciAssociators/Particle2MCAsct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/07/2002
 */

class ProtoParticle2MCAsct : public Associator<ProtoParticle,MCParticle>
{
  friend ToolFactory<ProtoParticle2MCAsct>;
  
public:
  // Define data types
  typedef Relation1D<ProtoParticle,MCParticle>  Table;
  typedef OwnType                               Asct;
  
protected:
  /// Standard constructor
  ProtoParticle2MCAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : Asct( type, name, parent) {

    setProperty( "Location", ProtoParticle2MCAsctLocation );
    setProperty( "AlgorithmType", "ProtoParticle2MCLinks" );
    setProperty( "AlgorithmName", "ProtoParticle2MC" );
  }; 

  virtual ~ProtoParticle2MCAsct() { }; ///< Destructor

private:

};

typedef ProtoParticle2MCAsct::FromRange       ProtoParticlesToMC;
typedef ProtoParticle2MCAsct::FromIterator    ProtoParticlesToMCIterator;
typedef ProtoParticle2MCAsct::ToRange         MCsFromProtoParticle;
typedef ProtoParticle2MCAsct::ToIterator      MCsFromProtoParticleIterator;
typedef ProtoParticle2MCAsct::Table           ProtoParticle2MCTable;

#endif // ASSOCIATORS_ProtoParticle2MCASCT_H
