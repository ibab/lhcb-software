// $Id: OTDigit2MCParticleAsct.h,v 1.1 2003-06-10 09:04:15 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCPARTICLEASCT_H
#define OTASSOCIATORS_OTDIGIT2MCPARTICLEASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTDigit.h"
#include "Event/MCParticle.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTDigit2MCParticleLocation = 
                   "Rec/Relations/OTDigits2MCParticles";

/** @class OTDigit2MCParticleAsct OTDigit2MCParticleAsct.h \
 *         "OTAssociators/OTDigit2MCParticleAsct.h"
 *  
 *  Associator for the OTDigits with the corresponding MCParticles. 
 *  Makes use of the associator with the MCHits. By default OTDigits 
 *  coming from spillover are not associated.
 *
 *  @author J. van Tilburg
 *  @date   14/05/2002
 */


class OTDigit;
class MCParticle;

class OTDigit2MCParticleAsct: public Associator<OTDigit,MCParticle>
{
  friend class ToolFactory<OTDigit2MCParticleAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTDigit,MCParticle>          Table;
  typedef OwnType                                 Asct;
  typedef FromRange                               Digits;
  typedef FromIterator                            DigitsIterator;
  typedef ToRange                                 MCParticles;
  typedef ToIterator                              MCParticlesIterator;
    
private:

  /// Standard constructor
  OTDigit2MCParticleAsct(const std::string& type,
                         const std::string& name,
                         const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", OTDigit2MCParticleLocation );
    setProperty( "AlgorithmType", "OTDigit2MCParticleAlg" );
    setProperty( "AlgorithmName", "OTDigit2MCParticle" );
  }; 

  /// Destructor
  virtual ~OTDigit2MCParticleAsct() {}; ///< Destructor


};
#endif // OTASSOCIATORS_OTDIGIT2MCPARTICLEASCT_H
