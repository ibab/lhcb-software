// $Id: OTDigit2MCDepositAsct.h,v 1.1 2003-06-10 09:04:15 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCDEPOSITASCT_H 
#define OTASSOCIATORS_OTDIGIT2MCDEPOSITASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTDigit.h"
#include "Event/MCOTDeposit.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTDigit2MCDepositLocation =
                       "Rec/Relations/OTDigits2MCDeposits";

/** @class OTDigit2MCDepositAsct OTDigit2MCDepositAsct.h \
 *         "OTAssociators/OTDigit2MCDepositAsct.h"
 *  
 *  Associator for the OTDigits with the corresponding MCOTDeposits. 
 *  By default OTDigits coming from spillover are also associated.
 *
 *  @author J. van Tilburg
 *  @date   05/06/2003
 */

class OTDigit2MCDepositAsct: public Associator<OTDigit,MCOTDeposit>
{
  friend class ToolFactory<OTDigit2MCDepositAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTDigit,MCOTDeposit>         Table;
  typedef OwnType                                 Asct;
  typedef FromRange                               Digits;
  typedef FromIterator                            DigitsIterator;
  typedef ToRange                                 MCDeposits;
  typedef ToIterator                              MCDepositsIterator;
    
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
