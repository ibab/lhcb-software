// Include files 
#include "TaggerKaonSameTool.h"
#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerKaonSameTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerKaonSameTool )

//====================================================================
  TaggerKaonSameTool::TaggerKaonSameTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent ) :
    GaudiTool ( type, name, parent ),
    m_myMCreader( NULL),
    m_util( NULL )
{
  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName   = "NNetTool_MLP" );
  declareProperty( "KaonSame_AverageOmega",    m_AverageOmega   = 0.33 );

  declareProperty( "KaonSame_Pt_cut", m_Pt_cut_kaonS = 0.85 *GeV );
  declareProperty( "KaonSame_P_cut",  m_P_cut_kaonS  = 5.25 *GeV );
  declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS = 4.125 );
  declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS = 0.825 );
  declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS = 0.6 );
  declareProperty( "KaonSame_dR_cut", m_dRcut_kaonS= 10. );
  declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS  = 1.850 *GeV);
  declareProperty( "KaonSame_lcs_cut",   m_lcs_cut      = 3.75 );
  declareProperty( "KaonSame_ipPU_cut", m_ipPU_cut_kS      = 3.0 );
  declareProperty( "KaonSame_distPhi_cut", m_distPhi_cut_kS= 0.005 );
  declareProperty( "KaonSame_PIDk_cut", m_KaonSPID_kS_cut  =  3.5 );
  declareProperty( "KaonSame_PIDkp_cut",m_KaonSPID_kpS_cut = -8.5 );

  declareProperty( "KaonSame_ProbMin",   m_ProbMin_kaonS    = 0. ); //no cut
  // 1fb-1 paper tuning
  declareProperty( "KaonSame_P0_Cal",  m_P0_Cal_kaonS   = 0.350 );
  declareProperty( "KaonSame_P1_Cal",  m_P1_Cal_kaonS   = 0.51 );
  declareProperty( "KaonSame_Eta_Cal", m_Eta_Cal_kaonS  = 0.324 );
  declareProperty( "isMonteCarlo",     m_isMonteCarlo  = 0 );

  declareProperty( "P0_ks_scale", m_P0ks =  1.22418); //dec2011_v2
  declareProperty( "P1_ks_scale", m_P1ks = -1.63297);
  declareProperty( "P2_ks_scale", m_P2ks =  0.401361);
  declareProperty( "P3_ks_scale", m_P3ks =  0.);

  m_nnet = 0;
  m_descend = 0;
}

TaggerKaonSameTool::~TaggerKaonSameTool() {}

//=====================================================================
StatusCode TaggerKaonSameTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "KSS calib ctt: P0_Cal "<<m_P0_Cal_kaonS
            <<", P1_Cal "<<m_P1_Cal_kaonS<<endmsg;

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  if (m_isMonteCarlo) {
    std::vector<std::string> variable_names;
    
    variable_names.push_back("mult");      
    variable_names.push_back("ptB");       
    variable_names.push_back("partPt");    
    variable_names.push_back("IPs");       
    variable_names.push_back("nndeta");    
    variable_names.push_back("nndphi");    
    variable_names.push_back("nndq");      
    variable_names.push_back("nnkrec");    
    variable_names.push_back("nndr");      
    m_myMCreader = new MCKaonSSWrapper(variable_names);
  }
  
  return sc;
}
//================================================================================
StatusCode  TaggerKaonSameTool::finalize()
{
  if(m_isMonteCarlo==1){
    delete m_myMCreader; 
    m_myMCreader = NULL;
  }
  
  return GaudiTool::finalize();
}

//=====================================================================
Tagger TaggerKaonSameTool::tag( const Particle* AXB0, 
                                const RecVertex* RecVert, 
                                const int nPV,
                                Particle::ConstVector& vtags )
{
  Tagger tkaonS;
  if(!RecVert) return tkaonS;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--Kaon SS Tagger--"<<endmsg;

  const Gaudi::LorentzVector& ptotB = AXB0->momentum();

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaonS=0;
  double ptmaxkS = -99.0, ncand=0;
  double save_dphi=0, save_dQ=0, save_IPsig=0;

  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ++ipart )
  {

    const double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );
    const double pidp=(*ipart)->proto()->info( ProtoParticle::CombDLLp, -1000.0 );

    if(pidk==0) continue;
    if(pidk < m_KaonSPID_kS_cut ) continue;
    if(pidk - pidp < m_KaonSPID_kpS_cut ) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<"     KaonS PIDk="<<pidk<<" PIDp="<<pidp <<endmsg;

    const double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_kaonS )  continue;

    const double P  = (*ipart)->p();
    if( P < m_P_cut_kaonS )  continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<" KaonS P="<<P<<" Pt="<<Pt <<endmsg;

    const Track* track = (*ipart)->proto()->track();
    const double lcs = track->chi2PerDoF();
    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<" KaonS lcs1="<<lcs<<endmsg;
    if( lcs > m_lcs_cut ) continue;
    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<" KaonS lcs2="<<lcs<<endmsg;

    //calculate signed IP wrt RecVert
    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = IP/IPerr;
    if(fabs(IPsig) > m_IP_cut_kaonS) continue;
    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<" KaonS IPs="<<IPsig<<endmsg;

    const double ippu=(*ipart)->info(LHCb::Particle::FlavourTaggingIPPUs,100000.);
    if(ippu < m_ipPU_cut_kS) continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if ( msgLevel(MSG::DEBUG) )
      debug()<<" distPhi="<<distphi<<endmsg;
    if( distphi < m_distPhi_cut_kS ) continue;

    const double deta = std::fabs(std::log(std::tan(ptotB.Theta()/2.)/std::tan(asin(Pt/P)/2.)));
    const double dphi = std::fabs(TaggingHelpers::dphi((*ipart)->momentum().Phi(), ptotB.Phi()));
    const double dR = std::sqrt(deta*deta+dphi*dphi);

    if(deta > m_etacut_kaonS) continue;
    if(dphi > m_phicut_kaonS) continue;
    if(dR > m_dRcut_kaonS) continue;

    const Gaudi::LorentzVector& pm  = (*ipart)->momentum();
    const double E = std::sqrt(pm.P() * pm.P()+ 493.677*MeV * 493.677*MeV);
    Gaudi::LorentzVector pmK ( pm.Px(),pm.Py(),pm.Pz(), E);

    const double dQ    = (ptotB+pmK).M() - ptotB.M();
    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<" KaonS Deta="<<deta<<" KaonS dQ="<<dQ
               <<" Dphi="<<dphi<<endmsg;

    if(dQ > m_dQcut_kaonS ) continue;

    ++ncand;

    if( Pt > ptmaxkS ) 
    {
      ikaonS  = (*ipart);
      ptmaxkS = Pt;
      save_dphi=dphi;
      save_dQ=dQ;
      save_IPsig=IPsig;
    }

  }
  if( !ikaonS ) return tkaonS;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") 
  {

    const double ang  = std::asin(ikaonS->pt()/ikaonS->p());
    const double deta = std::log(std::tan(ptotB.Theta()/2))-std::log(std::tan(ang/2));
    const double dphi = std::fabs(TaggingHelpers::dphi(ikaonS->momentum().Phi(), ptotB.Phi()));
    const double dR   = std::sqrt(deta*deta+dphi*dphi);
    if(m_isMonteCarlo ) {
      std::vector<double> inputVals;
      inputVals.push_back( (double)m_util->countTracks(vtags));
      inputVals.push_back( (double)log(AXB0->pt()/GeV));
      inputVals.push_back( (double)log(ikaonS->pt()/GeV));
      inputVals.push_back( (double)log(fabs(save_IPsig)));
      inputVals.push_back( (double)deta);
      inputVals.push_back( (double)save_dphi);
      inputVals.push_back( (double)save_dQ/GeV);
      inputVals.push_back( (double)nPV);
      inputVals.push_back( (double)dR);

      double rnet = m_myMCreader->GetMvaValue(inputVals);
      if (rnet>=0 && rnet<=1) {
        pn = 1.0 -TaggingHelpers::funcNN(rnet, m_P0ks, m_P1ks, m_P2ks, m_P3ks);
      } else {
        debug()<<"**********************BAD TRAINING kSame"<<rnet<<endmsg;
        pn = -1.;
      }


      if ( msgLevel(MSG::DEBUG) )
      {
        debug()<<" TaggerKaonSS: "<<ikaonS->charge()<<" omega="<<1-pn<<" ";
        for(unsigned int iloop=0; iloop<inputVals.size(); iloop++){
          debug() << inputVals[iloop]<<" ";
        }
        debug()<<endmsg;
      }
   
    }else{ // Old format for the moment
      std::vector<double> NNinputs(10);
      NNinputs.at(0) = m_util->countTracks(vtags);
      NNinputs.at(1) = AXB0->pt()/GeV;
      //    NNinputs.at(2) = ikaonS->p()/GeV;
      NNinputs.at(3) = ikaonS->pt()/GeV;
      NNinputs.at(4) = save_IPsig;
      NNinputs.at(5) = deta;
      NNinputs.at(6) = save_dphi;
      NNinputs.at(7) = save_dQ/GeV;
      NNinputs.at(8) = nPV;
      NNinputs.at(9) = dR;      
      pn = m_nnet->MLPkS( NNinputs );
      
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose()<<" KaonS pn inputs="<<NNinputs<<endmsg;
        verbose()<<" KaonS pn ="<<pn<<endmsg;
      }
    }
    
    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_kaonS - m_P1_Cal_kaonS * ( (1-pn)-m_Eta_Cal_kaonS);
    if ( msgLevel(MSG::DEBUG) )
      debug() << " SS Kaon pn="<< pn <<" w="<<1-pn<<endmsg;

    if( pn < 0 || pn > 1 ) return tkaonS;
    if( pn < m_ProbMin_kaonS ) return tkaonS;

  }

  tkaonS.setOmega( 1-pn );
  tkaonS.setDecision(ikaonS->charge()>0 ? 1: -1);
  tkaonS.setType( Tagger::SS_Kaon );
  tkaonS.addToTaggerParts(ikaonS);

  return tkaonS;
}
//==========================================================================
