// $Id: RichG4ReconTransformHpd.h,v 1.6 2008-05-30 13:43:23 gcorti Exp $
#ifndef RICHANALYSIS_RICHG4RECONTRANSFORMPMT_H
#define RICHANALYSIS_RICHG4RECONTRANSFORMPMT_H 1
// Include files
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "GaudiKernel/Transform3DTypes.h"
 

/** @class RichG4ReconTransformPmt RichG4ReconTransformPmt.h RichAnalysis/RichG4ReconTransformPmt.h
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-09
 */
class RichG4ReconTransformPmt {

public:

  /// Standard constructor
  RichG4ReconTransformPmt( );

  RichG4ReconTransformPmt(int aRichDetNum, int PmtModuleNum, int aPmtNum );

  virtual ~RichG4ReconTransformPmt( ); ///< Destructor

  const Gaudi::Transform3D & PmtLocalToGlobal()
  {
    return m_PmtLocalToGlobal;
  }

  const Gaudi::Transform3D & PmtGlobalToLocal()
  {
    return m_PmtGlobalToLocal;
  }
  const Gaudi::Transform3D & PmtPhDetPanelToLocal() 
  {  return m_PmtPhDetPanelToLocal;}
  const Gaudi::Transform3D & PmtLocalToPmtPhDetPanel()
  {   return m_PmtLocalToPmtPhDetPanel;}
  
    
  
  void initialise();
protected:

private:

  Gaudi::Transform3D m_PmtLocalToGlobal;

  Gaudi::Transform3D m_PmtGlobalToLocal;


  Gaudi::Transform3D m_PmtPhDetPanelToLocal;
  Gaudi::Transform3D m_PmtLocalToPmtPhDetPanel;
  
  int   m_Rich1SubMasterPvIndex;

  std::string   m_Rich1MagShPvName0;
  std::string   m_Rich1MagShPvName1;
  int   m_Rich1PhotDetSupPvIndex;
  int  m_RichPmtInModuleStartIndex;
  
  int   m_PmtSMasterIndex;
  // int   m_Rich2PmtPanelIndex0;
  // int   m_Rich2PmtPanelIndex1;

  int m_Rich1PmtArrayMaxH0;
  int m_Rich2PmtArrayMaxH0;

  int m_Rich1PmtModuleMaxH0;
  int m_Rich2PmtModuleMaxH0;
  int m_RichNumPmtInModule;

  //  int m_Rich2N2EnclIndex0;
  // int m_Rich2N2EnclIndex1;

  std::string m_Rich2PmtPanelName0;
  std::string  m_Rich2PmtPanelName1;
  std::string m_Rich2PmtN2EnclName0;
  std::string m_Rich2PmtN2EnclName1;
  std::string m_Rich1PmtModuleMasterNamePrefClassic;
  std::string m_Rich1PmtModuleMasterNamePrefLens;
  std::vector<std::string> m_Rich1PmtModuleMasterNameSuf;

  std::string m_Rich1PhtDetSupName0;
  std::string m_Rich1PhtDetSupName1;
  

  //  int  m_Rich1MagShPvIndexH0;
  // int   m_Rich1MagShPvIndexH1;

};
#endif // RICHANALYSIS_RICHG4RECONTRANSFORMPMT_H
