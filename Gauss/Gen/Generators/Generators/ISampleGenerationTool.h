// $Id: ISampleGenerationTool.h,v 1.2 2005-11-29 15:45:56 robbep Exp $
#ifndef GENERATORS_ISAMPLEGENERATIONTOOL_H 
#define GENERATORS_ISAMPLEGENERATIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class HepMCEvent ;
class HardInfo ;

/** @class ISampleGenerationTool ISampleGenerationTool.h 
 *         Generators/ISampleGenerationTool.h
 *  
 *  Abstract interface to generation of event samples
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_ISampleGenerationTool( "ISampleGenerationTool" , 
                                                    1 , 0 ) ;

class ISampleGenerationTool : virtual public IAlgTool {
 public:
  typedef std::vector< HepMCEvent * > EventVector ;
  typedef std::vector< HardInfo * >   HardVector  ;

  static const InterfaceID& interfaceID() { return IID_ISampleGenerationTool; }

  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) = 0 ;

  virtual void printCounters( ) const = 0 ;
};
#endif // GENERATORS_ISAMPLEGENERATIONTOOL_H
