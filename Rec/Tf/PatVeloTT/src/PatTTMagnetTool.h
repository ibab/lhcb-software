// $Id: PatTTMagnetTool.h,v 1.5 2009-10-30 13:20:50 wouter Exp $ 
#ifndef PATTTMAGNETTOOL_H
#define PATTTMAGNETTOOL_H 1


// Include files
#include "GaudiAlg/GaudiTool.h"
#include <boost/optional/optional_io.hpp>

// Forward declarations
class ILHCbMagnetSvc;
class PatTableForFunction;
#include "STDet/DeSTDetector.h"


static const InterfaceID IID_PatTTMagnetTool ( "PatTTMagnetTool", 1, 0 );



  /** @class PatTTMagnetTool PatTTMagnetTool.h newtool/PatTTMagnetTool.h
   *
   *  Magnet tool for Pat
   *
   *  @author Mariusz Witek
   *  @date   2006-09-25
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

class PatTTMagnetTool : public GaudiTool {
public:
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatTTMagnetTool; }
  
  /// Standard constructor
  PatTTMagnetTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);
  
  /// Standard Destructor
  virtual ~PatTTMagnetTool();
  
  virtual StatusCode initialize();
  
  /// Actual operator function
  float bdlIntegral(float ySlopeVelo,float zOrigin,float zVelo);
  float zBdlMiddle(float ySlopeVelo,float zOrigin,float zVelo);
  float dist2mom(float ySlope);
  void dxNormFactorsTT(float ySlope, std::vector<float>& nfact);
  void dxNormFactorsTT(float ySlope, std::array<float,4>& nfact);
  
  
  float zMidTT();
  float zMidField();
  float averageDist2mom();
  void prepareBdlTables();
  void prepareDeflectionTables();
  StatusCode updateField() ;
  
protected:
  void f_bdl(float slopeY, float zOrigin , float zStart, float zStop);
  
private:
  
  float m_BdlTrack;
  float m_zHalfBdlTrack;
  std::vector<float> m_zLayers;
  
  std::vector<float> m_bdlTmp, m_zTmp;
  
  /// pointer to mag field service
  ILHCbMagnetSvc* m_magFieldSvc;
  PatTableForFunction* m_lutBdl;
  PatTableForFunction* m_lutZHalfBdl;
  
  float m_zCenterTT;
  float m_zMidField;
  
  float m_dist2mom;
  PatTableForFunction* m_lutDxLay;
  PatTableForFunction* m_lutDxToMom;
  
  std::vector<float> m_lutVar; // auxiliary vector used as argument
  
  DeSTDetector*       m_STDet;         ///< ST Detector element
  std::string m_detLocation;
  
  bool m_noField;
  float m_bdlIntegral_NoB;
  float m_zBdlMiddle_NoB;
  float m_zMidField_NoB;
  float m_averageDist2mom_NoB;
  boost::optional<double> m_current;
  
};


#endif // PATTTMAGNETTOOL_H
