// Include files

#include "DetDesc/Condition.h"

// local
#include "RichTemplateDataTool.h"
#include "RichLocalDefaultTemplateData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTemplateDataTool
//
// 2007-06-06 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichTemplateDataTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  RichTemplateDataTool::RichTemplateDataTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
    :RichRingRecToolBase ( type, name , parent ),
     m_TemplateInp(VVC(0)),
     m_RadTemplateInp(VVD(0)),
     m_ThTemplateInp(VVD(0)),
     m_NumHitsInTemplate(VI(0)),
     m_NumTemplate(0),
     m_TemplateCenterInp(VVC(0)),
     m_CHitTemplate(VVC(0)),
     m_RHitTemplate(VVD(0)),
     m_ThHitTemplate(VVD(0)),
     m_CondDbTemplatePathNames(VS(0)),
     m_CondDbTemplateParamNames(VVS(0)),
     m_ActivateTemplateInputFromLocalDataSet(true)
{
  declareInterface<IRichTemplateDataTool>(this);


  m_CondDbTemplatePathNames.clear();
  m_CondDbTemplatePathNames.reserve(3);
  //m_CondDbTemplatePathNames.reserve( RConst()->maxNumRadiator());

  if(! m_ActivateTemplateInputFromLocalDataSet ) {

    m_CondDbTemplatePathNames.push_back("/dd/Conditions/Calibration/Rich1/AerogelTemplateRingCoordinates" );
    m_CondDbTemplatePathNames.push_back("/dd/Conditions/Calibration/Rich1/Rich1GasTemplateRingCoordinates" );
    m_CondDbTemplatePathNames.push_back("/dd/Conditions/Calibration/Rich2/Rich2GasTemplateRingCoordinates");
  }

  m_CondDbTemplateParamNames.clear(); m_CondDbTemplateParamNames.reserve(3);
  VS aAgelParamNames; VS aR1gNames; VS aR2gNames;
  aAgelParamNames.push_back("AerogelHitXY");
  aAgelParamNames.push_back("AerogelTrackProjectionXY");
  aR1gNames.push_back("Rich1GasHitXY");
  aR1gNames.push_back("Rich1GasTrackProjectionXY");
  aR2gNames.push_back("Rich2GasHitXY");
  aR2gNames.push_back("Rich2GasTrackProjectionXY");
  m_CondDbTemplateParamNames.push_back(aAgelParamNames);
  m_CondDbTemplateParamNames.push_back(aR1gNames);
  m_CondDbTemplateParamNames.push_back(aR2gNames);




}
//=============================================================================
// Destructor
//=============================================================================
RichTemplateDataTool::~RichTemplateDataTool() {}

//=============================================================================
void RichTemplateDataTool::clearTemplateInputs() {

  int aNumTemplate=RConst()->maxNumRadiator();

  m_TemplateInp.clear(); m_TemplateInp.resize(aNumTemplate);
  m_RadTemplateInp.clear();m_RadTemplateInp.resize(aNumTemplate);
  m_ThTemplateInp.clear(); m_ThTemplateInp.resize(aNumTemplate);
  m_NumHitsInTemplate.clear(); m_NumHitsInTemplate.resize(aNumTemplate);
  m_TemplateCenterInp.clear();  m_TemplateCenterInp.resize(aNumTemplate);
  m_NumAgelTemplate=0;
  m_NumR1gTemplate=0;
  m_NumR2gTemplate=0;
  m_NumTemplate=0;


}
void RichTemplateDataTool::clearTemplateConfig() {

  int aNumTemplate=RConst()->maxNumRadiator();
  m_CHitTemplate.clear(); m_CHitTemplate.reserve(aNumTemplate);
  m_RHitTemplate.clear(); m_RHitTemplate.reserve(aNumTemplate);
  m_ThHitTemplate.clear(); m_ThHitTemplate.reserve(aNumTemplate);

}

StatusCode  RichTemplateDataTool::init(){
  StatusCode sc = StatusCode::SUCCESS;
  clearTemplateInputs();

  // for the each Radiator template
  if(!m_ActivateTemplateInputFromLocalDataSet) {

    for(int i=0; i< (RConst()->maxNumRadiator()) ; ++i ) {

      std::string aPath = m_CondDbTemplatePathNames[i];
      sc= RegisterCurrentCondition(i,aPath);

    }





    sc = updMgrSvc()->update(this);

    if (sc.isFailure()) return Error ( "Failed Rich template UMS update", sc );

  }else {

    for(int irr=0; irr< (RConst()->maxNumRadiator()) ; ++irr ) {

      sc = AcquireTemplateFromLocalDefault(irr);

    }


  }




  // test print
  //   PrintTemplate();
  // end of test print
  return sc;
}


StatusCode RichTemplateDataTool::RegisterCurrentCondition(int i, std::string aPath){
  StatusCode sc = StatusCode::SUCCESS;
  debug() <<" Now registering condition "<<i<<"  "<<aPath<<endmsg;

  if( i==0 ) {
    updMgrSvc()->registerCondition(this,aPath,
                                   &RichTemplateDataTool::umsAerogelTemplateAcquire);
  }else if (i== 1 ) {
    updMgrSvc()->registerCondition(this,aPath,
                                   &RichTemplateDataTool::umsRich1GasTemplateAcquire);
  } else if (i== 2 ) {
    updMgrSvc()->registerCondition(this,aPath,
                                   &RichTemplateDataTool::umsRich2GasTemplateAcquire);
  }
  return sc;

}

StatusCode  RichTemplateDataTool::umsAerogelTemplateAcquire(){

  StatusCode sc= AcquireTemplate(0);
  return sc;

}
StatusCode  RichTemplateDataTool::umsRich1GasTemplateAcquire(){

  StatusCode sc= AcquireTemplate(1);
  return sc;

}

StatusCode  RichTemplateDataTool::umsRich2GasTemplateAcquire(){

  StatusCode sc= AcquireTemplate(2);
  return sc;

}

StatusCode RichTemplateDataTool::AcquireTemplate(int irad)
{
  StatusCode sc = StatusCode::SUCCESS;
  const Condition * dataCond = getDet<Condition>(m_CondDbTemplatePathNames[irad]);
  const VD dataHitXY = dataCond->paramVect<double>(m_CondDbTemplateParamNames [irad] [0]);
  VD::const_iterator iv = dataHitXY.begin();
  double zh=0.0;
  VC aHitV;

  while(iv != dataHitXY.end() ) {
    double xh = (*iv);
    iv++;
    double yh = (*iv);
    aHitV.push_back(Gaudi::XYZPoint(xh,yh,zh));

    iv++;
  }

  const VD dataTrackProjXY = dataCond->paramVect<double>(m_CondDbTemplateParamNames [irad] [1]);
  VD::const_iterator ir = dataTrackProjXY.begin();
  double zt=0.0;
  VC aTkV;
  while(ir != dataTrackProjXY.end()) {
    double xt = (*ir);
    ir++;
    double yt= (*ir);
    aTkV.push_back(Gaudi::XYZPoint(xt,yt,zt));
    ir++;
  }

  // for now, take the first one in the track list to calculate the radius and phi.
  int aSize = (int) aHitV.size();
  VD aRadV; aRadV.clear(); aRadV.reserve(aSize);
  VD aPhiV; aPhiV.clear(); aPhiV.reserve(aSize);


  double xtkc= aTkV [0].x();
  double ytkc= aTkV [0].y();
  for(int ih=0; ih< aSize ; ++ih ){
    double aRad=  Tfm()->  CartToPolRad (aHitV[ih].x(),aHitV[ih].y(), xtkc, ytkc);
    double aPhi = Tfm()-> CartToPolPhi (aHitV[ih].x(),aHitV[ih].y(), xtkc, ytkc);
    aRadV.push_back( aRad);
    aPhiV.push_back(aPhi);
  }



  m_TemplateInp[irad]=aHitV;
  m_NumHitsInTemplate[irad]= (int) aHitV.size();
  m_TemplateCenterInp [irad]= aTkV;
  m_RadTemplateInp [irad]   = aRadV;
  m_ThTemplateInp [irad]    = aPhiV;

  //  m_TemplateInp.insert(m_TemplateInp.begin()+m_NumAgelTemplate,aHitV);
  //  m_NumHitsInTemplate.insert( m_NumHitsInTemplate.begin()+m_NumAgelTemplate, (int) aHitV.size());
  // m_TemplateCenterInp.insert( m_TemplateCenterInp.begin()+m_NumAgelTemplate, aTkV);
  // m_RadTemplateInp.insert(m_RadTemplateInp.begin()+m_NumAgelTemplate ,aRadV);
  // m_ThTemplateInp.insert(m_ThTemplateInp.begin()+m_NumAgelTemplate,aPhiV);
  //    m_NumR1gTemplate++;
  // }else if(irad == 2  ) {
  //  m_TemplateInp.push_back(aHitV);
  // m_NumHitsInTemplate.push_back((int) aHitV.size());
  // m_TemplateCenterInp.push_back(aTkV);
  //  m_RadTemplateInp.push_back(aRadV);
  // m_ThTemplateInp.push_back(aPhiV);
  // m_NumR2gTemplate++;
  // }

  if(irad==0 ) {
    m_NumAgelTemplate++;
  }else if( irad == 1) {
    m_NumR1gTemplate++;
  }else if(irad == 2  ) {
    m_NumR2gTemplate++;
  }

  m_NumTemplate++;



  return sc;


}
StatusCode RichTemplateDataTool::AcquireTemplateFromLocalDefault(int irad){
  // This is a temporary measure to read the read from a local C++ data class.
  // The permananet measure to read from the DB available, but it requires
  // the template data to be stored in the conditions DB.
  // SE 26-11-2008

  StatusCode sc = StatusCode::SUCCESS;
  double zh=0.0;
  VC aHitV; aHitV.clear(); aHitV.reserve(150);

  RichLocalDefaultTemplateData* aLocalData= RichLocalDefaultTemplateData::getRichLocalDefaultTemplateDataInstance();
  int idataIndex=0;
  int aSize =  aLocalData->getDefaultRichTemplateSize(irad);


  while(idataIndex < aSize ) {
    double xh = aLocalData->getCurRichRadiatorTemplateDataX(irad,idataIndex);
    double yh = aLocalData->getCurRichRadiatorTemplateDataY(irad,idataIndex);
    aHitV.push_back(Gaudi::XYZPoint(xh,yh,zh));
    idataIndex++;
  }
  double xtkc=   aLocalData->getCurrentTrackDefaultTemplate_X(irad);
  double ytkc=   aLocalData->getCurrentTrackDefaultTemplate_Y(irad);
  double ztkc=0.0;
  VC aTkV; aTkV.clear();
  aTkV.push_back(Gaudi::XYZPoint(xtkc,ytkc,ztkc)) ;

  VD aRadV; aRadV.clear(); aRadV.reserve(aSize);
  VD aPhiV; aPhiV.clear(); aPhiV.reserve(aSize);
  for(int ih=0; ih< aSize  ; ++ih ){
    double aRad=  Tfm()->  CartToPolRad (aHitV[ih].x(),aHitV[ih].y(), xtkc, ytkc);
    double aPhi = Tfm()->  CartToPolPhi (aHitV[ih].x(),aHitV[ih].y(), xtkc, ytkc);
    aRadV.push_back( aRad);
    aPhiV.push_back(aPhi);
  }



  m_TemplateInp[irad]=aHitV;
  m_NumHitsInTemplate[irad]= (int) aHitV.size();
  m_TemplateCenterInp [irad]= aTkV;
  m_RadTemplateInp [irad]   = aRadV;
  m_ThTemplateInp [irad]    = aPhiV;
  if(irad==0 ) {
    m_NumAgelTemplate++;
  }else if( irad == 1) {
    m_NumR1gTemplate++;
  }else if(irad == 2  ) {
    m_NumR2gTemplate++;
  }

  m_NumTemplate++;



  return sc;

}


StatusCode RichTemplateDataTool::ConfigureTemplateForRadiator(){

  StatusCode sc = StatusCode::SUCCESS;
  clearTemplateConfig();

  // for now, take the first track to make the calculations.
  // the current assumption is one template per radiator.
  // for multiple general templates per radiator, this would need modification.

  for(int rad=0; rad< (RConst()->maxNumRadiator()) ; ++rad ) {
    VC aC; aC.clear(); aC.reserve(NumHitsInTemplateValue(rad));
    VD aR; aR.clear(); aR.reserve(NumHitsInTemplateValue(rad));
    VD aT; aT.clear(); aT.reserve(NumHitsInTemplateValue(rad));

    double curTemplateRadiusZoomFactor=RConst()->TemplateRadiusZoomFactorValue(rad);

    double aZ=0;

    double TkXc= XTemplateCenterInpValue(0,  rad);

    double TkYc= XTemplateCenterInpValue(0,  rad);

    for(int ih=0; ih<NumHitsInTemplateValue(rad); ++ih) {
      double phihit = ThTemplateInpValue (ih,rad);
      int aPhiIndex = Tfm()->SearchIndexInVect(aT, phihit);
      aT.insert(aT.begin()+aPhiIndex , phihit);
      aR.insert(aR.begin()+aPhiIndex ,RadTemplateInpValue(ih,rad)*curTemplateRadiusZoomFactor);
      double aX = (XTemplateInpValue(ih,rad) -  TkXc)* curTemplateRadiusZoomFactor;
      double aY = (YTemplateInpValue(ih,rad) -  TkYc)* curTemplateRadiusZoomFactor;
      aC.push_back(Gaudi::XYZPoint(aX,aY,aZ));
    } // end loop over hits

    m_CHitTemplate.push_back(aC);
    m_RHitTemplate.push_back(aR);
    m_ThHitTemplate.push_back(aT);

  } // end loop over radiators

  // test print
  // PrintRadiatorHitTemplate ();
  // end test print

  return sc;


}

void RichTemplateDataTool::PrintTemplate()
{
  info()<<" Number of Templates "<<m_NumTemplate<<endmsg;
  info()<<" Number of Templates In Aerogel R1g R2g  "<<m_NumAgelTemplate<<"  "<<
    m_NumR1gTemplate<<"  "<<m_NumR2gTemplate<<endmsg;

  if(  m_NumTemplate <= ( RConst()->maxNumRadiator()) ){

    for(int itemp =0; itemp<m_NumTemplate  ; ++itemp ) {
      VC aHitV =  m_TemplateInp[itemp];
      VD aRadV =  m_RadTemplateInp[itemp];
      VD aPhiV =  m_ThTemplateInp[itemp];

      int aNumhit = m_NumHitsInTemplate[itemp];
      info()<<" Number of hits in template "<<itemp<<"  "<<aNumhit<<endmsg;
      for(int ih=0; ih< aNumhit; ++ih) {
        info() <<" X Y Rad phi of template hits "<<ih<<"  "<< aHitV[ih].x()<<"   "<< aHitV[ih].y()<<"  "
               << aRadV[ih] <<"  "<< aPhiV[ih] << endmsg;
      }
      int aNumTk =  m_TemplateCenterInp[itemp].size();
      for(int it=0; it< aNumTk ; ++it) {
        info()<<" XY of track projection "<<it<<"   "<< (m_TemplateCenterInp[itemp] [it]).x()<<
          "    "<<(m_TemplateCenterInp[itemp] [it]).y()<<endmsg;

      }

    }
  }else {

    info() << "Multiple templates per radiator "<<endmsg;

  }



}
void RichTemplateDataTool::PrintRadiatorHitTemplate (){
  for(int itemp =0; itemp<  ( RConst()->maxNumRadiator())  ; ++itemp ) {
    VC aHV =  m_CHitTemplate[itemp];
    VD aRV =  m_RHitTemplate[itemp];
    VD aTV =  m_ThHitTemplate[itemp];
    int aNumhit = (int) m_ThHitTemplate[itemp].size();
    info()<<" Number of hits in template "<<itemp<<"  "<<aNumhit<<endmsg;
    for(int ih=0; ih< aNumhit; ++ih) {
      info()<<" X Y Rad Phi of Configured Template "<<aHV[ih].x()<<"  "<<aHV[ih].y()<<"  "<<aRV[ih]
            <<"   "<<aTV[ih]<<endmsg;
    }

  }

}






