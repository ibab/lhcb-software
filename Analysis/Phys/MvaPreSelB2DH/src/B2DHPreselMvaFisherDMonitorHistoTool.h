// $Id: $
#ifndef B2DHPRESELMVAFISHERDMONITORHISTOTOOL_H 
#define B2DHPRESELMVAFISHERDMONITORHISTOTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
//#include "GaudiAlg/GaudiTool.h"
#include "IB2DHPreselMvaFisherDMonitorHistoTool.h"            // Interface
#include "IB2DHPreselMvaUtilityTool.h"
#include "IB2DHMvaPreSelectionParamTool.h"


/** @class B2DHPreselMvaFisherDMonitorHistoTool B2DHPreselMvaFisherDMonitorHistoTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-07-19
 */
class B2DHPreselMvaFisherDMonitorHistoTool : public GaudiTupleTool, virtual public IB2DHPreselMvaFisherDMonitorHistoTool {
public: 
  /// Standard constructor
  B2DHPreselMvaFisherDMonitorHistoTool( const std::string& type, 
                                        const std::string& name,
                                        const IInterface* parent);

  virtual ~B2DHPreselMvaFisherDMonitorHistoTool( ); ///< Destructor
  virtual void fillHistoForEventSelectionSteps(const LHCb::Particles * partsBs,int anumCand, int indexOfbestCand,
                                                                     std::vector<double> aProb,std::vector<double>aIps,
                                                                     std::vector<int> aCndNoSel, 
                                                                     std::vector<int> aCandPrelimSel,
                                                                     std::vector<int> aCandFisherSel,
                                                                     std::vector<int> aCandAuxSel,
                                                                     std::vector<int> aCandMassSel,
                                                                      std::vector<int> aCandIndex );
  virtual StatusCode initialize();
  double getBsMassDiffValue(const LHCb::Particles * partsBs,int aIndex);

  void fillHistoAfterSelWithL0(const LHCb::Particles * partsBs,int aIndex ) ;
  void fillHistoAfterAllSelBeforeL0(const LHCb::Particles * partsBs,int aIndex);
  void fillHistoAfterAuxSel(const LHCb::Particles * partsBs,int aIndex );
  void fillHistoAfterFisherSel(const LHCb::Particles * partsBs,int aIndex );
  void fillHistoAfterPrelimSel(const LHCb::Particles * partsBs,int aIndex );
  void fillHistoNoSel(const LHCb::Particles * partsBs,int aIndex );

protected:

private:

  IB2DHPreselMvaUtilityTool * m_B2DHPreselMvaUtilityTool;
  IB2DHMvaPreSelectionParamTool  * m_B2DHMvaPreSelectionParamTool;

};
#endif // B2DHPRESELMVAFISHERDMONITORHISTOTOOL_H
