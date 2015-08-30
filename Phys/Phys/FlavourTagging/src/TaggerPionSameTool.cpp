// Include files 
#include "TaggerPionSameTool.h"
#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerPionSameTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerPionSameTool )

//====================================================================
TaggerPionSameTool::TaggerPionSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ),
  m_myMCreader( NULL ),
  m_util( NULL)

{
  declareInterface<ITagger>(this);

  declareProperty( "CombTech", m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName  = "NNetTool_MLP" );

  declareProperty( "PionSame_Pt_cut",     m_Pt_cut_pionS  = 0.6 *GeV );
  declareProperty( "PionSame_P_cut",      m_P_cut_pionS   = 4.0 *GeV );
  declareProperty( "PionSame_IPs_cut",    m_IPs_cut_pionS = 9. );
  declareProperty( "PionSame_lcs_cut",    m_lcs_cut       = 5.0 );
  declareProperty( "PionSame_deta_max_cut", m_eta_max_cut_pionS   = 0.35);
  declareProperty( "PionSame_deta_min_cut", m_eta_min_cut_pionS   = -0.5);
  declareProperty( "PionSame_dphi_cut",   m_phi_cut_pionS   = 0.75);
  declareProperty( "PionSame_dR_cut",     m_dR_cut_pionS   = 0.75);
  declareProperty( "PionSame_dQ_cut",     m_dQcut_pionS   = 2.5 *GeV);
  declareProperty( "PionSame_dQ_extra_cut", m_dQcut_extra_pionS = 1.2 *GeV);
  declareProperty( "PionSame_ghost_cut",  m_ghost_cut     = -999.0);
  declareProperty( "PionSame_ipPU_cut",   m_ipPU_cut_pS      = 6.0 );
  declareProperty( "PionSame_distPhi_cut",m_distPhi_cut_pS= 0.005 );
  declareProperty( "PionSame_PIDNoK_cut", m_PionSame_PIDNoK_cut = 4.3);
  declareProperty( "PionSame_PIDNoP_cut", m_PionSame_PIDNoP_cut = 14.0);
  // 1fb-1 paper tuning
  //declareProperty( "PionSame_P0_Cal",  m_P0_Cal_pionS   = 0.388 );
  //declareProperty( "PionSame_P1_Cal",  m_P1_Cal_pionS   = 0.88 );
  //declareProperty( "PionSame_P2_Cal",  m_P2_Cal_pionS   = -2.06 );
  //declareProperty( "PionSame_Eta_Cal", m_Eta_Cal_pionS  = 0.361 );

  // Last value from D*munu analysis with 10% B+ background
  //declareProperty( "PionSame_P0_Cal",  m_P0_Cal_pionS   = 0.4049 );
  //declareProperty( "PionSame_P1_Cal",  m_P1_Cal_pionS   = 0.83   );
  //declareProperty( "PionSame_P2_Cal",  m_P2_Cal_pionS   = -1.99  );
  //declareProperty( "PionSame_Eta_Cal", m_Eta_Cal_pionS  = 0.3908 );

  // Last value from Bd2JpsiK* Reco14 analysis
  declareProperty( "PionSame_P0_Cal",  m_P0_Cal_pionS   = 0.4493 );
  declareProperty( "PionSame_P1_Cal",  m_P1_Cal_pionS   = 0.7794 );
  declareProperty( "PionSame_P2_Cal",  m_P2_Cal_pionS   = -1.8686  );
  declareProperty( "PionSame_Eta_Cal", m_Eta_Cal_pionS  = 0.3908 );

  declareProperty( "PionSame_AverageOmega",  m_AverageOmega   = 0.40 );
  declareProperty( "PionSame_ProbMin",       m_PionProbMin   = 0.56);
  declareProperty( "isMonteCarlo",       m_isMonteCarlo   = 0);
  declareProperty( "P0_ps_scale", m_P0ps =  1.22123);
  declareProperty( "P1_ps_scale", m_P1ps = -1.76027);
  declareProperty( "P2_ps_scale", m_P2ps =  0.651766);
  declareProperty( "P3_ps_scale", m_P3ps =  0.);

  m_nnet = 0;
  m_descend = 0;

}

TaggerPionSameTool::~TaggerPionSameTool() {}

//=====================================================================
StatusCode TaggerPionSameTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "PiSS calib ctt: P0_Cal "<<m_P0_Cal_pionS
            << ", P1_Cal "<<m_P1_Cal_pionS<<endmsg;

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  
  if (m_isMonteCarlo) {
    std::vector<std::string> variable_names;
    variable_names.push_back("mult");    
    variable_names.push_back("ptB");     
    variable_names.push_back("partPt");  
    variable_names.push_back("IPs");     
    variable_names.push_back("nndr");    
    variable_names.push_back("nndq");    
    variable_names.push_back("nnkrec");  

    m_myMCreader = new MCPionSSWrapper(variable_names);
  }

  return sc;
}
//================================================================================
StatusCode  TaggerPionSameTool::finalize()
{
  if(m_isMonteCarlo==1){
    delete m_myMCreader; 
    m_myMCreader = NULL;
  }
  
  return GaudiTool::finalize();
}

//=====================================================================
Tagger TaggerPionSameTool::tag( const Particle* AXB0, const RecVertex* RecVert, 
                                const int nPV,
                                Particle::ConstVector& vtags )
{

  Tagger tpionS;
  if(!RecVert) return tpionS;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--Pion SS Tagger--"<<endmsg;

  const Gaudi::LorentzVector& ptotB = AXB0->momentum();
  double B0mass = ptotB.M();

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select pionS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle * ipionS = NULL;
  double ptmaxpS = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart, jpart;
  for ( ipart = vtags.begin(); ipart != vtags.end(); ++ipart )
  {
    //PID cuts to select the pion
    const ProtoParticle* proto = (*ipart)->proto();
    const double PIDk = proto->info( ProtoParticle::CombDLLk,  -1000.0 );
    const double PIDp = proto->info( ProtoParticle::CombDLLp,  -1000.0 );
    bool pidpass=false;
    if( PIDk==0 ) pidpass=true;
    if( PIDk!=0 ) if(PIDk < m_PionSame_PIDNoK_cut &&
                     PIDp < m_PionSame_PIDNoP_cut) pidpass=true;
    if( (*ipart)->particleID().abspid() != 211 ) continue;
    if(!pidpass) continue;
    const Track* track = proto->track();
    if( track->type() != Track::Long ) continue;


    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<" Pion PIDk="<< PIDk <<endmsg;

    const double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_pionS )  continue;
    const double P  = (*ipart)->p();
    if( P  < m_P_cut_pionS )  continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose()<<" Pion P="<< P <<" Pt="<< Pt <<endmsg;


    const double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut ) continue;

    const double tsa = track->likelihood();
    if(tsa < m_ghost_cut) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Pion lcs="<< lcs <<" tsa="<<tsa<<endmsg;

    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = std::fabs(IP/IPerr);
    if(IPsig > m_IPs_cut_pionS)  continue;

    const double ippu=(*ipart)->info(LHCb::Particle::FlavourTaggingIPPUs,100000.);
    if(ippu < m_ipPU_cut_pS) continue;


    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_pS ) continue;

    const double dQ = (ptotB+(*ipart)->momentum()).M() - B0mass;
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Pion IPs="<< IPsig <<" dQ="<<dQ<<endmsg;
    if(dQ > m_dQcut_pionS ) continue;

    const double deta = std::fabs(log(tan(ptotB.Theta()/2.)/tan(asin(Pt/P)/2.)));
    const double dphi = std::fabs(TaggingHelpers::dphi((*ipart)->momentum().Phi(), ptotB.Phi()));
    const double dR = std::sqrt(deta*deta+dphi*dphi);

    if(deta > m_eta_max_cut_pionS) continue;
    if(deta < m_eta_min_cut_pionS) continue;
    if(dphi > m_phi_cut_pionS) continue;
    if(dR > m_dR_cut_pionS) continue;


    ++ncand;

    if( Pt <= ptmaxpS ) continue;//equal sign in "<=" is used to kill duplicates

    //accept candidate
    ipionS = (*ipart);
    ptmaxpS = Pt;

  }
  if( !ipionS ) return tpionS;

  const double extra_dQ = (ptotB+ipionS->momentum()).M() - B0mass;
  if( extra_dQ > m_dQcut_extra_pionS ) return tpionS;
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " Pion dQExtra="<< extra_dQ <<endmsg;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if ( m_CombinationTechnique == "NNet" ) 
  {

    //calculate omega
    double IP(0), IPerr(0);
    m_util->calcIP(ipionS, RecVert, IP, IPerr);
    const double B0the= ptotB.Theta();
    const double B0phi= ptotB.Phi();
    const double ang = std::asin((ipionS->pt())/(ipionS->p()));
    const double deta= std::log(std::tan(B0the/2.))-std::log(std::tan(ang/2.));
    const double dphi= std::fabs(TaggingHelpers::dphi(ipionS->momentum().Phi(), B0phi));
    const double dQ  = ((ptotB+ ipionS->momentum() ).M() - B0mass);
    const double dR  = std::sqrt(deta*deta+dphi*dphi);

    if(m_isMonteCarlo) {
      std::vector<double> inputVals;
      inputVals.push_back( (double)m_util->countTracks(vtags));
      inputVals.push_back( (double)log(AXB0->pt()/GeV));
      inputVals.push_back( (double)log(ipionS->pt()/GeV));
      inputVals.push_back( (double)log(fabs(IP/IPerr)));
      inputVals.push_back( (double)dR);
      inputVals.push_back( (double)dQ/GeV);
      inputVals.push_back( (double)nPV);
      
      double  rnet = m_myMCreader->GetMvaValue(inputVals);
      if (rnet>=0 && rnet<=1) {
        pn = 1.0 - TaggingHelpers::funcNN(rnet, m_P0ps, m_P1ps, m_P2ps, m_P3ps);
      } else {
        if ( msgLevel(MSG::DEBUG) ) 
          debug()<<"**********************BAD TRAINING PionSS"<<rnet<<endmsg;
        pn = -1.;
      }

      if ( msgLevel(MSG::DEBUG) )
      {
        debug()<<" TaggerPionSS: "<<ipionS->charge()<<" omega="<<1-pn<<" ";
        for(unsigned int iloop=0; iloop<inputVals.size(); iloop++){
          debug() << inputVals[iloop]<<" ";
        }
        debug()<<endmsg;
      }


    }else{  // Old format for the moment      
      std::vector<double> NNinputs(10);
      NNinputs.at(0) = m_util->countTracks(vtags);
      NNinputs.at(1) = AXB0->pt()/GeV;
      NNinputs.at(3) = ipionS->pt()/GeV;
      NNinputs.at(4) = fabs(IP/IPerr);
      NNinputs.at(5) = dR;
      NNinputs.at(7) = dQ/GeV;
      NNinputs.at(8) = nPV;
      
      pn = m_nnet->MLPpS( NNinputs );
      if ( msgLevel(MSG::DEBUG) )
      {
        debug()<<" TaggerPionSS: "<<ipionS->charge()<<" omega="<<1-pn<<" ";
        for(unsigned int iloop=0; iloop<10; iloop++){
          debug() << NNinputs[iloop]<<" ";
        }
        debug()<<endmsg;
      }


    }
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Pion pn="<< pn <<endmsg;
    
    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    //pn = 1 - m_P0_Cal_pionS - m_P1_Cal_pionS * ( (1-pn)-m_Eta_Cal_pionS);
    pn = 1 - m_P0_Cal_pionS - m_P1_Cal_pionS * ((1-pn)-m_Eta_Cal_pionS) - m_P2_Cal_pionS * ((1-pn)-m_Eta_Cal_pionS) * ((1-pn)-m_Eta_Cal_pionS);
    

    if ( msgLevel(MSG::DEBUG) )
      debug() << " PionS pn="<< pn <<" w="<<1-pn<<endmsg;

    if( pn < 0 || pn > 1 ) return tpionS;
    if( pn < m_PionProbMin ) return tpionS;
  }

  int tagdecision = ipionS->charge()>0 ? 1: -1;
  if( AXB0->particleID().hasUp() ) tagdecision = -tagdecision; //is Bu

  tpionS.setDecision( tagdecision );
  tpionS.setOmega( 1-pn );
  tpionS.setType( Tagger::SS_Pion );
  tpionS.addToTaggerParts(ipionS);

  return tpionS;
}
//==========================================================================
