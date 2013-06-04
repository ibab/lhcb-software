// $Id: $
#ifndef B2DHMVAPRESELECTIONPARAMTOOL_H
#define B2DHMVAPRESELECTIONPARAMTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IB2DHMvaPreSelectionParamTool.h"            // Interface

#include "Kernel/IDistanceCalculator.h"
//#include <TMVA/Reader.h>

#include "IB2DHPreselMvaUtilityTool.h"      // Interface
//#include "IClassifierReader.h"
#include "TMVAnalysis_Bd2DH_F621Sph_FisherD.class.h"

class IDistanceCalculator;
class IDVAlgorithm;


/** @class B2DHMvaPreSelectionParamTool B2DHMvaPreSelectionParamTool.h
 *
 *
 *  @author Sajan EASO
 *  @date   2010-07-16
 */
class B2DHMvaPreSelectionParamTool : public GaudiTool, 
                                     virtual public IB2DHMvaPreSelectionParamTool
{

public:

  /// Standard constructor
  B2DHMvaPreSelectionParamTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent);

  virtual ~B2DHMvaPreSelectionParamTool( ); ///< Destructor
  virtual StatusCode  initialize();


  virtual int get_current_B2DH_channel_Number()
  {    return  m_current_B2DH_channel_Number;}
  virtual void set_current_B2DH_channel_Number(int ach )
  { m_current_B2DH_channel_Number=ach; }


  virtual StatusCode BookMvaForFisherD(const std::string& WeightFileName, 
                                       const std::string& ClassifierName);

  virtual StatusCode acquireSelectionParams(const LHCb::Particle* b,
                                            const  LHCb::RecVertex* aPV);

  StatusCode getFisherDStringVar();

  StatusCode getSelectionParamsForBsParticle(const LHCb::Particle* b,
                                             const  LHCb::RecVertex* aPV);

  StatusCode getSelectionCutsParamForBsParticle(const LHCb::Particle* b,
                                                const  LHCb::RecVertex* aPV);

  StatusCode getSelectionParamsForDsDaughterOfBs(const LHCb::Particle* b,
                                                 const  LHCb::RecVertex* aPV);

  StatusCode getSelectionCutsParamsForDsDaughterOfBs(const LHCb::Particle* aDsPart,
                                                     const  LHCb::RecVertex* aPV);
  StatusCode  getSelectionParamsForBachelorDaughterOfBs(const LHCb::Particle* b,
                                                        const  LHCb::RecVertex* aPV);
  StatusCode getSelectionCutsParamsForBachelorDaughterOfBs(const LHCb::Particle* aBach);

  StatusCode getSelectionParamsForSecondaryDaughtersOfBs(const LHCb::Particle* b,
                                                         const  LHCb::RecVertex* aPV);

  StatusCode getSelectionCutsParamsForSecondaryDaughtersOfBs(const LHCb::Particle* aPartSec,int aSecNum);
  virtual void initializeLocalParam();
  void resetParamInCurrentEvent();

  virtual double evaluateFisherDProb();

  double evaluateFisherResponseVal();

  void setfisherDProbValue(double aProbVal);

  virtual double getfisherDProbValue()
  {  return m_probValue;}
  void setfisherDResponseValue(double aResponseVal);

  virtual double getfisherDResponseVal()
  {  return m_responseValue;}

  virtual double evaluateFisherResponseValWithStandAlone(const std::vector<double>& aParamList );


  virtual  std::string get_classifierName(){  return m_classifierName;}
  virtual  std::string get_weightFileName(){  return m_weightFileName;}





  virtual double  ipsBs() {return m_ipsBs;}
  virtual double  ipsBachelorPart() {return m_ipsBachelorPart;}
  virtual double  FsBs() {return m_FsBs;}
  virtual double  cosBs() {return m_cosBs;}
  virtual double  momBs() {return m_momBs;}
  virtual double  bvtxchisq() {return m_bvtxchisq;}
  virtual double  BsMassDiff() {return m_BsMassDiff;}
  virtual double  ZPvDiff() {return m_ZPvDiff;}
  virtual double  dvtxchisq() {return m_dvtxchisq ;}
  virtual double  ptDs() {return m_ptDs;}
  virtual double  FsDs() {    return m_FsDs;  }
  virtual double  momDs() {return m_momDs;}




  virtual double  DsMassDiff() {return m_DsMassDiff;}
  virtual double  ptBachelorPart() {return m_ptBachelorPart;}
  virtual double  momBachelorPart() {return m_momBachelorPart;}
  virtual double  BachelorKaDll() {return m_BachelorKaDll;}
  virtual double  BachelorPrDll() {return m_BachelorPrDll;}
  virtual double  BachelorMuDll() {return m_BachelorMuDll;}

  virtual double  BdMassDiff() {return m_BdMassDiff;}
  virtual double  DdMassDiff() {return m_DdMassDiff;}

  virtual int numSecPart()  { return m_numSecPart;}

  virtual std::vector<double> ptSecPart(){  return m_ptSecPart;}
  virtual std::vector<double> secMomValue(){  return m_secMomValue;}
  virtual std::vector<double> ipsSecPart() {  return m_ipsSecPart;}
  virtual std::vector<double> bKaonSecDll(){  return m_bKaonSecDll;}
  virtual std::vector<double> bProtonSecDll(){ return  m_bProtonSecDll;}

  virtual std::vector<double> FisherDParamList(){  return m_FisherDParamList;}

private:

  const IDistanceCalculator * m_distanceCalculator;
  IDVAlgorithm* m_dva;

  IB2DHPreselMvaUtilityTool * m_B2DHPreselMvaUtilityTool;


  IClassifierReader*   m_B2DHFisherDReader;

  //   TMVA::Reader * m_B2DHFisherDReader;

  int m_current_B2DH_channel_Number; // this is set to 0 for Bs2DsK, 1 for Bs2DsPi.

  std::string m_classifierName;
  std::string m_weightFileName;
  std::vector<double>  m_FisherDParamList;
  std::vector<Float_t>  m_FisherDParamListInFloat;
  std::vector<std::string>  m_FisherDStringVarList;



  int m_numFisherDParam;

  double m_probValue;
  double m_responseValue;


  double  m_ipsBs;
  double  m_ipsBachelorPart;
  double  m_FsBs;
  double  m_cosBs;
  double  m_momBs;
  double  m_bvtxchisq;
  double  m_BsMassDiff;
  double  m_ZPvDiff;
  double  m_dvtxchisq ;
  double  m_ptDs;
  double  m_FsDs;
  double  m_momDs;
  double  m_DsMassDiff;
  double  m_ptBachelorPart;
  double  m_momBachelorPart;
  double  m_BachelorKaDll;
  double  m_BachelorPrDll;
  double  m_BachelorMuDll;

  double  m_BdMassDiff;
  double  m_DdMassDiff;


  int m_numSecPart;
  std::vector<double> m_ptSecPart;
  std::vector<double> m_secMomValue;
  std::vector<double> m_ipsSecPart;

  std::vector<double> m_bKaonSecDll;
  std::vector<double> m_bProtonSecDll;



};
#endif // B2DHMVAPRESELECTIONPARAMTOOL_H
