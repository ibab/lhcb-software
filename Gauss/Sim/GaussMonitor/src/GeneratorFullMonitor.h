// $Id: GeneratorFullMonitor.h,v 1.2 2004-04-29 17:19:13 gcorti Exp $
#ifndef GAUSSMONITOR_GENERATORFULLMONITOR_H 
#define GAUSSMONITOR_GENERATORFULLMONITOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/INTuple.h"


/** @class GeneratorFullMonitor GeneratorFullMonitor.h GaussMonitor/GeneratorFullMonitor.h
 *  Algorithm filling an ntuple with the content of the MC Truth
 *
 *  @author Patrick Robbe
 *  @date   2004-03-02
 */
class GeneratorFullMonitor : public Algorithm {
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
  INTuple * m_nTuple ;

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
  /// Fill Ntuple info from one particle
  void FillNtuple( HepMC::GenParticle * thePart , int motherPdgCode ,
                   int motherIndex ) ;
  /// Maximum size of PART block
  int m_nPartMax ;
  /// Maximum size of INTER block
  int m_nInterMax ;
  /// Location where to find HepMC event
  std::string m_inputData ;
};
#endif // GAUSSMONITOR_GENERATORFULLMONITOR_H
