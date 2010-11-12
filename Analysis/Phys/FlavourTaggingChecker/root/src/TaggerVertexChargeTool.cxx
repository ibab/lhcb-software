#include "TaggerVertexChargeTool.h"
#include "NNetTool_MLP.h"

TaggerVertexChargeTool::TaggerVertexChargeTool() {

  declareProperty( "SecondaryVertexName",
		   m_SecondaryVertexToolName = "SVertexOneSeedTool");

  //"NNet", "Probability"
  declareProperty( "UseObsoleteSV", m_UseObsoleteSV       = false );
  declareProperty( "AverageOmega",  m_AverageOmega        = 0.41 );

  declareProperty( "PowerK",        m_PowerK              = 0.4 );
  declareProperty( "MinimumVCharge",m_MinimumVCharge      = 0.15 );
  declareProperty( "ProbMin_vtx",   m_ProbMin_vtx         = 0.53);

  //For CombinationTechnique "Probability":
  declareProperty( "P0",           m_P0                   =  5.255669e-01 );
  declareProperty( "P1",           m_P1                   = -3.251661e-01 );
  declareProperty( "Gt075",        m_Gt075                = 0.35 );
  declareProperty( "TracksEq2",    m_wSameSign2           = 0.4141 );
  declareProperty( "TracksGt2",    m_wSameSignMoreThan2   = 0.3250 );

  NNetTool_MLP nnet;
  tVch = new Tagger();
  if(m_SecondaryVertexToolName=="SVertexOneSeedTool") 
    m_svtool= new SVertexOneSeedTool();
  else err()<<"Unknown SecondaryVertexName: "
	    <<m_SecondaryVertexToolName <<endmsg;

}

///////////////////////////////////////////////////
Tagger* TaggerVertexChargeTool::tag(Event& event) {
  tVch->reset();

  verbose()<<"--VertexCharge Tagger--"<<endreq;

  Particles parts = event.particles();
  Particles::iterator i, ip;
  Particles Pfit; Pfit.clear();
  double maxprobf = 0;

  ///--- Inclusive Secondary Vertex ---
  //look for a secondary Vtx due to opposite B

  Vertex* mysecvtx=NULL;
  
  if(m_UseObsoleteSV) { //use DaVinci processed information
    for (i=parts.begin(); i!=parts.end(); ++i) {
      if((*i)->ComesFromSVtx()) Pfit.push_back(*i);
    }
  } else { //use seed from DaVinci and add tracks custom
   Vertices allvtx = m_svtool->buildVertex( parts, &event );
   if(allvtx.empty()) return tVch;
   mysecvtx = allvtx.at(0);
    if( mysecvtx->type() != Vertex::Secondary ) return tVch;
    Pfit = mysecvtx->outgoingParticles();
    maxprobf= mysecvtx->likelihood();
  }

  debug()<<"Pfit.size: "<<Pfit.size()<<endreq;

  //if Vertex does not contain any daughters, exit
  if(Pfit.size()<1) return tVch;
  debug()<<"--- SVTOOL buildVertex returns vertex"
         <<" with "<<Pfit.size()<<"tracks"<<endreq;
  debug()<<" -- likelihood seed "<<maxprobf<<endreq;


  //calculate vertex charge and other variables for NN
  double Vch = 0, norm = 0;
  double Vptmin = 0, Vipsmin = 0, Vflaglong = 0, Vdocamax = 0;
  int vflagged = 0;
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) { 
    debug() <<"SVTOOL  VtxCh, adding track pt= "<<(*ip)->pt()<<endmsg;
    double a = pow((*ip)->pt()/GeV, m_PowerK);
    Vch += (*ip)->charge() * a;
    norm+= a;
    vflagged++;
    Vptmin  += (*ip)->pt()/GeV;
    Vipsmin += (*ip)->IPs();
    if( (*ip)->type()== Particle::Long || 
        (*ip)->type()== Particle::Matched) Vflaglong++;
    Vdocamax += (*ip)->DOCA();
    debug()<<"docaSV:"<<(*ip)->DOCA()<<endmsg;
  }
  if(norm) {
    Vch /= norm;
    if(fabs(Vch) < m_MinimumVCharge ) Vch = 0;
    Vptmin  /= vflagged;
    Vipsmin /= vflagged;
    Vdocamax/= vflagged;
  }
  debug()<<"Vch: "<<Vch<<endreq;

  if(!m_UseObsoleteSV) { //needed by NNtuner
    mysecvtx->setVflagged(vflagged);
    mysecvtx->setVptmin(Vptmin);
    mysecvtx->setVipsmin(Vipsmin);
    mysecvtx->setVdocamax(Vdocamax);
    mysecvtx->setVratio(Vflaglong/(vflagged? vflagged:1));
    mysecvtx->setVCharge(fabs(Vch));
  }
  
  if( Vch==0 ) return tVch;


  //calculate omega
  debug()<<"calculate omega, use obsolete (0=no): "<<m_UseObsoleteSV<<endreq;
  double omega = m_AverageOmega;
  if(m_UseObsoleteSV) {
    if( fabs(Vch)<0.75 ) omega = m_P0 + m_P1*fabs(Vch) ;
    if( fabs(Vch)>0.75 ) omega = m_Gt075;
    if( fabs(Vch)>0.99 ) { // tracks have all the same charge
      if(Pfit.size()==2) omega = m_wSameSign2;
      if(Pfit.size() >2) omega = m_wSameSignMoreThan2;
    }
    nnet.m_rnet_vtx = fabs(Vch);
  } else {
    std::vector<double> NNinputs(10);
    NNinputs.at(0) = parts.size();
    NNinputs.at(1) = event.pileup();
    NNinputs.at(2) = event.signalB()->pt();
    NNinputs.at(3) = vflagged;
    NNinputs.at(4) = Vptmin;
    NNinputs.at(5) = Vipsmin;
    NNinputs.at(6) = Vdocamax;
    NNinputs.at(7) = maxprobf;
    NNinputs.at(8) = Vflaglong/(vflagged? vflagged:1);
    NNinputs.at(9) = fabs(Vch);

    omega = 1 - nnet.MLPvtx( NNinputs );
  }

  verbose() <<" VtxCh= "<< Vch <<" with "<< Pfit.size() <<" parts"
            <<", omega= "<< omega <<endmsg;

  if( 1-omega < m_ProbMin_vtx ) return tVch;
  if(   omega > m_ProbMin_vtx ) return tVch;

  verbose()<<"Vtx passed"<<endreq;

  tVch->setDecision( Vch>0 ? -1 : 1 );
  tVch->setRawNNetOutput( nnet.m_rnet_vtx );
  tVch->setOmega( omega );
  tVch->setType( Tagger::VtxCharge ); 
  for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) {
    tVch->addToTaggerParts(*ip);
  }
  
  return tVch;
}
