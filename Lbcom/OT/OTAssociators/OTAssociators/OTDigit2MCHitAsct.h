// $Id: OTDigit2MCHitAsct.h,v 1.2 2003-07-15 11:31:06 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCHITASCT_H 
#define OTASSOCIATORS_OTDIGIT2MCHITASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTDigit.h"
#include "Event/MCHit.h"

// Associators
#include "Relations/AssociatorWeighted.h"
#include "Relations/RelationWeighted1D.h"

static const std::string& OTDigit2MCHitLocation =
                       "Rec/Relations/OTDigits2MCHits";

/** @class OTDigit2MCHitAsct OTDigit2MCHitAsct.h \
 *         "OTAssociators/OTDigit2MCHitAsct.h"
 *  
 *  Associator for the OTDigits with the corresponding MCHits. 
 *  By default OTDigits coming from spillover are not associated.
 *  Since an OTDigit could contain more than one tdc-time, an integer is 
 *  stored in the relation table from OTDigits to MCHits which points to 
 *  the tdc-time.
 * 
 *  @author J. van Tilburg
 *  @date   14/05/2002
 */

class OTDigit2MCHitAsct: public AssociatorWeighted<OTDigit,MCHit,int>
{
  friend class ToolFactory<OTDigit2MCHitAsct>;
  
public:
  // Define data types
  typedef RelationWeighted1D<OTDigit,MCHit,int>   Table;
  typedef OwnType                                 Asct;
  typedef FromRange                               Digits;
  typedef FromIterator                            DigitsIterator;
  typedef ToRange                                 MCHits;
  typedef ToIterator                              MCHitsIterator;
    
private:

  /// Standard constructor
  OTDigit2MCHitAsct(const std::string& type,
                    const std::string& name,
                    const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", OTDigit2MCHitLocation);
    setProperty( "AlgorithmType", "OTDigit2MCHitAlg" );
    setProperty( "AlgorithmName", "OTDigit2MCHit" );
  }; 

  /// Destructor
  virtual ~OTDigit2MCHitAsct() {}; ///< Destructor

};
#endif // OTASSOCIATORS_OTDIGIT2MCHITASCT_H

