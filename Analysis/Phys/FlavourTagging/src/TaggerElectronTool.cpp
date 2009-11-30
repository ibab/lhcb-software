// Include files 
#include "TaggerElectronTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerElectronTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerElectronTool );

//====================================================================
TaggerElectronTool::TaggerElectronTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {

  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName   = "NNetTool_MLP" );
  declareProperty( "Ele_Pt_cut",   m_Pt_cut_ele = 1.1 * GeV );
  declareProperty( "Ele_P_cut",    m_P_cut_ele  = 0.0 * GeV );
  declareProperty( "Ele_lcs_cut",  m_lcs_cut_ele= 2.0 );
  declareProperty( "Ele_ghost_cut",m_ghost_cut_ele= -20.0 );
  declareProperty( "VeloChargeMin",m_VeloChMin  = 0.0 );
  declareProperty( "VeloChargeMax",m_VeloChMax  = 1.3 );
  declareProperty( "EoverP",       m_EoverP     = 0.85 );
  declareProperty( "AverageOmega", m_AverageOmega = 0.33 );
  m_nnet = 0;
  m_util = 0;
  m_electron = 0;
}
TaggerElectronTool::~TaggerElectronTool() {}; 

//=====================================================================
StatusCode TaggerElectronTool::initialize() { 

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_electron = tool<ICaloElectron>( "CaloElectron");
  if(! m_electron) {
    fatal() << "Unable to retrieve ICaloElectronTool"<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerElectronTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                                std::vector<const Vertex*>& allVtx,
                                Particle::ConstVector& vtags ){
  Tagger tele;
  if(!RecVert) return tele;

  verbose()<<"allVtx.size()="<< allVtx.size() << endreq;

  //select electron tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  Particle::ConstVector vele(0);
  const Particle* iele=0;
  double ptmaxe = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    
    if( (*ipart)->particleID().abspid() != 11 ) continue;
    bool inEcalACC= (*ipart)->proto()->info(ProtoParticle::InAccEcal,false);
    if(!inEcalACC) continue;
    
    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_ele )  continue;

    double P = (*ipart)->p();
    if( P < m_P_cut_ele )  continue;

    const Track* track = (*ipart)->proto()->track();
    double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut_ele ) continue;
    if(track->type() != Track::Long 
       && track->type() != Track::Upstream) continue;

    double tsa = track->likelihood();
    if(tsa < m_ghost_cut_ele) continue;

    double eOverP  = -999;
    if( m_electron->set(*ipart) ) { 
      eOverP  = m_electron->eOverP();
    }
 
    if(eOverP > m_EoverP || eOverP<-100) {
      verbose() << " Elec P="<<P <<" Pt="<<Pt << " E/P=" << eOverP <<endreq;

      double veloch = (*ipart)->proto()->info( ProtoParticle::VeloCharge, 0.0 );
      if( veloch > m_VeloChMin && veloch < m_VeloChMax ) {
        verbose() << "   veloch=" << veloch << endreq;
        
        ncand++;
        if( Pt > ptmaxe ) { 
          iele = (*ipart);
          ptmaxe = Pt;
        }
      }
    }
  }
  if( !iele ) return tele;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {

    double IP, IPerr;
    m_util->calcIP(iele, RecVert, IP, IPerr);

    std::vector<double> NNinputs(10);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;;
    NNinputs.at(2) = iele->p()/GeV;
    NNinputs.at(3) = iele->pt()/GeV;
    NNinputs.at(4) = IP/IPerr;
//    NNinputs.at(8) = m_util->getNvtx();
    NNinputs.at(8) = allVtx.size();
    NNinputs.at(9) = ncand;

    pn = m_nnet->MLPe( NNinputs );

  }

  tele.setOmega( 1-pn );
  tele.setDecision(iele->charge()>0 ? -1: 1);
  tele.addToTaggerParts(iele);
  tele.setType( Tagger::OS_Electron );

  return tele;
}

//====================================================================
StatusCode TaggerElectronTool::finalize() { return StatusCode::SUCCESS; }

