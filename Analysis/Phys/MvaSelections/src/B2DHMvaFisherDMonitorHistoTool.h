// $Id: B2DHMvaFisherDMonitorHistoTool.h,v 1.1 2010/06/10 12:42:21 seaso Exp $
#ifndef B2DHMVAMONITORHISTOTOOL_H 
#define B2DHMVAMONITORHISTOTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
//#include "BasePlotTool.h"
#include "IB2DHMvaMonitorHistoTool.h"            // Interface
#include "IBs2DsHMvaUtilityTool.h"            // Interface
#include "IB2DHSelectionParamTool.h"

/** @class B2DHMvaMonitorHistoTool B2DHMvaMonitorHistoTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2009-07-17
 */
class B2DHMvaMonitorHistoTool : public GaudiTupleTool, virtual public IB2DHMvaMonitorHistoTool {
public: 
  /// Standard constructor
  B2DHMvaMonitorHistoTool( const std::string& type, 
                                 const std::string& name,
                                 const IInterface* parent);

  virtual ~B2DHMvaMonitorHistoTool( ); ///< Destructor
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

  IBs2DsHMvaUtilityTool * m_Bs2DsHMvaUtilityTool;
  IB2DHSelectionParamTool * m_B2DHSelectionParamTool;
  
  
};
#endif // B2DHMVAMONITORHISTOTOOL_H
