// $Id: FilterToFixOppositeBFractions.cpp,v 1.12 2008-07-23 11:38:41 cattanem Exp $
// Include files
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCb
#include "LHCbMath/LHCbMath.h"

// local
#include "FilterToFixOppositeBFractions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterToFixOppositeBFractions
//
//
// 2007-02-22 : Stephane Poss
//-----------------------------------------------------------------------------
using namespace LHCb ;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterToFixOppositeBFractions )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterToFixOppositeBFractions::FilterToFixOppositeBFractions( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_coutSameB(0)
    ,m_coutBd(0)
    ,m_coutBs(0)
    ,m_coutBu(0)
    ,m_coutOt(0)
    ,m_rejected(0)
    ,m_coutBdAC(0)
    ,m_coutBsAC(0)
    ,m_coutBuAC(0)
    ,m_coutOtAC(0)
    ,m_coutevt(0)
    ,m_coutevtAC(0)
    ,m_BsBC(0)
    ,m_BdBC(0)
    ,m_BuBC(0)
    ,m_OtBC(0)
    ,m_BsAC(0)
    ,m_BdAC(0)
    ,m_BuAC(0)
    ,m_OtAC(0)
    ,m_errBsBC(0)
    ,m_errBdBC(0)
    ,m_errBuBC(0)
    ,m_errOtBC(0)
    ,m_errBsAC(0)
    ,m_errBdAC(0)
    ,m_errBuAC(0)
    ,m_errOtAC(0)
    ,m_ievt(0)
    ,m_print(0)
    ,m_printevt(0)
{
  declareProperty("ActivateCorrection",   m_activate=false);
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
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if(!m_activate) return StatusCode::SUCCESS;

  info() << "This filter Will correct opposite B composition"<< endmsg;
  m_coutevt=m_coutevtAC=0.0;
  m_coutSameB=m_coutBd=m_coutBs=m_coutBu=m_coutOt=m_rejected=0;
  m_coutBdAC=m_coutBsAC=m_coutBuAC=m_coutOtAC=0;
  m_print=false;
  m_printevt=false;
  m_ievt = 0;
  

  //initialize table
  //      Event type                               gx    fx 
  m_evtGxFx[11144001] = std::pair<double,double>(0.4998,0.406);// Bd_JpsiKst,mm=DecProdCut
  m_evtGxFx[11874001] = std::pair<double,double>(0.4954,0.406);// Bd_Dstmunu,Kpi=cocktail,D0muInAcc
  m_evtGxFx[12143001] = std::pair<double,double>(0.5026,0.406);// Bu_JpsiK,mm=DecProdCut
  m_evtGxFx[12463011] = std::pair<double,double>(0.4879,0.406);// Bu_D0h,Kpi=cocktail,D0InAcc
  m_evtGxFx[12873001] = std::pair<double,double>(0.4990,0.406);// Bu_D0munu,Kpi=cocktail,D0muInAcc
  m_evtGxFx[13264001] = std::pair<double,double>(0.1372,0.099);// Bs_Dspi=DecProdCut
  m_evtGxFx[13774001] = std::pair<double,double>(0.1404,0.099);// Bs_Dsmunu=cocktail,DsmuInAcc
  m_evtGxFx[11102003] = std::pair<double,double>(0.5654,0.406);// Bd_K+pi-=CPV,DecProdCut
  m_evtGxFx[11102013] = std::pair<double,double>(0.5683,0.406);// Bd_pi+pi-=CPV,DecProdCut
  m_evtGxFx[11144103] = std::pair<double,double>(0.7044,0.406);// Bd_JpsiKS,mm=CPV,DecProdCut
  m_evtGxFx[13112001] = std::pair<double,double>(0.1349,0.099);// Bs_mumu=DecProdCut
  m_evtGxFx[13102201] = std::pair<double,double>(0.1245,0.099);// Bs_phigamma=DecProdCut
  m_evtGxFx[11164011] = std::pair<double,double>(0.4908,0.406);// Bd_D0Kst,Kpi=DecProdCut
  m_evtGxFx[13104011] = std::pair<double,double>(0.1338,0.099);// Bs_phiphi=DecProdCut
  m_evtGxFx[13264401] = std::pair<double,double>(0.1443,0.099);// Bs_Dsrho=DecProdCut
  m_evtGxFx[13336001] = std::pair<double,double>(0.1423,0.099);// Bs_etacphi=DecProdCut
  m_evtGxFx[11463000] = std::pair<double,double>(0.4527,0.406);// Bd_Dst+X,2body
  m_evtGxFx[12463000] = std::pair<double,double>(0.4469,0.406);// Bu_Dst+X,2body
  m_evtGxFx[12163011] = std::pair<double,double>(0.5034,0.406);// Bu_D0K,Kpi=DecProdCut
  m_evtGxFx[11166101] = std::pair<double,double>(0.5025,0.406);// Bd_D0Kst,KSpipi=DecProdCut
  m_evtGxFx[11166111] = std::pair<double,double>(0.4940,0.406);// Bd_D0Kst,KSKK=DecProdCut
  m_evtGxFx[12265001] = std::pair<double,double>(0.5053,0.406);// Bu_D0K,Kpipipi=DecProdCut
  m_evtGxFx[12265101] = std::pair<double,double>(0.4950,0.406);// Bu_D0K,KSKK=DecProdCut
  m_evtGxFx[11104121] = std::pair<double,double>(0.4886,0.406);// Bd_KSpi+pi-=DecProdCut
  m_evtGxFx[12565001] = std::pair<double,double>(0.5070,0.406);// Bu_D0tau,Kpi,pipipi=DecProdCut
  m_evtGxFx[15102228] = std::pair<double,double>(0.1103,0.089);// Lb_gammaLambda1670=trpol,DecProdCut
  m_evtGxFx[15104111] = std::pair<double,double>(0.1111,0.089);// Lb_phiLambda=DecProdCut
  m_evtGxFx[15104105] = std::pair<double,double>(0.1130,0.089);// Lb_rhoLambda=omegaMix,DecProdCut
  m_evtGxFx[12165001] = std::pair<double,double>(0.4979,0.406);// Bu_D0K,KKpipi=DecProdCut
  m_evtGxFx[11124001] = std::pair<double,double>(0.4997,0.406);// Bd_Kstee=DecProdCut
  m_evtGxFx[13396001] = std::pair<double,double>(0.1427,0.099);// Bs_DsstDsst=DecProdCut
  m_evtGxFx[11264001] = std::pair<double,double>(0.5004,0.406);// Bd_D-pi+=DecProdCut
  m_evtGxFx[11114001] = std::pair<double,double>(0.5019,0.406);// Bd_Kstmumu=DecProdCut
  m_evtGxFx[11912001] = std::pair<double,double>(0.5053,0.406);// Bd_smumu=DecProdCut
  m_evtGxFx[12912001] = std::pair<double,double>(0.5088,0.406);// Bu_smumu=DecProdCut
  m_evtGxFx[15164001] = std::pair<double,double>(0.1207,0.089);// Lb_Lambdacpi=DecProdCut
  m_evtGxFx[15264001] = std::pair<double,double>(0.1217,0.089);// Lb_Dsp=DecProdCut
  m_evtGxFx[11102021] = std::pair<double,double>(0.4835,0.406);// Bd_K+K-=DecProdCut
  m_evtGxFx[11102031] = std::pair<double,double>(0.4900,0.406);// Bd_pp=DecProdCut
  m_evtGxFx[13102013] = std::pair<double,double>(0.1561,0.099);// Bs_pi+K-=CPV,DecProdCut
  m_evtGxFx[13102021] = std::pair<double,double>(0.1338,0.099);// Bs_pi+pi-=DecProdCut
  m_evtGxFx[13102031] = std::pair<double,double>(0.1334,0.099);// Bs_pp=DecProdCut
  m_evtGxFx[15102001] = std::pair<double,double>(0.1175,0.089);// Lb_pK=DecProdCut
  m_evtGxFx[15102011] = std::pair<double,double>(0.1175,0.089);// Lb_ppi=DecProdCut
  m_evtGxFx[13442001] = std::pair<double,double>(0.1333,0.099);// Bs_JpsiX,mm=JpsiInAcc
  m_evtGxFx[12442001] = std::pair<double,double>(0.4935,0.406);// Bu_JpsiX,mm=JpsiInAcc
  m_evtGxFx[11442001] = std::pair<double,double>(0.4885,0.406);// Bd_JpsiX,mm=JpsiInAcc
  m_evtGxFx[15442001] = std::pair<double,double>(0.1162,0.089);// Lb_JpsiX,mm=JpsiInAcc
  m_evtGxFx[13410001] = std::pair<double,double>(0.1337,0.099);// Bs_DsX=cocktail,DsInAcc
  m_evtGxFx[12163021] = std::pair<double,double>(0.4974,0.406);// Bu_D0K,KK=DecProdCut  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterToFixOppositeBFractions::execute() {

  setFilterPassed( false );

  //execute code only if filter is activated
  if(!m_activate){
    setFilterPassed( true );
    return StatusCode::SUCCESS;
  }
  
  m_coutevt = m_coutevt+1.0;

  GenHeader* gene = get<GenHeader> (GenHeaderLocation::Default);

  ///Look at Gauss version used to produce evt.
  if(gene->applicationVersion()=="v25r7"){
    Warning("Gauss version v25r7, correction activated",
            StatusCode::SUCCESS,1);
    /// Look at event type :
    /// proceed only if evt type corresponds to buggy type
    m_ievt=gene->evType();
    std::map<int, std::pair<double,double> >::iterator iET = m_evtGxFx.find(m_ievt);
    
    if(iET != m_evtGxFx.end()){
      std::ostringstream t;
      t<<m_ievt;      
      Warning("Event type " + t.str() + 
              " corresponds to buggy type : Correction activated !"
              ,StatusCode::SUCCESS,1);

      //retrieve HepMC event
      LHCb::HepMCEvents* hepVect = get<LHCb::HepMCEvents>(HepMCEventLocation::Default );

      //Retrieve Rec header to have Event and Run numbers
      RecHeader* evt = get<RecHeader> (RecHeaderLocation::Default);

      //Retrieve MC parts
      MCParticles* mcpart = get<MCParticles>(MCParticleLocation::Default);

      int evtn = evt->evtNumber();
      int runn = evt->runNumber();
      debug()<<"Event ="<<evtn<<"    Run="<<runn<<endmsg;

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
      debug()<<"Signal B ID = "<<BS->particleID().pid()<<endmsg;
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
              info()<<"Origin vertex of a B inexistant"<<endmsg;
            }
          }
        }
      }
      if(!BO){
        info()<<"Opposite B not found !!"<<endmsg;
        setFilterPassed(false);
        return StatusCode::SUCCESS;
      }
      debug()<<"B Opposite ID = "<<BO->particleID().pid()<<endmsg;
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
        double gx = iET->second.first;
        double fx = iET->second.second;
        if(a < 50.*(1-((1/gx)-1)/((1/fx)-1))){
          m_rejected++;
          debug()<<"Event rejected !!"<<endmsg;
          setFilterPassed(false);
          return StatusCode::SUCCESS;
        }
      }
      debug()<<"Event accepted !!"<<endmsg;
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

  if(!m_activate || 0 == m_ievt) return GaudiAlgorithm::finalize();

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

  info() << "   Event Type was :"<<m_ievt<<endmsg;
  info() << "   Total evts Before any corrections= " <<m_coutevt<<endmsg;
  info() << " Fraction where opposite B ID = Signal B ID : "
         <<r(percent(m_coutSameB,m_coutevt))<<" +/- "
         <<r(errorp(m_coutSameB,m_coutevt))<<" %"
         <<endmsg;
  info() << "        Total Bu oppo = "
         <<m_coutBu<<"/"<<m_coutevt<<" = "
         <<r(m_BuBC)<<" +/- "
         <<r(m_errBuBC)<<" %"<<endmsg;
  info() << "        Total Bd oppo = "
         <<m_coutBd<<"/"<<m_coutevt<<" = "
         <<r(m_BdBC)<<" +/- "
         <<r(m_errBdBC)<<" %"<<endmsg;
  info() << "        Total Bs oppo = "
         <<m_coutBs<<"/"<<m_coutevt<<" = "
         <<r(m_BsBC)<<" +/- "
         <<r(m_errBsBC)<<" %"<<endmsg;
  info() << "        Total Ot oppo = "
         <<m_coutOt<<"/"<<m_coutevt<<" = "
         <<r(m_OtBC)<<" +/- "
         <<r(m_errOtBC)<<" %"<<endmsg;
  info() << endmsg;
  info() << "        Total rejected = "
         <<r(percent(m_rejected,m_coutevt))<<" +/- "
         <<r(errorp(m_rejected,m_coutevt))<<" %"<<endmsg;
  info() << "   Total evts After Correction = "
         <<m_coutevtAC<<endmsg;
  info() << "        Total Bu oppo AC= "
         <<m_coutBuAC<<"/"<<m_coutevtAC<<" = "
         <<r(m_BuAC)<<" +/- "
         <<r(m_errBuAC)<<" %"<<endmsg;
  info() << "        Total Bd oppo AC= "
         <<m_coutBdAC<<"/"<<m_coutevtAC<<" = "
         <<r(m_BdAC)<<" +/- "
         <<r(m_errBdAC)<<" %"<<endmsg;
  info() << "        Total Bs oppo AC= "
         <<m_coutBsAC<<"/"<<m_coutevtAC<<" = "
         <<r(m_BsAC)<<" +/- "
         <<r(m_errBsAC)<<" %"<<endmsg;
  info() << "        Total Ot oppo AC= "
         <<m_coutOtAC<<"/"<<m_coutevtAC<<" = "
         <<r(m_OtAC)<<" +/- "
         <<r(m_errOtAC)<<" %"<<endmsg;


  return GaudiAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
MCParticle* FilterToFixOppositeBFractions::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles>(MCParticleLocation::Default);
  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().rho();
  double moththeta = hepmcp->momentum().theta();
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
double FilterToFixOppositeBFractions::percent(int a, double b)
{
  return 100.*a/b;
}
//=============================================================================
double FilterToFixOppositeBFractions::errorp(int a, double b)
{
  double err = 100.*sqrt((a/b)*(1-(a/b))/b);
  return err;
}
//=============================================================================
double FilterToFixOppositeBFractions::r(double x)
{
  // return LHCbMath::round(x * 100.)/100.;
  return LHCb::Math::round(x * 100.)/100.;
}
