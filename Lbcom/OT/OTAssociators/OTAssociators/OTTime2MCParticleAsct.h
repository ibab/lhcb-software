// $Id: OTTime2MCParticleAsct.h,v 1.1 2004-09-03 12:08:09 jnardull Exp $
#ifndef OTASSOCIATORS_OTTIME2MCPARTICLEASCT_H
#define OTASSOCIATORS_OTTIME2MCPARTICLEASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTTime.h"
#include "Event/MCParticle.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTTime2MCParticleLocation = 
                   "Rec/Relations/OTTimes2MCParticles";

/** @class OTTime2MCParticleAsct OTTime2MCParticleAsct.h \
 *         "OTAssociators/OTTime2MCParticleAsct.h"
 *  
 *  Associator for the OTTimes with the corresponding MCParticles. 
 *  Makes use of the associator with the MCHits. By default OTTimes 
 *  coming from spillover are not associated.
 *
 *  @author J. van Tilburg & Jacopo Nardulli
 *  @date   15/06/2004
 */


class OTTime;
class MCParticle;

class OTTime2MCParticleAsct: public Associator<OTTime,MCParticle>
{
  friend class ToolFactory<OTTime2MCParticleAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTTime,MCParticle>               Table;
  typedef OwnType                                     Asct;
  typedef FromRange                                   Times;
  typedef FromIterator                                TimesIterator;
  typedef ToRange                                     MCParticles;
  typedef ToIterator                                  MCParticlesIterator;
    
private:

  /// Standard constructor
  OTTime2MCParticleAsct(const std::string& type,
                         const std::string& name,
                         const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", OTTime2MCParticleLocation );
    setProperty( "AlgorithmType", "OTTime2MCParticleAlg" );
    setProperty( "AlgorithmName", "OTTime2MCParticle" );
  }; 

  /// Destructor
  virtual ~OTTime2MCParticleAsct() {}; ///< Destructor


};
#endif // OTASSOCIATORS_OTTIME2MCPARTICLEASCT_H
