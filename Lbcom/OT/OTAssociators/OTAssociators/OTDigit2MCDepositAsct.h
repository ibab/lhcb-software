// $Id: OTDigit2MCDepositAsct.h,v 1.2 2003-07-15 11:31:06 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCDEPOSITASCT_H 
#define OTASSOCIATORS_OTDIGIT2MCDEPOSITASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTDigit.h"
#include "Event/MCOTDeposit.h"

// Associators
#include "Relations/AssociatorWeighted.h"
#include "Relations/RelationWeighted1D.h"

static const std::string& OTDigit2MCDepositLocation =
                       "Rec/Relations/OTDigits2MCDeposits";

/** @class OTDigit2MCDepositAsct OTDigit2MCDepositAsct.h \
 *         "OTAssociators/OTDigit2MCDepositAsct.h"
 *  
 *  Associator for the OTDigits with the corresponding MCOTDeposits. 
 *  By default OTDigits coming from spillover are also associated.
 *  Since an OTDigit could contain more than one tdc-time, an integer is 
 *  stored in the relation table from OTDigits to MCOTDeposits which points 
 *  to the tdc-time.
 *  If other MCOTDeposits on the same channel but killed by dead time, are
 *  recorded within a certain (small) time-window, they can be associated as 
 *  well.
 *
 *  @author J. van Tilburg
 *  @date   05/06/2003
 */

class OTDigit2MCDepositAsct: public AssociatorWeighted<OTDigit,MCOTDeposit,int>
{
  friend class ToolFactory<OTDigit2MCDepositAsct>;
  
public:
  // Define data types
  typedef RelationWeighted1D<OTDigit,MCOTDeposit,int> Table;
  typedef OwnType                                     Asct;
  typedef FromRange                                   Digits;
  typedef FromIterator                                DigitsIterator;
  typedef ToRange                                     MCDeposits;
  typedef ToIterator                                  MCDepositsIterator;
    
private:

  /// Standard constructor
  OTDigit2MCDepositAsct(const std::string& type,
                        const std::string& name,
                        const IInterface* parent ) 
    :Asct( type, name, parent) {
    setProperty( "Location", OTDigit2MCDepositLocation);
    setProperty( "AlgorithmType", "OTDigit2MCDepositAlg" );
    setProperty( "AlgorithmName", "OTDigit2MCDeposit" );
  }; 

  /// Destructor
  virtual ~OTDigit2MCDepositAsct() {}; ///< Destructor


};
#endif // OTASSOCIATORS_OTDIGIT2MCDEPOSITASCT_H
