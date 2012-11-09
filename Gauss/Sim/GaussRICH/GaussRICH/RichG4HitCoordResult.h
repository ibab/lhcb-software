// $Id: RichG4HitCoordResult.h,v 1.2 2008-05-30 13:43:23 gcorti Exp $
#ifndef GAUSSRICH_RICHG4HITCOORDRESULT_H 
#define GAUSSRICH_RICHG4HITCOORDRESULT_H 1

// Include files
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "GaudiKernel/Transform3DTypes.h"
  

/** @class RichG4HitCoordResult RichG4HitCoordResult.h RichAnalysis/RichG4HitCoordResult.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2006-02-17
 */
class RichG4HitCoordResult {
public: 
  /// Standard constructor
  RichG4HitCoordResult( ); 

  virtual ~RichG4HitCoordResult( ); ///< Destructor

  int getradiatorNum() {return m_radiatorNum ;}
  void setradiatorNum(int ar ){ m_radiatorNum= ar;}
  Gaudi::XYZPoint DetPtFromPixelNum()
  {return m_DetPtFromPixelNum;}
  Gaudi::XYZPoint  DetPtFromTrueLocalHit() 
  {
    return m_DetPtFromTrueLocalHit;
  }
  Gaudi::XYZPoint DetPtFromGlobalPhCathode()
  {
    return m_DetPtFromGlobalPhCathode;
  }
  Gaudi::XYZPoint DetPtInPhDetFromPixelNum()
  {
    return m_DetPtInPhDetFromPixelNum;
  }
  Gaudi::XYZPoint DetPtInPhDetFromTrueLocalHit()
  {
   return  m_DetPtInPhDetFromTrueLocalHit;
  }
  Gaudi::XYZPoint DetPtInPhDetFromGlobalPhCathode()
  {return m_DetPtInPhDetFromGlobalPhCathode;}
  
  void setDetPtFromPixelNum( Gaudi::XYZPoint ap1 ) 
  {m_DetPtFromPixelNum = ap1; 
  }
  void setDetPtFromTrueLocalHit(Gaudi::XYZPoint ap2 )
  {m_DetPtFromTrueLocalHit= ap2;
  }
  void setDetPtFromGlobalPhCathode(Gaudi::XYZPoint ap3 )
  {m_DetPtFromGlobalPhCathode= ap3;
  }
  void  setDetPtInPhDetFromPixelNum(Gaudi::XYZPoint ap4 )
  {m_DetPtInPhDetFromPixelNum=ap4;
  }
  void setDetPtInPhDetFromTrueLocalHit(Gaudi::XYZPoint ap5 )
  {m_DetPtInPhDetFromTrueLocalHit=ap5;
  }
  void setDetPtInPhDetFromGlobalPhCathode(Gaudi::XYZPoint ap6 )
  {m_DetPtInPhDetFromGlobalPhCathode=ap6;
  }
  
    
    
    
protected:

private:

  int m_radiatorNum ;
  Gaudi::XYZPoint m_DetPtFromPixelNum;
  Gaudi::XYZPoint m_DetPtFromTrueLocalHit;
  Gaudi::XYZPoint m_DetPtFromGlobalPhCathode;
  

  Gaudi::XYZPoint m_DetPtInPhDetFromPixelNum;
  Gaudi::XYZPoint m_DetPtInPhDetFromTrueLocalHit;
  Gaudi::XYZPoint m_DetPtInPhDetFromGlobalPhCathode;
  

};
#endif // GAUSSRICH_RICHG4HITCOORDRESULT_H
