// $Id: $
#ifndef RICHDET_DERICHPMT_H
#define RICHDET_DERICHPMT_H 1

// Include files
// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IPVolume.h"

// LHCbKernel
#include "Kernel/ILHCbMagnetSvc.h"

// RichDet
#include "RichDet/DeRichPD.h"

// GSL
#include "gsl/gsl_math.h"

// STL
#include <cmath>

// External declarations
extern const CLID CLID_DERichPMT;

//=============================================================================
/** @class DeRichPMT DeRichPMT.h RichDet/DeRichPMT.h
 *
 *  PMT Photodetector
 *
 *  @author Sajan Easo
 *  @date   2011-10-10
 */
//=============================================================================

class DeRichPMT : public DeRichPD
{

public:

  /// Standard constructor
  DeRichPMT(const std::string & name = "" );

  virtual ~DeRichPMT( ); ///< Destructor

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  // @brief Converts a RichSmartID to a point in global coordinates.
  virtual StatusCode detectionPoint ( const LHCb::RichSmartID smartID,
                                      Gaudi::XYZPoint& detectPoint,
                                      bool photoCathodeSide = false ) const;

  // Converts an x,y point from the anode to the photocathode in the
  // coordinate system of the PMT.
  virtual StatusCode detectionPoint ( const double fracPixelCol,
                                      const double fracPixelRow,
                                      Gaudi::XYZPoint& detectPoint,
                                      const bool photoCathodeSide = true ) const;

  /** Converts a RichSmartID to a point on the anode in global coordinates.
   *  @param[in] smartID The RichSmartID for the PMT channel
   *  @return The detection point on the anode in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode ( const LHCb::RichSmartID smartID ) const;

  void setPmtLensFlag(bool aflag)  {m_PmtLensFlag=aflag;}
  int pmtCopyNumber() {  return m_number;}
  bool PmtLensFlag()  {  return m_PmtLensFlag;}
  
  void setPmtIsGrandFlag(bool aflagG ) {m_PmtIsGrand  = aflagG ;}   
  bool PmtIsGrand()  {  return m_PmtIsGrand;}
    
private:

  StatusCode getPMTParameters();
  StatusCode initPMTQuantumEff();
  StatusCode updateGeometry();
  Gaudi::XYZPoint getAnodeHitCoordFromPixelNum( const double fracPixelCol,
                                                const double fracPixelRow ) const;

  Gaudi::XYZPoint getAnodeHitCoordFromGrandPixelNum( const double fracPixelCol,
                                                const double fracPixelRow ) const;

  Gaudi::XYZPoint getAnodeHitCoordFromMultTypePixelNum( const double fracPixelCol,
                                                        const double fracPixelRow,
                                                const LHCb::RichSmartID smartID  ) const;


  Gaudi::XYZPoint RichPmtLensReconFromPhCath(  const Gaudi::XYZPoint & aPhCathCoord  ) const ;

private:

  IDetectorElement* m_dePmtAnode = nullptr; ///< The PMT Anode detector element
  int m_number;                    ///<Pmt number (should be the same as copy number)
  std::string  m_PmtQELocation;

  double m_PmtAnodeXSize;
  double m_PmtAnodeYSize;
  double m_PmtAnodeZSize;

  double m_PmtAnodeLocationInPmt;
  double m_PmtPixelXSize;
  double m_PmtPixelYSize;
  double m_PmtPixelGap;
  double m_PmtEffectivePixelXSize;
  double m_PmtEffectivePixelYSize;
  double m_PmtAnodeHalfThickness;
  double m_PmtNumPixCol;
  double m_PmtNumPixRow;
  double m_PmtQwZSize;
  double m_QwToAnodeZDist ;
  bool m_PmtLensFlag{false};
  double m_PmtLensMagnificationRatio;
  double m_PmtLensRoc;
  
  double m_GrandPmtAnodeXSize;
  double m_GrandPmtAnodeYSize;
  double m_GrandPmtAnodeZSize;
  double m_GrandPmtPixelXSize;
  double m_GrandPmtPixelYSize;
  double m_GrandPmtPixelGap;
  double m_GrandPmtEdgePixelXSize;
  double m_GrandPmtEdgePixelYSize;
  double m_GrandPmtEffectivePixelXSize;
  double m_GrandPmtEffectivePixelYSize;
  double m_GrandPmtEdgePixelXDiff;
  double m_GrandPmtEdgePixelYDiff;
  double m_GrandPmtAnodeHalfThickness;
  bool m_Rich2UseGrandPmt{false};
  bool m_Rich2UseMixedPmt{false};
  double m_Rich1Rich2ZDivideLimit;
  int m_Rich2PmtArrayConfig;
  bool m_PmtIsGrand{false};
  
};

#endif // RICHDET_DERICHPMT_H
