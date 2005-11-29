// $Id: Special.h,v 1.2 2005-11-29 15:58:24 robbep Exp $
#ifndef GENERATORS_SPECIAL_H 
#define GENERATORS_SPECIAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ExternalGenerator.h" 

/** @class Special Special.h 
 *  
 *  Tool for special generation
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

  virtual StatusCode initialize( ) ;

  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

  virtual void printCounters( ) const ;

protected:

private:
  unsigned int m_nEventsBeforeCut ;
  unsigned int m_nEventsAfterCut ;
};
#endif // GENERATORS_SPECIAL_H
