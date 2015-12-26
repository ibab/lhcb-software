#ifndef SMARTMUONMEASUREMENTPROVIDER_H 
#define SMARTMUONMEASUREMENTPROVIDER_H 1

#include <array>
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
typedef std::pair<std::unique_ptr<LHCb::Measurement>,std::unique_ptr<LHCb::Measurement>> MeasPair;
typedef std::vector<MeasPair> MeasPairList;
typedef std::pair<LHCb::LHCbID,double> LHCbIDWeight;

class SmartMuonMeasProvider : public DataObject {
public: 
  /// Standard constructor
  SmartMuonMeasProvider( ) = default;
  
  ~SmartMuonMeasProvider() override = default;

  const MeasPairList& atStation(int istation) const { return m_meas[istation]; }
  MeasPairList& atStation(int istation) { return m_meas[istation]; }

  std::vector<LHCb::LHCbID> idsInRange(const LHCb::State& state, double nsigmas = 5.,int discriminationValue=1);
  
  std::pair<double,double> linearExtrapolator(const LHCb::State& state,double z) const;

  int findClosestStation(double z);

  double distance(const MeasPair& meas, const LHCb::State& state) const
  {
      return std::sqrt(std::pow(distx(meas,state),2)+std::pow(disty(meas,state),2));
  }

  double distx(const MeasPair& meas, const LHCb::State& state) const
  {
    return std::abs(meas.first->measure() - linearExtrapolator(state,meas.first->z()).first);
  }
  
  double disty(const MeasPair& meas, const LHCb::State& state) const
  {
    return std::abs(meas.second->measure() - linearExtrapolator(state,meas.first->z()).second);
  }
  
  //get uncx from errors on pair and state
  double uncx(const MeasPair& meas, const LHCb::State& state) const{
    return sqrt(state.errX2()+std::pow(meas.first->errMeasure(),2));
  }

  //get uncy from errors on pair and state
  double uncy(const MeasPair& meas, const LHCb::State& state) const {
    return sqrt(state.errY2()+std::pow(meas.second->errMeasure(),2));
  }
  
  double chi2x(const MeasPair& meas, const LHCb::State& state) const
  {return std::pow(distx(meas,state)/uncx(meas,state),2);}

  double chi2y(const MeasPair& meas, const LHCb::State& state) const
  {return std::pow(disty(meas,state)/uncy(meas,state),2);}

  double chi2(const MeasPair& meas, const LHCb::State& state) const
  {return chi2x(meas,state)+chi2y(meas,state);}
  
private:
  
  std::array<MeasPairList,5> m_meas;

  struct OrderByValue {
    bool operator()(const LHCbIDWeight& id1, const LHCbIDWeight& id2) const
    {return id1.second < id2.second;}
  };
  
};
#endif // MUONMEASUREMENTPROVIDER_H
