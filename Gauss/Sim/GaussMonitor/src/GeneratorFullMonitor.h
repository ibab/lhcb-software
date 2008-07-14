// $Id: GeneratorFullMonitor.h,v 1.6 2008-07-14 20:34:20 robbep Exp $
#ifndef GAUSSMONITOR_GENERATORFULLMONITOR_H 
#define GAUSSMONITOR_GENERATORFULLMONITOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/INTuple.h"


/** @class GeneratorFullMonitor GeneratorFullMonitor.h GaussMonitor/GeneratorFullMonitor.h
 *  Algorithm filling an ntuple with the content of the MC Truth and 
 *  with the hard process info
 *
 *  @author Patrick Robbe, Paul Szczypka
 *  @date   2004-03-02 (last modified: 2007-01-30)  
 */
class GeneratorFullMonitor : public GaudiAlgorithm {
public:
  /// Standard constructor
  GeneratorFullMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GeneratorFullMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// pointer to NTuple
  INTuple* m_nTuple ;

  /// NTuple variables PART block
  NTuple::Item < long > m_nPart  ; ///< Number of particles in event
  NTuple::Array< float > m_e     ; ///< Energie of particle (lab. frame)
  NTuple::Array< float > m_px, m_py, m_pz ; ///< Momentum of particle (lab.)
  NTuple::Array< float > m_vxProd, m_vyProd, m_vzProd,
    m_vtProd ; ///< Prod vertex
  NTuple::Array< float > m_vxDecay, m_vyDecay, m_vzDecay,
    m_vtDecay ; ///< Decay vertex
  NTuple::Array< long  > m_pdgId ; ///< PDG Id of particle
  NTuple::Array< long  > m_nDau ; ///< number of daughters of particle
  NTuple::Array< long  > m_pdgIdMother ; ///< PDG Id of mother particle
  NTuple::Array< long  > m_pdgIdDau1, m_pdgIdDau2, m_pdgIdDau3, m_pdgIdDau4,
    m_pdgIdDau5, m_pdgIdDau6 ; ///< PDG Id of daughter particles
  NTuple::Array< long  > m_indexMother ; ///< Index of mother in NTuple
  NTuple::Array< long  > m_indexInter  ; ///< Index of interaction in NTuple

  /// NTuple variables INTER Block
  NTuple::Item < long > m_nInter ; ///< Number of interactions in event
  NTuple::Array< long > m_isBB ; ///< 1 if b-bbar interaction 0 if not

  // NTuple variables Mandelstam block and process ID
  NTuple::Item < long  > m_event;/// number of hard events
  NTuple::Array< float > m_shat; ///< S_hat madelstam variable
  NTuple::Array< float > m_that; ///< T_hat madelstam variable
  NTuple::Array< float > m_uhat; ///< U_hat madelstam variable
  NTuple::Array< float > m_pthat; ///< pt_hat madelstam variable
  NTuple::Array< float > m_x1;   ///< x1 mandelstam variable
  NTuple::Array< float > m_x2;   ///< x2 madelstam variable
  NTuple::Array< long  > m_procId;   ///< hard process ID

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

};
#endif // GAUSSMONITOR_GENERATORFULLMONITOR_H
