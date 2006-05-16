// $Id: Particle2VertexAsct.h,v 1.4 2006-05-16 10:33:16 jpalac Exp $
#ifndef ASSOCIATORS_Particle2VertexASCT_H 
#define ASSOCIATORS_Particle2VertexASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Event
#include "Event/Vertex.h"
#include "Event/Particle.h"
// Associators
#include "Relations/AssociatorWeighted.h"

static const std::string& 
Particle2VertexAsctLocation = "/Event/Phys/Relations/Particle2VertexWithIPS";

/** @class Particle2VertexAsct Particle2VertexAsct.h Associators/Particle2VertexAsct.h
 *  
 *
 *  @author Gerhard Raven
 *  @date   05/04/2002
 */

class Particle2VertexAsct : public AssociatorWeighted<LHCb::Particle,LHCb::Vertex,double>
{
  friend class ToolFactory<Particle2VertexAsct>;
  
public:

  // Define data types
  typedef RelationWeighted1D<LHCb::Particle,LHCb::Vertex,double> Table;
  typedef OwnType                                    Asct;
  
protected:
  /// Standard constructor
  Particle2VertexAsct(const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
    : Asct( type, name, parent) {

    setProperty( "Location", Particle2VertexAsctLocation );
    setProperty( "AlgorithmType", "Particle2VertexWithIPS" );
    setProperty( "AlgorithmName", "Particle2VertexWithIPS" );
    
  }; 

  virtual ~Particle2VertexAsct() { }; ///< Destructor

private:
};

typedef Particle2VertexAsct::FromRange                 ParticlesToVertex;
typedef Particle2VertexAsct::FromIterator              ParticlesToVertexIterator;
typedef Particle2VertexAsct::ToRange                   VerticesFromParticle;
typedef Particle2VertexAsct::ToIterator                VerticesFromParticleIterator;
typedef Particle2VertexAsct::Table                     Particle2VertexTable;
  
#endif // ASSOCIATORS_Particle2VertexASCT_H
