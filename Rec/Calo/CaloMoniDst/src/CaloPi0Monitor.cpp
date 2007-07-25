// $Id: CaloPi0Monitor.cpp,v 1.4 2007-07-25 19:49:13 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/11/07 12:16:38  odescham
// v2r0 - adapt to the new Track Event Model
//
// Revision 1.2  2005/05/08 09:58:25  ibelyaev
//  remove associators, update options
//
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloHypo.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/CaloMomentum.h"
// ============================================================================
// local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloPi0Monitor CaloPi0Monitor.cpp
 *
 *  Simple pi0 monitoring algorithm
 *  It produces 7 histograms:
 *  <ol>
 *  <li> "Raw" mass distribution of 2 photons </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for combination </li>
 *  </ol>
 *
 *  @see   CaloMoniAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */
class CaloPi0Monitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloPi0Monitor>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize()
  { StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1( "1", "Gamma-Gamma Mass",                   0, 600 );
    hBook1( "2", "Gamma-Gamma Mass (    PtGamma cut)", 0, 600 );
    hBook1( "3", "Gamma-Gamma Mass (0.5xPtGamma cut)", 0, 600 );
    hBook1( "4", "Gamma-Gamma Mass (2.5xPtGamma cut)", 0, 600 );
    hBook1( "5", "Gamma-Gamma Mass (    PtPi0   cut)", 0, 600 );
    hBook1( "6", "Gamma-Gamma Mass (0.5xPtPi0   cut)", 0, 600 );
    hBook1( "7", "Gamma-Gamma Mass (2.5xPtPi0   cut)", 0, 600 );
    return StatusCode::SUCCESS;
  }
  /// standard algorithm execution
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloPi0Monitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
    , m_ptPhoton( 250 * Gaudi::Units::MeV )
    , m_ptPi0( 500 * Gaudi::Units::MeV )
  { declareProperty( "PhotonPt", m_ptPhoton );
    declareProperty( "Pi0Pt",    m_ptPi0 );
    // set the appropriate default for input data
    addToInputs( LHCb::CaloHypoLocation::Photons );
    addToInputs( LHCb::CaloHypoLocation::SplitPhotons );
  }
  /// destructor (virtual and protected)
  virtual ~CaloPi0Monitor() {}
private:
  /// default  construstor is private
  CaloPi0Monitor();
  /// copy     construstor is private
  CaloPi0Monitor( const CaloPi0Monitor& );
  /// assignement operator is private
  CaloPi0Monitor &operator=( const CaloPi0Monitor& );
private:
  // cut on Photon Pt
  double m_ptPhoton;
  // cut on Pi0    Pt
  double m_ptPi0;
};

DECLARE_ALGORITHM_FACTORY( CaloPi0Monitor );

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloPi0Monitor::execute()
{ typedef const LHCb::CaloHypo::Container    Hypos;
  typedef const LHCb::CaloHypo               Photon;
  typedef std::vector<const LHCb::CaloHypo*> Photons;
  typedef Photons::const_iterator            photon;

  if ( !produceHistos() ) return StatusCode::SUCCESS;

  if ( inputs().empty() ) return Error( "No input data are specified" );

  Photons photons;
  for( std::vector<std::string>::const_iterator input = inputs().begin();
        inputs().end() != input; ++input )
  { Hypos* hypos = get<Hypos>( *input );
    if ( 0 == hypos ) return StatusCode::FAILURE;
    photons.insert( photons.end(), hypos->begin(), hypos->end() );
  }

  if ( photons.empty() )
  { return Warning( "No Photons arte found!", StatusCode::SUCCESS );
  }

// loop over the first photon
  for( photon g1 = photons.begin(); photons.end() != g1; ++g1 )
  { if ( 0 == *g1 ) continue;
    LHCb::CaloMomentum momentum1( *g1 );
    Gaudi::LorentzVector v1( momentum1.momentum() );
// loop over the second photon
    for( photon g2 = g1 + 1; photons.end() != g2; ++g2 )
    { if ( 0 == *g2 ) continue;
      LHCb::CaloMomentum momentum2( *g2 );
      Gaudi::LorentzVector v2( momentum2.momentum() );

// 4-vector for pi0
      Gaudi::LorentzVector pi0( v1 + v2 );

// pi0 mass
      const double mass = pi0.mass() / Gaudi::Units::MeV;

      hFill1( "1", mass );

      const double pt1 = v1.pt();
      const double pt2 = v2.pt();
      if ( pt1 >     m_ptPhoton &&
           pt2 >     m_ptPhoton  ) hFill1( "2", mass );
      if ( pt1 > 0.5*m_ptPhoton &&
           pt2 > 0.5*m_ptPhoton  ) hFill1( "3", mass );
      if ( pt1 > 2.0*m_ptPhoton &&
           pt2 > 2.0*m_ptPhoton  ) hFill1( "4", mass );

      const double pt  = pi0.pt();
      if ( pt >     m_ptPi0 ) hFill1( "5", mass );
      if ( pt > 0.5*m_ptPi0 ) hFill1( "6", mass );
      if ( pt > 2.0*m_ptPi0 ) hFill1( "7", mass ); 
    }
  }
  return StatusCode::SUCCESS;
}
