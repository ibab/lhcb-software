// $Id: Inclusive.h,v 1.3 2005-12-07 22:54:49 robbep Exp $
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
  Inclusive( const std::string& type, const std::string& name,
             const IInterface* parent);
  
  virtual ~Inclusive( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;
  
  virtual bool generate( const unsigned int nPileUp ,
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

  virtual void printCounters( ) const ;

protected:

  void updateHadronCounters( const HepMC::GenEvent * theEvent ) ;

  void updateExcitedStatesCounters( const HepMC::GenEvent * theEvent ) ;

private:
  double        m_lightestMass     ;
  unsigned int  m_lightestPID      ;
  unsigned int  m_nEventsBeforeCut ;
  unsigned int  m_nEventsAfterCut  ;
  unsigned int  m_nInvertedEvents  ;
  PIDs          m_pids             ;
  std::vector< int > m_pidVector   ;

  unsigned int  m_nB0 , m_nB0bar ;
  unsigned int  m_nBp , m_nBm    ;
  unsigned int  m_nBs0 , m_nBs0bar ;
  unsigned int  m_nbBaryon , m_nantibBaryon ;
  unsigned int  m_nBcp , m_nBcm  ;
  unsigned int  m_nbb ;
  
  unsigned int  m_nD0 , m_nD0bar ;
  unsigned int  m_nDp , m_nDm    ;
  unsigned int  m_nDsp , m_nDsm  ;
  unsigned int  m_ncBaryon , m_nanticBaryon ;
  unsigned int  m_ncc ;

  unsigned int  m_n0starB , m_n1starB , m_n2starB ;
  unsigned int  m_n0starC , m_n1starC , m_n2starC ;
};
#endif // GENERATORS_INCLUSIVE_H
