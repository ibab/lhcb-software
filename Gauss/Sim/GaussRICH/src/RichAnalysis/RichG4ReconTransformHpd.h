// $Id: RichG4ReconTransformHpd.h,v 1.6 2008-05-30 13:43:23 gcorti Exp $
#ifndef RICHANALYSIS_RICHG4RECONTRANSFORMHPD_H
#define RICHANALYSIS_RICHG4RECONTRANSFORMHPD_H 1
// Include files
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "GaudiKernel/Transform3DTypes.h"
 

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

  const Gaudi::Transform3D & HpdLocalToGlobal()
  {
    return m_HpdLocalToGlobal;
  }

  const Gaudi::Transform3D & HpdGlobalToLocal()
  {
    return m_HpdGlobalToLocal;
  }

  const Gaudi::Transform3D & HpdPhDetToLocal() 
  {
   return   m_HpdPhDetToLocal;

  }

  const Gaudi::Transform3D & HpdLocalToPhDet () 
  {
    return m_HpdLocalToPhDet;
  }
  

  void initialise();
protected:

private:

  Gaudi::Transform3D m_HpdLocalToGlobal;

  Gaudi::Transform3D m_HpdGlobalToLocal;

  Gaudi::Transform3D m_HpdPhDetToLocal;
  Gaudi::Transform3D m_HpdLocalToPhDet;
  

  int   m_Rich1SubMasterPvIndex;

  std::string   m_Rich1MagShPvName0;
  std::string   m_Rich1MagShPvName1;
  int   m_Rich1PhotDetSupPvIndex;
  int   m_HpdSMasterIndex;
  // int   m_Rich2HpdPanelIndex0;
  // int   m_Rich2HpdPanelIndex1;

  int m_Rich1HpdArrayMaxH0;
  int m_Rich2HpdArrayMaxH0;

  //  int m_Rich2N2EnclIndex0;
  // int m_Rich2N2EnclIndex1;

  std::string m_Rich2HpdPanelName0;
  std::string  m_Rich2HpdPanelName1;
  std::string m_Rich2HpdN2EnclName0;
  std::string m_Rich2HpdN2EnclName1;


  //  int  m_Rich1MagShPvIndexH0;
  // int   m_Rich1MagShPvIndexH1;

};
#endif // RICHANALYSIS_RICHG4RECONTRANSFORMHPD_H
