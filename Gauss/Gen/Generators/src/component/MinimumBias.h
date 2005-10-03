// $Id: MinimumBias.h,v 1.1 2005-10-03 10:29:04 robbep Exp $
#ifndef GENERATORS_MINIMUMBIAS_H 
#define GENERATORS_MINIMUMBIAS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ExternalGenerator.h" 

/** @class MinimumBias MinimumBias.h 
 *  
 *  Tool for minimum bias generation
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class MinimumBias : public ExternalGenerator {
public:
  /// Standard constructor
  MinimumBias( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~MinimumBias( ); ///< Destructor

  virtual StatusCode initialize( ) ;

  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

  virtual void printCounters( ) const { ; }

protected:

private:

};
#endif // GENERATORS_MINIMUMBIAS_H
