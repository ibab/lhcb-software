// $Id: IBs2DsHMvaUtilityTool.h,v 1.1 2010/06/10 12:42:22 seaso Exp $
#ifndef IBS2DSHMVAUTILITYTOOL_H 
#define IBS2DSHMVAUTILITYTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IBs2DsHMvaUtilityTool ( "IBs2DsHMvaUtilityTool", 1, 0 );

/** @class IBs2DsHMvaUtilityTool IBs2DsHMvaUtilityTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-02-11
 */
class IBs2DsHMvaUtilityTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBs2DsHMvaUtilityTool; }

  virtual void initializeDataSelectionPathStrings() =0;
  virtual void initializeStdParamForMvaSel() =0;
  virtual std::string getcurrentBsLocation(int aChannelNumA) =0;
  virtual std::string getparticle2RefittedSortedPVRelationsLoc
                      (int aChannelNumB) =0;
  virtual std::string getparticle2RefittedPVRelationsLoc
                      (int aChannelNumC) =0;

  virtual std::string getcurrentStdPVLocation() =0;


  virtual int pdgDsValue()=0;
  virtual double massStdDsValue() =0;
  virtual int pdgKaonValue() =0;
  virtual double massStdBsValue() =0;
  virtual int pdgPionValue() =0;
  virtual bool getL0Decision ()=0;
  
  virtual int FindBestCandidateInEv(std::vector<int> aCandIndex,
                                    std::vector<double> aIps_Bs, 
                    std::vector<double>aprob_Bs, int SelOption)=0;
  
  virtual std::string fisherWeightsFullFileName(int channelnum)=0;


  
  
protected:

private:

};
#endif // IBS2DSHMVAUTILITYTOOL_H
