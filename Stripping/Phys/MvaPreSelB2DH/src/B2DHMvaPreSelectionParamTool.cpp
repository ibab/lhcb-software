// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>


// local
#include "B2DHMvaPreSelectionParamTool.h"

//#include "TMVAnalysis_Bd2DH_F621Sph_FisherD.class.h"

//-----------------------------------------------------------------------------
// Implementation file for class : B2DHMvaPreSelectionParamTool
//
// 2010-07-16 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( B2DHMvaPreSelectionParamTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
B2DHMvaPreSelectionParamTool::B2DHMvaPreSelectionParamTool( const std::string& type,
                                                            const std::string& name,
                                                            const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_distanceCalculator(0),
    m_dva(0),
    m_B2DHPreselMvaUtilityTool(0),
    m_B2DHFisherDReader(0),
    m_maxNumPhysicsChannelsToSelect(2),
    m_current_B2DH_channel_Number(0),
    m_classifierName(std::string("Fisher method")),
    m_weightFileName(std::string("MyFileName")),
    m_FisherDParamList(std::vector<double> (0)),
    m_FisherDParamListInFloat(std::vector<Float_t> (0)),
    m_FisherDStringVarList(std::vector<std::string> (0)),
    m_numFisherDParam(10),
    m_probValue(0.0),
    m_responseValue(0.0),
    m_ipsBs(0.0),
    m_ipsBachelorPart(0.0),
    m_FsBs(0.0),
    m_cosBs(0.0),
    m_momBs(0.0),
    m_bvtxchisq(0.0),
    m_BsMassDiff(0.0),
    m_ZPvDiff(0.0),
    m_dvtxchisq (0.0),
    m_ptDs(0.0),
    m_FsDs(0.0),
    m_momDs(0.0),
    m_DsMassDiff(0.0),
    m_ptBachelorPart(0.0),
    m_momBachelorPart(0.0),
    m_BachelorKaDll(0.0),
    m_BachelorPrDll(0.0),
    m_BachelorMuDll(0.0),
    m_BdMassDiff(0.0),
    m_DdMassDiff(0.0),
    m_numSecPart(3),
    m_ptSecPart(std::vector<double> (0)),
    m_secMomValue(std::vector<double> (0)),
    m_ipsSecPart(std::vector<double> (0)),
    m_bKaonSecDll(std::vector<double> (0)),
    m_bProtonSecDll(std::vector<double> (0))
{
  declareInterface<IB2DHMvaPreSelectionParamTool>(this);



  //    m_B2DHFisherDReader=new TMVA::Reader();


}
//=============================================================================
// Destructor
//=============================================================================
B2DHMvaPreSelectionParamTool::~B2DHMvaPreSelectionParamTool() {

  if(m_B2DHFisherDReader) delete m_B2DHFisherDReader;

}
//=============================================================================


StatusCode B2DHMvaPreSelectionParamTool::initialize() {
  StatusCode sc = StatusCode::SUCCESS;

  m_dva=Gaudi::Utils::getIDVAlgorithm(contextSvc(),this);
  if (0 == m_dva) {
    return Error(" B2DHMvaPreSelectionParamTool  initialize could not get parent DValgorithm",
                 StatusCode::FAILURE);
  }


  m_distanceCalculator = m_dva->distanceCalculator();
  if(! m_distanceCalculator) {
    Error (" B2DHSelectionParamTool Unable to retrieve DistanceCalculator");
    return StatusCode::FAILURE;
  }


  m_B2DHPreselMvaUtilityTool = tool<IB2DHPreselMvaUtilityTool> ("B2DHPreselMvaUtilityTool");


  // The following initializations done from B2DHMvaSelectionAlg. Hence commented out here.
  // initializeLocalParam();
  // BookMvaForFisherD(m_weightFileName,m_classifierName);

  return sc;


}
//=============================================================================
StatusCode B2DHMvaPreSelectionParamTool::BookMvaForFisherD(std::string WeightFileName, std::string ClassifierMethodName ){
  StatusCode sc = StatusCode::SUCCESS;




  sc = sc && getFisherDStringVar();

  //  for(int i=0; i< m_numFisherDParam; ++i ) {
  //  m_B2DHFisherDReader->AddVariable( m_FisherDStringVarList[i], &m_FisherDParamListInFloat[i]);
  // }

  m_B2DHFisherDReader=new ReadFisherD(m_FisherDStringVarList);


  info()<<" B2DHSelectionParamTool : Now the Selection weights are from StandAlone class for "<<WeightFileName
        << ", for the classifier "<<ClassifierMethodName
        <<  "for Channel number  " <<m_current_B2DH_channel_Number<<  endmsg;


  // m_B2DHFisherDReader->BookMVA(m_classifierName ,WeightFileName );


  info()<<" B2DHSelectionParamTool : Booked Fisher method "<<endmsg;



  return sc;
}



StatusCode B2DHMvaPreSelectionParamTool::getFisherDStringVar() {
  StatusCode sc = StatusCode::SUCCESS;


  m_numFisherDParam=8;

  m_FisherDStringVarList.clear();
  m_FisherDParamList.clear();
  m_FisherDStringVarList.resize(m_numFisherDParam);
  m_FisherDParamList.resize(m_numFisherDParam);
  m_FisherDParamListInFloat.resize(m_numFisherDParam);


  //for now the parameters used for the two channels are same.
  // this can be changed by checking on the m_current_B2DH_channel_Number
  // and setting different set of parameters.   SE Feb-16-2010



  m_FisherDStringVarList[0]= "log(FSBd+100)";
  m_FisherDStringVarList[1]= "log(IpsPiKBachlor/IpsBd)";
  m_FisherDStringVarList[2]= "log(MomPiKBachlor)";
  m_FisherDStringVarList[3]= "log(BMomentum*sin(acos(CosBd)))";
  m_FisherDStringVarList[4]= "log((IpsPiSec_0*IpsPiSec_1*IpsPiSec_2)/IpsBd)";
  m_FisherDStringVarList[5]= "log(PtPiSec_0*PtPiSec_1*PtPiSec_2)";
  m_FisherDStringVarList[6]= "log(dvtxchi2)";
  m_FisherDStringVarList[7]= "log(bvtxchi2)";



  return sc;

}
StatusCode B2DHMvaPreSelectionParamTool::acquireSelectionParams(const LHCb::Particle* b,
                                                                const  LHCb::RecVertex* aPV) {
  StatusCode sc = StatusCode::SUCCESS;


  //info()<< "Now get selection parameters for this event "<<endmsg;

  resetParamInCurrentEvent() ;

  sc = sc && (getSelectionParamsForBsParticle( b, aPV) );
  //info()<< "Now get selection parameters Ds daughter  in this event "<<endmsg;



  sc = sc && (getSelectionParamsForDsDaughterOfBs( b, aPV) );
  //info()<< "Now get selection parameters kaon bachelor  in this event "<<endmsg;
  sc = sc && (getSelectionParamsForBachelorDaughterOfBs (b, aPV));

  //info()<< "Now get selection parameters daughters of Ds  in this event "<<endmsg;
  sc = sc && (getSelectionParamsForSecondaryDaughtersOfBs (b, aPV));

  double aipsRatioBach= (m_ipsBs != 0.0) ? (m_ipsBachelorPart /m_ipsBs): (m_ipsBachelorPart /0.00001);

  //info()<<" IPs of bachelor and Bs "<<m_ipsBachelorPart<<"  "<<m_ipsBs <<endmsg;

  m_FisherDParamList[1]= (aipsRatioBach > 0.0) ? log(aipsRatioBach): -100.0;

  double m_ipsSecProd = m_ipsSecPart[0] * m_ipsSecPart[1] * m_ipsSecPart[2];
  //info()<<" mSec IPs and their prod and  BsIPS  "<<m_ipsSecPart[0]
  //      <<"  "<<m_ipsSecPart[1]<<"  "<<m_ipsSecPart[2]<<"   "<<m_ipsSecProd<< m_ipsBs <<endmsg;

  double aipsRatoSec = (m_ipsBs !=0.0 ) ? (m_ipsSecProd/ m_ipsBs) :( m_ipsSecProd/0.00001);


  m_FisherDParamList[4]= (aipsRatoSec> 0.0) ? log(aipsRatoSec ) : -100.0 ;



  //info()<<" end of acquireSelectionParams  number of fisher param "<< m_numFisherDParam<< endmsg;


  for(int i=0; i< m_numFisherDParam; ++i ) {
    m_FisherDParamListInFloat[i] = static_cast <Float_t> (m_FisherDParamList[i]);
  }

  const std::vector<double> aFisherDParamListInDouble =m_FisherDParamList;

  double afisherMVa = evaluateFisherResponseValWithStandAlone( aFisherDParamListInDouble );

  //    double afisherProb = evaluateFisherDProb();
  //verbose()<<" CurrentfisherProb Value "<<afisherProb<<endmsg;

  verbose()<< " Current Fisher Mva Value is "<<afisherMVa<<endmsg;





  return sc;

}
StatusCode B2DHMvaPreSelectionParamTool::getSelectionParamsForBsParticle(const LHCb::Particle* b,const  LHCb::RecVertex* aPV) {
  StatusCode sc = StatusCode::SUCCESS;



  double fom=0.0;
  double chisq=0.0;
  double aIpsBs=0.0;

  double aFDA=0.0;
  double aFerrA=0.0;
  double achiFsA=0.0;

  double aFsBs =0.0;




  //    const IDistanceCalculator*  distanceCalculator = m_contextTool->distanceTool();


  sc= m_distanceCalculator->distance(b, aPV , fom, chisq);
  if( chisq > 0 ) aIpsBs =sqrt(chisq);
  m_ipsBs = aIpsBs;

  //info()<<" Bs Ips "<<aIpsBs<<endmsg;

  sc = sc && ( m_distanceCalculator-> pathDistance ( b, aPV , aFDA, aFerrA, achiFsA) );
  if( aFerrA !=0.0 )  aFsBs=aFDA/aFerrA;
  m_FsBs= (aFsBs > 0.0) ? aFsBs : ( aFsBs * (-1.0));

  //info()<<" BS FS "<<aFsBs<<endmsg;
  m_FisherDParamList[0]=(aFsBs>-100.0) ? log(aFsBs+100): 0.0;




  const Gaudi::XYZPoint aPrimVtxPt =  Gaudi::XYZPoint(aPV->position().x(),
                                                      aPV->position().y(),aPV->position().z());
  const Gaudi::XYZVector rUnit = (b->endVertex()->position() - aPrimVtxPt).Unit();
  const Gaudi::XYZVector pUnit= b->momentum().Vect().Unit();
  m_cosBs = pUnit.Dot(rUnit);
  m_momBs = b->p();


  double aBa= m_momBs* sin(acos(m_cosBs));
  //info()<<" Bs Mom costheta sintheta "<< m_momBs<<"  "<<m_cosBs<<"  "<<sin(acos(m_cosBs))<<endmsg;

  m_FisherDParamList[3]= (aBa > 0.0)? log(aBa): 100.0;




  m_bvtxchisq = b->endVertex()-> chi2();
  //info()<<"  bvertex chisq "<<m_bvtxchisq <<endmsg;

  m_FisherDParamList[7]=(m_bvtxchisq>0.0) ? log(m_bvtxchisq): 100.0;


  sc=sc && getSelectionCutsParamForBsParticle( b, aPV);



  return sc;

}
StatusCode B2DHMvaPreSelectionParamTool::getSelectionCutsParamForBsParticle(const LHCb::Particle* b,const  LHCb::RecVertex* aPV) {

  StatusCode sc = StatusCode::SUCCESS;


  // now for the standard basic cuts for further preselection.



  m_ZPvDiff=(b->endVertex()->position().z()) - aPV->position().z() ;
  m_BsMassDiff =  (b-> measuredMass())- (m_B2DHPreselMvaUtilityTool->massStdBsValue());
  m_BdMassDiff =  (b-> measuredMass())- (m_B2DHPreselMvaUtilityTool->massStdBsValue());
  //info()<<" Bs measured mass diff "<<m_BsMassDiff<<endmsg;

  // end of standard basic cuts for further preselection.

  return sc;
}

StatusCode B2DHMvaPreSelectionParamTool::getSelectionParamsForDsDaughterOfBs(const LHCb::Particle* b,const  LHCb::RecVertex* aPV){
  StatusCode sc = StatusCode::SUCCESS;



  int aDsCount =0;
  m_dvtxchisq=-100.0;

  const SmartRefVector< LHCb::Particle > & curBsDaughters = b->daughters();
  debug()<<" current PV position "<<aPV->position()<<endmsg;


  // temporary debug print to avoid the compilation warning of unused parameter.


  SmartRefVector<LHCb::Particle>::const_iterator iterPartA = curBsDaughters.begin();
  while( ( iterPartA !=  curBsDaughters.end()) && ( aDsCount < 2)  ) { // loop through Bs daughters
    const LHCb::Particle* aPart = (*iterPartA);
    int aPidVal= aPart -> particleID().pid();
    //info()<< " Pid val for Bs daughter "<<aPidVal<<endmsg;

    // the PidVal gt 10000 with some of D to be taken care of in the future.
    //      if( (aPidVal == (m_B2DHPreselMvaUtilityTool ->pdgDsValue())) ||
    //    (aPidVal == -1* (m_B2DHPreselMvaUtilityTool ->pdgDsValue()))||
    //     (aPidVal ==  (m_B2DHPreselMvaUtilityTool ->pdgDdValue())) ||
    //    (aPidVal == -1* (m_B2DHPreselMvaUtilityTool ->pdgDdValue())) ) { // this is a Ds or Dd

    if(aPidVal < 0 ) aPidVal = aPidVal * ( -1 );



    if( ( aPidVal >  (m_B2DHPreselMvaUtilityTool ->pdg_MinDValue())) &&
        ( aPidVal <  (m_B2DHPreselMvaUtilityTool ->pdg_MaxDValue())) ) { // this is some form of D

      aDsCount++;
      m_dvtxchisq =    aPart->endVertex()-> chi2();
      //info()<<" Dvertex chisq "<< m_dvtxchisq  <<endmsg;

      sc= sc &&   getSelectionCutsParamsForDsDaughterOfBs(aPart,aPV);

    }
    iterPartA++;
  }// end loop over Bs daughters
  if(aDsCount > 1 ) {
    debug () << " multiple Ds daughters for the same Bs in preselected Bs2DsK events " << aDsCount<< endmsg;
  }

  m_FisherDParamList[6]=(m_dvtxchisq>0.0) ? log(m_dvtxchisq): 100.0;


  return sc;

}
StatusCode B2DHMvaPreSelectionParamTool::getSelectionCutsParamsForDsDaughterOfBs(const LHCb::Particle* aDsPart,
                                                                                 const  LHCb::RecVertex* aPV) {

  StatusCode sc = StatusCode::SUCCESS;


  m_ptDs =aDsPart->pt();
  m_momDs = aDsPart ->p();

  //info()<<" Current Ds  Pt Mom mass "<<    m_ptDs  <<"  "<<m_momDs <<"  "<< aDsPart->measuredMass()<< endmsg;

  m_DsMassDiff = ( aDsPart->measuredMass() ) - (m_B2DHPreselMvaUtilityTool-> massStdDsValue()) ;
  m_DdMassDiff = ( aDsPart->measuredMass() ) - (m_B2DHPreselMvaUtilityTool-> massStdDdValue()) ;



  double aFDA=0.0;
  double aFerrA=0.0;
  double achiFsA=0.0;

  double aFsDs =0.0;
  sc = sc && ( m_distanceCalculator-> pathDistance ( aDsPart, aPV , aFDA, aFerrA, achiFsA) );
  if( aFerrA !=0.0 )  aFsDs=aFDA/aFerrA;
  m_FsDs=(aFsDs > 0.0) ? aFsDs : (aFsDs * (-1.0));

  return sc;

}
StatusCode B2DHMvaPreSelectionParamTool::getSelectionParamsForBachelorDaughterOfBs(const LHCb::Particle* b,
                                                                                   const  LHCb::RecVertex* aPV) {


  StatusCode sc = StatusCode::SUCCESS;

  int aBachelorKaonCount =0;
  int aBachelorPionCount =0;

  const SmartRefVector< LHCb::Particle > & curBsDaughters = b->daughters();

  SmartRefVector<LHCb::Particle>::const_iterator iterPartA = curBsDaughters.begin();
  while(( iterPartA !=  curBsDaughters.end()) && (aBachelorKaonCount<2 ) && (aBachelorPionCount<2 ) ) {


    // loop through Bs daughters

    const LHCb::Particle* aPart = (*iterPartA);
    int aPidVal= aPart -> particleID().pid();
    if(aPidVal < 0 ) aPidVal = aPidVal * (-1);

    if(m_current_B2DH_channel_Number == 0) {
      if (  ( aPidVal ==   (m_B2DHPreselMvaUtilityTool-> pdgKaonValue()  ) ) ) { // this is kaon
        aBachelorKaonCount++;


      }

    }else if (  m_current_B2DH_channel_Number == 1) {

      if (   aPidVal ==   (m_B2DHPreselMvaUtilityTool-> pdgPionValue() )) { // this is pion
        aBachelorPionCount++;


      }

    }
    //info()<<" current Kaon and Pion count  "<<aBachelorKaonCount<<"    "<< aBachelorPionCount<< endmsg;

    if( aBachelorKaonCount >0 ||   aBachelorPionCount > 0 ) {
      m_momBachelorPart = aPart->p();
      //info()<<" bachelor Momentum "<< m_momBachelorPart <<endmsg;
      double fom=0.0;
      double chisq=0.0;
      //const IDistanceCalculator*  distanceCalculator = m_contextTool->distanceTool();
      sc= m_distanceCalculator->distance(aPart , aPV , fom, chisq);
      if( chisq > 0 ) m_ipsBachelorPart = sqrt(chisq);

      //info()<<" Bachelor Ips "<<m_ipsBachelorPart<<endmsg;

      sc=sc &&  getSelectionCutsParamsForBachelorDaughterOfBs(aPart);

    }

    iterPartA++;


  }// end loop over Bs daughters
  if( aBachelorKaonCount > 1 || aBachelorPionCount > 1 ) {
    debug () << " multiple bachelor Kaon/Pion daughters for the same Bs in preselected Bs2DsK events "
             << aBachelorKaonCount<< "  "<<aBachelorPionCount<< endmsg;
  }

  m_FisherDParamList[2]=(m_momBachelorPart>0.0) ? log(m_momBachelorPart): 0.0;



  return sc;

}
StatusCode B2DHMvaPreSelectionParamTool::getSelectionCutsParamsForBachelorDaughterOfBs(const LHCb::Particle* aBach) {
  StatusCode sc = StatusCode::SUCCESS;


  m_ptBachelorPart = aBach->pt();
  //info()<<" Pt of bachelor "<<m_ptBachelorPart<<endmsg;

  const LHCb::ProtoParticle*  bProto= aBach->proto();
  //info()<< " Proto part for bachelor "<<bProto<<endmsg;
  if(bProto ) {
    if(bProto->hasInfo(LHCb::ProtoParticle::CombDLLk)) {
      m_BachelorKaDll= bProto->info(LHCb::ProtoParticle::CombDLLk, -1000);
    }

    if(bProto->hasInfo(LHCb::ProtoParticle::CombDLLp)){
      m_BachelorPrDll= bProto->info(LHCb::ProtoParticle::CombDLLp, -1000);
    }
    if(bProto->hasInfo(LHCb::ProtoParticle::CombDLLmu)){
      m_BachelorMuDll= bProto->info(LHCb::ProtoParticle::CombDLLmu, -1000);
    }

  }


  return sc;
}
StatusCode B2DHMvaPreSelectionParamTool::getSelectionParamsForSecondaryDaughtersOfBs(const LHCb::Particle* b,
                                                                                     const  LHCb::RecVertex* aPV) {

  StatusCode sc = StatusCode::SUCCESS;


  int aDsCount=0;

  const SmartRefVector< LHCb::Particle > & curBsDaughters = b->daughters();

  SmartRefVector<LHCb::Particle>::const_iterator iterPartA = curBsDaughters.begin();
  while(iterPartA !=  curBsDaughters.end() &&  (aDsCount<2) ) { // loop through Bs daughters
    const LHCb::Particle* aPart = (*iterPartA);
    int aPidVal = aPart -> particleID().pid();
    if( aPidVal < 0 ) aPidVal =   aPidVal * (-1);


    //      if( ( aPart -> particleID().pid() == ( m_B2DHPreselMvaUtilityTool->pdgDsValue())) ||
    //    ( aPart -> particleID().pid() == (-1* ( m_B2DHPreselMvaUtilityTool->pdgDsValue()))) ) {    // this is Ds

    if( (  aPidVal >  (m_B2DHPreselMvaUtilityTool ->pdg_MinDValue())) &&
        (  aPidVal <  (m_B2DHPreselMvaUtilityTool ->pdg_MaxDValue())) ) { // this is some form of D



      aDsCount++;
      int aSecCount=0;
      const SmartRefVector<LHCb::Particle> DsDaughters = (*iterPartA)->daughters();
      SmartRefVector<LHCb::Particle>::const_iterator iterPartS = DsDaughters.begin();
      while(iterPartS != DsDaughters.end() && (aSecCount<m_numSecPart) ) {
        //            int dnum=iterPartS-DsDaughters.begin();

        const LHCb::Particle* aPiKPart= (*iterPartS);
        m_ptSecPart[aSecCount] = aPiKPart->pt();
        //info()<<" Pt of Sec "<<aSecCount <<"  "<<m_ptSecPart[aSecCount]<<endmsg;
        const LHCb::ProtoParticle*  bProto= aPiKPart->proto();
        if(bProto) {
          if(bProto->hasInfo(LHCb::ProtoParticle::CombDLLk)){
            m_bKaonSecDll[aSecCount]  = bProto->info(LHCb::ProtoParticle::CombDLLk, -1000);
            //info()<<" Kaon dll Sec "<<aSecCount<<"  "<<m_bKaonSecDll[aSecCount]<<endmsg;
          }
        }


        double fom=0.0;
        double chisq=0.0;
        //const IDistanceCalculator*  distanceCalculator = m_contextTool->distanceTool();
        sc= m_distanceCalculator->distance(aPiKPart , aPV , fom, chisq);

        if( chisq > 0 ) m_ipsSecPart[aSecCount]= sqrt(chisq);
        //info()<<" Sec Ips "<<aSecCount <<"   "<< m_ipsSecPart[aSecCount]<<endmsg;


        sc= sc && getSelectionCutsParamsForSecondaryDaughtersOfBs(aPiKPart,aSecCount);

        aSecCount++;
        iterPartS++;
      }// end loop over Ds daughters

    } // end verification for Ds


    iterPartA++;
  } // end loop over Bs daughters

    //info()<<" Sec Pt prod "<<m_ptSecPart[0] * m_ptSecPart[1] * m_ptSecPart[2]<<endmsg;
    // info()<<" Sec kaon dll 012 "<<m_bKaonSecDll[0]<< "  "<<m_bKaonSecDll[1]<<"  "
    //      <<m_bKaonSecDll[2]<<endmsg;

  m_FisherDParamList[5]=log(m_ptSecPart[0] * m_ptSecPart[1] * m_ptSecPart[2]);
  //    m_FisherDParamList[6]=log((m_bKaonSecDll[0]+20)* (m_bKaonSecDll[1]+20));
  // m_FisherDParamList[7]=m_bKaonSecDll[2];


  return sc;
}
StatusCode B2DHMvaPreSelectionParamTool::getSelectionCutsParamsForSecondaryDaughtersOfBs(const LHCb::Particle* aPartSec,
                                                                                         int aSecNum)
{
  StatusCode sc = StatusCode::SUCCESS;



  m_secMomValue[aSecNum] = aPartSec->p();

  const LHCb::ProtoParticle*  bProto= aPartSec->proto();
  if(bProto) {
    if(bProto->hasInfo(LHCb::ProtoParticle::CombDLLp)){
      m_bProtonSecDll[aSecNum]= bProto->info(LHCb::ProtoParticle::CombDLLp, -1000);
    }


  }



  return sc;

}
double B2DHMvaPreSelectionParamTool::evaluateFisherDProb() {


  // m_responseValue = evaluateFisherResponseVal();

  //  m_probValue = m_B2DHFisherDReader->GetProba ( m_classifierName);
  //info()<<" Channel num FisherResponse prob Val "<<m_current_B2DH_channel_Number
  //       <<"   "<<  m_responseValue<<"  "<< m_probValue  << endmsg;


  return  m_probValue  ;

  // return m_probValue;data/2010/SDST/00006563
}
double B2DHMvaPreSelectionParamTool::evaluateFisherResponseVal(){

  //info()<<" evalresponse current channel classifier "<<  m_current_B2DH_channel_Number <<"   "
  //      <<  m_classifierName<<endmsg;

  //  double aresponseVal =  m_B2DHFisherDReader->EvaluateMVA( m_classifierName);
  //info()<< "current response value "<<aresponseVal<<endmsg;
  double aresponseVal=0.0;

  return aresponseVal;

}

double B2DHMvaPreSelectionParamTool::evaluateFisherResponseValWithStandAlone(const std::vector<double>& aParamList )
{
  double m_responseValue  = m_B2DHFisherDReader->GetMvaValue (aParamList );

  return  m_responseValue;

}

void B2DHMvaPreSelectionParamTool::setfisherDProbValue(double aProbVal) {

  m_probValue=aProbVal;


}
void B2DHMvaPreSelectionParamTool::setfisherDResponseValue (double aResponseVal){

  m_responseValue=aResponseVal;



}
void B2DHMvaPreSelectionParamTool::initializeLocalParam(){


  m_numSecPart=3;  // number of secondary particles from Ds.

  m_ptSecPart.resize(m_numSecPart);
  m_secMomValue.resize(m_numSecPart);
  m_ipsSecPart.resize(m_numSecPart);
  m_bKaonSecDll.resize(m_numSecPart);
  m_bProtonSecDll.resize(m_numSecPart);

  m_weightFileName =
    m_B2DHPreselMvaUtilityTool ->fisherWeightsFullFileName(m_current_B2DH_channel_Number);

}
void B2DHMvaPreSelectionParamTool::resetParamInCurrentEvent()
{

  m_ipsBs=10000.0;
  m_ipsBachelorPart=0.0;
  m_FsBs=0.0;
  m_cosBs=0.0;
  m_momBs=0.0;
  m_bvtxchisq=1000.0;
  m_BsMassDiff=-1000.0;
  m_ZPvDiff=-100.0;
  m_dvtxchisq=1000.0;
  m_ptDs=0.0;
  m_FsDs=0.0;
  m_momDs=0.0;
  m_DsMassDiff=-1000.0;
  m_ptBachelorPart=0.0;
  m_ipsBachelorPart=0.0;
  m_momBachelorPart=0.0;
  m_BachelorKaDll=-1000.0;
  m_BachelorPrDll=-1000.0;
  m_BachelorMuDll=-1000.0;


  for(int i=0; i< m_numSecPart; ++i ) {
    m_ptSecPart[i]=0.0;
    m_secMomValue[i]=0.0;
    m_ipsSecPart[i]=0.0;
    m_bKaonSecDll[i]=-1000.0;
    m_bProtonSecDll[i]=-1000.0;
  }
  m_FisherDParamList.clear();
  m_FisherDParamList.resize(m_numFisherDParam);
  std::fill(m_FisherDParamList.begin(), m_FisherDParamList.end(), 100.);
  //   for(int j=0; j<m_numFisherDParam; ++j) {
  //     m_FisherDParamList[j]==100.0;
  //   }
  //  m_FisherDParamList.assign(m_numFisherDParam,-100.0);



}
