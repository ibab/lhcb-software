// $Id: RichG4ReconTransformHpd.h,v 1.1 2004-02-04 13:53:01 seaso Exp $
#ifndef RICHANALYSIS_RICHG4RECONTRANSFORMHPD_H 
#define RICHANALYSIS_RICHG4RECONTRANSFORMHPD_H 1
// Include files
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"

/** @class RichG4ReconTransformHpd RichG4ReconTransformHpd.h RichAnalysis/RichG4ReconTransformHpd.h
 *  
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-09
 */
class RichG4ReconTransformHpd {
public:
  /// Standard constructor
  RichG4ReconTransformHpd( ); 

  RichG4ReconTransformHpd(int aRichDetNum, int aHpdNum ); 

  
  virtual ~RichG4ReconTransformHpd( ); ///< Destructor

  HepTransform3D HpdLocalToGlobal() 
  {
    return m_HpdLocalToGlobal;
  }
  HepTransform3D HpdGlobalToLocal()
  {
    return m_HpdGlobalToLocal;
  }

  
protected:

private:

  HepTransform3D m_HpdLocalToGlobal;
  
  HepTransform3D m_HpdGlobalToLocal;

  int   m_Rich1SubMasterPvIndex;
  
  std::string   m_Rich1MagShPvName0;
  std::string   m_Rich1MagShPvName1;
  int   m_Rich1PhotDetSupPvIndex;
  int   m_HpdSMasterIndex;
  int   m_Rich2HpdPanelIndex0;
  int   m_Rich2HpdPanelIndex1;
  
  int m_Rich1HpdArrayMaxH0;
  int m_Rich2HpdArrayMaxH0;

  std::string m_Rich2HpdPanelName0;
  std::string  m_Rich2HpdPanelName1;
  
  int  m_Rich1MagShPvIndexH0;
  int   m_Rich1MagShPvIndexH1;
  
};
#endif // RICHANALYSIS_RICHG4RECONTRANSFORMHPD_H
