// $Id: LoKiJetMakerAlg.cpp,v 1.6 2010-01-11 08:37:42 cocov Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IJetMaker.h"
#include "Kernel/Particle2Vertex.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/PVSentry.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"

#include "GaudiKernel/IHistogramSvc.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "AIDA/IHistogram2D.h"
#include "boost/lexical_cast.hpp"

#include "TH2D.h"
#include "TMath.h"
#include "Math/VectorUtil.h"

class TH2D;
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class JetMaker
   *
   *  This file is a part of LoKi project -
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
   *  contributions and advices from G.Raven, J.van Tilburg,
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date   2005-03-21
   */
  class JetMaker : public LoKi::Algo
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<LoKi::JetMaker> ;
    // ========================================================================
  protected:
    // ========================================================================
    /** Standard constructor
     *  @param name instance name
     *  @param pSvc pointer to Service Locator
     */
    JetMaker
    ( const std::string& name ,
      ISvcLocator*       pSvc )
      : LoKi::Algo ( name , pSvc )
        //
      , m_makerName ( "LoKi::FastJetMaker"   )
      , m_maker     ( 0   )
      , m_associate2Vertex ( false  )
      , m_runJetID ( true  )
    {
      //
      declareProperty
        ( "JetMaker"  ,
          m_makerName ,
          "Type type/name of jet-maker tool (IJetMaker interface)") ;
      declareProperty
        ( "Associate2Vertex"  ,
          m_associate2Vertex ,
          "Jet reconstruction per vertex") ;
      declareProperty
        ( "JetID"  ,
          m_runJetID ,
          "Append Jet ID information to the jet") ;
      declareProperty
        ( "ApplyJetID"  ,
          m_applyJetID = false,
          "Apply jet ID cuts") ;
      declareProperty("ApplyJEC"  ,  m_applyJEC = false );

      declareProperty("HistoPath"  , m_histo_path = "JEC"     );
      //
    }
    /// destructor
    virtual ~JetMaker( ){}
    // ========================================================================
  public:
    // ========================================================================
    /** standard execution of the algorithm
     *  @see LoKi::Algo
     *  @return status code
     */
    virtual StatusCode initialize   () ;
    virtual StatusCode analyse   () ;

    StatusCode appendJetIDInfo   ( LHCb::Particle * jet ) ;
    StatusCode JEC               ( LHCb::Particle* jet )  ;

    // ========================================================================
  private:
    // ========================================================================
    // the default constructor is disabled
    JetMaker () ;
    // the copy constructor is disabled
    JetMaker ( const JetMaker& )  ;
    // the assignement operator is disabled
    JetMaker& operator=( const  JetMaker& )  ;
    // ========================================================================
  private:
    // ========================================================================
    /// maker name
    std::string      m_makerName ; // jet maker name
    /// maker
    const IJetMaker* m_maker     ; // jet maker to be used
    /// associate two vertex?
    bool m_associate2Vertex      ; // make jet per vertex
    /// append jet ID info
    bool m_runJetID              ; // append jet ID info
    /// apply JEC
    bool m_applyJEC              ;
    /// apply JEC
    bool m_applyJetID              ;
    /// histograms for JEC
    std::vector<TH2D*> m_histosJEC ;
    /// histo path
    std::string m_histo_path ;




    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/** @file
 *  Implementation file for class  LoKi::JetMaker
 *  @date  2005-03-21
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
/*  standard execution of the algorithm
 *  @see LoKi::Algo
 *  @return status code
 */
// ===========================================================================
StatusCode LoKi::JetMaker::initialize ()
{
  StatusCode sc = LoKi::Algo::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // Read in the histograms:
  if ( m_applyJEC ){
    for( int i = 1; i < 4; ++i ) {
      std::string histoname = "JEC_PV"+boost::lexical_cast<std::string>(i);
      AIDA::IHistogram2D *aida =  get<AIDA::IHistogram2D> (histoSvc(), m_histo_path + histoname);
      if( 0==aida ) warning()<<"Could not find AIDA::IHistogram2D* "
                             << m_histo_path + histoname<<"."<<endmsg;
      m_histosJEC.push_back( Gaudi::Utils::Aida2ROOT::aida2root(aida) );
    }
  }
  //
  return sc ;
}


StatusCode LoKi::JetMaker::analyse   ()
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts ;

  if ( m_associate2Vertex ){

    // A cut to get the x position of the bestPV of input particles (would be better to code a VKEY functor)
    LoKi::Types::Fun bestVertexVX = BPV(VX);
    LoKi::Types::Fun bestVertexVY = BPV(VY);
    LoKi::Types::Fun bestVertexVZ = BPV(VZ);

    // A cut to check that a jet contains information able to link it to a PV
    LoKi::Types::Cut withPVPointingInfo = NINTREE(( ABSID == 310 || ABSID == 3122 )
                                                  || ( HASTRACK && LHCb::Track::Downstream != TRTYPE ))>0 ;

    Range part = select("part",PALL);

    // Loop over PV list and make jets out of appropriate inputs related to the PVs
    const LHCb::RecVertex::Range pvs = this->primaryVertices () ;
    for ( LHCb::RecVertex::Range::const_iterator i_pv = pvs.begin() ; pvs.end() != i_pv ; i_pv++ )
    {
      IJetMaker::Input inputs;
      for (Range::const_iterator i_p = part.begin() ; part.end() != i_p ; i_p++ ){
        // Neutral inputs
        if (Q(*i_p) == 0 ){
          // Take all neutrals exept V0s
          if ( ABSID(*i_p) != 310 && ABSID(*i_p) != 3122 ) inputs.push_back(*i_p) ;
          // Keep only the V0s that match to the vertex
          else if ( bestVertexVX(*i_p) == VX( *i_pv) &&
                    bestVertexVY(*i_p) == VY( *i_pv) &&
                    bestVertexVZ(*i_p) == VZ( *i_pv)  ) inputs.push_back(*i_p) ;
          else continue ;
        }
        // Charged inputs
        else{
          // Take all downstream tracks
          if ( LHCb::Track::Downstream == TRTYPE(*i_p) ) inputs.push_back(*i_p) ;
          // Keep only the tracks with velo segment that match to the vertex
          else if ( bestVertexVX(*i_p) == VX( *i_pv) &&
                    bestVertexVY(*i_p) == VY( *i_pv) &&
                    bestVertexVZ(*i_p) == VZ( *i_pv)  ) inputs.push_back(*i_p);
          else continue ;
        }
      }

      // input container of "particles"
      IJetMaker::Jets jets ;

      LoKi::Types::Fun mtf = LoKi::Cuts::INFO(9003,-10.);
      LoKi::Types::Fun n90 = LoKi::Cuts::INFO(9002,-10.);
      LoKi::Types::Fun cpf = LoKi::Cuts::INFO(9006,-10.);
      LoKi::Types::Fun width = LoKi::Cuts::INFO(9007,-10.);
      LoKi::Types::Fun nPVInfo = LoKi::Cuts::INFO(9005,-10.);
      if ( 0 == m_maker )
      { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }

      // make the jets
      StatusCode sc = m_maker->makeJets ( inputs.begin () , inputs.end   () , jets  ) ;

      if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }

      // save all jets



      while ( !jets.empty() )
      {
        LHCb::Particle* jet = jets.back() ;
        if(m_applyJEC)this->JEC(jet);
        if(m_runJetID) this->appendJetIDInfo(jet);
        // If the jet contain info on PV, assign a PV and update the P2PV relation table
        if ( withPVPointingInfo(jet) ){
          jet->setReferencePoint( Gaudi::XYZPoint((*i_pv)->position ()) );
          LHCb::Vertex* vJet = new LHCb::Vertex();
          vJet->setPosition((*i_pv)->position());
          vJet->setCovMatrix((*i_pv)->covMatrix());
          vJet->setChi2((*i_pv)->chi2());
          vJet->setNDoF((*i_pv)->nDoF());
          vJet->setOutgoingParticles(jet->daughters());
          jet->setEndVertex(vJet);
          this->relate ( jet , *i_pv );
        }
        if ( !( m_applyJetID && m_runJetID &&( mtf(jet)>0.75 || nPVInfo(jet)<2 ) ) )
        {
          sc = save ( "jets" , jet ) ;
          if ( sc.isFailure() ) { return Error ( "Error from save function in jet maker" , sc ) ; }

        }
        jets.pop_back() ;
        unRelatePV(jet);
        delete jet->endVertex();
        delete jet;
      }

    }
  }
  else{

    Range all = select ( "all" , LoKi::Cuts::PALL ) ;
    // input container of "particles"
    IJetMaker::Jets jets ;

    if ( 0 == m_maker )
    { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }


    // make the jets
    StatusCode sc = m_maker->makeJets ( all.begin () , all.end   () , jets  ) ;

    if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }

    LoKi::Types::Fun mtf = LoKi::Cuts::INFO(9003,-10.);
    LoKi::Types::Fun n90 = LoKi::Cuts::INFO(9002,-10.);
    LoKi::Types::Fun cpf = LoKi::Cuts::INFO(9006,-10.);
    LoKi::Types::Fun width = LoKi::Cuts::INFO(9007,-10.);
    LoKi::Types::Fun nPVInfo = LoKi::Cuts::INFO(9005,-10.);

    // save all jets
    while ( !jets.empty() )
    {
      LHCb::Particle* jet = jets.back() ;
      if(m_runJetID) this->appendJetIDInfo(jet);
      if (m_applyJetID && m_runJetID && ( mtf(jet)>0.75 || nPVInfo(jet)<2 )){
        jets.pop_back() ;
        delete jet ;
        continue;
      }

      sc = save ( "jets" , jet ) ;
      if ( sc.isFailure() ) { return Error ( "Error from save function in jet maker" , sc ) ; }

      jets.pop_back() ;
      delete jet ;
    }
  }

  if ( statPrint() || msgLevel ( MSG::DEBUG ) )
  { counter ( "#jets" ) += selected ("jets").size() ; }
  setFilterPassed ( true ) ;

  return StatusCode::SUCCESS ;
}

StatusCode LoKi::JetMaker::appendJetIDInfo( LHCb::Particle* jet )
{
  // Get Jet Daughters
  std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
  std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin();

  double mtf;    /// Highest pT track / Jet pT
  double cpf;    /// charged pT fraction - V0s are not included
  double width;  /// jet width
  int    n90;    /// Number of items responsible for at least 90% of the jet momentum
  int    ntrk;   /// Number of tracks

  float auxptmax=-1, sumpt=0; int iitems=0;
  double tpx=0, tpy=0;
  std::vector<float> itemspt;
  ntrk=n90=width=0;

  for (;idaughter != daughtersvector.end() ; ++idaughter){
    const LHCb::Particle * daughter = *idaughter;
    if(daughter->particleID().threeCharge()!=0) {
      ntrk++; auxptmax = daughter->momentum().Pt() > auxptmax ? daughter->momentum().Pt() : auxptmax;
      tpx+=daughter->momentum().Px(); tpy+=daughter->momentum().Py();
    }
    iitems++; float pt = daughter->momentum().Pt(); sumpt+=pt;
    itemspt.push_back(pt);
    for(int ii=0; ii<iitems; ii++) {
      if(itemspt[ii]<pt) {
        float aux = itemspt[ii]; itemspt[ii]=pt; pt = aux;
      }
    }
    width += ROOT::Math::VectorUtil::DeltaR(daughter->momentum(),jet->momentum()) * daughter->momentum().Pt();
  }

  mtf = auxptmax / jet->momentum().Pt(); mtf = 0 > mtf ? 0 : mtf; mtf = 1 < mtf ? 1 : mtf;
  cpf = std::sqrt(tpx*tpx+tpy*tpy)/jet->momentum().Pt();
  width /= sumpt;

  sort (itemspt.begin(), itemspt.end());
  float auxptsum = 0; n90=0;
  for(int ii=iitems-1; ii>=0; ii--) {auxptsum+=itemspt[ii]; n90++; if(auxptsum/sumpt>0.9) break; }

  LoKi::Types::Fun NsatCells = LoKi::Cuts::SUMTREE(LoKi::Cuts::INFO(955,0.),LoKi::Cuts::Q==0, 0.);
  LoKi::Types::Fun N_HasPVInfo = LoKi::Cuts::NINTREE(( LoKi::Cuts::ABSID == 310 ||LoKi::Cuts::ABSID == 3122 ) ||
                                                     ( LoKi::Cuts::HASTRACK && LHCb::Track::Downstream != LoKi::Cuts::TRTYPE ));


  jet->addInfo ( 9001 , ntrk );
  jet->addInfo ( 9002 , n90 );
  jet->addInfo ( 9003 , mtf );
  jet->addInfo ( 9006 , cpf );
  jet->addInfo ( 9007 , width );
  jet->addInfo ( 9004 , NsatCells(jet) );
  jet->addInfo ( 9005 , N_HasPVInfo(jet) );

  return SUCCESS;
}


StatusCode LoKi::JetMaker::JEC( LHCb::Particle* jet )
{
  const int PV = this->primaryVertices().size();
  const int usePV = ( PV > 3 ? 3 : PV );
  const TH2D* histo = m_histosJEC[usePV-1];
  const int xbin = histo->GetXaxis()->FindFixBin(LoKi::Cuts::PT(jet)/1000.);
  const int ybin = histo->GetYaxis()->FindFixBin(LoKi::Cuts::ETA(jet));
  const double cor = histo->GetBinContent(xbin,ybin);
  // Store the uncorrected kinematics

  jet->addInfo ( 9100 , cor );
  jet->addInfo ( 9101 , PV  );

  jet->setMomentum( Gaudi::LorentzVector( cor*LoKi::Cuts::PX(jet),
                                          cor*LoKi::Cuts::PY(jet),
                                          cor*LoKi::Cuts::PZ(jet),
                                          cor*LoKi::Cuts::E(jet) ) );

  return SUCCESS;

}


// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,JetMaker)
// ============================================================================
// The END
// ============================================================================
