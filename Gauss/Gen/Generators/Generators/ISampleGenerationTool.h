// $Id: ISampleGenerationTool.h,v 1.3 2005-12-11 23:21:47 robbep Exp $
#ifndef GENERATORS_ISAMPLEGENERATIONTOOL_H 
#define GENERATORS_ISAMPLEGENERATIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"


// Forward declarations
class HepMCEvent ;
class HardInfo ;

/** @class ISampleGenerationTool ISampleGenerationTool.h "Generators/ISampleGenerationTool.h"
 *  
 *  Abstract interface to generation of event samples. Concrete tools implement
 *  the algorithms used to generate the various event types generated in LHCb
 *  (Minimum bias, inclusive, signal with repeated hadronization, ...)
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_ISampleGenerationTool( "ISampleGenerationTool" , 
                                                    1 , 0 ) ;

class ISampleGenerationTool : virtual public IAlgTool {
 public:
  /// Vector of HepMCEvent (LHCb wrapper around HepMC::GenEvent)
  typedef std::vector< HepMCEvent * > EventVector ;

  /// Vector of HardInfo (hard process informations)
  typedef std::vector< HardInfo * >   HardVector  ;

  static const InterfaceID& interfaceID() { return IID_ISampleGenerationTool; }

  /** Generate a set of pile-up interactions to form an event
   *  @param[in]  nPileUp         Number of pile-up event to generate for 
   *                              this event.
   *  @param[out] theEventVector  Vector containing the generated pile-up 
   *                              interactions.
   *  @param[out] theHardVector   Vector containing hard process information
   *                              for each pile-up interaction.
   */
  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) = 0 ;

  /// Print counters and efficiencies at the end of the job.
  virtual void printCounters( ) const = 0 ;
};
#endif // GENERATORS_ISAMPLEGENERATIONTOOL_H
