// $Id: RichG4TransformPhDet.h,v 1.3 2008-05-30 13:43:23 gcorti Exp $
#ifndef GAUSSRICH_RICHG4TRANSFORMPHDET_H 
#define GAUSSRICH_RICHG4TRANSFORMPHDET_H 1

// Include files
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "GaudiKernel/Transform3DTypes.h"
  

/** @class RichG4TransformPhDet RichG4TransformPhDet.h RichAnalysis/RichG4TransformPhDet.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2006-02-17
 */
class RichG4TransformPhDet {
public: 
  /// Standard constructor
  RichG4TransformPhDet( ); 
  RichG4TransformPhDet(int RichDetNum, int RichSectnum ); 

  virtual ~RichG4TransformPhDet( ); ///< Destructor

  const Gaudi::Transform3D & PhDetLocalToGlobal()
  {
    return m_PhDetLocalToGlobal;
  }

  const Gaudi::Transform3D & PhDetGlobalToLocal()
  {
    return m_PhDetGlobalToLocal;
  }
  void initialise();
  
protected:

private:

  Gaudi::Transform3D m_PhDetLocalToGlobal;

  Gaudi::Transform3D m_PhDetGlobalToLocal;

  int   m_Rich1SubMasterPvIndex;

  std::string   m_Rich1MagShPvName0;
  std::string   m_Rich1MagShPvName1;
  int   m_Rich1PhotDetSupPvIndex;
  int   m_HpdSMasterIndex;
  // int   m_Rich2HpdPanelIndex0;
  // int   m_Rich2HpdPanelIndex1;
  std::string m_Rich2HpdN2EnclName0;
  std::string  m_Rich2HpdN2EnclName1;

  std::string m_Rich2HpdPanelName0;
  std::string m_Rich2HpdPanelName1;
 


};
#endif // GAUSSRICH_RICHG4TRANSFORMPHDET_H
