// $Id: SmartMuonMeasProvider.h,v 1.1 2009-07-01 18:27:12 polye Exp $
#ifndef SMARTMUONMEASUREMENTPROVIDER_H 
#define SMARTMUONMEASUREMENTPROVIDER_H 1

#include "GaudiKernel/DataObject.h"
#include "Event/Measurement.h"
#include "Event/State.h"


// Include files

/** @class SmartMuonMeasProvider SmartMuonMeasProvider.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-16
 */
typedef std::pair<LHCb::Measurement*,LHCb::Measurement*> MeasPair;
typedef std::vector<std::pair<LHCb::Measurement*,LHCb::Measurement*> > MeasPairList;
typedef std::pair<LHCb::LHCbID,double> LHCbIDWeight;

class SmartMuonMeasProvider : public DataObject{
public: 
  /// Standard constructor
  SmartMuonMeasProvider( );
  
  virtual ~SmartMuonMeasProvider();
  

  std::vector<double> m_stationsz;


  MeasPairList& atStation(int istation) {return m_meas[istation];}

  MeasPair getMeasPair() 
  {
    MeasPair out;
    return out;
  }
  

  std::vector<LHCb::LHCbID> idsInRange(LHCb::State& state, double nsigmas = 5.,int discriminationValue=1);
  
  std::vector<double> linearExtrapolator(const LHCb::State& state,const double z);

  int findClosestStation(double z);

  double distance(const MeasPair& meas, const LHCb::State& state){return sqrt(pow(distx(meas,state),2)+pow(disty(meas,state),2));}


  double distx(const MeasPair& meas, const LHCb::State& state)
  {
    return fabs(meas.first->measure() - linearExtrapolator(state,meas.first->z())[0]);
  }
  
  double disty(const MeasPair& meas, const LHCb::State& state)
  {
    return fabs(meas.second->measure() - linearExtrapolator(state,meas.first->z())[1]);
  }
  

  double uncx(const MeasPair& meas, const LHCb::State& state);

  double uncy(const MeasPair& meas, const LHCb::State& state);
  
  double chi2x(const MeasPair& meas, const LHCb::State& state){return pow(distx(meas,state),2)/pow(uncx(meas,state),2);}

  double chi2y(const MeasPair& meas, const LHCb::State& state){return pow(disty(meas,state),2)/pow(uncy(meas,state),2);}

  double chi2(const MeasPair& meas, const LHCb::State& state){return (chi2x(meas,state)+chi2y(meas,state));}
  


protected:
  
  std::vector<MeasPairList> m_meas;
  
  
  // OrderByValue _orderByValue;
  
  class OrderByValue {
  public:
    bool operator() (const LHCbIDWeight& id1, const LHCbIDWeight& id2) 
    {return id1.second < id2.second;}
  };
  
  

private:

};
#endif // MUONMEASUREMENTPROVIDER_H
