// $Id: Inclusive.h,v 1.1 2005-10-03 10:29:42 robbep Exp $
#ifndef GENERATORS_INCLUSIVE_H 
#define GENERATORS_INCLUSIVE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/ExternalGenerator.h" 

namespace HepMC {
  class GenParticle ;
}

/** @class Inclusive Inclusive.h 
 *  
 *  Tool for inclusive samples generation
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class Inclusive : public ExternalGenerator {
public:
  /// Standard constructor
  Inclusive( const std::string& type, 
             const std::string& name,
             const IInterface* parent);
  
  virtual ~Inclusive( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;
  
  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

  virtual void printCounters( ) const ;

protected:

private:
  double        m_lightestMass     ;
  unsigned int  m_lightestPID      ;
  unsigned int  m_nEventsBeforeCut ;
  unsigned int  m_nEventsAfterCut  ;
  unsigned int  m_nInvertedEvents  ;
  PIDs          m_pids             ;
  std::vector< int > m_pidVector   ;
};
#endif // GENERATORS_INCLUSIVE_H
