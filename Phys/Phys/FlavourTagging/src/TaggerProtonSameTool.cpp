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
                                            const IInterface* parent )
: GaudiTool ( type, name, parent ),
  m_myBDT_reader ( NULL ),
  m_fitter       ( NULL ),
  m_util         ( NULL )
{
  declareInterface<ITagger>(this);

  declareProperty( "CombTech", m_CombinationTechnique = "BDT" );
  declareProperty( "NeuralNetName",  m_NeuralNetName  = "NNetTool_MLP" );

  declareProperty( "ProtonSame_PIDp_cut",   m_PIDp_cut_protonS = 5.0); //ok
  declareProperty( "ProtonSame_Pt_cut",     m_Pt_cut_protonS  = 0.4 *GeV );//ok
  declareProperty( "ProtonSame_IPs_cut",    m_IPs_cut_protonS = 4. ); // ok
  declareProperty( "ProtonSame_ghostprob_cut",m_ghostprob_cut = 0.5 ); // ok

  declareProperty( "ProtonSame_dQ_cut",     m_dQcut_protonS     = 1.3 *GeV); //ok
  declareProperty( "ProtonSame_Bp_Pt_cut",  m_Bp_Pt_cut_protonS = 3.0 *GeV); //ok
  declareProperty( "ProtonSame_Bp_vtxChi2_cut",m_Bp_vtxChi2_cut_protonS = 100);//ok
  declareProperty( "ProtonSame_deta_cut",   m_eta_cut_protonS   = 1.2);
  declareProperty( "ProtonSame_dphi_cut",   m_phi_cut_protonS   = 1.2);
  declareProperty( "ProtonSame_BDT_cut",    m_BDT_cut_protonS   = 0.);

  declareProperty( "ProtonSame_P0_pol",     m_P0_pol_protonS   = 0.4831 );
  declareProperty( "ProtonSame_P1_pol",     m_P1_pol_protonS   = -0.07304 );
  declareProperty( "ProtonSame_P2_pol",     m_P2_pol_protonS   = -0.05043 );
  declareProperty( "ProtonSame_P3_pol",     m_P3_pol_protonS   = -0.1749  );




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

  m_fitter = m_util->getVertexFitter();//tool<IVertexFit>("OfflineVertexFitter");

  //m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  //====== BDT definition ========
  std::vector<std::string> variable_names;

  variable_names.push_back("log(P)");
  variable_names.push_back("log(Pt)");
  variable_names.push_back("log(ipsig)");
  variable_names.push_back("dR");
  variable_names.push_back("log(deta)");
  variable_names.push_back("dQ_p");
  variable_names.push_back("log(Pt_tot)");
  variable_names.push_back("log(PVndof)");
  variable_names.push_back("log(PIDp)");


  if(m_isMonteCarlo==1) m_myBDT_reader = new ProtonSSWrapper(variable_names);//BDTReaderCompil_MC(variable_names);
  else                  m_myBDT_reader = new ProtonSSWrapper(variable_names);

  return sc;
}

//=====================================================================

StatusCode TaggerProtonSameTool::finalize()
{
  delete m_myBDT_reader; m_myBDT_reader = NULL;
  return GaudiTool::finalize();
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

  const double mB0 = 5279.58;  // PDG
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
  double bestBDT = -99.0;
  unsigned int ncand=0;
  Particle::ConstVector::const_iterator ipart, jpart;
  for ( ipart = vtags.begin(); ipart != vtags.end(); ++ipart )
  {
    //PID cuts to select the proton
    const ProtoParticle* proto = (*ipart)->proto();
    const double PIDk = proto->info( ProtoParticle::CombDLLk,  -1000.0 );

    const double PIDp = proto->info( ProtoParticle::CombDLLp,  -1000.0 );
    if ( msgLevel(MSG::DEBUG) )
      debug()<<" Proton PIDp="<< PIDp <<" cut is >"<<m_PIDp_cut_protonS<<" Proton PIDk="<< PIDk<<" (no cut)"<<" n. of candidates="<<vtags.size()<<endreq;

    if( PIDp <-999 )  continue;    
    if( PIDp < m_PIDp_cut_protonS ) continue;

    const Track* track = proto->track();
    if( track->type() != Track::Long ) continue;

    const double Pt = (*ipart)->pt();
    if ( msgLevel(MSG::DEBUG) )
      debug() << " Proton Pt="<< Pt <<" cut is >"<<m_Pt_cut_protonS<<endmsg;
    if( Pt < m_Pt_cut_protonS )  continue;

    if ( msgLevel(MSG::DEBUG) )
      debug()<<" Proton P="<< (*ipart)->p() <<" Pt="<< Pt <<endreq;

    //    const double lcs = track->chi2PerDoF();
    //    if( lcs > m_lcs_cut ) continue;  //looser cut than default

    if ( msgLevel(MSG::DEBUG) )
      debug() <<" ghostProb="<<track->ghostProbability()<<endmsg;
    if(track->ghostProbability() > m_ghostprob_cut )  continue;

    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = std::fabs(IP/IPerr);
    if(IPsig > m_IPs_cut_protonS)  continue;


    //distphi
    //if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    //if( distphi < m_distPhi_cut_pS ) continue;

    Gaudi::LorentzVector ptot_pS = (*ipart)->momentum();

    const double mp = 938.27;  // PDG
    ptot_pS.SetE(std::sqrt(mp * mp + ptot_pS.P2()));
    if ( msgLevel(MSG::DEBUG) )
      debug()<< " Setting PDG proton mass "<<mp<<" instead of "<< ptot_pS.M() <<" to match the BDT computation "<<endreq;    
    
    const double dQ = (ptot_B+ptot_pS).M() - B0mass - mp;

    if ( msgLevel(MSG::DEBUG) )
      debug() << " Proton IPs="<< IPsig <<" dQ="<<dQ<<endmsg;
    if(dQ > m_dQcut_protonS ) continue;

    const double pT_Bp = (ptot_B+ptot_pS).Pt();
    if ( msgLevel(MSG::DEBUG) )
      debug() << " B+Proton Pt="<< pT_Bp <<" cut is >"<<m_Bp_Pt_cut_protonS<<endmsg;
    if(pT_Bp < m_Bp_Pt_cut_protonS ) continue;

    
    const double deta = std::fabs(log(tan(ptot_B.Theta()/2.)/tan(ptot_pS.Theta()/2.)));
    const double dphi = std::fabs(TaggingHelpers::dphi(ptot_B.Phi(), ptot_pS.Phi()));

    if ( msgLevel(MSG::DEBUG) )
      debug()<< " deta="<<deta <<" (cut is <"<< m_eta_cut_protonS<<") dphi="<<dphi<<" (cut is <"<< m_phi_cut_protonS<<")"<< endreq;    

    const double dR = std::sqrt(deta*deta+dphi*dphi);

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
    values.reserve(10);
    values.push_back(log(ptot_pS.P()/GeV));
    values.push_back(log(ptot_pS.Pt()/GeV)); // lab1=SSp
    values.push_back(log(IPsig));
    values.push_back(dR);
    values.push_back(log(deta));
    values.push_back(dQ/GeV);
    values.push_back(log((ptot_B+ptot_pS).Pt()/GeV));  // lab0=B**
    values.push_back(log(RecVert->nDoF()));
    values.push_back(log(PIDp));


    if ( msgLevel(MSG::DEBUG) ){
      debug() << "SSproton BDT computation for candidate "<<ncand<<" ";
      for(unsigned int i=0; i<values.size(); ++i) debug() << values.at(i)<<" ";
    }

    const double BDT = m_myBDT_reader->GetMvaValue(values);

    if ( msgLevel(MSG::DEBUG) ) debug() << " BDT="<<  BDT<<endmsg;

    if( msgLevel(MSG::DEBUG) ) if (BDT<-1 || BDT > 1) debug()<<" WARNING SSproton BDT out of range "<<BDT<<endmsg;
    //    if( BDT < m_BDT_cut_protonS) continue; // ??
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

  if ( msgLevel(MSG::DEBUG) )
    debug() << " Found a SSproton tagging candidate decision="<< tagdecision <<" w="<<1-pn<<endmsg;
  return tprotonS;
}
//==========================================================================
