// $Id: $
#ifndef IB2DHMVAPRESELECTIONPARAMTOOL_H 
#define IB2DHMVAPRESELECTIONPARAMTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"

static const InterfaceID IID_IB2DHMvaPreSelectionParamTool ( "IB2DHMvaPreSelectionParamTool", 1, 0 );

/** @class IB2DHMvaPreSelectionParamTool IB2DHMvaPreSelectionParamTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-07-16
 */
class IB2DHMvaPreSelectionParamTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IB2DHMvaPreSelectionParamTool; }
  virtual int get_current_B2DH_channel_Number() =0;
  virtual void set_current_B2DH_channel_Number(int ach ) =0;
  virtual StatusCode BookMvaForFisherD(const std::string& WeightFileName,
                                       const std::string& ClassifierName )=0;
  virtual void initializeLocalParam()=0;
  virtual StatusCode acquireSelectionParams(const LHCb::Particle* b, const  LHCb::RecVertex* aPV)=0;
  virtual double evaluateFisherDProb()=0;
  virtual double getfisherDProbValue()=0;
  virtual double getfisherDResponseVal()=0;
  virtual double evaluateFisherResponseValWithStandAlone(const std::vector<double>& aParamList )=0;
  
  virtual  std::string get_classifierName()=0;
  virtual  std::string get_weightFileName()=0;
  

  virtual double  ipsBs() =0; 
  virtual double  ipsBachelorPart() =0; 
  virtual double  FsBs() =0;
  virtual double  cosBs() =0;
  virtual double  momBs() =0;
  virtual double  bvtxchisq() =0; 
  virtual double  BsMassDiff() =0; 
  virtual double  ZPvDiff() =0; 
  virtual double  dvtxchisq() =0;
  virtual double  ptDs() =0;
  virtual double  momDs()=0;
  virtual double  FsDs() =0;
  virtual double  DsMassDiff() =0;
  virtual double  ptBachelorPart() =0;
  virtual double  momBachelorPart() =0;
  virtual double  BachelorKaDll() =0;
  virtual double  BachelorPrDll() =0;
  virtual double  BachelorMuDll() =0;

  virtual double  BdMassDiff() =0; 
  virtual double  DdMassDiff() =0;

  virtual std::vector<double> ptSecPart()=0;
  virtual std::vector<double> secMomValue()=0;
  virtual std::vector<double> ipsSecPart()=0;
  virtual std::vector<double> bKaonSecDll()=0;
  virtual std::vector<double> bProtonSecDll()=0;


  virtual std::vector<double> FisherDParamList() =0;


protected:

private:

};
#endif // IB2DHMVAPRESELECTIONPARAMTOOL_H
