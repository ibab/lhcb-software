// Include files 
#include "TaggerProtonSameTool.h"

#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerProtonSameTool
//
// Author: Antonio Falabella, Marta Calvi and Vava  --  November 2013
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerProtonSameTool )

//====================================================================
TaggerProtonSameTool::TaggerProtonSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent )
{
  declareInterface<ITagger>(this);

  declareProperty( "CombTech", m_CombinationTechnique = "BDT" );
  declareProperty( "NeuralNetName",  m_NeuralNetName  = "NNetTool_MLP" );

  declareProperty( "ProtonSame_PIDp_cut",   m_PIDp_cut_protonS = 5.0);
  declareProperty( "ProtonSame_Pt_cut",     m_Pt_cut_protonS  = 0.4 *GeV );
  declareProperty( "ProtonSame_IPs_cut",    m_IPs_cut_protonS = 4. ); // corresponding to IPChi2<16
  declareProperty( "ProtonSame_lcs_cut",    m_lcs_cut         = 5.0 );

  declareProperty( "ProtonSame_dQ_cut",     m_dQcut_protonS     = 2.5 *GeV);
  declareProperty( "ProtonSame_Bp_Pt_cut",  m_Bp_Pt_cut_protonS = 3.0 *GeV);
  declareProperty( "ProtonSame_Bp_vtxChi2_cut",m_Bp_vtxChi2_cut_protonS = 100);
  declareProperty( "ProtonSame_deta_cut",   m_eta_cut_protonS   = 1.2);
  declareProperty( "ProtonSame_dphi_cut",   m_phi_cut_protonS   = 1.2);
  declareProperty( "ProtonSame_cosTheta_cut",m_cosTheta_cut_protonS = -0.5);
  declareProperty( "ProtonSame_BDT_cut",    m_BDT_cut_protonS   = 0.);

  declareProperty( "ProtonSame_P0_pol",     m_P0_pol_protonS   = 0.4049 );
  declareProperty( "ProtonSame_P1_pol",     m_P1_pol_protonS   = 0.83   );
  declareProperty( "ProtonSame_P2_pol",     m_P2_pol_protonS   = -1.99  );
  declareProperty( "ProtonSame_P3_pol",     m_P3_pol_protonS   = -1.99  );


  declareProperty( "ProtonSame_AverageOmega",  m_AverageOmega   = 0.40 );
  declareProperty( "ProtonSame_ProbMin",       m_ProtonProbMin  = 0.5);
  declareProperty( "isMonteCarlo",             m_isMonteCarlo   = 0);

  //m_nnet = 0;
  m_util = 0;
  m_fitter = 0;  
  //m_descend = 0;

}

TaggerProtonSameTool::~TaggerProtonSameTool() {}

//=====================================================================
StatusCode TaggerProtonSameTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "ProtonSS calib ctt: P0_pol "<<m_P0_pol_protonS
            << ", P1_pol "<<m_P1_pol_protonS<< ", P2_pol "<<m_P2_pol_protonS
            << ", P3_pol "<<m_P3_pol_protonS<<endreq;
  

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  //m_nnet = tool<INNetTool> ( m_NeuralNetName, this);

  m_fitter = tool<IVertexFit>("OfflineVertexFitter");

  //m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  //====== BDT definition ========
  std::vector<std::string> variable_names;
  variable_names.push_back("dQ");
  variable_names.push_back("log(lab0_PT)");
  variable_names.push_back("log(lab1_PT)");
  variable_names.push_back("log(lab2_PT)");
  variable_names.push_back("log(lab1_P)");
  variable_names.push_back("log(Dphi)");
  variable_names.push_back("log(Deta)");
  variable_names.push_back("log(lab1_PIDp)");
  variable_names.push_back("log(lab0_OWNPV_NDOF)");
  variable_names.push_back("log(lab1_IPCHI2_OWNPV)");

  if(m_isMonteCarlo==1) myBDT_reader = new ReadssProton(variable_names);//BDTReaderCompil_MC(variable_names);
  else                  myBDT_reader = new ReadssProton(variable_names);
  
  return sc;
}

//=====================================================================
Tagger TaggerProtonSameTool::tag( const Particle* AXB0, const RecVertex* RecVert, 
                                const int nPV,
                                Particle::ConstVector& vtags )
{

  Tagger tprotonS;
  if(!RecVert) return tprotonS;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--Proton SS Tagger--"<<endreq;

  Gaudi::LorentzVector ptot_B = AXB0->momentum();

  const double mB0 = 5279.15;  // from Antonio&Vava
  if ( msgLevel(MSG::DEBUG) )
    debug()<< " Setting PDG B0 mass "<<mB0<<" instead of "<< ptot_B.M() <<" to match the BDT computation "<<endreq;

  ptot_B.SetE(std::sqrt(mB0 * mB0 + ptot_B.P2()));
  double B0mass = ptot_B.M();

  //fill auxdaugh for distphi
  //double distphi;
  //Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  //axdaugh.push_back( AXB0 );

  //select protonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest BDT response
  const Particle * iprotonS = NULL;
  double bestBDT = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart, jpart;
  for ( ipart = vtags.begin(); ipart != vtags.end(); ++ipart )
  {
    //PID cuts to select the proton
    const ProtoParticle* proto = (*ipart)->proto();
    //const double PIDk = proto->info( ProtoParticle::CombDLLk,  -1000.0 );
    const double PIDp = proto->info( ProtoParticle::CombDLLp,  -1000.0 );

    if(  PIDp <-999 )  continue;    
    if( PIDp < m_PIDp_cut_protonS ) continue;

    if ( msgLevel(MSG::DEBUG) )
      debug()<<" Proton PIDp="<< PIDp <<"="<<PIDp<<endreq;
    
    const Track* track = proto->track();
    if( track->type() != Track::Long ) continue;

    const double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_protonS )  continue;

    //const double P  = (*ipart)->p();
    //if( P  < m_P_cut_protonS )  continue;

    if ( msgLevel(MSG::DEBUG) )
      debug()<<" Proton P="<< (*ipart)->p() <<" Pt="<< Pt <<endreq;

    const double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut ) continue;

    if ( msgLevel(MSG::DEBUG) )
      debug() << " Proton lcs="<< lcs <<" tsa="<<track->likelihood()<<" ghostProb="<<track->ghostProbability()<<endmsg;

    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = std::fabs(IP/IPerr);
    if(IPsig > m_IPs_cut_protonS)  continue;

    //if(track->ghostProbability() > m_ghostprob_cut )  continue; 

    //const double ippu=(*ipart)->info(LHCb::Particle::FlavourTaggingIPPUs,100000.);
    //if(ippu < m_ipPU_cut_pS) continue;

    //distphi
    //if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    //if( distphi < m_distPhi_cut_pS ) continue;

    Gaudi::LorentzVector ptot_pS = (*ipart)->momentum();
    
    const double mp = 938.27;  // from Antonio&Vava
    ptot_pS.SetE(std::sqrt(mp * mp + ptot_pS.P2()));
    if ( msgLevel(MSG::DEBUG) )
      debug()<< " Setting PDG proton mass "<<mp<<" instead of "<< ptot_pS.M() <<" to match the BDT computation "<<endreq;    
    
    const double dQ = (ptot_B+ptot_pS).M() - B0mass;

    if ( msgLevel(MSG::DEBUG) )
      debug() << " Proton IPs="<< IPsig <<" dQ="<<dQ<<endmsg;
    if(dQ > m_dQcut_protonS ) continue;

    const double pT_Bp = (ptot_B+ptot_pS).Pt();
    if(pT_Bp < m_Bp_Pt_cut_protonS ) continue;  


    double cosT = cosTheta(ptot_B+ptot_pS, ptot_B);
    if ( msgLevel(MSG::DEBUG) )
      debug()<< " cosTheta(B**,B*)="<<cosT<<" cut is >-0.5 "<<endreq;    
    if(cosT< m_cosTheta_cut_protonS) continue;
    
    const double deta = std::fabs(log(tan(ptot_B.Theta()/2.)/tan(ptot_pS.Theta()/2.)));
    const double dphi = std::fabs(TaggingHelpers::dphi(ptot_B.Phi(), ptot_pS.Phi()));

    if ( msgLevel(MSG::DEBUG) )
      debug()<< " deta="<<deta <<" (cut is <"<< m_eta_cut_protonS<<") dphi="<<dphi<<" (cut is <"<< m_phi_cut_protonS<<")"<< endreq;    

    //const double dR = std::sqrt(deta*deta+dphi*dphi);

    if(deta > m_eta_cut_protonS) continue;
    if(dphi > m_phi_cut_protonS) continue;


    Vertex vtx;    
    StatusCode sc = m_fitter->fit(vtx,*AXB0,**ipart);    
    if( sc.isFailure() ) continue;
    if ( msgLevel(MSG::DEBUG) )
      debug()<< " Vertex Fit Chi2="<<vtx.chi2() <<"/"<<vtx.nDoF()<<" (cut is <"<< m_Bp_vtxChi2_cut_protonS <<")"<< endreq;    
 
    if( vtx.chi2() / vtx.nDoF() > m_Bp_vtxChi2_cut_protonS ) continue;


    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " ignore nPV variable in this tagger "<< nPV <<endmsg;


    ++ncand;
    std::vector<double> values;
    values.push_back(dQ);  
    values.push_back(log((ptot_B+ptot_pS).Pt()));  // lab0=B**
    values.push_back(log(ptot_pS.Pt())); // lab1=SSp
    values.push_back(log(ptot_B.Pt())); // lab2=B
    values.push_back(log(ptot_pS.P()));
    values.push_back(log(dphi));
    values.push_back(log(deta));
    values.push_back(log(PIDp));    
    values.push_back(log(RecVert->nDoF()));
    values.push_back(log(IPsig*IPsig));
    
    if ( msgLevel(MSG::DEBUG) ){      
      debug() << " BDT computation for candidate "<<ncand<<" ";      
      for(unsigned int i=0; i<values.size(); ++i) debug() << values.at(i)<<" ";      
    }
    
    
    double BDT = myBDT_reader->GetMvaValue(values);

    if ( msgLevel(MSG::DEBUG) ) debug() << " BDT="<<  BDT<<endmsg;
    
    if( BDT<-1 || BDT > 1) std::cout<<" ATTENZIONE BDT fuori dal range "<<BDT<<std::endl;    
    if( BDT < m_BDT_cut_protonS) continue;    
    if( BDT < bestBDT ) continue;

    //accept candidate
    iprotonS = (*ipart);
    bestBDT = BDT;
    

  }
  if( !iprotonS ) return tprotonS;
  
  //calculate omega
  double pn = 1-m_AverageOmega;
  if ( m_CombinationTechnique == "BDT" ) 
  {
    pn = 1. - ( m_P0_pol_protonS + 
                m_P1_pol_protonS*bestBDT +  
                m_P2_pol_protonS*bestBDT*bestBDT + 
                m_P3_pol_protonS*bestBDT*bestBDT*bestBDT);
    
  
    if ( msgLevel(MSG::DEBUG) )
      debug() << " ProtonS pn="<< pn <<" w="<<1-pn<<endmsg;
    
    if( pn < 0 || pn > 1 ) return tprotonS;
    if( pn < m_ProtonProbMin ) return tprotonS;
  }


  int tagdecision = iprotonS->charge()>0 ? -1: 1;  // valid also for Bu

  tprotonS.setDecision( tagdecision );
  tprotonS.setOmega( 1-pn );
  tprotonS.setType( Tagger::SS_Proton );
  tprotonS.addToTaggerParts(iprotonS);

  return tprotonS;
}
//==========================================================================
