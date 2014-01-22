// Include files 
#include "TaggerPionBDTSameTool.h"

#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerPionBDTSameTool
//
// Author: Antonio Falabella, Marta Calvi and Vava  --  November 2013
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerPionBDTSameTool )

//====================================================================
TaggerPionBDTSameTool::TaggerPionBDTSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent )
{
  declareInterface<ITagger>(this);

  declareProperty( "CombTech", m_CombinationTechnique = "BDT" );
  declareProperty( "NeuralNetName",  m_NeuralNetName  = "NNetTool_MLP" );

  declareProperty( "PionBDTSame_PIDp_cut",   m_PIDp_cut_pionS = 5.0);
  declareProperty( "PionBDTSame_PIDk_cut",   m_PIDk_cut_pionS = 5.0);
  declareProperty( "PionBDTSame_Pt_cut",     m_Pt_cut_pionS  = 0.4 *GeV );
  declareProperty( "PionBDTSame_IPs_cut",    m_IPs_cut_pionS = 3. ); // corresponding to IPChi2<16
  declareProperty( "PionBDTSame_lcs_cut",    m_lcs_cut         = 5.0 );

  declareProperty( "PionBDTSame_dQ_cut",     m_dQcut_pionS     = 0.9 *GeV);
  declareProperty( "PionBDTSame_Bp_Pt_cut",  m_Bp_Pt_cut_pionS = 3.0 *GeV);
  declareProperty( "PionBDTSame_Bp_vtxChi2_cut",m_Bp_vtxChi2_cut_pionS = 100);
  declareProperty( "PionBDTSame_deta_cut",   m_eta_cut_pionS   = 1.2);
  declareProperty( "PionBDTSame_dphi_cut",   m_phi_cut_pionS   = 1.1);
  declareProperty( "PionBDTSame_cosTheta_cut",m_cosTheta_cut_pionS = -0.5);
  declareProperty( "PionBDTSame_BDT_cut",    m_BDT_cut_pionS   = -1.);

  declareProperty( "PionBDTSame_P0_pol",     m_P0_pol_pionS   =  0.4848  );
  declareProperty( "PionBDTSame_P1_pol",     m_P1_pol_pionS   = -0.1295 );
  declareProperty( "PionBDTSame_P2_pol",     m_P2_pol_pionS   =  0.2035  );
  declareProperty( "PionBDTSame_P3_pol",     m_P3_pol_pionS   = -0.3222  );
  declareProperty( "PionBDTSame_P4_pol",     m_P4_pol_pionS   =  0.0     );
  declareProperty( "PionBDTSame_P5_pol",     m_P5_pol_pionS   =  0.0     );

  declareProperty( "PionBDTSame_AverageOmega",  m_AverageOmega   = 0.40 );
  declareProperty( "PionBDTSame_ProbMin",       m_PionProbMin  = 0.5);
  declareProperty( "isMonteCarlo",             m_isMonteCarlo   = 0);

  //m_nnet = 0;
  m_util = 0;
  m_fitter = 0;  
  //m_descend = 0;

}

TaggerPionBDTSameTool::~TaggerPionBDTSameTool() {}

//=====================================================================
StatusCode TaggerPionBDTSameTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "PionSS calib ctt: P0_pol "<<m_P0_pol_pionS
            << ", P1_pol "<<m_P1_pol_pionS<< ", P2_pol "<<m_P2_pol_pionS
            << ", P3_pol "<<m_P3_pol_pionS<< ", P4_pol "<<m_P4_pol_pionS
            << ", P5_pol "<<m_P5_pol_pionS<<endreq;
  

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
  //  variable_names.push_back("log(lab1_PIDp)");
  variable_names.push_back("log(lab0_OWNPV_NDOF)");
  variable_names.push_back("log(lab1_IPCHI2_OWNPV)");
  variable_names.push_back("dr");
  variable_names.push_back("lab1_CosTheta");
  variable_names.push_back("lab1_TRACK_GhostProb");

  if(m_isMonteCarlo==1) myBDT_reader = new ReadssPion(variable_names);//BDTReaderCompil_MC(variable_names);
  else                  myBDT_reader = new ReadssPion(variable_names);
  
  return sc;
}

//=====================================================================
Tagger TaggerPionBDTSameTool::tag( const Particle* AXB0, const RecVertex* RecVert, 
                                const int nPV,
                                Particle::ConstVector& vtags )
{

  Tagger tpionS;
  if(!RecVert) return tpionS;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--Pion SS Tagger--"<<endreq;

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

  //select pionS sameside tagger(s)
  //if more than one satisfies cuts, take the highest BDT response
  const Particle * ipionS = NULL;
  double bestBDT = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart, jpart;
  for ( ipart = vtags.begin(); ipart != vtags.end(); ++ipart )
  {
    //PID cuts to select the pion
    const ProtoParticle* proto = (*ipart)->proto();
    const double PIDk = proto->info( ProtoParticle::CombDLLk,  -1000.0 );
    const double PIDp = proto->info( ProtoParticle::CombDLLp,  -1000.0 );

    if(  PIDp <-999 )  continue;    
    if( PIDp > m_PIDp_cut_pionS ) continue;

    if(  PIDk <-999 )  continue;    
    if( PIDk > m_PIDk_cut_pionS ) continue;

    if ( msgLevel(MSG::DEBUG) )
      debug()<<" Pion PIDp="<< PIDp <<"="<<PIDp<<endreq;
    
    const Track* track = proto->track();
    if( track->type() != Track::Long ) continue;

    const double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_pionS )  continue;

    //const double P  = (*ipart)->p();
    //if( P  < m_P_cut_pionS )  continue;

    if ( msgLevel(MSG::DEBUG) )
      debug()<<" Pion P="<< (*ipart)->p() <<" Pt="<< Pt <<endreq;

    const double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut ) continue;

    const double trackGhostProb = track->ghostProbability();
    
    if ( msgLevel(MSG::DEBUG) )
      debug() << " Pion lcs="<< lcs <<" tsa="<<track->likelihood()<<" ghostProb="<<trackGhostProb<<endmsg;

    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = std::fabs(IP/IPerr);
    if(IPsig > m_IPs_cut_pionS)  continue;

    //if(track->ghostProbability() > m_ghostprob_cut )  continue; 

    //const double ippu=(*ipart)->info(LHCb::Particle::FlavourTaggingIPPUs,100000.);
    //if(ippu < m_ipPU_cut_pS) continue;

    //distphi
    //if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    //if( distphi < m_distPhi_cut_pS ) continue;

    Gaudi::LorentzVector ptot_pS = (*ipart)->momentum();
    
    const double mp = 139 ;  // from Antonio&Vava
    ptot_pS.SetE(std::sqrt(mp * mp + ptot_pS.P2()));
    if ( msgLevel(MSG::DEBUG) )
      debug()<< " Setting PDG pion mass "<<mp<<" instead of "<< ptot_pS.M() <<" to match the BDT computation "<<endreq;    
    
    const double dQ = (ptot_B+ptot_pS).M() - B0mass;

    if ( msgLevel(MSG::DEBUG) )
      debug() << " Pion IPs="<< IPsig <<" dQ="<<dQ<<endmsg;
    if(dQ > m_dQcut_pionS ) continue;

    const double pT_Bp = (ptot_B+ptot_pS).Pt();
    if(pT_Bp < m_Bp_Pt_cut_pionS ) continue;  


    double cosT = cosTheta(ptot_B+ptot_pS, ptot_B);
    if ( msgLevel(MSG::DEBUG) )
      debug()<< " cosTheta(B**,B*)="<<cosT<<" cut is >-0.5 "<<endreq;    
    if(cosT< m_cosTheta_cut_pionS) continue;
    
    const double deta = std::fabs(log(tan(ptot_B.Theta()/2.)/tan(ptot_pS.Theta()/2.)));
    const double dphi = std::fabs(TaggingHelpers::dphi(ptot_B.Phi(), ptot_pS.Phi()));

    if ( msgLevel(MSG::DEBUG) )
      debug()<< " deta="<<deta <<" (cut is <"<< m_eta_cut_pionS<<") dphi="<<dphi<<" (cut is <"<< m_phi_cut_pionS<<")"<< endreq;    

    const double dR = std::sqrt(deta*deta+dphi*dphi);

    if(deta > m_eta_cut_pionS) continue;
    if(dphi > m_phi_cut_pionS) continue;


    Vertex vtx;    
    StatusCode sc = m_fitter->fit(vtx,*AXB0,**ipart);    
    if( sc.isFailure() ) continue;
    if ( msgLevel(MSG::DEBUG) )
      debug()<< " Vertex Fit Chi2="<<vtx.chi2() <<"/"<<vtx.nDoF()<<" (cut is <"<< m_Bp_vtxChi2_cut_pionS <<")"<< endreq;    
 
    if( vtx.chi2() / vtx.nDoF() > m_Bp_vtxChi2_cut_pionS ) continue;


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
    //    values.push_back(log(PIDp));    
    values.push_back(log(RecVert->nDoF()));
    values.push_back(log(IPsig*IPsig));
    values.push_back(dR);
    values.push_back(cosT);
    values.push_back(trackGhostProb);
    
    if ( msgLevel(MSG::DEBUG) ){      
      debug() << " BDT computation for candidate "<<ncand<<" ";      
      for(unsigned int i=0; i<values.size(); ++i) debug() << values.at(i)<<" ";      
    }
    
    
    double BDT = myBDT_reader->GetMvaValue(values);

    if ( msgLevel(MSG::DEBUG) ) debug() << " BDT="<<  BDT<<endmsg;
    
    if( msgLevel(MSG::INFO) ) if (BDT<-1 || BDT > 1) info()<<" WARNING SSpion BDT out of range "<<BDT<<endmsg;    
    if( BDT < m_BDT_cut_pionS) continue;    
    if( BDT < bestBDT ) continue;

    //accept candidate
    ipionS = (*ipart);
    bestBDT = BDT;
    

  }
  if( !ipionS ) return tpionS;
  
  //calculate omega
  double pn = 1-m_AverageOmega;
  if ( m_CombinationTechnique == "BDT" ) 
  {
    pn = 1. - ( m_P0_pol_pionS + 
                m_P1_pol_pionS*bestBDT +  
                m_P2_pol_pionS*bestBDT*bestBDT + 
                m_P3_pol_pionS*bestBDT*bestBDT*bestBDT+
                m_P4_pol_pionS*bestBDT*bestBDT*bestBDT*bestBDT + 
                m_P5_pol_pionS*bestBDT*bestBDT*bestBDT*bestBDT*bestBDT);
    
  
    if ( msgLevel(MSG::DEBUG) )
      debug() << " PionS pn="<< pn <<" w="<<1-pn<<endmsg;
    
    if( pn < 0 || pn > 1 ) return tpionS;
    if( pn < m_PionProbMin ) return tpionS;
  }


  int tagdecision = ipionS->charge()>0 ? 1: -1;  
  if( AXB0->particleID().hasUp()) tagdecision = - tagdecision;
  
  tpionS.setDecision( tagdecision );
  tpionS.setOmega( 1-pn );
  tpionS.setType( Tagger::SS_PionBDT );
  tpionS.addToTaggerParts(ipionS);

  return tpionS;
}
//==========================================================================
