// $Id: OTTime2MCHitAsct.h,v 1.1 2004-09-03 12:08:09 jnardull Exp $
#ifndef OTASSOCIATORS_OTTIME2MCHITASCT_H 
#define OTASSOCIATORS_OTTIME2MCHITASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTTime.h"
#include "Event/MCHit.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTTime2MCHitLocation =
                       "Rec/Relations/OTTimes2MCHits";

/** @class OTTime2MCHitAsct OTTime2MCHitAsct.h \
 *         "OTAssociators/OTTime2MCHitAsct.h"
 *  
 *  Associator for the OTTimes with the corresponding MCHits. 
 *  By default OTTimes coming from spillover are not associated.
 * 
 *  @author J. van Tilburg & Jacopo Nardulli
 *  @date   15/06/2004
 */

class OTTime2MCHitAsct: public Associator<OTTime,MCHit>
{
  friend class ToolFactory<OTTime2MCHitAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTTime,MCHit>                Table;
  typedef OwnType                                 Asct;
  typedef FromRange                               Times;
  typedef FromIterator                            TimesIterator;
  typedef ToRange                                 MCHits;
  typedef ToIterator                              MCHitsIterator;
    
private:

  /// Standard constructor
  OTTime2MCHitAsct(const std::string& type,
                   const std::string& name,
                   const IInterface* parent ) 
    :Asct( type, name, parent) {
    setProperty( "Location", OTTime2MCHitLocation);
    setProperty( "AlgorithmType", "OTTime2MCHitAlg" );
    setProperty( "AlgorithmName", "OTTime2MCHit" );
  }; 

  /// Destructor
  virtual ~OTTime2MCHitAsct() {}; ///< Destructor

};
#endif // OTASSOCIATORS_OTTIME2MCHITASCT_H

