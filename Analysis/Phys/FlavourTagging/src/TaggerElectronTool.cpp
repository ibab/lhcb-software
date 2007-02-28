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
  declareProperty( "NeuralNetName",  m_NeuralNetName   = "NNetTool_v1" );
  declareProperty( "Ele_Pt_cut",   m_Pt_cut_ele = 1.2 );
  declareProperty( "Ele_P_cut",    m_P_cut_ele  = 5.0 );
  declareProperty( "VeloChargeMin",m_VeloChMin  = 0.0 );
  declareProperty( "VeloChargeMax",m_VeloChMax  = 1.8 );
  declareProperty( "EoverP",       m_EoverP     = 0.92 );
  declareProperty( "AverageOmega", m_AverageOmega = 0.30 );
  m_nnet = 0;
  m_Geom = 0;
}
TaggerElectronTool::~TaggerElectronTool() {};

//=====================================================================
StatusCode TaggerElectronTool::initialize() {

  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
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
  const Vertex * SecVert= 0;
  if(!allVtx.empty()) SecVert = allVtx.at(0);

  //select electron tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  Particle::ConstVector vele(0);
  const Particle* iele=0;
  double ptmaxe = -99.0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 11 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_ele )  continue;
    double P = (*ipart)->p()/GeV;
    if( P < m_P_cut_ele )  continue;

    double Emeas= -1;
    const ProtoParticle* proto = (*ipart)->proto();
    const Track* track = proto->track();
    const SmartRefVector<CaloHypo>& vcalo = proto->calo();
    //if(vcalo.size()==1) Emeas = (vcalo.at(0))->e()/GeV; //xxx//crashes
    double eOverP = -999.9;
    if(m_electron->set(*ipart)){
      eOverP = m_electron->eOverP();
    }
    debug() << " Elec P="<<P <<" Pt="<<Pt
            << " type=" << track->type() << " Emeas/P=" << Emeas/P <<endreq;

    if(eOverP > m_EoverP || eOverP<-100) {
      if(track->type() == Track::Long) {
        double veloch = proto->info( ProtoParticle::VeloCharge, 0.0 );
        if(!veloch) debug()<<"VeloCharge is not running?" <<endreq;
        if( veloch > m_VeloChMin && veloch < m_VeloChMax ) {
          debug() << "   veloch=" << veloch << endreq;

          if( Pt > ptmaxe ) {
            iele = (*ipart);
            ptmaxe = Pt;
          }

        }
      }
    }
  }
  if( !iele ) return tele;
  tele.addTaggerPart(*iele);
  tele.setDecision(iele->charge()>0 ? -1: 1);

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    Gaudi::LorentzVector ptotB = AXB0->momentum();
    double B0the  = ptotB.Theta();
    double B0p    = ptotB.P()/GeV;
    double IP, IPerr, ip, iperr, IPT=0.;

    calcIP(iele, RecVert, IP, IPerr);
    if(SecVert) {
      calcIP(iele, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.;
    std::vector<double> inputs(8);
    inputs.at(0) = B0p;
    inputs.at(1) = B0the;
    inputs.at(2) = vtags.size();
    inputs.at(3) = 100;
    inputs.at(4) = iele->p()/GeV;
    inputs.at(5) = iele->pt()/GeV;
    inputs.at(6) = IP/IPerr;
    inputs.at(7) = IPT;

    pn = m_nnet->MLPe( inputs );

  }
  tele.setOmega( 1-pn );
  tele.setType( Tagger::OS_Electron );

  return tele;
}
//==========================================================================
StatusCode TaggerElectronTool::calcIP( const Particle* axp,
                                       const Vertex* RecVert,
                                       double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc =
    m_Geom->calcImpactPar(*axp, *RecVert, ip, iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ;
    iperr= iperr;
  }
  return sc;
}
//==========================================================================
StatusCode TaggerElectronTool::calcIP( const Particle* axp,
                                       const RecVertex* RecVert,
                                       double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Gaudi::XYZVector ipVec;
  Gaudi::SymMatrix9x9 errMatrix;
  StatusCode sc =
    m_Geom->calcImpactPar(*axp, *RecVert, ip, iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ;
    iperr= iperr;
  }
  return sc;
}
//====================================================================
StatusCode TaggerElectronTool::finalize() { return StatusCode::SUCCESS; }

