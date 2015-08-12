// $Id: HighPtIsoLeptonAndTagPV.cpp,v 1.1 2007/12/10 15:38:23 cpotterat Exp $
// ============================================================================
// Include files
// ============================================================================
// Local
// ============================================================================
#include "HighPtIsoLeptonAndTagPV.h"
// ============================================================================
//#include <Kernel/IContextTool.h>
#include <Kernel/IDistanceCalculator.h>
#include <Kernel/IVertexFit.h>
#include "LHCbMath/LHCbMath.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/ITupleTool.h"
#include "Kernel/DaVinciAlgorithm.h"
//#include <Kernel/IGeomDispCalculator.h>
#include "Kernel/IParticleTransporter.h"
#include "Event/ODIN.h"

// ============================================================================
// ============================================================================
/** @file
 *  Implementation file for class  HighPtIsoLeptonAndTagPV
 *  @author Cedric POTTERAT cedric.potterat@epfl.ch
 *  @date   2008-11-24
 */
// ============================================================================
/*  standard initialization of the tool
 *  @return status code
 */
// ============================================================================


DECLARE_ALGORITHM_FACTORY( HighPtIsoLeptonAndTagPV )



StatusCode HighPtIsoLeptonAndTagPV::initialize ()
{

  StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( 0 == m_pVertexFit )
    m_pVertexFit = vertexFitter("OfflineVertexFitter");

  if ( 0 == m_combiner )
    m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;

  if ( 0 == m_transporter )
    m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ;


  m_dist       = tool<IDistanceCalculator>("LoKi::DistanceCalculator");
  if ( !m_dist ) {
    err() << "Unable to Retrieve LoKi::DistanceCalculator" << endreq;
    return StatusCode::FAILURE;
  }


  if ( msgLevel(MSG::DEBUG) )
  {
    debug()<<" ------------------------------------------------------ "<<endmsg;
    debug()<<" |                                                    | "<<endmsg;
    debug()<<" |                Prompt Lepton                       | "<<endmsg;
    debug()<<" |                                                    | "<<endmsg;
    debug()<<" |              EPFLausanne - 2011                    | "<<endmsg;
    debug()<<" |             A. Bay & C. Potterat                   | "<<endmsg;
    debug()<<" |                                                    | "<<endmsg;
    debug()<<" ------------------------------------------------------ "<<endmsg;
    debug()<<" |                                                      "<<endmsg;
    debug()<<" |   parameters:                                        "<<endmsg;
    debug()<<" |   ISOPromptLeptonID   "<<   m_LeptID                  <<endmsg;
    debug()<<" |   NoISOPromptLeptonID "<<   m_niLeptID                <<endmsg;
    debug()<<" |   JetR                "<<   m_LeptRmax                <<endmsg;
    debug()<<" |   MuPtTrackMin        "<<   m_LeptMuPtTrackMin        <<endmsg;
    debug()<<" |   ElPtTrackMin        "<<   m_LeptElPtTrackMin        <<endmsg;
    debug()<<" |   IPmax               "<<   m_LeptIPmax               <<endmsg;
    debug()<<" |   EJoverEL            "<<   m_LeptEJoverEL            <<endmsg;
    debug()<<" |                                                      "<<endmsg;
    debug()<<" ------------------------------------------------------ "<<endmsg;
  }

  return sc ;
}

// ===========================================================================
// find the jets
// ===========================================================================
StatusCode HighPtIsoLeptonAndTagPV::execute()
{


  setFilterPassed(true);


  std::vector<LHCb::Particle> MyLeptons;
  LHCb::Particle::Vector LeptonsSeliso;
  LHCb::Particle::Vector LeptonsSelnoiso;

  StatusCode sc;

  //----------------------------------------//
  // Options for the PV:                    //
  // if multiple PV -> choose the one with  //
  // biggest the sum of tracks.Pt()         //
  //----------------------------------------//

  LHCb::Particles* IsoLept = new LHCb::Particles();
  LHCb::Particles* IsoLeptJet = new LHCb::Particles();
  Table* table = new Table();


  std::string location_iso, location_isoj ,location_isopv;
  std::string   location_noisopv  , location_noisoj,  location_noiso;
  std::string   location_z0pv  , location_z0;

  location_iso   =     "/Event/Phys/IsoLeptons/Particles";
  location_isoj      = "/Event/Phys/IsoLeptons/IsoLeptJets/Particles";

  std::string table_loc;
  table_loc = "/Event/Phys/IsoLeptons/Lept2JetsRelations";


  put(IsoLept, location_iso );
  put(IsoLeptJet, location_isoj );
  put(table, table_loc ) ;

  LHCb::Particle::ConstVector RealParts = this->i_particles();
  const LHCb::RecVertex::Range pvs = this->primaryVertices () ;

  if (pvs.size()<1)  return StatusCode::SUCCESS ;
  if (RealParts.size()<1)  return StatusCode::SUCCESS ;


  for ( LHCb::RecVertex::Range::const_iterator i_pv = pvs.begin() ; pvs.end() != i_pv ; i_pv++ )
  {


    std::vector<Gaudi::XYZVector> Slopes_flag;

    LHCb::Particle::ConstVector all;
    LHCb::Particle::Vector LeptonsSel;
    LHCb::Particle::Vector LeptonsSelJet;
    LHCb::RecVertex::Vector myPV;

    if ( msgLevel(MSG::DEBUG) ) debug()  << "(*i_pv)               ->position = " << (*i_pv)->position()<< endmsg;


    for (  LHCb::Particle::ConstVector::const_iterator ip = RealParts.begin();
           RealParts.end() != ip ; ++ip )     {
      const LHCb::Particle* p = *ip ;

      if ( 0 == p ) { Warning ( "Invalid input particle" ) ; continue ; }

      if (p->charge() == 0 ){
        if ( p->particleID().abspid() != 310 && p->particleID().abspid() != 3122 ) {}
        else if ( (DaVinciAlgorithm::bestPV(p))->key() == (*i_pv)->key() ) {}
        else
          continue ;
      }
      else{

        if (( p->particleID().isLepton()) ){
          if ( msgLevel(MSG::DEBUG) ) debug()  << "p->particleID().abspid() = " << p->particleID().abspid()<< endmsg;

          if ( msgLevel(MSG::DEBUG) ) debug()  << "DaVinciAlgorithm::bestPV(p)->position = " << (DaVinciAlgorithm::bestPV(p))->position()<< endmsg;

        }

        if ( LHCb::Track::Downstream == p->proto()->track()->type() ) {}
        else if ((DaVinciAlgorithm::bestPV(p))->key() == (*i_pv)->key() ) {}
        else {}//continue ;}
      }



      bool m_FilterPart = true;
      //----------------------------------------//
      // check if 1 particle is not twice on    //
      // the TES with the slopes                //
      //----------------------------------------//
      bool flag_same = false;
      if  (m_FilterPart){
        if(Slopes_flag.size() !=0){
          for(int k = 0; k< (int)Slopes_flag.size();k++){
            if(Slopes_flag.at(k) == p->slopes()){
              flag_same = true;
              // verbose()  << "same slopes !!  s1: "  <<Slopes_flag.at(k)  << "   s2: " << p->slopes() <<endmsg;
              break;
            }
          }
        }
        if(!flag_same){
          Slopes_flag.push_back(p->slopes());
        }
        else{
          continue;
        }
      }



      all.push_back(p);

      // verbose()  << "p->particleID().abspid() = " << p->particleID().abspid()<< endmsg;
      // verbose()  << "p->charge() = " << p->charge()<< endmsg;
      if( p->charge() == 0 ) continue;
      // verbose()  << "p->proto() = " << p->proto()<< endmsg;
      if( p->proto()==NULL ) continue;
      // verbose()  << "p->proto()->track() = " << p->proto()->track()<< endmsg;
      if( p->proto()->track()==NULL ) continue;
      // verbose()  << "p->proto()->track()->type() = " << p->proto()->track()->type()<< endmsg;
      if( p->proto()->track()->type() != LHCb::Track::Long ) continue;
      // verbose()  << "p->proto()->track()->chi2PerDoF() = " << p->proto()->track()->chi2PerDoF()<< endmsg;
      if( p->proto()->track()->chi2PerDoF() >  m_TrkChi2DoF ) continue;


      double ipl =0.;
      double chi2l = 9999999999999.;

      LHCb::VertexBase* VertB =  (*i_pv)->clone();
      sc = m_dist->distance(p, VertB, ipl, chi2l);

      delete VertB;
      if(sc.isFailure() ) {
        //   warning()<< "can measure dist ip, chi2 from pv"<< endmsg;
        continue;
      }
      // verbose()  << "      delete *VertB;"<< endmsg;
      // verbose()  << "p->particleID() = "<<p->particleID() << endmsg;
      // verbose()  << "p->particleID().abspid( = "<<p->particleID().abspid() << endmsg;

      if (!( p->particleID().isLepton()) ) continue;
      if ( msgLevel(MSG::DEBUG) ) debug()  << "lepton :"<<p->particleID().abspid()<<" (pt :"<<p->pt()<<"): ip / chi2: "<< ipl<<" "<<chi2l<<" "<<endmsg;

      if ( p->particleID().abspid() == 11 )
        if ( p->pt()<m_LeptElPtTrackMin )   continue;

      if ( p->particleID().abspid() == 13 )
        if ( p->pt()<m_LeptMuPtTrackMin )   continue;

      if ( sqrt(chi2l) > m_LeptSignif) continue;

      if ( ipl > m_LeptIPmax ) continue;

      LeptonsSel.push_back(p->clone());

      myPV.push_back((*i_pv)->clone());


    }//end particles


    //----------------------------------------//
    // prepare the inputs                     //
    // selected the good particles for the    //
    // isoletp  algo (ip,ipe,charge,pt......) //
    // and the prompt leptons                 //
    //----------------------------------------//

    if ( msgLevel(MSG::DEBUG) ) debug()  <<"Particle ALL size                       = " << RealParts.size() << endmsg;
    if ( msgLevel(MSG::DEBUG) ) debug()  <<"Particle ALL size (JetCone)             = " << all.size() << endmsg;
    if ( msgLevel(MSG::DEBUG) ) debug()  <<"NLeptons for iso test                   = " << LeptonsSel.size() << endmsg;




    //----------------------------------------//
    // get the Isolated and NoIso Leptons     //
    // check on the prompt leptons            //
    //----------------------------------------//
    int isol=0;
    if((int)LeptonsSel.size() > 0 ){

      //----------------------------------------//
      // check the Energy of the cone arround   //
      // the lepton, if EJ over EL is < than ...//
      // the lepton is locally isolated         //
      //----------------------------------------//

      LHCb::Particle::Vector::iterator ip1;
      int mv = 0;
      for ( ip1=LeptonsSel.begin(); ip1 != LeptonsSel.end(); ++ip1) {
        LHCb::Particle* p_iso = *ip1 ;



        LHCb::Particle P_jet = JetCone(m_LeptRmax,  Gaudi::XYZPoint(p_iso->momentum().Vect()) , p_iso->referencePoint(),m_LeptID , all);
        if ( msgLevel(MSG::DEBUG) ) debug() << "TEST | ID:  "<< p_iso->particleID().pid() <<" | Pt: "<<  p_iso->pt() <<" | EJ over EL: "<<  P_jet.momentum().E()/p_iso->momentum().E() <<endmsg;

        if((P_jet.momentum().E()/p_iso->momentum().E()) < m_LeptEJoverEL ){
          LHCb::Particle* P_Jet = P_jet.clone();
          p_iso->setReferencePoint((*i_pv)->position());
          IsoLept->insert(p_iso);
          IsoLeptJet->insert(P_Jet);
          table->relate(p_iso,P_Jet);
          isol++;
          if ( msgLevel(MSG::DEBUG) ) debug() << "Ilept"<<mv<<"| ID:  "<< p_iso->particleID().pid() <<" | Pt: "<<  p_iso->pt() <<" | EJ over EL: "<<  P_jet.momentum().E()/p_iso->momentum().E() <<endmsg;
          if ( msgLevel(MSG::DEBUG) ) debug() <<       "       | m  : "<<  p_iso->momentum() << endmsg;
          if ( msgLevel(MSG::DEBUG) ) debug() <<       "       | eta: "<<   p_iso->momentum().eta() << "  / phi: " <<    p_iso->momentum().phi() <<endmsg;
        }
      }
      mv++;


    }//end of p
  }



  if ( statPrint() || msgLevel ( MSG::DEBUG ) )
  { counter ( "#Plepton" ) += MyLeptons.size()  ; }




  //----------------------------------------//
  setFilterPassed(true);
  return StatusCode::SUCCESS ;


}




//=============================================================================
// JETCONE: THE HEART OF EVERYTHING, BUILDS A JET AROUND A SEED
//=============================================================================
LHCb::Particle HighPtIsoLeptonAndTagPV::JetCone(
                                                const  double &Rmax,
                                                Gaudi::XYZPoint v,
                                                Gaudi::XYZPoint v1,
                                                const int& myID ,
                                                LHCb::Particle::ConstVector &parts) const{
  // kind =0 : all
  //       1:  chg
  //       2 : pho
  //  using namespace LoKi          ;
  //  using namespace LoKi::Fits    ;
  //  using namespace LoKi::Cuts    ;
  //  using namespace LoKi::Extract ;


  LHCb::Particle              pJet ;
  LHCb::Vertex                vJet      ;
  LHCb::Particle::ConstVector daughters ;

  pJet.setParticleID     (  LHCb::ParticleID( myID)) ;

  pJet.setReferencePoint ( v1 ) ;


  const double PI=3.14159265;
  double R,Dphi,phi,phi2,e,e2;

  const LHCb::ParticleID IDg(22);
  // Particle::ConstVector* JetParts;

  //  Range parts = select ( "all" , PALL ) ;
  //  const Particle::ConstVector& parts = PartIP;
  //  const Particle::ConstVector& parts = m_context->desktop()->particles();
  if ( msgLevel(MSG::DEBUG) ) debug()  <<"Particle size (JetCone)= " << parts.size() << endmsg;

  phi = v.phi();  e   = v.eta();
  Gaudi::LorentzVector ptot =  Gaudi::LorentzVector(0., 0., 0. ,0.);
  LHCb::Particle::ConstVector::const_iterator ip;


  for ( ip=parts.begin(); ip != parts.end(); ++ip) {


    const LHCb::Particle * myPart = *ip;
    Gaudi::LorentzVector p1 =  (*ip)->momentum();


    LHCb::ParticleID ID = myPart->particleID();

    phi2 = (*ip)->momentum().phi();
    e2   = (*ip)->momentum().eta();
    Dphi = fabs(phi-phi2);
    if (Dphi>PI) { Dphi = 2*PI - Dphi; };
    R = sqrt( Dphi*Dphi + (e-e2)*(e-e2) );

    if (R>Rmax)      continue;
    ptot = ptot + p1;
    daughters.push_back ( myPart ) ;


  }

  if ( daughters.empty() )
  {
    Warning("Empty list of of daughter particles, skip it").ignore() ;
  }

  StatusCode sc = m_combiner->combine ( daughters , pJet , vJet ) ;
  if ( sc.isFailure() )    {
    Warning ( "Error from momentum combiner, skip" , sc, 0 ) ;

  }

  pJet.setMomentum        ( Gaudi::LorentzVector ( ptot )  ) ;

  return pJet;

}
//=============================================================================
// END OF JETCONE
//=============================================================================
