// $Id: RichLocalTargetConfig.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef TARGETDATA_RICHLOCALTARGETCONFIG_H 
#define TARGETDATA_RICHLOCALTARGETCONFIG_H 1

// Include files
// from Gaudi
#include <TRandom.h>

#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichLocalTargetConfig.h"            // Interface


/** @class RichLocalTargetConfig RichLocalTargetConfig.h TargetData/RichLocalTargetConfig.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-13
 */
class RichLocalTargetConfig : public RichRingRecToolBase, 
                              virtual public IRichLocalTargetConfig {
public: 
  /// Standard constructor
  RichLocalTargetConfig( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);


  StatusCode SelectHitsInFoi(int itk, int irad , int iRich );

  void SetRadiatorParam(int irad);

  StatusCode InitTargetConfigParam();

  VD CartToPolNumFromRP(double R, double P);

  void resetTargetHitLists(int iRich);

  StatusCode ConfigureTargetHitsInFoiForFF(int itk, int irad , int iRich );
  

  VD   RpnTarget()  {  return m_RpnTarget;}
  VD   PpnTarget()  {  return m_PpnTarget;}
    
  double GetScaleFromLog(VI aVec );
  

  StatusCode   EstimateMeanRadiusFromRawRadius( int itk, int irad , int iRich );
  
  virtual ~RichLocalTargetConfig( ); ///< Destructor

protected:

private:

  double m_rMin;   // changed for each track.
  double m_rMax;   // changed for each radiator.
  double m_rRatio; // changed for each track.

  double m_rMinNominal;
  double m_rMinNominalR2gLowP;
  double m_rMinNominalR2gHighP;
  double m_rMinNominalR1gLowP;
  double m_rMinNominalR1gHighP;
  double m_rMinNominalAgel;
  double m_rMin_Shift;


  double m_Maxradius_SatAgel;  
  double m_Maxradius_SatR1g;
  double m_Maxradius_SatR2g;
  VD m_rMax_Radiator;
  

  VB m_HitIsSelected;
  

  // constants copied

  double m_cMinTheta;  
  double m_cMaxTheta;
  int m_cNumW ;
  int m_cNumR;
  
  // local track arrays
  VD m_XHitTarget;
  VD m_YHitTarget;
  VD m_RHitTarget;
  VD m_ThHitTarget;
  VD m_RpnTarget;
  VD m_PpnTarget;
  int m_NumHitsSelectedForATrack;

  TRandom * m_RandomNumSeedForLocalPhi;
  
 
};
#endif // TARGETDATA_RICHLOCALTARGETCONFIG_H
