// $Id: Part2MCPartAsct.h,v 1.2 2002-04-26 17:05:10 gcorti Exp $
#ifndef ASSOCIATORS_Part2MCPartASCT_H 
#define ASSOCIATORS_Part2MCPartASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Event
#include "Event/MCParticle.h"
#include "Event/Particle.h"
// Associators
#include "Relations/Associator.h"
#include "Relations/Relation2D.h"

static const std::string& Part2MCPartAsctLocation = "Phys/Relations/Part2MC";

/** @class Part2MCPartAsct Part2MCPartAsct.h Associators/Part2MCPartAsct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   05/04/2002
 */

class Part2MCPartAsct : public Associator<Particle,MCParticle>
{
  friend ToolFactory<Part2MCPartAsct>;
  
public:
  // Define data types
  typedef Relation2D<Particle,MCParticle>  Table;
  typedef OwnType                               Asct;
  typedef ToRange                               Parts;
  typedef ToIterator                            PartsIterator;
  typedef FromRange                             MCParts;
  typedef FromIterator                          MCPartsIterator;
  
  
protected:
  /// Standard constructor
  Part2MCPartAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : Asct( type, name, parent) {

    setProperty( "Location", Part2MCPartAsctLocation );
    setProperty( "AlgorithmType", "Part2MCPartChi2Alg" );
    setProperty( "AlgorithmName", "Part2MC" );
    
  }; 

  virtual ~Part2MCPartAsct() { }; ///< Destructor

private:

};
#endif // ASSOCIATORS_Part2MCPartASCT_H
