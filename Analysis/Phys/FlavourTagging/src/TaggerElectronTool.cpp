// Include files 
#include "TaggerElectronTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerElectronTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TaggerElectronTool>          s_factory ;
const        IToolFactory& TaggerElectronToolFactory = s_factory ; 

//====================================================================
TaggerElectronTool::TaggerElectronTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {

  declareInterface<ITagger>(this);

  declareProperty( "TrVeloChargeName",m_veloChargeName = "TrVeloCharge" );
  declareProperty( "Ele_Pt_cut",   m_Pt_cut_ele = 1.0 );
  declareProperty( "Ele_P_cut",    m_P_cut_ele  = 5.0 );
  declareProperty( "VeloChargeMin",m_VeloChMin  = 21  );
  declareProperty( "VeloChargeMax",m_VeloChMax  = 32  );
  declareProperty( "EoverP",       m_EoverP     = 0.8 );
}
TaggerElectronTool::~TaggerElectronTool() {}; 

//=====================================================================
StatusCode TaggerElectronTool::initialize() { 

  m_veloCharge= tool<ITrVeloCharge> (m_veloChargeName, this);
  if ( ! m_veloCharge ) {   
    fatal() << " Unable to retrieve " << m_veloChargeName << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}

//=====================================================================
ParticleVector TaggerElectronTool::taggers( const Particle* AXB0, 
                                            const Vertex* RecVert, 
                                            const ParticleVector& vtags ){
  verbose() << "B pt=" << AXB0->pt()/GeV 
            << " VTXz="<< RecVert->position().z() <<endreq;

  //select electron tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  ParticleVector vele(0);
  Particle* ele=0;
  double ptmaxe = -99.0;
  ParticleVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    if( (*ipart)->particleID().abspid() != 11 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_ele )  continue;
    double P = (*ipart)->p()/GeV;
    if( P < m_P_cut_ele )  continue;

    long   trtyp= 0;
    double Emeas= -1;
    double lcs  = 1000.;
    ContainedObject* contObj = (*ipart)->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      if ( proto ) {
        SmartRefVector<CaloHypo>& vcalo = proto->calo();
        if(vcalo.size()==1) Emeas = (*(vcalo.begin()))->e()/GeV;
        TrStoredTrack* track = proto->track();
        if((track->measurements()).size() > 5)
          lcs = track->lastChiSq()/((track->measurements()).size()-5);
        if(track->forward()) trtyp = 1;
      }
    }
    
    debug() << " Elec P="<<P <<" Pt="<<Pt 
            << " trtyp=" << trtyp << " Emeas/P=" << Emeas/P <<endreq;

    if(Emeas/P > m_EoverP || Emeas<0) {
      if(trtyp == 1) {
        double veloch = m_veloCharge->calculate(*ipart);
        debug() << "      veloch=" <<veloch << endreq;
        if(veloch > m_VeloChMin && veloch < m_VeloChMax ) {
          if( Pt > ptmaxe ) { 
            ele = (*ipart);
            ptmaxe = Pt;
          }
        }
      } 
    }
  } 
  if( ele ) vele.push_back( ele );

  return vele;
}
//====================================================================
StatusCode TaggerElectronTool::finalize() { return StatusCode::SUCCESS; }

