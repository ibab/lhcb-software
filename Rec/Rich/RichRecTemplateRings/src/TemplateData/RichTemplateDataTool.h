// $Id: RichTemplateDataTool.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef TEMPLATEDATA_RICHTEMPLATEDATATOOL_H 
#define TEMPLATEDATA_RICHTEMPLATEDATATOOL_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichTemplateDataTool.h"            // Interface


/** @class RichTemplateDataTool RichTemplateDataTool.h TemplateData/RichTemplateDataTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-06
 */
class RichTemplateDataTool : public RichRingRecToolBase , 
                             virtual public IRichTemplateDataTool {
public: 
  /// Standard constructor
  RichTemplateDataTool( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~RichTemplateDataTool( ); ///< Destructor

  StatusCode init();

  StatusCode  RegisterCurrentCondition(int i, std::string aPath);
  
  StatusCode umsAerogelTemplateAcquire();
  StatusCode umsRich1GasTemplateAcquire();
  StatusCode umsRich2GasTemplateAcquire();
  
  StatusCode AcquireTemplate(int irad);
  StatusCode AcquireTemplateFromLocalDefault(int irad);
  StatusCode ConfigureTemplateForRadiator();
  
  void   PrintTemplate();
  void PrintRadiatorHitTemplate ();
  


  void clearTemplateInputs();
  void clearTemplateConfig();
  
  CD CTemplateInp (int i, int irad ){return  m_TemplateInp [irad] [i];}
  double XTemplateInpValue( int i, int irad ){  return m_TemplateInp [irad] [i].x();}
  double YTemplateInpValue( int i,  int irad ){  return m_TemplateInp [irad] [i].y();}
  double RadTemplateInpValue(int i,  int irad ){ return m_RadTemplateInp [irad] [i];}
  double ThTemplateInpValue(int i, int irad ){ return m_ThTemplateInp [irad] [i];}
  int NumHitsInTemplateValue(int irad ){ return m_NumHitsInTemplate [irad];}
  CD CTemplateCenterInp ( int i, int irad ){return m_TemplateCenterInp [irad] [i];}
  double XTemplateCenterInpValue (int i, int irad ){  return m_TemplateCenterInp [irad] [i].x();}
  double YTemplateCenterInpValue (int i, int irad ){  return m_TemplateCenterInp [irad] [i].y();}
 
  CD CHitTemplate(int i, int irad)  {  return  m_CHitTemplate [irad] [i];}
  double XHitTemplateValue(int i, int irad) {  return m_CHitTemplate [irad] [i].x();}
  double YHitTemplateValue(int i, int irad) {  return m_CHitTemplate [irad] [i].y();}
  double RHitTemplateValue(int i, int irad) {  return m_RHitTemplate [irad] [i];}
  double ThHitTemplateValue(int i, int irad){  return m_ThHitTemplate [irad] [i];}

protected:

private:

  VVC m_TemplateInp;
  VVD m_RadTemplateInp;
  VVD m_ThTemplateInp;
  VI m_NumHitsInTemplate;
  int m_NumTemplate;
  VVC m_TemplateCenterInp;
  int m_NumAgelTemplate;
  int m_NumR1gTemplate;
  int m_NumR2gTemplate;


  VVC m_CHitTemplate;
  VVD m_RHitTemplate;
  VVD m_ThHitTemplate;
  
  
  
      
  VS m_CondDbTemplatePathNames;  
  VVS m_CondDbTemplateParamNames;

  bool m_ActivateTemplateInputFromLocalDataSet;
  

};
#endif // TEMPLATEDATA_RICHTEMPLATEDATATOOL_H
