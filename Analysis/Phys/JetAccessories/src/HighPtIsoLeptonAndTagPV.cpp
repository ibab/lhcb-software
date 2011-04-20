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
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/ITupleTool.h"
#include "Kernel/DVAlgorithm.h"
//#include <Kernel/IGeomDispCalculator.h>
#include "Kernel/IParticleTransporter.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "Event/ODIN.h"
#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetDVAlgorithm.h>



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


DECLARE_ALGORITHM_FACTORY( HighPtIsoLeptonAndTagPV );



StatusCode HighPtIsoLeptonAndTagPV::initialize () 
{

  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( 0 == m_pVertexFit ) 
    m_pVertexFit = vertexFitter("OfflineVertexFitter"); 

  if ( 0 == m_combiner ) 
    m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ; 
  
  if ( 0 == m_transporter ) 
    m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ; 

  
  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
			     StatusCode::FAILURE);
  


  m_dist       = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  if ( !m_dist ) {
    err() << "Unable to Retrieve LoKi::DistanceCalculator" << endreq;
    return StatusCode::FAILURE;
  }


  
  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |                Prompt Lepton                       | "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |              EPFLausanne - 2011                    | "<<std::endl;
  std::cout<<" |             A. Bay & C. Potterat                   | "<<std::endl; 
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                      "<<std::endl;
  std::cout<<" |   parameters:                                        "<<std::endl;
  std::cout<<" |   ISOPromptLeptonID   "<<   m_LeptID                  <<std::endl;
  std::cout<<" |   NoISOPromptLeptonID "<<   m_niLeptID                <<std::endl;
  std::cout<<" |   JetR                "<<   m_LeptRmax                <<std::endl;
  std::cout<<" |   MuPtTrackMin        "<<   m_LeptMuPtTrackMin        <<std::endl;
  std::cout<<" |   ElPtTrackMin        "<<   m_LeptElPtTrackMin        <<std::endl;
  std::cout<<" |   IPmax               "<<   m_LeptIPmax               <<std::endl;
  std::cout<<" |   EJoverEL            "<<   m_LeptEJoverEL            <<std::endl;
  std::cout<<" |                                                      "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;
  
  return StatusCode::SUCCESS ;
} 

// ===========================================================================
// find the jets 
// ===========================================================================
StatusCode HighPtIsoLeptonAndTagPV::execute() 
{



  

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
  LHCb::Particles* NoIsoLept = new LHCb::Particles();
  LHCb::Particles* IsoLeptJet = new LHCb::Particles();
  LHCb::Particles* NoIsoLeptJet = new LHCb::Particles();
  LHCb::RecVertices* IsoLeptVtx = new LHCb::RecVertices();
  LHCb::RecVertices* NoIsoLeptVtx = new LHCb::RecVertices();
  LHCb::Particles* Z0 = new LHCb::Particles();
  LHCb::RecVertices* Z0PV = new LHCb::RecVertices();
  Table* table = new Table();
  
  
  std::string location_iso, location_isoj ,location_isopv;
  std::string   location_noisopv  , location_noisoj,  location_noiso;
  std::string   location_z0pv  , location_z0;
  location_iso   =     "/Event/Phys/IsoLeptons/IsoLeptParts/Particles";
  location_isoj      = "/Event/Phys/IsoLeptons/IsoLeptJets/Particles";
  location_isopv     = "/Event/Phys/IsoLeptons/IsoLeptPV/Vertecies";

  location_noiso     = "/Event/Phys/IsoLeptons/NoIsoLeptParts/Particles";
  location_noisoj    = "/Event/Phys/IsoLeptons/NoIsoLeptJets/Particles";
  location_noisopv   = "/Event/Phys/IsoLeptons/NoIsoLeptPV/Vertecies";
    
  location_z0  = "/Event/Phys/IsoLeptons/Z0/Particles";
  location_z0pv   = "/Event/Phys/IsoLeptons/Z0PV/Vertecies";

  std::string table_loc;
  table_loc = "/Event/Phys/IsoLeptons/IsoLeptParts/Lept2JetsRelations";
             

  put(IsoLept, location_iso );
  put(IsoLeptJet, location_isoj );
  put(IsoLeptVtx, location_isopv );

  put(NoIsoLept, location_noiso );
  put(NoIsoLeptJet, location_noisoj );
  put(NoIsoLeptVtx, location_noisopv );
  

  put(Z0, location_z0);
  put(Z0PV, location_z0pv);
  put ( table , table_loc ) ; 

  const LHCb::RecVertex::Range PVs =  this->primaryVertices();
  LHCb::Particle::ConstVector RealParts = this->i_particles();  
  
  
  for(LHCb::RecVertex::Range::const_iterator  ipv2= PVs.begin();
      ipv2!=PVs.end();++ipv2){
    const  LHCb::RecVertex* myV0 = *ipv2;
    
    std::vector<Gaudi::XYZVector> Slopes_flag;
    
    LHCb::Particle::ConstVector all;
    LHCb::Particle::Vector LeptonsSel;
    LHCb::Particle::Vector LeptonsSelJet;
    LHCb::RecVertex::Vector myPV;

    //----------------------------------------//
    // remove tracks from another PV          //
    //----------------------------------------//
    
    for(LHCb::RecVertex::Range::const_iterator  ipv3= PVs.begin();
	ipv3!=PVs.end();++ipv3){
      const  LHCb::RecVertex myV02 = *(*ipv3);
      if(myV0->position() != myV02.position())
	RemoveTracks( RealParts, myV02);
    }
    
    
    for (  LHCb::Particle::ConstVector::const_iterator ip = RealParts.begin(); 
	   RealParts.end() != ip ; ++ip )     {
      const LHCb::Particle* p = *ip ;
      
      
      if ( 0 == p ) { Warning ( "Invalid input particle" ) ; continue ; }         
      const LHCb::ProtoParticle * myProto = p->proto();
      if(myProto != NULL){
	const LHCb::Track* trk   = myProto->track();
	if( trk != NULL ){
	  if( trk->flags() == 1 || trk->flags() == 4) continue;
	  // == 1 : backward, == 4 clone
	  if( trk->flags() == 1) continue;
	  double CloneDist = trk->info(LHCb::Track::CloneDist,9999.);
	  double TrkLi =     trk->likelihood();
	  double GhostProb = trk->ghostProbability();
	  double LongTrk =   trk->type();         
	  verbose()  << "addinfo Likelihood = " << TrkLi << endmsg;
	  verbose()  << "addinfo CloneDist  = " << CloneDist<< endmsg;
	  verbose()  << "addinfo GhostProb  = " << GhostProb << endmsg;
	  verbose()  << "addinfo Longb      = " << LongTrk << endmsg;
	  verbose()  << "p Trk   = " <<  trk->momentum() / Gaudi::Units::GeV << endmsg;    
	  //----------------------------------------//
	  // discard clones and Ghost               //
	  //----------------------------------------//
	  if(CloneDist < 9999.) continue;
	  if(TrkLi     < -60. ) continue;	  
	}
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
	      verbose()  << "same slopes !!  s1: "  <<Slopes_flag.at(k)  << "   s2: " << p->slopes() <<endmsg;
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

      
      if( p->charge() == 0 ) continue;
      if( p->proto()==NULL ) continue;
      if( p->proto()->track()==NULL ) continue;
      if( p->proto()->track()->type() != LHCb::Track::Long ) continue;
      if( p->proto()->track()->chi2PerDoF() >  m_TrkChi2DoF ) continue;


      double ipl =0.;
      double chi2l = 9999999999999.;   

      LHCb::VertexBase* VertB =  myV0->clone();
      sc = m_dist->distance(p, VertB, ipl, chi2l);

      delete VertB;
      if(sc.isFailure() ) {
	warning()<< "can measure dist ip, chi2 from pv"<< endmsg;
	continue;
      }
      verbose()  << "      delete *VertB;"<< endmsg;    

      if (!( p->particleID().isLepton()) ) continue;
      debug()  << "lepton :"<<p->particleID().abspid()<<" (pt :"<<p->pt()<<"): ip / chi2: "<< ipl<<" "<<chi2l<<" "<<endmsg;
      
      if ( p->particleID().abspid() == 11 )
	if ( p->pt()<m_LeptElPtTrackMin )   continue; 	

      if ( p->particleID().abspid() == 13 )
	if ( p->pt()<m_LeptMuPtTrackMin )   continue; 

      if ( sqrt(chi2l) > m_LeptSignif) continue;

      if ( ipl > m_LeptIPmax ) continue;

      LeptonsSel.push_back(p->clone());

      myPV.push_back(myV0->clone());   


    }//end particles
  
  
    //----------------------------------------//
    // prepare the inputs                     //
    // selected the good particles for the    //
    // isoletp  algo (ip,ipe,charge,pt......) //
    // and the prompt leptons                 //
    //----------------------------------------//   

    debug()  <<"Particle ALL size                       = " << RealParts.size() << endmsg;
    debug()  <<"Particle ALL size (JetCone)             = " << all.size() << endmsg;
    debug()  <<"NLeptons for iso test                   = " << LeptonsSel.size() << endmsg;




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
	LHCb::Particle* p = *ip1 ;

    
	LHCb::Particle::ConstVector input2;

	LHCb::Particle P = JetCone(m_LeptRmax,  Gaudi::XYZPoint(p->momentum().Vect()) , p->referencePoint(),m_LeptID , all);
	LeptonsSelJet.push_back(P.clone());
  
	table->relate(p,LeptonsSelJet.back());


	p->setReferencePoint( myV0->position());
     
	if((P.momentum().E()/p->momentum().E()) > m_LeptEJoverEL ){
	  P.setParticleID     (  LHCb::ParticleID( m_niLeptID)) ;  //change the ID if not iso
	  NoIsoLept->insert(p);
	  NoIsoLeptJet->insert(LeptonsSelJet.back());
	  NoIsoLeptVtx->insert( myV0->clone() );
	  debug() << "NIlept "<<mv<<"| ID:  "<< p->particleID().pid() <<" | Pt: "<<  p->pt() <<" | EJ over EL: "<<  P.momentum().E()/p->momentum().E() <<endmsg;
	  debug() <<       "       | m  : "<<  p->momentum() << endmsg;
	  debug() <<       "       | eta: "<<   p->momentum().eta() << "  / phi: " <<    p->momentum().phi() <<endmsg;
       

	}else{
	  IsoLept->insert(p);
	  IsoLeptJet->insert(LeptonsSelJet.back());
	  IsoLeptVtx->insert( myV0->clone() );
	  isol++;
	  debug() << "Ilept"<<mv<<"| ID:  "<< p->particleID().pid() <<" | Pt: "<<  p->pt() <<" | EJ over EL: "<<  P.momentum().E()/p->momentum().E() <<endmsg;
	  debug() <<       "       | m  : "<<  p->momentum() << endmsg;
	  debug() <<       "       | eta: "<<   p->momentum().eta() << "  / phi: " <<    p->momentum().phi() <<endmsg;
     

	}
	mv++;
	
	
      }//end of p
    } 
    
    

    if((int)LeptonsSelJet.size() > 1 ){
      LHCb::Particle::Vector::iterator ip1;
      int i = 0;      

      for ( ip1=LeptonsSelJet.begin(); ip1 != LeptonsSelJet.end(); ++ip1) {
	LHCb::Particle* jet1 = *ip1 ;	
	int j = i+1;   
	LHCb::Particle::Vector::iterator ip2;	

	for ( ip2=ip1+1; ip2 != LeptonsSelJet.end(); ++ip2) {
	  LHCb::Particle* jet2 = *ip2 ;

	  if(LeptonsSel.at(i)->particleID().pid() +  LeptonsSel.at(j)->particleID().pid() == 0){
	  

	    LHCb::Particle              pZ;
	    LHCb::Vertex                vZ;
	    int IDZ = (int) jet1->particleID().pid() + (int) jet2->particleID().pid();
	    pZ.setParticleID     (  LHCb::ParticleID(IDZ) ) ;
	    
	    pZ.setReferencePoint (  myV0->position() ) ;
  


	    LHCb::Particle::ConstVector Zdaughters ;

	    Gaudi::LorentzVector myZ = Gaudi::LorentzVector (
							   Gaudi::LorentzVector(LeptonsSel.at(i)->momentum()) +
							   Gaudi::LorentzVector ( LeptonsSel.at(j)->momentum()));
	    
	    pZ.setMomentum        ( Gaudi::LorentzVector (  myZ )  ) ;  
	    
	    
	    Zdaughters.push_back ( LeptonsSel.at(i) ) ;
  	    Zdaughters.push_back ( LeptonsSel.at(j) ) ;
	    
	    
	    StatusCode sc = m_combiner->combine ( Zdaughters , pZ , vZ ) ;
	    
	    Z0->insert( pZ.clone());
	    Z0PV->insert( myV0->clone());

	    double Zmass = myZ.M()/1000.;
	    plot(Zmass,"ZmassAll", "ZmassAll", 0.,150.,300);
	    
	    if(jet1->particleID().pid() ==m_LeptID && jet2->particleID().pid() ==m_LeptID )
	      plot(Zmass,"ZmassII", "ZmassII", 0.,150.,300);	  
	    
	    if(jet1->particleID().pid() !=m_LeptID && jet2->particleID().pid() ==m_LeptID )
	      plot(Zmass,"ZmassIN", "ZmassIN", 0.,150.,300);
	    
	    if(jet1->particleID().pid() ==m_LeptID && jet2->particleID().pid() !=m_LeptID )
	      plot(Zmass,"ZmassIN", "ZmassIN", 0.,150.,300);
	    
	    if(jet1->particleID().pid() !=m_LeptID && jet2->particleID().pid() !=m_LeptID )
	      plot(Zmass,"ZmassNN", "ZmassNN", 0.,150.,300);
	  }
	  
	  j++;
	}  //end of p2   	
	i++;
      }  //end of p1   
    }    
  }//end PVs
  




  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
    { counter ( "#Plepton" ) += MyLeptons.size()  ; }
  



  //----------------------------------------// 
  setFilterPassed(true); 
  return StatusCode::SUCCESS ;
  

}







//=============================================================================
// Remove trks form a PV
//=============================================================================

void HighPtIsoLeptonAndTagPV::RemoveTracks(LHCb::Particle::ConstVector & particles, 
					   const LHCb::RecVertex PV ) const{

  //Remove all tracks from the PV
  LHCb::Particle::ConstVector tmp;
  SmartRefVector< LHCb::Track >::const_iterator iPV = PV.tracks().begin();
  int endkey = PV.tracks().back()->key();

  for (  LHCb::Particle::ConstVector::const_iterator i = particles.begin();
	 i != particles.end();++i) {
    if( (*i)->proto()->track() == NULL ) continue; 
    const LHCb::Track * tk = (*i)->proto()->track();

    while( ((*iPV)->key() < tk->key()) && (*iPV)->key() != endkey ){
      iPV++;
    }
    if( (*iPV)->key() == tk->key() ){ 
      if( (*iPV)->key() != endkey ) iPV++; 
      continue;
    }
    tmp.push_back( *i );
  }
  //Copy back tmp to particles
  particles = tmp;

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
  debug()  <<"Particle size (JetCone)= " << parts.size() << endmsg;

  phi = v.phi();  e   = v.eta();
  Gaudi::LorentzVector ptot =  Gaudi::LorentzVector(0., 0., 0. ,0.);
  LHCb::Particle::ConstVector::const_iterator ip;


  for ( ip=parts.begin(); ip != parts.end(); ++ip) {
    
    
    const LHCb::Particle* myPart = *ip; 
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
      Warning("Empty list of of daughter particles, skip it") ;
  
    }

  

  StatusCode sc = m_combiner->combine ( daughters , pJet , vJet ) ;
  if ( sc.isFailure() )    {
    Error ( "Error from momentum combiner, skip" , sc ) ;
  
  }
  
  pJet.setMomentum        ( Gaudi::LorentzVector ( ptot )  ) ;  

  return pJet;

}
//=============================================================================
// END OF JETCONE
//=============================================================================


//=============================================================================
// Finalization
//=============================================================================
StatusCode HighPtIsoLeptonAndTagPV::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}

