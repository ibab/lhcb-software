// $Id: RichRingRecTransformTool.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGRECTRANSFORMTOOL_H 
#define RICHRINGRECTRANSFORMTOOL_H 1

// Include files
// from Gaudi
#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichRingRecTransformTool.h"            // Interface


/** @class RichRingRecTransformTool RichRingRecTransformTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-12
 */
class RichRingRecTransformTool : public RichRingRecToolBase, 
                                 virtual public IRichRingRecTransformTool {
public: 
  /// Standard constructor
  RichRingRecTransformTool( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

  virtual ~RichRingRecTransformTool( ); ///< Destructor

  double CartToPolRad(double x, double y, double aXc, double aYc );
  double CartToPolPhi(double x, double y, double aXc, double aYc );
  double CartToPolRadFromTHNum(int aHitNum, int aTrackNum , int rad);
  double CartToPolPhiFromTHNum(int aHitNum, int aTrackNum , int rad);
  double getYHitCoord(int aHitNum, int aTrackNum , int rad, int iRich);
  int  SearchIndexInVect(VD aVect, double aVal);
  double XDiffValue(int aHitNum, int aTrackNum, int  rad, int iRich);
  double YDiffValue(int aHitNum, int aTrackNum, int  rad, int iRich);
  VD GetCorrelation (VD aTar, VD aTemp);

  VI FindPeakInRP(VVD aPolVec, int rad);  
  VI getMeanPeakPolarInvA(VVD aPolVec, double aThrValue );
  


  int RichDetNumFromRadiator(int rad);
  int RichDetNumFromHitZ(double GlobalZHit);
  void InitRichRingRecTransformTool();
  VI getTrackIndexLimits( int irad) ;  

  void setRadiusToBGParam();
  void setBGToRadiusParam();
  void setRadiusFitLimits ();
  double  getBGFromRadius(int irad, double aRadius);
  double getRadiusFromBG(int irad, double aBG);
  double TrackExpectedRadiusValue(int itk, int irad, int PartType);
  double TrackExpectedRadiusFromTruePidCode(int itk, int irad, 
                                               int aTruePidCode);
  double getTrackMassFromRadius(int itk, int irad, double aRadius );
  VD TrackExpectedRadii(int itk, int irad);
  bool CheckBGThreshold( double aMass, int itk, int irad);
  double  MaxRadiusFit( int irad);
  
protected:

private:

  // local variables

  int m_NumPeakSearch;
 
  
  VD m_AgelRadiusToBGParam;
  VD m_AgelBGToRadiusParam;
  
  VD m_R1gRadiusToBGParam;
  VD m_R1gBGToRadiusParam;
  
  VD m_R2gRadiusToBGParam;
  VD m_R2gBGToRadiusParam;
  
  
  double  m_AgelMinRadiusFit;
  double  m_AgelMaxRadiusFit;
  double  m_R1gMinRadiusFit;
  double  m_R1gMaxRadiusFit;
  double  m_R2gMinRadiusFit;
  double  m_R2gMaxRadiusFit;
  double   m_AgelMinBGFit;
  double   m_AgelMaxBGFit;
  double   m_R1gMinBGFit;
  double   m_R1gMaxBGFit;
  double   m_R2gMinBGFit;
  double   m_R2gMaxBGFit;
  VD m_MinBGFit;
  
  double   m_AgelBGTolerence;
  double   m_R1gBGTolerence;
  double   m_R2gBGTolerence;
  
 
};
#endif // RICHRINGRECTRANSFORMTOOL_H
