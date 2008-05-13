// $Id: FidelTuple.cpp,v 1.1 2008-05-13 14:30:22 sfurcas Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RecHeader.h"
#include "Event/ProcessHeader.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Kernel/Particle2MCLinker.h"
#include "Event/HltSummary.h"
#include "Event/HltEnums.h"
#include "Event/L0DUReport.h"
#include "Kernel/IRelatedPV.h"

#include "gsl/gsl_cdf.h"

using namespace LHCb;
using namespace Gaudi::Units;

// local
#include "FidelTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FidelTuple
//
// 2008-03-26 : Sara Furcas & Rolf Oldeman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FidelTuple );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FidelTuple::FidelTuple( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  ,m_pLinker(0)

{
}
//=============================================================================
// Destructor
//=============================================================================
FidelTuple::~FidelTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FidelTuple::initialize() {

  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_relatedPV = tool<IRelatedPV>("RelatedPV",this );
  if (!m_relatedPV) {
    fatal() << "    Unable to retrieve RelatedPV " ;
    return StatusCode::FAILURE;
  }
  
  m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Chi2,"");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FidelTuple::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  StatusCode sc = StatusCode::SUCCESS ;
  
  RecHeader* evt = get<RecHeader>(LHCb::RecHeaderLocation::Default);
  int Evt = evt->evtNumber();
        
  LHCb::L0DUReport* report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
  long Level0decision = report->decision();
  const LHCb::HltSummary* hlt = get<LHCb::HltSummary>(LHCb::HltSummaryLocation::Default);

  double distB,errdistB,fsB;//signed Fd-Fs B-PV
  double distB1,errdistB1,fsB1;//projected Fd-Fs B res1
  double distB2,errdistB2,fsB2;//projected Fd-Fs B res2  
  double errdist_B,dist_B,fs_B;//projected Fd-Fs B-PV
  
  double impCand,iperr,ips,angleP;
  double chi2,ndof;
  double errdist1=-99;double dist1=-99;  
  double sig1=-99; double errsig1=-99; double fsS1=-99;//signed Fs Res1-PV
  double sig2=-99; double errsig2=-99; double fsS2=-99;
  double fs1,imp1,iperr1,ips1;
  double errdist2=-99;double dist2=-99;double fs2,imp2,iperr2,ips2;

  double chi2_tr1 = -11 ;
  double ProbChi2_tr1 = -11;
  double chi2_tr2 = -11 ;
  double ProbChi2_tr2 = -11;
  double ndof_tr1=-11;  
  double ndof_tr2=-11;
  double chi2perdof_tr1=-11;  
  double chi2perdof_tr2=-11;
  double Fd1XY;double Fd2XY;
  

  std::vector<double> v_mass,v_pt,v_p,v_mass1,v_mass2;
  std::vector<double> v_fs_B,v_angleP,v_ips; 
  std::vector<double> v_fs1,v_ips1; 
  std::vector<double> v_fs2,v_ips2;
  std::vector<double> v_impCand,v_imperr;
  std::vector<double> v_imp1,v_iperr1; 
  std::vector<double> v_imp2,v_iperr2;

  std::vector<double> v_errdist_B,v_dist_B; //proj B-PV
  std::vector<double> v_errdist1,v_dist1; // proj res1-PV
  std::vector<double> v_errdist2,v_dist2;// proj res2-PV
  std::vector<double> v_distB,v_errdistB,v_fsB;//signed Fs Fd B-PV
  std::vector<double> v_distB1,v_errdistB1,v_fsB1;//projected Fd-Fs B res1
  std::vector<double> v_distB2,v_errdistB2,v_fsB2;//projected Fd-Fs B res2
  std::vector<double> v_sig1,v_errsig1,v_fsS1;//signed Fs Res1-PV
  std::vector<double> v_sig2,v_errsig2,v_fsS2;//signed Fs Res2-PV


  std::vector<double> v_chi2,v_ndof,Pchi2;
  std::vector<double> v_chi2_1,v_ndof_1,Pchi2_1;
  std::vector<double> v_chi2_2,v_ndof_2,Pchi2_2;
  std::vector<int> v_MCB,v_MCID1,v_MCID2;
  std::vector<int> v_ID1,v_ID2;
  std::vector<double> v_pt1,v_pt2;
  
  std::vector<double> v_chi2_tr1, v_ProbChi2_tr1, v_chi2_tr2, v_ProbChi2_tr2,v_ndof_tr1,v_ndof_tr2;
  std::vector<double> v_chi2perdof_tr1,v_chi2perdof_tr2;
  std::vector<double> v_Fd1XY,v_Fd2XY,v_FdBPVxy;
  

  const LHCb::MCParticle* mcB;
  const LHCb::MCParticle* mc1;
  const LHCb::MCParticle* mc2;
        
  Tuple MyTupla = nTuple("MyTupla");

  LHCb::Particle::ConstVector parts = desktop()->particles();
  LHCb::Particle::ConstVector  Bcands;
  StatusCode scB = particleFilter() -> filterByPID(parts,Bcands,511,true);


  for ( LHCb::Particle::ConstVector::const_iterator b = Bcands.begin();b != Bcands.end() ; ++b){

    if(Bcands.size()>100)continue;
    
    
    MyTupla->column("evt",Evt);
    MyTupla->column("L0",Level0decision);
    MyTupla->column("Hlt",          hlt->decision());
    MyTupla->column("HltMuon",      hlt->checkDecisionType(LHCb::HltEnums::Muon));  
    MyTupla->column("HltDiMuon",    hlt->checkDecisionType(LHCb::HltEnums::DiMuon));
    MyTupla->column("HltJpsi",      hlt->checkDecisionType(LHCb::HltEnums::JPsi));
    MyTupla->column("HltMuonHadron",hlt->checkDecisionType(LHCb::HltEnums::MuonHadron));
    MyTupla->column("HltHadron",    hlt->checkDecisionType(LHCb::HltEnums::Hadron));
    MyTupla->column("HltDiHadron",  hlt->checkDecisionType(LHCb::HltEnums::DiHadron));
    MyTupla->column("HltElectron",  hlt->checkDecisionType(LHCb::HltEnums::Electron));
    MyTupla->column("HltDiElectron",hlt->checkDecisionType(LHCb::HltEnums::DiElectron));
    MyTupla->column("HltGamma",     hlt->checkDecisionType(LHCb::HltEnums::Gamma));
    //MyTupla->Column("HltFidel",   hlt->checkDecisionType(LHCb::HltEnums::Fidel));
    
    if((*b)->endVertex()!=NULL)v_chi2.push_back( (*b)->endVertex()->chi2() );
    if((*b)->endVertex()!=NULL)v_ndof.push_back( (*b)->endVertex()->nDoF() );    
    
    v_mass.push_back( (*b)->momentum().mass() );
    v_pt.push_back( (*b)->pt() );
    v_p.push_back( (*b)-> p() );
    
    
    if((*b)->endVertex()!=NULL){
      chi2=(*b)->endVertex()->chi2();    
      ndof=(*b)->endVertex()->nDoF(); 
    }
    else{
      chi2 = -999;
      ndof = -999;      
    }
    
    double pchi2 =  gsl_cdf_chisq_Q(chi2,ndof);
    Pchi2.push_back(pchi2);

    const LHCb::VertexBase* bestPV  = m_relatedPV->bestPV(*b);    
    
    if(bestPV!=NULL){
      Gaudi::XYZPoint Origin = bestPV->position();      
      StatusCode sc = geomDispCalculator()->calcImpactPar(*(*b),*bestPV,impCand,iperr);      
      if(sc){
        v_impCand.push_back(impCand);
        v_imperr.push_back(iperr);        
        ips=impCand/iperr;
        v_ips.push_back(ips);        
      }
      else{
        v_impCand.push_back(-99);
        v_imperr.push_back(-99);
        v_ips.push_back(-99);
      }

      StatusCode sc_B = geomDispCalculator()->calcProjectedFlightDistance(*bestPV,*(*b),dist_B,errdist_B);
      if(sc_B){
        fs_B = dist_B / errdist_B;
        v_dist_B.push_back(dist_B);
        v_errdist_B.push_back(errdist_B);
        v_fs_B.push_back(fs_B);        
      }
      else{
        v_dist_B.push_back(-99);
        v_errdist_B.push_back(-99);
        v_fs_B.push_back(-99);        
      }
      
      StatusCode scB = geomDispCalculator()->calcSignedFlightDistance(*bestPV,*(*b),distB,errdistB);
      if(scB){
        fsB = distB / errdistB;
        v_distB.push_back(distB);
        v_errdistB.push_back(errdistB);
        v_fsB.push_back(fsB);        
      }
      else{
        v_distB.push_back(-99);
        v_errdistB.push_back(-99);
        v_fsB.push_back(-99);        
      }

      Gaudi::XYZPoint x = (*b)->endVertex()->position();      
      Gaudi::XYZVector myDist = x - Origin;
      Gaudi::XYZVector p((*b)->momentum().Vect());

      double cosangle = p.Dot(myDist)/sqrt(p.mag2())/sqrt(myDist.mag2());
      if(cosangle>0){
        angleP = acos(cosangle);//angle in rad
        v_angleP.push_back(angleP);
      }      
      else{  
        v_angleP.push_back(-9);        
      }  
        
    } //bestPV
    

    //*******************//
    // MC + Daughters    //
    //*******************//
    mcB = m_pLinker->firstMCP( *b );    
    if(mcB){v_MCB.push_back(mcB-> particleID().pid());}    
    if(mcB==0){v_MCB.push_back(-999);}
    
    LHCb::Particle::ConstVector daughters=(*b)->daughtersVector();
    
    for ( LHCb::Particle::ConstVector::const_iterator id1 = daughters.begin();id1 != daughters.end() ; ++id1 ){
      for ( LHCb::Particle::ConstVector::const_iterator id2 = id1+1; id2 != daughters.end() ; ++id2 ){

        v_mass1.push_back( (*id1)->momentum().mass() );
        v_mass2.push_back( (*id2)->momentum().mass() );

        if((*id1)->proto()){
          chi2_tr1 = (*id1)->proto()->track()->chi2();
          ndof_tr1 = (*id1)->proto()->track()->nDoF();
          ProbChi2_tr1 = (*id1)->proto()->track()->probChi2();
          chi2perdof_tr1 = (*id1)->proto()->track()->chi2PerDoF();
        }
        else{
          chi2_tr1 = -11;
          ndof_tr1 = -11;
          ProbChi2_tr1 = -11;
          chi2perdof_tr1 = -11;
        }
        if((*id2)->proto()){
          chi2_tr2 = (*id2)->proto()->track()->chi2();          
          ndof_tr2 = (*id2)->proto()->track()->nDoF();
          ProbChi2_tr2 = (*id2)->proto()->track()->probChi2();
          chi2perdof_tr2 = (*id2)->proto()->track()->chi2PerDoF();
        }
        else{
          chi2_tr2 = -11;
          ProbChi2_tr2 =-11;
          ndof_tr2 = -11; 
          chi2perdof_tr2 = -11;
        }
        v_chi2_tr1.push_back(chi2_tr1);
        v_chi2_tr2.push_back(chi2_tr2);
        v_ProbChi2_tr1.push_back(ProbChi2_tr1);
        v_ProbChi2_tr2.push_back(ProbChi2_tr2);
        v_ndof_tr1.push_back(ndof_tr1);
        v_ndof_tr2.push_back(ndof_tr2);
        v_chi2perdof_tr1.push_back(chi2perdof_tr1);
        v_chi2perdof_tr2.push_back(chi2perdof_tr2);

        if((*id1)->endVertex()!=NULL){
          v_chi2_1.push_back( (*id1)->endVertex()->chi2() );
          v_ndof_1.push_back( (*id1)->endVertex()->nDoF() );    
        }  
        else{ 
          v_chi2_1.push_back(-99);
          v_ndof_1.push_back(-99);          
        }
        if((*id2)->endVertex()!=NULL){
          v_chi2_2.push_back( (*id2)->endVertex()->chi2() );
          v_ndof_2.push_back( (*id2)->endVertex()->nDoF() );   
        }
        else{
         v_chi2_2.push_back(-99);
         v_ndof_2.push_back(-99);
        }

        v_ID1.push_back((*id1)->particleID().pid());
        v_ID2.push_back((*id2)->particleID().pid());
        v_pt1.push_back((*id1)->pt());
        v_pt2.push_back((*id2)->pt());

        mc1 = m_pLinker->firstMCP( *id1 );
        mc2 = m_pLinker->firstMCP( *id2 );

        if(mc1 && mc2){          
          v_MCID1.push_back(mc1 -> particleID().pid());
          v_MCID2.push_back(mc2 -> particleID().pid());
        } 
        if(mc1 && mc2==0) {
          v_MCID1.push_back(mc1 -> particleID().pid());
          v_MCID2.push_back(-999);
        }   
         if(mc2 && mc1==0) {
           v_MCID1.push_back(-999);
           v_MCID2.push_back(mc2 -> particleID().pid());
         } 
         if(mc1==0 && mc2==0) {
           v_MCID1.push_back(-999);
           v_MCID2.push_back(-999);
         }

         if(bestPV!=NULL){ 
           StatusCode sc1 = geomDispCalculator()->calcImpactPar(*(*id1),*bestPV,imp1,iperr1);      
           if(sc1){
             v_imp1.push_back(imp1);
             v_iperr1.push_back(iperr1);        
             ips1=imp1/iperr1;
             v_ips1.push_back(ips1);             
           }
           else{ 
             v_imp1.push_back(-999);
             v_iperr1.push_back(-999);
             v_ips1.push_back(-999); 
           }
           StatusCode sc2 = geomDispCalculator()->calcImpactPar(*(*id2),*bestPV,imp2,iperr2);      
           if(sc2){
             v_imp2.push_back(imp2);
             v_iperr2.push_back(iperr2);        
             ips2=imp2/iperr2;
             v_ips2.push_back(ips2);        
           }
           else{ 
             v_imp2.push_back(-999);
             v_iperr2.push_back(-999);
             v_ips2.push_back(-999); 
           }
           
           // Projected Res1-2 PV
           StatusCode sc11 = geomDispCalculator()->calcProjectedFlightDistance(*bestPV,*(*id1),dist1,errdist1);
           StatusCode sc22 = geomDispCalculator()->calcProjectedFlightDistance(*bestPV,*(*id2),dist2,errdist2);
           // Signed Res1-2 PV
           StatusCode sca = geomDispCalculator()->calcSignedFlightDistance(*bestPV,*(*id1),sig1,errsig1);
           StatusCode scb = geomDispCalculator()->calcSignedFlightDistance(*bestPV,*(*id2),sig2,errsig2);

           if(sc11){
             fs1 = dist1 / errdist1;
             v_dist1.push_back(dist1);
             v_errdist1.push_back(errdist1);
             v_fs1.push_back(fs1);
           }
           else{
             v_dist1.push_back(-999.0);
             v_errdist1.push_back(-999.0);
             v_fs1.push_back(-999.0);             
           }

           if(sca){
             fsS1 = sig1 / errsig1;
             v_sig1.push_back(sig1);
             v_errsig1.push_back(errsig1);
             v_fsS1.push_back(fsS1);
           }
           else{
             v_sig1.push_back(-999.0);
             v_errsig1.push_back(-999.0);
             v_fsS1.push_back(-999.0);             
           }

           if(sc22){
             fs2 = dist2 / errdist2;
             v_dist2.push_back(dist2);
             v_errdist2.push_back(errdist2);
             v_fs2.push_back(fs2);
           }
           else{
             v_dist2.push_back(-999.0);
             v_errdist2.push_back(-999.0);
             v_fs2.push_back(-999.0);             
           }
           
           if(scb){
             fsS2 = sig2 / errsig2;
             v_sig2.push_back(sig2);
             v_errsig2.push_back(errsig2);
             v_fsS2.push_back(fsS2);
           }
           else{
             v_sig2.push_back(-999.0);
             v_errsig2.push_back(-999.0);
             v_fsS2.push_back(-999.0);             
           }
           
           Gaudi::XYZPoint BPos = (*b)->endVertex()->position(); 
           double Bx =  BPos.X(); double By = BPos.Y();

           Gaudi::XYZPoint posPV = bestPV->position();
           double xPV =  posPV.X(); double yPV = posPV.Y(); 

           double B_PVx = Bx-xPV;double B_PVy = By-yPV;
           double FdBPVxy = sqrt((B_PVx*B_PVx)+(B_PVy*B_PVy));
           v_FdBPVxy.push_back(FdBPVxy);
           

           if((*id1)->endVertex()!=NULL){
             Gaudi::XYZPoint pos1 = (*id1)->referencePoint();
             double x1 = pos1.X();    double y1 = pos1.Y();
             double diff1x = x1 - Bx; double diff1y = y1 -By;
             Fd1XY = sqrt((diff1x*diff1x)+(diff1y*diff1y));             
           }
           else{
             Fd1XY = -999;
           }
           

           if((*id2)->endVertex()!=NULL){
             Gaudi::XYZPoint pos2 = (*id2)->referencePoint();
             double x2 = pos2.X();double y2 = pos2.Y(); 
             double diff2x = x2 -Bx; double diff2y = y2 -By;
             Fd2XY = sqrt((diff2x*diff2x)+(diff2y*diff2y));
           } 
           else{
             Fd2XY = -999;
           }
           
           v_Fd1XY.push_back(Fd1XY); 
           v_Fd2XY.push_back(Fd2XY);         

         }//bestPV

         //Projected Fd-Fs B Res 1-2
         const LHCb::VertexBase *BVtx = (*b) ->endVertex();
         StatusCode scB1 = geomDispCalculator()->calcProjectedFlightDistance(*BVtx,*(*id1),distB1,errdistB1);
         StatusCode scB2 = geomDispCalculator()->calcProjectedFlightDistance(*BVtx,*(*id2),distB2,errdistB2);

         if(scB1){
           fsB1 = distB1 / errdistB1;
           v_distB1.push_back(distB1);
           v_errdistB1.push_back(errdistB1);
           v_fsB1.push_back(fsB1);        
         }
         else{
           v_distB1.push_back(-999.0);
           v_errdistB1.push_back(-999.0);
           v_fsB1.push_back(-999.0);             
         }

         if(scB2){
           fsB2 = distB2 / errdistB2;
           v_distB2.push_back(distB2);
           v_errdistB2.push_back(errdistB2);
           v_fsB2.push_back(fsB2);        
         }
         else{
           v_distB2.push_back(-999.0);
           v_errdistB2.push_back(-999.0);
           v_fsB2.push_back(-999.0);             
         }

      } //id2 
    }//id1

  }

  MyTupla->column("NumberBcand",Bcands.size());
  
  MyTupla->farray("Bmass",v_mass,"NB",200);
  MyTupla->farray("pt_b",v_pt,"NB",200);
  MyTupla->farray("p_b",v_p,"NB",200);
  MyTupla->farray("BVtxChi2",v_chi2,"NB",200);
  MyTupla->farray("BVtxNdof",v_ndof,"NB",200);
  MyTupla->farray("Pchi2",Pchi2,"NB",200);
  MyTupla->farray("Ip_b",v_impCand,"NB",200);
  MyTupla->farray("Ipe_b",v_imperr,"NB",200);
  MyTupla->farray("Ips_b",v_ips,"NB",200);
  MyTupla->farray("Fd_b",v_dist_B,"NB",200);//fd proj
  MyTupla->farray("FdErr_b",v_errdist_B,"NB",200);
  MyTupla->farray("Fs_b",v_fs_B,"NB",200);
  MyTupla->farray("FdBPVxy",v_FdBPVxy,"NB",200);
  
  MyTupla->farray("Fdb",v_distB,"NB",200);//fd sign
  MyTupla->farray("FdErr",v_errdistB,"NB",200);
  MyTupla->farray("Fs",v_fsB,"NB",200);
  
  MyTupla->farray("pointing",v_angleP,"NB",200);
  MyTupla->farray("MCB",v_MCB,"NB",200);
  
  MyTupla->farray("ID1",v_ID1,"NB",200);
  MyTupla->farray("ID2",v_ID2,"NB",200);
  MyTupla->farray("mass1",v_mass1,"NB",200);
  MyTupla->farray("mass2",v_mass2,"NB",200);    
  MyTupla->farray("pt1",v_pt1,"NB",200);
  MyTupla->farray("pt2",v_pt2,"NB",200);
  MyTupla->farray("MCID1",v_MCID1,"NB",200);
  MyTupla->farray("MCID2",v_MCID2,"NB",200);    
  MyTupla->farray("Fs1",v_fs1,"NB",200);//proj 1-PV
  MyTupla->farray("Fs2",v_fs2,"NB",200);
  MyTupla->farray("Fd1",v_dist1,"NB",200);
  MyTupla->farray("Fd2",v_dist2,"NB",200);

  MyTupla->farray("ProbChi2_tr1",v_ProbChi2_tr1,"NB",200);
  MyTupla->farray("chi2_tr1",v_chi2_tr1,"NB",200);
  MyTupla->farray("ProbChi2_tr2",v_ProbChi2_tr2,"NB",200);
  MyTupla->farray("chi2_tr2",v_chi2_tr2,"NB",200);
  MyTupla->farray("ndof_tr1",v_ndof_tr1,"NB",200); 
  MyTupla->farray("ndof_tr2",v_ndof_tr2,"NB",200);
  
  MyTupla->farray("FsB1",v_fsB1,"NB",200);//proj 1-B
  MyTupla->farray("FsB2",v_fsB2,"NB",200);
  MyTupla->farray("FdB1",v_distB1,"NB",200);
  MyTupla->farray("FdB2",v_distB2,"NB",200);
  MyTupla->farray("ErrFdB1",v_errdistB1,"NB",200);
  MyTupla->farray("ErrFdB2",v_errdistB2,"NB",200);

  MyTupla->farray("Fd1s",v_sig1,"NB",200);//signed PV-Res
  MyTupla->farray("Fd2s",v_sig2,"NB",200);
  MyTupla->farray("Fs1s",v_fsS1,"NB",200);
  MyTupla->farray("Fs2s",v_fsS2,"NB",200);
  MyTupla->farray("ErrFd1s",v_errsig1,"NB",200);
  MyTupla->farray("ErrFd2s",v_errsig2,"NB",200);

  MyTupla->farray("Fd1XY",v_Fd1XY,"NB",200);// rispetto al vertice della B
  MyTupla->farray("Fd2XY",v_Fd2XY,"NB",200);

  MyTupla->farray("Ips1",v_ips1,"NB",200);
  MyTupla->farray("Ips2",v_ips2,"NB",200);  
  MyTupla->farray("Ip1",v_imp1,"NB",200);
  MyTupla->farray("Ip2",v_imp2,"NB",200);

  MyTupla->farray("Chi2_1",v_chi2_1,"NB",200);
  MyTupla->farray("Ndof_1",v_ndof_1,"NB",200);
  MyTupla->farray("Chi2_2",v_chi2_2,"NB",200);
  MyTupla->farray("Ndof_2",v_ndof_2,"NB",200);
  
  MyTupla->write();    
    


  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FidelTuple::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if(NULL!=m_pLinker) delete m_pLinker;
  
  return DVAlgorithm::finalize(); 
}

//=============================================================================
