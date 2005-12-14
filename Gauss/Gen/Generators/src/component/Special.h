// $Id: Special.h,v 1.3 2005-12-14 22:16:42 robbep Exp $
#ifndef GENERATORS_SPECIAL_H 
#define GENERATORS_SPECIAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ExternalGenerator.h" 

/** @class Special Special.h 
 *  
 *  Tool for special samples (Higgs, W, Z, ...) generation.
 *  Concrete implementation of ISampleGenerationTool using
 *  the ExternalGenerator base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-11-14
 */
class Special : public ExternalGenerator {
public:
  /// Standard constructor
  Special( const std::string& type, const std::string& name,
           const IInterface* parent);

  virtual ~Special( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;

  /** Generate a single interaction (No Pile-up for the moment.
   *  Implements ISampleGenerationTool::generate.
   *  Accepts all events generated with the IProductionTool
   *  (usually configured with special options) and passing
   *  the generator level cut.
   */
  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

  /// Implements ISampleGenerationTool::printCounters
  virtual void printCounters( ) const ;

private:
  /// Counter of events before the generator level cut
  unsigned int m_nEventsBeforeCut ;

  /// Counter of events after the generator level cut
  unsigned int m_nEventsAfterCut ;
};
#endif // GENERATORS_SPECIAL_H
