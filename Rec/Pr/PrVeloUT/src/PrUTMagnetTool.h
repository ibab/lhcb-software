// $Id: PrUTMagnetTool.h,v 1.5 2009-10-30 13:20:50 wouter Exp $ 
#ifndef PRUTMAGNETTOOL_H
#define PRUTMAGNETTOOL_H 1


// Include files
#include "GaudiAlg/GaudiTool.h"

// Forward declarations
class IMagneticFieldSvc;
class PrTableForFunction;
#include "STDet/DeSTDetector.h"


static const InterfaceID IID_PrUTMagnetTool ( "PrUTMagnetTool", 1, 0 );



  /** @class PrUTMagnetTool PrUTMagnetTool.h newtool/PrUTMagnetTool.h
   *
   *  Magnet tool for Pr
   *
   *  @author Mariusz Witek
   *  @date   2006-09-25
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

class PrUTMagnetTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PrUTMagnetTool; }

    /// Standard constructor
    PrUTMagnetTool( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    /// Standard Destructor
    virtual ~PrUTMagnetTool();

    virtual StatusCode initialize();

    /// Actual operator function
    float bdlIntegral(float ySlopeVelo,float zOrigin,float zVelo);
    float zBdlMiddle(float ySlopeVelo,float zOrigin,float zVelo);
    float dist2mom(float ySlope);
    void dxNormFactorsUT(float ySlope, std::vector<float>& nfact);

    float zMidUT();
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
    IMagneticFieldSvc* m_magFieldSvc;
    PrTableForFunction* m_lutBdl;
    PrTableForFunction* m_lutZHalfBdl;

    float m_zCenterUT;
    float m_zMidField;

    float m_dist2mom;
    PrTableForFunction* m_lutDxLay;
    PrTableForFunction* m_lutDxToMom;

    std::vector<float> m_lutVar; // auxiliary vector used as argument

    DeSTDetector*       m_STDet;         ///< ST Detector element
    std::string m_detLocation;

    bool m_noField;
    float m_bdlIntegral_NoB;
    float m_zBdlMiddle_NoB;
    float m_zMidField_NoB;
    float m_averageDist2mom_NoB;

  };


#endif // PRUTMAGNETTOOL_H
