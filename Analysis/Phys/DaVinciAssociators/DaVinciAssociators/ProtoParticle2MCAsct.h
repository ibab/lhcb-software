// $Id: ProtoParticle2MCAsct.h,v 1.4 2003-04-24 13:55:07 gcorti Exp $
#ifndef ASSOCIATORS_ProtoParticle2MCASCT_H 
#define ASSOCIATORS_ProtoParticle2MCASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Event
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
// Associators
#include "Relations/AssociatorWeighted.h"

static const std::string& 
ChargedPP2MCAsctLocation = "Rec/Relations/ChargedPP2MC";

static const std::string& 
NeutralPP2MCAsctLocation = "Rec/Relations/NeutralPP2MC";

/** @class ProtoParticle2MCAsct DaVinciAssociators/Particle2MCAsct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/07/2002
 */

class ProtoParticle2MCAsct : 
public AssociatorWeighted<ProtoParticle,MCParticle,double>
{
  friend ToolFactory<ProtoParticle2MCAsct>;
  
public:
  // Define data types
  typedef RelationWeighted1D<ProtoParticle,MCParticle, double>  Table;
  typedef OwnType                               Asct;
  
protected:
  /// Standard constructor
  ProtoParticle2MCAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : Asct( type, name, parent) {

    setProperty( "Location", ChargedPP2MCAsctLocation );
    setProperty( "AlgorithmType", "ChargedPP2MC" );
    setProperty( "AlgorithmName", "ChargedPP2MC" );
//     setDecreasing( true );
  }; 

  virtual ~ProtoParticle2MCAsct() { }; ///< Destructor

private:

};

typedef ProtoParticle2MCAsct::FromRange       ProtoParticlesToMC;
typedef ProtoParticle2MCAsct::FromIterator    ProtoParticlesToMCIterator;
typedef ProtoParticle2MCAsct::ToRange         MCsFromProtoParticle;
typedef ProtoParticle2MCAsct::ToIterator      MCsFromProtoParticleIterator;
typedef ProtoParticle2MCAsct::Table           ProtoParticle2MCTable;
typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> Tr2MCPartAsct;

#endif // ASSOCIATORS_ProtoParticle2MCASCT_H
