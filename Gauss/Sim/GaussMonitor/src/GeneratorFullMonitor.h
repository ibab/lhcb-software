// $Id: GeneratorFullMonitor.h,v 1.7 2008-11-11 19:15:56 robbep Exp $
#ifndef GAUSSMONITOR_GENERATORFULLMONITOR_H 
#define GAUSSMONITOR_GENERATORFULLMONITOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

namespace HepMC { 
  class GenParticle ;
} ;

/** @class GeneratorFullMonitor GeneratorFullMonitor.h GaussMonitor/GeneratorFullMonitor.h
 *  Algorithm filling an ntuple with the content of the MC Truth and 
 *  with the hard process info
 *
 *  @author Patrick Robbe, Paul Szczypka
 *  @date   2004-03-02 (last modified: 2007-01-30)  
 */
class GeneratorFullMonitor : public GaudiTupleAlg {
public:
  /// Standard constructor
  GeneratorFullMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GeneratorFullMonitor( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  /// Fill Ntuple info from one particle
  void FillNtuple( HepMC::GenParticle * thePart , int motherPdgCode ,
                   int motherIndex ) ;
  
  /// Maximum size of PART block
  int m_nPartMax ;
  /// Maximum size of INTER block
  int m_nInterMax ;
  /// Maximum size of Madelstam block
  int m_event_max ;

  /// Location where to find HepMC event
  std::string m_inputHepMC;
  /// Location where to find GenCollision
  std::string m_inputColl;
  /// Location where to find GenHeader
  std::string m_inputHeader;

  int m_nPart , m_nInter ;
  std::vector< double > m_e , m_px , m_py , m_pz ;
  std::vector< double > m_vxProd , m_vyProd , m_vzProd , m_vtProd ;
  std::vector< double > m_vxDecay , m_vyDecay , m_vzDecay , m_vtDecay ;
  std::vector< int > m_pdgId , m_nDau , m_pdgIdMother ;
  std::vector< int > m_pdgIdDau1 , m_pdgIdDau2 , m_pdgIdDau3 , 
    m_pdgIdDau4 , m_pdgIdDau5 , m_pdgIdDau6 ;
  std::vector< int > m_indexMother , m_indexInter ;
  
};
#endif // GAUSSMONITOR_GENERATORFULLMONITOR_H
