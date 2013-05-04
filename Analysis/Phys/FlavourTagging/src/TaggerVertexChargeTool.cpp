// Include files
#include "TaggerVertexChargeTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerVertexChargeTool
//
// Author: Marco Musy & Marc Grabalosa
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerVertexChargeTool )

//====================================================================
TaggerVertexChargeTool::TaggerVertexChargeTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent ) :
  GaudiTool ( type, name, parent ) 
{

  declareInterface<ITagger>(this);

  declareProperty( "SecondaryVertexName",
                   m_SecondaryVertexToolName = "SVertexOneSeedTool" );
  declareProperty( "Vtx_AverageOmega", m_AverageOmega         = 0.41 );

  //NNet - no bias, Probability - old with bias:
  declareProperty( "CombTech",  m_CombinationTechnique    = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName      = "NNetTool_MLP" );

  declareProperty( "Vtx_PowerK",        m_PowerK              = 0.55 );
  declareProperty( "Vtx_MinimumVCharge",m_MinimumVCharge      = 0.2 );
  declareProperty( "Vtx_Ptsum",         m_Ptsum_vtx           = 1.55);
  declareProperty( "Vtx_Ptmean",        m_Ptmean_vtx          = 0.);
  declareProperty( "Vtx_IPSsum",        m_IPSsum_vtx          = 0.);
  declareProperty( "Vtx_DocaMaxsum",    m_DocaMaxsum_vtx      = 0.5);
  declareProperty( "Vtx_Psum",          m_Psum_vtx            = 8.);
  declareProperty( "Vtx_Msum",          m_Msum_vtx            = 0.6);

  declareProperty( "Vtx_ProbMin", m_ProbMin_vtx           = 0.54);
  declareProperty( "Vtx_P0_Cal",  m_P0_Cal_vtx   = 0.404 );
  declareProperty( "Vtx_P1_Cal",  m_P1_Cal_vtx   = 0.84 );
  declareProperty( "Vtx_Eta_Cal", m_Eta_Cal_vtx  = 0.362 );

  //For CombinationTechnique: "Probability"
  declareProperty( "P0",           m_P0                   = 5.255669e-01 );
  declareProperty( "P1",           m_P1                   = -3.251661e-01 );
  declareProperty( "Gt075",        m_Gt075                = 0.35 );
  declareProperty( "TracksEq2",    m_wSameSign2           = 0.4141 );
  declareProperty( "TracksGt2",    m_wSameSignMoreThan2   = 0.3250 );

  m_svtool = NULL;
  m_nnet   = NULL;
  m_util   = NULL;
}

TaggerVertexChargeTool::~TaggerVertexChargeTool() {}

//=====================================================================
StatusCode TaggerVertexChargeTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Vtx calib ctt: P0_Cal "<<m_P0_Cal_vtx
            << ", P1_Cal "<<m_P1_Cal_vtx<<endreq;

  m_svtool = tool<ISecondaryVertexTool> ("SVertexOneSeedTool",
                                         m_SecondaryVertexToolName, this);

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);

  return sc;
}

//=====================================================================
Tagger TaggerVertexChargeTool::tag( const Particle* AXB0,
                                    const RecVertex* RecVert,
                                    Vertex::ConstVector& allVtx,
                                    Particle::ConstVector& vtags ) 
{
  Tagger tVch;
  if(!RecVert) return tVch;
  if(vtags.empty()) return tVch;

  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"--Vertex Tagger--"<<endreq;

  ///--- Inclusive Secondary Vertex ---
  //look for a secondary Vtx due to opposite B
  std::vector<Vertex> vvec(0);
  if(m_svtool) vvec = m_svtool -> buildVertex( *RecVert, vtags );
  if(vvec.empty()) return tVch;
  const Particle::ConstVector& Pfit = vvec.at(0).outgoingParticlesVector();

  //if Vertex does not contain any daughters, exit
  if ( Pfit.empty() ) return tVch;
  if ( msgLevel(MSG::DEBUG) )
    debug()<<"--- SVTOOL buildVertex returns: "<<vvec.size()
           <<", with "<<Pfit.size()<<"tracks"<<endreq;
  double maxprobf = vvec.at(0).info(1, 0.5 );
  if ( msgLevel(MSG::DEBUG) ) 
    debug()<<" -- likelihood seed "<<maxprobf<<endreq;
  Vertex seedvtx;

  const Gaudi::XYZPoint& BoppDir = vvec.at(0).position();  // SV-PV !!!
  if ( msgLevel(MSG::DEBUG) )
    debug()<<"BoppDir: "<<BoppDir<<endreq;

  //calculate vertex charge and other variables for NN
  double Vch = 0, norm = 0;
  double Vptmean = 0, Vipsmean = 0, Vflaglong = 0, Vdocamax = 0;
  int vflagged = 0;
  Gaudi::LorentzVector SVmomentum;
  for ( Particle::ConstVector::const_iterator ip = Pfit.begin();
        ip!=Pfit.end(); ++ip ) 
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() <<"SVTOOL  VtxCh, adding track pt= "<<(*ip)->pt()<<endreq;
    SVmomentum += (*ip)->momentum();
    const double a = std::pow((*ip)->pt()/GeV, m_PowerK);
    Vch  += (*ip)->charge() * a;
    norm += a;
    ++vflagged;
    Vptmean += (*ip)->pt()/GeV;
    double minip(0), miniperr(0);
    m_util->calcIP(*ip, RecVert, minip, miniperr);
    minip = std::fabs(minip);
    Vipsmean += minip/miniperr;
    const Track* iptrack = (*ip)->proto()->track();
    if( iptrack->type()== Track::Long ) ++Vflaglong;
    double docaSV(0), docaErrSV(0);
    m_util->calcDOCAmin( *ip, Pfit.at(0), Pfit.at(1), docaSV, docaErrSV); //DOCA wrt the seeds
    Vdocamax += docaSV;
    if ( msgLevel(MSG::DEBUG) )
      debug()<<"docaSV:"<<docaSV<<endreq;
  }

  if ( norm )
  {
    Vch /= norm;
    if(fabs(Vch) < m_MinimumVCharge ) Vch = 0;
    Vptmean  /= vflagged;
    Vipsmean /= vflagged;
    Vdocamax/= vflagged;
  }
  if ( msgLevel(MSG::DEBUG) ) debug()<<"Vch: "<<Vch<<endreq;
  if( Vch==0 ) return tVch;

  //Variables of the SV (Seed Vertex)
  const double SVP = SVmomentum.P()/1000;
  const double SVM = SVmomentum.M()/1000;
  const double SVGP = SVP/(0.16*SVM+0.12);
  const double SVtau = std::sqrt(BoppDir.Mag2())*5.28/SVGP/0.299792458;
  if ( msgLevel(MSG::DEBUG) ) 
    debug()<<"BoppDir.Mag2: "<<sqrt(BoppDir.Mag2())<<", SVGP: "<<SVGP<<", SVtau: "<<SVtau<<endreq;
  if ( Vptmean < m_Ptmean_vtx)              return tVch;
  if ( Vptmean*vflagged < m_Ptsum_vtx)      return tVch;
  if (Vipsmean*vflagged < m_IPSsum_vtx)     return tVch;
  if (Vdocamax*vflagged > m_DocaMaxsum_vtx) return tVch;
  if (SVP < m_Psum_vtx ) return tVch;
  if (SVM < m_Msum_vtx ) return tVch;

  //calculate omega
  if ( msgLevel(MSG::DEBUG) ) 
    debug()<<"calculate omega with "<<m_CombinationTechnique<<endreq;
  double omega = m_AverageOmega;
  double pn = 1-omega;
  if(m_CombinationTechnique == "Probability") 
  {
    if( fabs(Vch)<0.75 ) omega = m_P0 + m_P1*fabs(Vch) ;
    if( fabs(Vch)>0.75 ) omega = m_Gt075;
    if( fabs(Vch)>0.99 ) { // tracks have all the same charge
      if(Pfit.size()==2) omega = m_wSameSign2;
      if(Pfit.size() >2) omega = m_wSameSignMoreThan2;
    }
    pn = 1 - omega;
  }
  if(m_CombinationTechnique == "NNet") 
  {
    std::vector<double> NNinputs(11);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = allVtx.size();
    NNinputs.at(2) = AXB0->pt()/GeV;;
    NNinputs.at(3) = vflagged;
    NNinputs.at(4) = Vptmean;
    NNinputs.at(5) = Vipsmean;
    NNinputs.at(9) = fabs(Vch);
    NNinputs.at(10) = SVM;
    pn = m_nnet->MLPvtx( NNinputs );
    omega = 1 - pn;
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() <<" VtxCh= "<< Vch <<" with "<< Pfit.size() <<" parts"
              <<", omega= "<< omega <<endreq;

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  omega =  m_P0_Cal_vtx + m_P1_Cal_vtx * ( omega-m_Eta_Cal_vtx);
  if ( msgLevel(MSG::DEBUG) )
    debug() << " Vtx pn="<< 1-omega <<" w="<<omega<<endmsg;

  if( omega < 0 || omega > 1 ) return tVch;
  if( 1-omega < m_ProbMin_vtx ) return tVch;
  //  if(   omega > m_ProbMin_vtx ) return tVch;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"Vtx passed"<<endreq;

  tVch.setDecision( Vch>0 ? -1 : 1 );
  tVch.setOmega( omega );
  tVch.setType( Tagger::VtxCharge );
  for ( Particle::ConstVector::const_iterator ip = Pfit.begin();
        ip!=Pfit.end(); ++ip )
  {
    tVch.addToTaggerParts(*ip);
  }

  return tVch;
}
//====================================================================
