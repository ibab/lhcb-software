// $Id: FilterToFixOppositeBFractions.cpp,v 1.3 2007-02-27 10:20:13 sposs Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "FilterToFixOppositeBFractions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterToFixOppositeBFractions
//
// This Filter is created to fix the Bug2 observed on DC06 data
// and reported by Patrick Robbe on the 13 feb 2007.
// This bug affects the B composition of the opposite side. This filter will 
// remove specific fraction of events where abs(BsigID) equal abs(BoppoID)
// depending on events type.
// It checks for signal B, using HepMC, looks for opposite B using the MC
// originVertex() method. The case with more than 2 B's coming from the same
// vertex is not handled for the time being.
// To use it, add :
// ApplicationMgr.DLLs += { "DaVinciMCTools" };
// and create a sequence with all the algorithms that have to be processed 
// each events. The FilterToFixOppositeBFractions has to be the first in the
// list.
//
// 2007-02-22 : Stephane Poss
//-----------------------------------------------------------------------------
using namespace LHCb ;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterToFixOppositeBFractions );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterToFixOppositeBFractions::FilterToFixOppositeBFractions( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
FilterToFixOppositeBFractions::~FilterToFixOppositeBFractions() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterToFixOppositeBFractions::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "This filter Will correct opposite B composition"<< endmsg;
  m_coutevt=m_coutevtAC=0.0;
  m_coutSameB=m_coutBd=m_coutBs=m_coutBu=m_coutOt=m_rejected=0;
  m_coutBdAC=m_coutBsAC=m_coutBuAC=m_coutOtAC=0;
  m_print=false;
  m_printevt=false;

  //initialize table
  m_et[0]=11102021;//Bd_K+K-=DecProdCut
  m_et[1]=11102031;// Bd_pp=DecProdCut
  m_et[2]=11104121;// Bd_KSpi+pi-=DecProdCut
  m_et[3]=11112011;// Bd_emu=DecProdCut
  m_et[4]=11114001;// Bd_Kstmumu=DecProdCut
  m_et[5]=11124001;// Bd_Kstee=DecProdCut
  m_et[6]=11144001;// Bd_JpsiKst,mm=DecProdCut
  m_et[7]=11154001;// Bd_JpsiKst,ee=DecProdCut
  m_et[8]=11164011;// Bd_D0Kst,Kpi=DecProdCut
  m_et[9]=11166101;// Bd_D0Kst,KSpipi=DecProdCut
  m_et[10]=11166111;// Bd_D0Kst,KSKK=DecProdCut
  m_et[11]=11264001;// Bd_D-pi+=DecProdCut
  m_et[12]=11442001;// Bd_JpsiX,mm=JpsiInAcc
  m_et[13]=11874001;// Bd_Dstmunu,Kpi=cocktail,D0muInAcc
  m_et[14]=11912001;// Bd_smumu=DecProdCut
  m_et[15]=12143001;// Bu_JpsiK,mm=DecProdCut
  m_et[16]=12163011;// Bu_D0K,Kpi=DecProdCut
  m_et[17]=12163021;// Bu_D0K,KK=DecProdCut
  m_et[18]=12163031;// Bu_D0K,pipi=DecProdCut
  m_et[19]=12165001;// Bu_D0K,KKpipi=DecProdCut
  m_et[20]=12265001;// Bu_D0K,Kpipipi=DecProdCut
  m_et[21]=12265101;// Bu_D0K,KSKK=DecProdCut
  m_et[22]=12442001;// Bu_JpsiX,mm=JpsiInAcc
  m_et[23]=12463011;// Bu_D0h,Kpi=cocktail,D0InAcc
  m_et[24]=12565001;// Bu_D0tau,Kpi,pipipi=DecProdCut
  m_et[25]=12873001;// Bu_D0munu,Kpi=cocktail,D0muInAcc
  m_et[26]=12912001;// Bu_smumu=DecProdCut
  m_et[27]=13102021;// Bs_pi+pi-=DecProdCut
  m_et[28]=13102031;// Bs_pp=DecProdCut
  m_et[29]=13102201;// Bs_phigamma=DecProdCut
  m_et[30]=13104011;// Bs_phiphi=DecProdCut
  m_et[31]=13112001;// Bs_mumu=DecProdCut
  m_et[32]=13112011;// Bs_emu=DecProdCut
  m_et[33]=13114001;// Bs_phimumu=DecProdCut
  m_et[34]=13144002;// Bs_Jpsiphi,mm=CPV,DecProdCut
  m_et[35]=13146003;// Bs_psi2Sphi,Jpsipipi,mm=CPV,DecProdCut
  m_et[36]=13154002;// Bs_Jpsiphi,ee=CPV,DecProdCut
  m_et[37]=13264001;// Bs_Dspi=DecProdCut
  m_et[38]=13264401;// Bs_Dsrho=DecProdCut
  m_et[39]=13336001;// Bs_etacphi=DecProdCut
  m_et[40]=13396001;// Bs_DsstDsst=DecProdCut
  m_et[41]=13410001;// Bs_DsX=cocktail,DsInAcc
  m_et[42]=13442001;// Bs_JpsiX,mm=JpsiInAcc
  m_et[43]=13774001;// Bs_Dsmunu=cocktail,DsmuInAcc
  m_et[44]=15102001;// Lb_pK=DecProdCut
  m_et[45]=15102011;// Lb_ppi=DecProdCut
  m_et[46]=15102228;// Lb_gammaLambda1670=trpol,DecProdCut
  m_et[47]=15102308;// Lb_gammaLambda=trpol,DecProdCut
  m_et[48]=15104105;// Lb_rhoLambda=omegaMix,DecProdCut
  m_et[49]=15104111;// Lb_phiLambda=DecProdCut
  m_et[50]=15114101;// Lb_Lambdamumu=phsp,DecProdCut
  m_et[51]=15164001;// Lb_Lambdacpi=DecProdCut
  m_et[52]=15264001;// Lb_Dsp=DecProdCut
  m_et[53]=15442001;// Lb_JpsiX,mm=JpsiInAcc
  m_et[54]=23263001;// incl_Ds=DecProdCut
  m_et[55]=24142001;// incl_Jpsi,mm=DecProdCut
  m_et[56]=24152001;// incl_Jpsi,ee=DecProdCut

  m_gx[1] = 0.5683;// Bd_pipi=DecProdCut
  m_gx[6] = 0.4998;// Bd_JpsiKst,mm=DecProdCut
  m_gx[13]= 0.4954;// Bd_Dstmunu,Kpi=cocktail,D0muInAcc
  m_gx[15]= 0.5026;// Bu_JpsiK,mm=DecProdCut
  m_gx[23]= 0.4879;// Bu_D0h,Kpi=cocktail,D0InAcc
  m_gx[25]= 0.4990;// Bu_D0munu,Kpi=cocktail,D0muInAcc
  m_gx[37]= 0.1372;// Bs_Dspi=DecProdCut
  m_gx[43]= 0.1404;// Bs_Dsmunu=cocktail,DsmuInAcc

  m_fx[1] = 0.406;// Bd_pipi=DecProdCut
  m_fx[6] = 0.406;// Bd_JpsiKst,mm=DecProdCut
  m_fx[13]= 0.406;// Bd_Dstmunu,Kpi=cocktail,D0muInAcc
  m_fx[15]= 0.406;// Bu_JpsiK,mm=DecProdCut
  m_fx[23]= 0.406;// Bu_D0h,Kpi=cocktail,D0InAcc
  m_fx[25]= 0.406;// Bu_D0munu,Kpi=cocktail,D0muInAcc
  m_fx[37]= 0.099;// Bs_Dspi=DecProdCut
  m_fx[43]= 0.099;// Bs_Dsmunu=cocktail,DsmuInAcc

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterToFixOppositeBFractions::execute() {

  setFilterPassed( false );

  m_evttype = false;
  m_coutevt = m_coutevt+1.0;

  GenHeader* gene = get<GenHeader> (GenHeaderLocation::Default);
  if ( !gene ) {
    err() << "    Unable to retrieve GenHeader !" << endreq;
    return StatusCode::FAILURE;
  }

  ///Look at Gauss version used to produce evt.
  if(gene->applicationVersion()=="v25r7"){
    if(!m_print){
      info()<<"Gauss version v25r7, correction activated"<<endreq;
      m_print=true;
    }
    /// Look at event type :
    /// proceed only if evt type corresponds to buggy type
    for(int i =0;i!=57;i++){
      if(m_et[i]==gene->evType()){
        m_ievt=i;
        m_evttype=true;
        break;
      }
    }
    if(m_evttype){
      if(!m_printevt){
        info()<<"Event type corresponds to buggy type : Correction activated"
              <<endreq;
        m_printevt=true;
      }
      //retrieve HepMC event
      LHCb::HepMCEvents* hepVect =
        get<LHCb::HepMCEvents>(HepMCEventLocation::Default );
      if(!hepVect){
        err() << "    Unable to retrieve HepMCEvent!" << endreq;
        return StatusCode::FAILURE;
      }
      //Retrieve Rec header to have Event and Run numbers
      RecHeader* evt = get<RecHeader> (RecHeaderLocation::Default);
      if ( !evt ) {
        err() << "Unable to Retrieve Event" << endreq;
        return StatusCode::FAILURE;
      }
      //Retrieve MC parts
      MCParticles* mcpart = get<MCParticles>(MCParticleLocation::Default);
      if ( ! mcpart ) {
        err() << "No MCParticles retrieved" << endreq;
        return StatusCode::FAILURE;
      }
      int evtn = evt->evtNumber();
      int runn = evt->runNumber();
      debug()<<"Event ="<<evtn<<"    Run="<<runn<<endreq;

      //Look for B forced to decay
      const MCParticle *BS = 0, *BO = 0;
      for( std::vector<LHCb::HepMCEvent*>::iterator q=hepVect->begin();
           q!=hepVect->end(); ++q ) {
        for ( HepMC::GenEvent::particle_iterator p  =
                (*q)->pGenEvt()->particles_begin();
              p != (*q)->pGenEvt()->particles_end();
              ++p ) {
          if( (*p)->status() != 889 ) continue;
          BS = associatedofHEP(*p);
          if(BS) break;
        }
      }
      debug()<<"Signal B ID = "<<BS->particleID().pid()<<endreq;
      //retrieve opposite B using the same origin vertex
      MCParticles::const_iterator imc;
      for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
        if( (*imc) != BS ) {
          if((*imc)->particleID().hasBottom()) {
            if((*imc)->originVertex()){
              if((*imc)->originVertex()==BS->originVertex()) {
                BO = (*imc);
              }
            }
            else{
              info()<<"Origin vertex of a B inexistant"<<endreq;
            }
          }
        }
      }
      if(!BO){
        info()<<"Opposite B not found !!"<<endreq;
        setFilterPassed(false);
        return StatusCode::SUCCESS;
      }
      debug()<<"B Opposite ID = "<<BO->particleID().pid()<<endreq;
      if(BO->particleID().abspid()==531) m_coutBs++;
      if(BO->particleID().abspid()==521) m_coutBu++;
      if(BO->particleID().abspid()==511) m_coutBd++;
      if(BO->particleID().abspid()!=531 &&
         BO->particleID().abspid()!=521 &&
         BO->particleID().abspid()!=511) m_coutOt++;
      //remove event acording to a certain prob if BS and BO are identical.
      if(BS->particleID().abspid()==BO->particleID().abspid()){
        m_coutSameB++;
        int a = evtn%50;
        if(a < 50.*(1-((1/m_gx[m_ievt])-1)/((1/m_fx[m_ievt])-1))){
          m_rejected++;
          debug()<<"Event rejected !!"<<endreq;
          setFilterPassed(false);
          return StatusCode::SUCCESS;
        }
      }
      debug()<<"Event accepted !!"<<endreq;
      m_coutevtAC = m_coutevtAC+1.0;
      if(BO->particleID().abspid()==531) m_coutBsAC++;
      if(BO->particleID().abspid()==521) m_coutBuAC++;
      if(BO->particleID().abspid()==511) m_coutBdAC++;
      if(BO->particleID().abspid()!=531 &&
         BO->particleID().abspid()!=521 &&
         BO->particleID().abspid()!=511) m_coutOtAC++;
    }
  }

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterToFixOppositeBFractions::finalize() {

  m_BdBC=percent(m_coutBd,m_coutevt);
  m_BsBC=percent(m_coutBs,m_coutevt);
  m_BuBC=percent(m_coutBu,m_coutevt);
  m_OtBC=percent(m_coutOt,m_coutevt);

  m_BdAC=percent(m_coutBdAC,m_coutevtAC);
  m_BsAC=percent(m_coutBsAC,m_coutevtAC);
  m_BuAC=percent(m_coutBuAC,m_coutevtAC);
  m_OtAC=percent(m_coutOtAC,m_coutevtAC);

  m_errBdBC=errorp(m_coutBd,m_coutevt);
  m_errBsBC=errorp(m_coutBs,m_coutevt);
  m_errBuBC=errorp(m_coutBu,m_coutevt);
  m_errOtBC=errorp(m_coutOt,m_coutevt);

  m_errBdAC=errorp(m_coutBdAC,m_coutevtAC);
  m_errBsAC=errorp(m_coutBsAC,m_coutevtAC);
  m_errBuAC=errorp(m_coutBuAC,m_coutevtAC);
  m_errOtAC=errorp(m_coutOtAC,m_coutevtAC);


  info() << "==> Finalize" << endmsg;

  info() << "   Event Type was :"<<m_et[m_ievt]<<endreq;
  info() << "   Total evts Before any corrections= " <<m_coutevt<<endreq;
  info() << " Fraction where opposite B ID = Signal B ID : "
         <<r(100.*m_coutSameB/m_coutevt)<<" +/- "<<r(m_errBuBC)<<" %"
         <<endreq;
  info() << "        Total Bu oppo = "<<r(m_BuBC)<<" +/- "
         <<r(m_errBuBC)<<" %"<<endreq;
  info() << "        Total Bd oppo = "<<r(m_BdBC)<<" +/- "
         <<r(m_errBdBC)<<" %"<<endreq;
  info() << "        Total Bs oppo = "<<r(m_BsBC)<<" +/- "
         <<r(m_errBsBC)<<" %"<<endreq;
  info() << "        Total Ot oppo = "<<r(m_OtBC)<<" +/- "
         <<r(m_errOtBC)<<" %"<<endreq;
  info() << endreq;
  info() << "        Total rejected = "
         <<r(100.*m_rejected/m_coutevt)<<" +/- "
         <<r(errorp(m_rejected,m_coutevt))<<" %"<<endreq;
  info() << "   Total evts After Correction = "
         <<m_coutevtAC<<endreq;
  info() << "        Total Bu oppo AC= "<<r(m_BuAC)<<" +/- "
         <<r(m_errBuAC)<<" %"<<endreq;
  info() << "        Total Bd oppo AC= "<<r(m_BdAC)<<" +/- "
         <<r(m_errBdAC)<<" %"<<endreq;
  info() << "        Total Bs oppo AC= "<<r(m_BsAC)<<" +/- "
         <<r(m_errBsAC)<<" %"<<endreq;
  info() << "        Total Ot oppo AC= "<<r(m_OtAC)<<" +/- "
         <<r(m_errOtAC)<<" %"<<endreq;


  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
MCParticle* FilterToFixOppositeBFractions::associatedofHEP(HepMC::GenParticle* hepmcp) {

  SmartDataPtr<MCParticles> mcpart (eventSvc(), MCParticleLocation::Default );

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().vect().mag();
  double moththeta = hepmcp->momentum().vect().theta();
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( mid == (*imc)->particleID().pid() ) {
      if( fabs(mothmom - (*imc)->momentum().P())< 0.1){
        if( fabs(moththeta -(*imc)->momentum().Theta())< 0.0001){
          return (*imc);
        }
      }
    }
  }
  return 0;
}
//=============================================================================
float FilterToFixOppositeBFractions::percent(int a, float b)
{
  return 100.*a/b;
}
//=============================================================================
float FilterToFixOppositeBFractions::errorp(int a, float b)
{
  float err = 100.*sqrt((a/b)*(1-(a/b))/b);
  return err;
}
//=============================================================================
float FilterToFixOppositeBFractions::r(float x)
{
  return round(x * 100.)/100.;
}
