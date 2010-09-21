// $Id: $
#ifndef B2DHMVAPRESELECTIONCUTSTOOL_H 
#define B2DHMVAPRESELECTIONCUTSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IB2DHMvaPreSelectionCutsTool.h"            // Interface

#include "IB2DHMvaPreSelectionParamTool.h"

/** @class B2DHMvaPreSelectionCutsTool B2DHMvaPreSelectionCutsTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-07-16
 */
class B2DHMvaPreSelectionCutsTool : public GaudiTool, virtual public IB2DHMvaPreSelectionCutsTool {
public: 
  /// Standard constructor
  B2DHMvaPreSelectionCutsTool( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);

  virtual ~B2DHMvaPreSelectionCutsTool( ); ///< Destructor
  void setCommonSelectionCutValues();
  void setBs2DsKCutValues();
  void setBs2DsPiCutValues();
  virtual StatusCode initialize();
  
  void setCutFlags_Common();
  void setCutFlags_Bs2DsK();
  void setCutFlags_Bs2DsPi();
  void resetSelectionFlags();

  virtual void setAllCutFlags();
  
  virtual bool ApplyPreTrainSel();
  virtual bool ApplyFisherSel(int channelNum);
  virtual bool ApplyAuxCuts( int aChannelNum);
  virtual bool ApplyMassWindowCuts(int aChannelNum);


  bool ApplyMassWindowCuts_Bs2DsK();  
  bool ApplyMassWindowCuts_Bs2DsPi();

protected:

private:
  IB2DHMvaPreSelectionParamTool * m_B2DHMvaPreSelectionParamTool;

  
  double m_zPVBsCutValue;
  double m_massBsWindowCutValue;
  double m_massDsWindowCutValue;
  double m_massBsWideWindowCutValue;
  double m_ipsBsTightCutValue;
  double m_ipsBsCutValue;
  double m_vtxBsChisqCutValue;
  double m_vtxDsChisqCutValue;
  double m_ptDsCutValue;
  double m_ipsBachelorCutValue;
  double m_ipsSecLowCutValue;
  double m_ipsSecUpCutValue;
  double m_totMomSecCutValue;


   double m_massDsWindowCutValue_Bs2DsK;
  double m_momBachelorUpperCutValue_Bs2DsK;
  double m_FsBsCutValue_Bs2DsK;
  double m_dllBachelorKaonCutValue_Bs2DsK;
  double m_dllKaonMinusProtonBachelorKaonCutValue_Bs2DsK;
  double m_dllProtonBachelorLowerCut_Bs2DsK;
  
  double m_dllProtonSecCut_Bs2DsK;
  double m_dllKaonMinusProtonSecCut_Bs2DsK;
  


  double m_fisherDCutValue_Bs2DsK;


  bool m_massBsWindowSel_Bs2DsK;  
  bool m_massDsWindowSel_Bs2DsK ;
  bool m_massWindowSel_Bs2DsK;

  bool m_massBsWindowSel_Bs2DsPi;  
  bool m_massDsWindowSel_Bs2DsPi ;
  bool m_massWindowSel_Bs2DsPi;


 
  double m_fisherDCutValue_Bs2DsPi;

  double m_massBsWindowCutValue_Bs2DsPi;

  double m_FsBsCutValue_Bs2DsPi; 
  double m_ptBachelorCutValue_Bs2DsPi;
  double m_momDsUpperCutValue_Bs2DsPi;
  double m_FsDsCutValue_Bs2DsPi;
  double m_momBachelorUpperCutValue_Bs2DsPi;
  double m_momBachelorLowerCutValue_Bs2DsPi;
  double m_dllKaonBachelorLowerCutValue_Bs2DsPi;
  double m_dllKaonBachelorUpperCutValue_Bs2DsPi;

  double m_dllProtonBachelorCutValue_Bs2DsPi;
  double m_dllMuonBachelorUpperCutValue_Bs2DsPi;
  double m_dllProtonMinusMuonBachelorCutValue_Bs2DsPi;
  
  double m_dllProtonSecCutValue_Bs2DsPi;
  double m_dllKaonMinusProtonSecCutValue_Bs2DsPi;
  double m_dllKaonSec1LowerCutValue_Bs2DsPi;
  double m_dllKaonSec1UpperCutValue_Bs2DsPi;
  double m_dllKaonSec0LowerCutValue_Bs2DsPi;
  double m_dllKaonSec0UpperCutValue_Bs2DsPi;
  double m_dllProtonSec1LowerCutValue_Bs2DsPi;
  double m_dllKaonSec2CutValue_Bs2DsPi;
  double m_dllProtonSec2UpperCutValue_Bs2DsPi;
  double m_dllProtonSec2LowerCutValue_Bs2DsPi;
  
  double  m_vtxBsChisqCutValue_Bs2DsPi;
  double  m_vtxDsChisqCutValue_Bs2DsPi;
  double  m_ipsBachelorCutValue_Bs2DsPi;


  // Now the combined Cut Flags

  bool m_zPVBsSel;
  bool m_massBsWindowSel;  
  bool m_massDsWindowSel ;
  bool m_massWindowSel;
  bool m_IpsSel;
  bool m_IpsSecSel;
  bool m_vtxChisqSel ;
  bool m_ptDs_momSec_Sel;
  bool m_Train_Sel;


  bool m_auxCut_Bs_Bs2DsK;  
  bool m_auxCut_Dll_Bs2DsK;
  bool m_fisher_Sel_Bs2DsK;

  bool m_auxCut_Bs_Bs2DsPi;  
  bool m_auxCutDll_Bachelor_Bs2DsPi ;
  bool m_auxCutDll_Sec_Bs2DsPi ;
  bool m_fisher_Sel_Bs2DsPi;  

  bool m_vtxChisqSel_Bs2DsPi ;
};
#endif // B2DHMVAPRESELECTIONCUTSTOOL_H
