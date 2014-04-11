// $Id: LoKiJetMakerAlg.cpp,v 1.6 2010-01-11 08:37:42 cocov Exp $
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
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
#include "Kernel/PFParticle.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"


#include "GaudiKernel/IHistogramSvc.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "AIDA/IHistogram3D.h"
#include "boost/lexical_cast.hpp"

#include "TH1D.h"
#include "TMath.h"
#include "Math/VectorUtil.h"

class TH1D;
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class PFJetMaker
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
  class PFJetMaker : public LoKi::Algo
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<LoKi::PFJetMaker> ;
    // ========================================================================
  protected:
    // ========================================================================
    /** Standard constructor
     *  @param name instance name
     *  @param pSvc pointer to Service Locator
     */
    PFJetMaker
    ( const std::string& name ,
      ISvcLocator*       pSvc )
      : LoKi::Algo ( name , pSvc )
      //
      , m_makerName ( "LoKi::FastJetMaker"   )
      , m_maker     ( 0   )
      , m_associate2Vertex ( false  )
      , m_applyJEC ( false  )
      , m_onlysavewithB ( false  )
      , m_minIPChi2PVassoc( false  )
      , m_dist(0)
      , m_minlogIPtoPV_Down(4.)
      , m_minlogIPtoPV_Up(1.5)
      , m_minlogIPtoPV_Long(1.5)
      , m_minlogIPChi2toPV_Down(7.5)
      , m_minlogIPChi2toPV_Up(9.5)
      , m_minlogIPChi2toPV_Long(9.5)
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
      declareProperty ( "ApplyJetID"  ,
			m_applyJetID = false,
			"Apply jet ID cuts") ;
      declareProperty ( "JetIDMinMPT"  ,
			m_JetID_MPT = 1200,
			"jet ID mpt cut") ;
      declareProperty ( "JetIDMinCPF"  ,
			m_JetID_CPF = 0.1,
			"jet ID cpf cut") ;
      declareProperty ( "JetIDMinNTrk"  ,
			m_JetID_NTRK = 1.5,
			"jet ID ntrk cut") ;
      declareProperty ( "JetIDMaxMPF"  ,
			m_JetID_MPF = 0.8,
			"jet ID MPF cut") ;
      declareProperty("ApplyJEC"  ,
		      m_applyJEC = false ,
		      "Apply jet energy corrections");
      declareProperty("ShiftJEC"  ,
		      m_shiftJEC = 0. ,
		      "Shift jet energy correction by n sigma");
      declareProperty("PFParticleTypes"  ,
		      m_inputTypes  ,
		      "Type of particles to consider");
      declareProperty("OnlySaveWithB"  ,
		      m_onlysavewithB  ,
		      "Only save jets containing a B meson");
      declareProperty("PVassociationMinIPChi2"  ,
		      m_minIPChi2PVassoc  ,
		      "True: will use the minIPchi2 to associate particles with pointing info to the PV, False is more refined");

      declareProperty("HistoPath"  , m_histo_path = "JEC14" , "The path of the JEC histograms" );
      //
    }
    /// destructor
    virtual ~PFJetMaker( ){}
    // ========================================================================
  public:
    // ========================================================================
    /** standard execution of the algorithm
     *  @see LoKi::Algo
     *  @return status code
     */

    enum ParticleExtraInfo{ StartJetExtraInfo =9000,
                            Ntracks = 9001 ,
                            N90 = 9002 ,
                            MTF = 9003 ,
                            NSatCalo = 9004,
                            NHasPV = 9005,
                            CPF = 9006,
                            JetWidth = 9007,
                            NSatECAL = 9008,
                            NSatHCAL = 9009,
                            NIPChi2Inf4 = 9010,
                            MPT = 9011,
                            MNF = 9012 ,
                            Charged = 9201,
                            ChargedHadron = 9202,
                            Muon  = 9203,
                            Electron = 9204,
                            Neutral  = 9205,
                            Photon = 9206,
                            Pi0 = 9207,
                            MergedPi0 = 9208,
                            ResolvedPi0 = 9209,
                            NeutralHadron = 9210,
                            NeutralRecovery = 9211 ,
                            Composite  = 9212  ,
                            V0 = 9213 ,
                            D = 9214 ,
                            B = 9215 ,
                            BadParticle = 9216  ,
                            Charged0Momentum = 9217  ,
                            ChargedInfMomentum = 9218 ,
                            BadPhotonMatchingT = 9219 ,
                            BadPhoton = 9220 ,
                            IsolatedPhoton = 9221
    };



    virtual StatusCode initialize   () ;
    virtual StatusCode analyse   () ;
    // Append the Jet ID variables to the jet extra infos
    StatusCode appendJetIDInfo   ( LHCb::Particle * jet ) ;
    // Apply the Jet energy corrections
    StatusCode JEC               ( LHCb::Particle * jet )  ;

    // ========================================================================
  private:
    // ========================================================================
    // the default constructor is disabled
    PFJetMaker () ;
    // the copy constructor is disabled
    PFJetMaker ( const PFJetMaker& )  ;
    // the assignement operator is disabled
    PFJetMaker& operator=( const  PFJetMaker& )  ;
    // ========================================================================
  private:
    // ========================================================================
    /// maker name
    std::string      m_makerName ; // jet maker name
    /// maker
    const IJetMaker* m_maker     ; // jet maker to be used
    /// associate two vertex?
    bool m_associate2Vertex      ; // make jet per vertex
    /// apply JEC
    bool m_applyJEC              ;
    float m_shiftJEC             ;
    /// append Jet ID
    bool m_applyJetID              ; // append jet ID info
    /// histograms for JEC
    TH1D* m_histosJEC[3][24][10][2] ;

    //std::vector<TH1D*> m_histosJEC ;
    /// histo path
    std::string m_histo_path ;
    /// input particles to consider
    std::vector < int > m_inputTypes ;
    bool m_onlysavewithB;
    bool m_minIPChi2PVassoc;
    const IDistanceCalculator* m_dist;

    double m_minlogIPtoPV_Down;
    double m_minlogIPtoPV_Up;
    double m_minlogIPtoPV_Long;
    double m_minlogIPChi2toPV_Down;
    double m_minlogIPChi2toPV_Up;
    double m_minlogIPChi2toPV_Long;

    double m_JetID_MPT ;
    double m_JetID_NTRK ;
    double m_JetID_CPF;
    double m_JetID_MPF ;


    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/** @file
 *  Implementation file for class  LoKi::PFJetMaker
 *  @date  2005-03-21
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
/*  standard execution of the algorithm
 *  @see LoKi::Algo
 *  @return status code
 */
// ===========================================================================
StatusCode LoKi::PFJetMaker::initialize ()
{
  StatusCode sc = LoKi::Algo::initialize() ;
  if ( sc.isFailure() ) { return sc ; }

  // Initialize the tool
  if ( 0 == m_maker ) { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }
  // Read in the histograms for JEC
  if ( m_applyJEC ){
      const int netabins=24, ncpfbins=10, nphibins=2;
      for(int inpvs=0; inpvs<3; inpvs++) for(int ieta=0; ieta<netabins; ieta++)
          for(int icpf=0; icpf<ncpfbins; icpf++) for(int iphi=0; iphi<nphibins; iphi++) {
              std::string histoname =
                  "JECSYS_PV"+boost::lexical_cast<std::string>(inpvs+1)+
                  "_ETA"+boost::lexical_cast<std::string>(ieta+20)+
                  "_CPF"+boost::lexical_cast<std::string>(icpf+1)+
                  "_PHI"+boost::lexical_cast<std::string>(iphi);
              AIDA::IHistogram1D *aida =  get<AIDA::IHistogram1D> (histoSvc(), m_histo_path + histoname);
              if( 0==aida ) warning()<<"Could not find AIDA::IHistogram1D* "
                  << m_histo_path + histoname<<"."<<endmsg;
              m_histosJEC[inpvs][ieta][icpf][iphi] = Gaudi::Utils::Aida2ROOT::aida2root(aida) ;
              //m_histosJEC.push_back( Gaudi::Utils::Aida2ROOT::aida2root(aida) );
          }
  }
  // Give some default values to the types if not defined
  if (m_inputTypes.empty()){
    m_inputTypes.push_back(LHCb::PFParticle::ChargedHadron);
    m_inputTypes.push_back(LHCb::PFParticle::Muon);
    m_inputTypes.push_back(LHCb::PFParticle::Electron);
    m_inputTypes.push_back(LHCb::PFParticle::Photon);
    m_inputTypes.push_back(LHCb::PFParticle::MergedPi0);
    m_inputTypes.push_back(LHCb::PFParticle::ResolvedPi0);
    m_inputTypes.push_back(LHCb::PFParticle::NeutralHadron);
    m_inputTypes.push_back(LHCb::PFParticle::NeutralRecovery);
    m_inputTypes.push_back(LHCb::PFParticle::V0);
    m_inputTypes.push_back(LHCb::PFParticle::D);
    m_inputTypes.push_back(LHCb::PFParticle::B);
    m_inputTypes.push_back(LHCb::PFParticle::Charged0Momentum);
    m_inputTypes.push_back(LHCb::PFParticle::ChargedInfMomentum);
    m_inputTypes.push_back(LHCb::PFParticle::IsolatedPhoton);
  }

  m_dist   = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  if ( !m_dist ) {
    fatal() << "Distance Calculator Tool could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  return sc ;
}


StatusCode LoKi::PFJetMaker::analyse   ()
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts ;

  if ( m_associate2Vertex ){


    // A cut to check that a jet contains information able to link it to a PV
    LoKi::Types::Cut withPVPointingInfo = NINTREE(( ABSID == 310 || ABSID == 3122 )
                                                  || ( HASTRACK && LHCb::Track::Downstream != TRTYPE ))>0 ;

    Range part = select("part",PALL);


    // Some definitions for jet ID
    LoKi::Types::Fun mtf = LoKi::Cuts::INFO(9003,-10.);
    LoKi::Types::Fun mnf = LoKi::Cuts::INFO(9012,-10.);
    LoKi::Types::Fun mpt = LoKi::Cuts::INFO(9011,-10.);
    LoKi::Types::Fun cpf = LoKi::Cuts::INFO(9006,-10.);
    LoKi::Types::Fun nPVInfo = LoKi::Cuts::INFO(9005,-10.);

    // A cut to get the position of the bestPV of input particles (would be better to code a VKEY functor)
    LoKi::Types::Fun bestVertexVX = BPV(VX);
    LoKi::Types::Fun bestVertexVY = BPV(VY);
    LoKi::Types::Fun bestVertexVZ = BPV(VZ);

    // Some cuts to select the inputs
    LoKi::Types::Fun PFType =  LoKi::Cuts::INFO(LHCb::PFParticle::Type,-10.);
    LoKi::Types::Cut GoodInput = fabs( PFType - m_inputTypes[0] ) < 1e-6 ;
    for ( int i = 1 ; i < (int) m_inputTypes.size() ; i++ ){
      GoodInput = GoodInput || fabs(  PFType - m_inputTypes[i] ) < 1e-6 ;
    }
    LoKi::Types::Cut PerPVinputs = ( PFType > LHCb::PFParticle::Charged && PFType < LHCb::PFParticle::Neutral && LHCb::Track::Downstream != TRTYPE )
      || ( PFType > LHCb::PFParticle::Composite  && PFType < LHCb::PFParticle::BadParticle )
      || ( PFType == LHCb::PFParticle::ChargedInfMomentum && LHCb::Track::Downstream != TRTYPE ) || ( PFType == LHCb::PFParticle::Charged0Momentum && LHCb::Track::Downstream != TRTYPE );
    LoKi::Types::Cut AllPVinputs = ( PFType > LHCb::PFParticle::Neutral && PFType < LHCb::PFParticle::Composite )
      || ( PFType == LHCb::PFParticle::BadPhotonMatchingT ) || ( PFType == LHCb::PFParticle::BadPhoton )  || ( PFType == LHCb::PFParticle::IsolatedPhoton )
      || ( PFType > LHCb::PFParticle::Charged && PFType < LHCb::PFParticle::Neutral &&  LHCb::Track::Downstream == TRTYPE )
      || ( PFType == LHCb::PFParticle::ChargedInfMomentum &&  LHCb::Track::Downstream == TRTYPE )
      || ( PFType == LHCb::PFParticle::Charged0Momentum &&  LHCb::Track::Downstream == TRTYPE );

    LoKi::Types::Cut PVPointingInfo = NINTREE( PerPVinputs )>0 ;

    // Loop over PV list and make jets out of appropriate inputs related to the PVs
    const LHCb::RecVertex::Range pvs = this->primaryVertices () ;
    for ( LHCb::RecVertex::Range::const_iterator i_pv = pvs.begin() ; pvs.end() != i_pv ; i_pv++ )
    {
      // Cut to get the particles pointing to the PV.
      const LHCb::RecVertex* pv = *i_pv;

      // Prepare the inputs
      IJetMaker::Input inputs;
      for (Range::const_iterator i_p = part.begin() ; part.end() != i_p ; i_p++ ){
        // General cut, if it is not a good input
       	if (!GoodInput(*i_p))continue;
        // If we decideded to only do the best ipchi2 association and the input is to be associated to a pv but does not point to this pv, discard
        if ( m_minIPChi2PVassoc && PerPVinputs(*i_p) && ( std::abs( bestVertexVZ(*i_p) - VZ(pv) ) > 1e-6
                                    || std::abs( bestVertexVY(*i_p) - VY(pv) ) > 1e-6
                                    || std::abs( bestVertexVX(*i_p) - VX(pv) ) > 1e-6 ) ){continue;}
        // Otherwise (if it does point to this pv) take it
        else if ( m_minIPChi2PVassoc && PerPVinputs(*i_p) )	inputs.push_back(*i_p);
        // Then if it is of the type to ba added to all pv, just add it to all pv
        else if ( m_minIPChi2PVassoc && AllPVinputs(*i_p) )	inputs.push_back(*i_p);
        else if (!m_minIPChi2PVassoc && PerPVinputs(*i_p) ){
          // First compute the ip and ipchi2 wrt this pv
          double ip(1e8), chi2(1e8);
          StatusCode dist = m_dist->distance ( (*i_p), pv, ip, chi2 );
          if (ip<0. || chi2<0. ) continue;
          double thisPVchi2 = chi2;
          // Cut for long tracks
          // If the ip and ipchi2 are below a maximal value, and no
          if ( (*i_p)->proto()!= NULL && (*i_p)->proto()->track()->type()== LHCb::Track::Long && ip< TMath::Log( m_minlogIPtoPV_Long) &&  TMath::Log(chi2) <  m_minlogIPChi2toPV_Long ){
            bool useIt = true;
            for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
              if (i_pv2 == i_pv) continue;
              dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
              if ( chi2<0. ) continue;
              if( chi2<thisPVchi2){
                useIt = false;
                break;
              }
            }
            if (useIt) inputs.push_back(*i_p);
          }
          else if( (*i_p)->proto()!= NULL && ((*i_p)->proto()->track()->type()== LHCb::Track::Upstream || (*i_p)->proto()->track()->type()== LHCb::Track::Velo) &&
                   TMath::Log(ip) < m_minlogIPtoPV_Up &&  TMath::Log(chi2) < m_minlogIPChi2toPV_Up ){
            bool useIt = true;
            for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
              if (i_pv2 == i_pv) continue;
              dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
              if (ip<0. || chi2<0. ) continue;
              if( chi2<thisPVchi2){
                useIt = false;
                break;
              }
            }
            if (useIt) inputs.push_back(*i_p);
          }
          else{
           bool useIt = true;
            for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
              if (i_pv2 == i_pv){
                continue;
              }
              dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
              if ( chi2<0. ) continue;
              if( chi2<thisPVchi2){
                useIt = false;
                break;
              }
            }
            if (useIt) inputs.push_back(*i_p);
          }
          // make the ip selection
        }
        else if (!m_minIPChi2PVassoc && AllPVinputs(*i_p)){
          // make the ip selection for downstream
          if (  ( PFType(*i_p) > LHCb::PFParticle::Charged && PFType(*i_p) < LHCb::PFParticle::Neutral )
                || ( PFType(*i_p) == LHCb::PFParticle::ChargedInfMomentum )
                || ( PFType(*i_p) == LHCb::PFParticle::Charged0Momentum )){

            if( (*i_p)->proto()->track()->type()== LHCb::Track::Downstream){
              double ip(1e8), chi2(1e8);
              StatusCode dist = m_dist->distance ( (*i_p), pv , ip, chi2 );
              double thisPVchi2 = chi2;
              if ( chi2<0. ) continue;
              if( TMath::Log(ip) < m_minlogIPtoPV_Down &&  TMath::Log(chi2) < m_minlogIPChi2toPV_Down ){
                bool useIt = true;
                for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
                  if (i_pv2 == i_pv) continue;
                  dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
                  if (ip<0. || chi2<0. ) continue;
                  if( chi2<thisPVchi2){
                    useIt = false;
                    break;
                  }
                }
                if (useIt) inputs.push_back(*i_p);
              }
            }
          }
          else{
            inputs.push_back(*i_p);
          }
        }
        else if (m_minIPChi2PVassoc && AllPVinputs(*i_p)){
            inputs.push_back(*i_p);
        }
        else{
          continue;
        }
      }

      // ouput container
      IJetMaker::Jets jets ;
      // make the jets
      StatusCode sc = m_maker->makeJets ( inputs.begin () , inputs.end   () , jets  ) ;

      if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }

      // save all jets
      while ( !jets.empty() )
      {
        LHCb::Particle* jet = jets.back() ;
        this->appendJetIDInfo(jet);
        if(m_applyJEC) this->JEC(jet);
        // If the jet contain info on PV, assign a PV and update the P2PV relation table
        if ( PVPointingInfo(jet) ){

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

	//    if (m_applyJetID && ( mtf(jet)>0.75 || nPVInfo(jet)<2 || mpt(jet)<1800 )){

        if ( !( m_applyJetID && ( mpt(jet)< m_JetID_MPT  || nPVInfo(jet)<   m_JetID_NTRK ||  cpf(jet) <  m_JetID_CPF || std::max(mtf(jet),mnf(jet))  >  m_JetID_MPF  ) ) )
	  {

	    if (!( m_onlysavewithB && jet->info(B,-100.)<1.e-6)){
	      verbose()<<PT(jet)<<" "<<jet->info(B,-100.)<<" "<<ID(jet)<<endreq;
	      sc = save ( "jets" , jet ) ;
	      if ( sc.isFailure() ) { return Error ( "Error from save function in jet maker" , sc ) ; }
	    }
	  }
        jets.pop_back() ;
        unRelatePV(jet);
        delete jet->endVertex();
        delete jet;


      }
    }
  }
  else{
    // Some definitions for jet ID
    LoKi::Types::Fun mtf = LoKi::Cuts::INFO(9003,-10.);
    LoKi::Types::Fun mnf = LoKi::Cuts::INFO(9012,-10.);
    LoKi::Types::Fun mpt = LoKi::Cuts::INFO(9011,-10.);
    LoKi::Types::Fun cpf = LoKi::Cuts::INFO(9006,-10.);
    LoKi::Types::Fun nPVInfo = LoKi::Cuts::INFO(9005,-10.);
    LoKi::Types::Fun PFType =  LoKi::Cuts::INFO(900,-10.);
    LoKi::Types::Cut GoodInput = fabs( PFType - m_inputTypes[0] ) < 1e-6 ;
    for ( int i = 1 ; i < (int) m_inputTypes.size() ; i++ ){
      GoodInput = GoodInput || fabs(  PFType - m_inputTypes[i] ) < 1e-6 ;
    }
    // The inputs range
    Range part = select("input4jets",PALL && GoodInput );
    IJetMaker::Input inputs;
    for (Range::const_iterator i_p = part.begin() ; part.end() != i_p ; i_p++ ){ inputs.push_back(*i_p); }
    // ouput container
    IJetMaker::Jets jets ;
    // make the jets
    StatusCode sc = m_maker->makeJets ( inputs.begin () , inputs.end   () , jets  ) ;
    if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }
    // save all jets
    while ( !jets.empty() )
    {
      LHCb::Particle* jet = jets.back() ;
      this->appendJetIDInfo(jet);


      if ( !( m_applyJetID && ( mpt(jet)< m_JetID_MPT  || nPVInfo(jet)<   m_JetID_NTRK ||  cpf(jet) <  m_JetID_CPF || std::max(mtf(jet),mnf(jet))  >  m_JetID_MPF  ) ) )
	  {

	    if (!( m_onlysavewithB && jet->info(B,-100.)<1.e-6)){
	      verbose()<<PT(jet)<<" "<<jet->info(B,-100.)<<" "<<ID(jet)<<endreq;
	      sc = save ( "jets" , jet ) ;
	      if ( sc.isFailure() ) { return Error ( "Error from save function in jet maker" , sc ) ; }
	    }
	  }
        jets.pop_back() ;
        delete jet;


    }
  }
  if ( statPrint() || msgLevel ( MSG::DEBUG ) )
  { counter ( "#jets" ) += selected ("jets").size() ; }
  setFilterPassed ( true ) ;

  return StatusCode::SUCCESS ;
}

StatusCode LoKi::PFJetMaker::appendJetIDInfo( LHCb::Particle* jet )
{
  // Get Jet Daughters
  std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
  std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin();

  double mtf;    /// Highest pT track / Jet pT
  double mnf;    /// Highest pT neutral part / Jet pT
  double mpt;    /// Highest pT track
  double cpf;    /// charged pT fraction - V0s are not included
  double width;  /// jet width
  int    n90;    /// Number of items responsible for at least 90% of the jet momentum
  int    ntrk;   /// Number of tracks

  float auxptmax=-1, auxptPmax=-1,sumpt=0;
  int iitems=0;
  double tpx=0, tpy=0;
  std::vector<float> itemspt;
  ntrk=n90=width=0;


  for (;idaughter != daughtersvector.end() ; ++idaughter){
    const LHCb::Particle * daughter = *idaughter;
    if(daughter->particleID().threeCharge()!=0) {
      ntrk++; auxptmax = daughter->momentum().Pt() > auxptmax ? daughter->momentum().Pt() : auxptmax;
      tpx+=daughter->momentum().Px(); tpy+=daughter->momentum().Py();
    }else{
      auxptPmax = daughter->momentum().Pt() > auxptPmax ? daughter->momentum().Pt() : auxptPmax;
    }
    iitems++; float pt = daughter->momentum().Pt(); sumpt+=pt;
    itemspt.push_back(pt);
    for(int ii=0; ii<iitems; ii++) if(itemspt[ii]<pt) {
      float aux = itemspt[ii]; itemspt[ii]=pt; pt = aux;}
    width += ROOT::Math::VectorUtil::DeltaR(daughter->momentum(),jet->momentum()) * daughter->momentum().Pt();
  }

  mtf = auxptmax / jet->momentum().Pt(); mtf = 0 > mtf ? 0 : mtf; mtf = 1 < mtf ? 1 : mtf;
  mnf = auxptPmax / jet->momentum().Pt(); mnf = 0 > mnf ? 0 : mnf; mnf = 1 < mnf ? 1 : mnf;

  mpt = auxptmax;
  cpf = TMath::Sqrt(tpx*tpx+tpy*tpy)/jet->momentum().Pt();
  width /= sumpt;

  sort (itemspt.begin(), itemspt.end());
  float auxptsum = 0; n90=0;
  for(int ii=iitems-1; ii>=0; ii--) {auxptsum+=itemspt[ii]; n90++; if(auxptsum/sumpt>0.9) break; }

  LoKi::Types::Fun PFType =  LoKi::Cuts::INFO(LHCb::PFParticle::Type,-10.);
  // Function to evaluate the saturated cells Number
  LoKi::Types::Fun NECALsatCells = LoKi::Cuts::SUMTREE( LoKi::Cuts::INFO(NSatECAL,0.),LoKi::Cuts::INFO(NSatECAL,-10.)>0., 0.);
  LoKi::Types::Fun NHCALsatCells = LoKi::Cuts::SUMTREE( LoKi::Cuts::INFO(NSatHCAL,0.),LoKi::Cuts::INFO(NSatHCAL,-10.)>0., 0.);
  LoKi::Types::Fun NsatCells = NECALsatCells + NHCALsatCells ;
  // Function to get the number of particles with PV information
  LoKi::Types::Fun BestPVIPChi2  = LoKi::Cuts::BPVIPCHI2 ("")  ;
  LoKi::Types::Fun N_HasPVInfo   = LoKi::Cuts::NINTREE( ( PFType > LHCb::PFParticle::Composite  && PFType < LHCb::PFParticle::BadParticle ) ||
							( LoKi::Cuts::HASTRACK && LHCb::Track::Downstream != LoKi::Cuts::TRTYPE ) );
  LoKi::Types::Fun N_HasPVInfoChi24   = LoKi::Cuts::NINTREE(( ( PFType > LHCb::PFParticle::Composite  && PFType < LHCb::PFParticle::BadParticle ) ||
							      ( LoKi::Cuts::HASTRACK && LHCb::Track::Downstream != LoKi::Cuts::TRTYPE )) && BestPVIPChi2 < 9 );

  jet->addInfo ( Ntracks , ntrk );
  jet->addInfo ( N90 , n90 );
  jet->addInfo ( MTF , mtf );
  jet->addInfo ( MNF , mnf );
  jet->addInfo ( MPT , mpt );
  jet->addInfo ( NSatCalo , NsatCells(jet) );
  jet->addInfo ( NHasPV   , N_HasPVInfo(jet) );
  jet->addInfo ( CPF , cpf );
  jet->addInfo ( JetWidth , width );
  jet->addInfo ( NSatECAL , NECALsatCells(jet) );
  jet->addInfo ( NSatHCAL , NHCALsatCells(jet) );
  jet->addInfo ( NIPChi2Inf4 , N_HasPVInfoChi24(jet) );

  LoKi::Types::Fun FractionChargedHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::ChargedHadron ) < 1e-6 , 0. );
  jet->addInfo ( ChargedHadron  , FractionChargedHadron(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionMuon = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Muon ) < 1e-6 , 0. );
  jet->addInfo ( Muon  , FractionMuon(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionElectron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Electron ) < 1e-6 , 0. );
  jet->addInfo ( Electron  , FractionElectron(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionCharged = FractionChargedHadron + FractionMuon + FractionElectron;
  jet->addInfo ( Charged  , FractionCharged(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Photon ) < 1e-6 , 0. );
  jet->addInfo (  Photon , FractionPhoton(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Pi0 ) < 1e-6 , 0. );
  jet->addInfo ( Pi0  , FractionPi0(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionMergedPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::MergedPi0 ) < 1e-6 , 0. );
  jet->addInfo ( MergedPi0  , FractionMergedPi0(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionResolvedPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::ResolvedPi0 ) < 1e-6 , 0. );
  jet->addInfo ( ResolvedPi0  , FractionResolvedPi0(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionNeutralHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::NeutralHadron ) < 1e-6 , 0. );
  jet->addInfo (  NeutralHadron , FractionNeutralHadron(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionNeutralRecovery = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::NeutralRecovery ) < 1e-6 , 0. );
  jet->addInfo ( NeutralRecovery  , FractionNeutralRecovery(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionIsolatedPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::IsolatedPhoton ) < 1e-6 , 0. );
  jet->addInfo (  IsolatedPhoton , FractionIsolatedPhoton(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionNeutral = FractionPhoton + FractionPi0 + FractionMergedPi0 + FractionResolvedPi0 + FractionNeutralHadron + FractionNeutralRecovery + FractionIsolatedPhoton;
  jet->addInfo ( Neutral  , FractionNeutral(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionV0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::V0 ) < 1e-6 , 0. );
  jet->addInfo ( V0  , FractionV0(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionD = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::D ) < 1e-6 , 0. );
  jet->addInfo ( D  , FractionD(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionB = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::B ) < 1e-6 , 0. );
  jet->addInfo ( B  , FractionB(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionComposite = FractionV0 + FractionD + FractionB ;
  jet->addInfo ( Composite  , FractionComposite(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionCharged0Momentum = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Charged0Momentum ) < 1e-6 , 0. );
  jet->addInfo ( Charged0Momentum  , FractionCharged0Momentum(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionChargedInfMomentum = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::ChargedInfMomentum ) < 1e-6 , 0. );
  jet->addInfo ( ChargedInfMomentum  , FractionChargedInfMomentum(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionBadPhotonMatchingT = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::BadPhotonMatchingT ) < 1e-6 , 0. );
  jet->addInfo ( BadPhotonMatchingT  , FractionBadPhotonMatchingT(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionBadPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::BadPhoton ) < 1e-6 , 0. );
  jet->addInfo (  BadPhoton , FractionBadPhoton(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionBadParticle = FractionCharged0Momentum + FractionChargedInfMomentum + FractionBadPhotonMatchingT + FractionBadPhoton;
  jet->addInfo ( BadParticle  , FractionBadParticle(jet)/LoKi::Cuts::E(jet) );

  return SUCCESS;
}


StatusCode LoKi::PFJetMaker::JEC( LHCb::Particle* jet )
{
  int PV = this->primaryVertices().size();
  int usePV = PV;
  if (PV > 3)usePV = 3;

  float jetpt = LoKi::Cuts::PT(jet)/1000.;
  float jeteta = LoKi::Cuts::ETA(jet);
  float jetphi = LoKi::Cuts::PHI(jet);
  float jetcpf = jet->info(CPF,-1.);

  if(jetpt<5)    jetpt=5;
  if(jetpt>299)  jetpt=298;
  if(jeteta<2.0) jeteta=2.0;
  if(jeteta>4.4) jeteta=4.5;

  int jec_eta=(int)(jeteta*10-20.5), jec_cpf=(int)(jetcpf*10-0.5);
  int jec_phi=0; jetphi = TMath::Abs(jetphi);
  if(jetphi<TMath::Pi()/4.||jetphi>3*TMath::Pi()/4.) jec_phi=0;
  if(TMath::Abs(jetphi-TMath::Pi()/2.)<TMath::Pi()/4.) jec_phi=1;
  if(jec_cpf>9) jec_cpf=9;
  if(jec_eta>23) jec_eta=23;

  TH1D* histo = m_histosJEC[usePV-1][jec_eta][jec_cpf][jec_phi];

  double cor = 1.;
  cor = histo->Interpolate(jetpt);
  double corerr = histo->GetBinError(histo->FindBin(jetpt));
  cor+=m_shiftJEC*corerr;
  // Store the uncorrected kinematics

  jet->addInfo ( 9100 , cor );
  jet->addInfo ( 9101 , PV);
  jet->addInfo ( 9102 , corerr );

  Gaudi::LorentzVector newMom(cor*LoKi::Cuts::PX(jet),cor*LoKi::Cuts::PY(jet),cor*LoKi::Cuts::PZ(jet),cor*LoKi::Cuts::E(jet));
  jet->setMomentum(newMom);

  return SUCCESS;
}


// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,PFJetMaker)
// ============================================================================
// The END
// ============================================================================
