// $Id: $
#ifndef IB2DHPRESELMVAUTILITYTOOL_H 
#define IB2DHPRESELMVAUTILITYTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IB2DHPreselMvaUtilityTool ( "IB2DHPreselMvaUtilityTool", 1, 0 );

/** @class IB2DHPreselMvaUtilityTool IB2DHPreselMvaUtilityTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-07-16
 */
class IB2DHPreselMvaUtilityTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IB2DHPreselMvaUtilityTool; }
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
  virtual int pdgDdValue()=0;
  virtual double massStdDdValue()=0;
  virtual double massStdBdValue()=0;
  virtual int pdg_MinDValue()=0;
  virtual int pdg_MaxDValue()=0;
  
  
  virtual int FindBestCandidateInEv(std::vector<int> aCandIndex,
                                    std::vector<double> aIps_Bs, 
                    std::vector<double>aprob_Bs, int SelOption)=0;
  
  virtual std::string fisherWeightsFullFileName(int channelnum)=0;



protected:

private:

};
#endif // IB2DHPRESELMVAUTILITYTOOL_H
