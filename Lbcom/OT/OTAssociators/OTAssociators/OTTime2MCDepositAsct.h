// $Id: OTTime2MCDepositAsct.h,v 1.1 2004-09-03 12:08:09 jnardull Exp $
#ifndef OTASSOCIATORS_OTTIME2MCDEPOSITASCT_H 
#define OTASSOCIATORS_OTTIME2MCDEPOSITASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTTime.h"
#include "Event/MCOTDeposit.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTTime2MCDepositLocation =
                       "Rec/Relations/OTTimes2MCDeposits";

/** @class OTTime2MCDepositAsct OTTime2MCDepositAsct.h \
 *         "OTAssociators/OTTime2MCDepositAsct.h"
 *  
 *  Associator for the OTTimes with the corresponding MCOTDeposits. 
 *  By default OTTimes coming from spillover are also associated.
 *  If other MCOTDeposits on the same channel but killed by dead time, are
 *  recorded within a certain (small) time-window, they can be associated as 
 *  well.
 *
 *  @author J. van Tilburg & Jacopo Nardulli
 *  @date   15/06/2004
 */

class OTTime2MCDepositAsct: public Associator<OTTime,MCOTDeposit>
{
  friend class ToolFactory<OTTime2MCDepositAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTTime,MCOTDeposit>              Table;
  typedef OwnType                                     Asct;
  typedef FromRange                                   Times;
  typedef FromIterator                                TimesIterator;
  typedef ToRange                                     MCDeposits;
  typedef ToIterator                                  MCDepositsIterator;
    
private:

  /// Standard constructor
  OTTime2MCDepositAsct(const std::string& type,
                       const std::string& name,
                       const IInterface* parent ) 
    :Asct( type, name, parent) {
    setProperty( "Location", OTTime2MCDepositLocation);
    setProperty( "AlgorithmType", "OTTime2MCDepositAlg" );
    setProperty( "AlgorithmName", "OTTime2MCDeposit" );
  }; 

  /// Destructor
  virtual ~OTTime2MCDepositAsct() {}; ///< Destructor


};
#endif // OTASSOCIATORS_OTTIME2MCDEPOSITASCT_H
