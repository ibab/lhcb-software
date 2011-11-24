/** @file DeRich.h
 *
 *  Header file for detector description class : DeRich
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICH_H
#define RICHDET_DERICH_H 1

// STL
#include <vector>

// DetDesc
#include "RichDet/Rich1DTabProperty.h"

// Mathcore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// LHCbKernel
#include "Kernel/RichSide.h"

// Local
#include "RichDet/DeRichBase.h"
#include "RichDet/RichMirrorSegPosition.h"
#include "RichDet/DeRichLocations.h"
#include "RichDet/RichDetConfigType.h"

class DeRichPDPanel;

/** @class DeRich DeRich.h
 *
 * Class for generic info about the Rich(1/2) system.
 * DeRich is an abstract class contaning the common parts of the DeRich1
 * and DeRich2 classes.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
class DeRich: public DeRichBase
{

public:
  /**
   * Constructor for this class
   */
  DeRich( const std::string & name = "" );

  /**
   * Default destructor
   */
  virtual ~DeRich();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Returns the nominal centre of curvature of the spherical mirror for
   * this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal centre of curvature
   */
  virtual const Gaudi::XYZPoint& nominalCentreOfCurvature(const Rich::Side side) const = 0;

  /**
   * Returns the nominal normal vector of the flat mirror plane for this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal normal vector
   */
  virtual const Gaudi::XYZVector& nominalNormal(const Rich::Side side) const = 0;

  /**
   * Returns the nominal flat mirror plane for this Rich
   *
   * @param side Which side: top, bottom (Rich1), left, right (Rich2)
   * @return The nominal flat mirror plane
   */
  virtual const Gaudi::Plane3D& nominalPlane(const Rich::Side side) const = 0;

  /**
   * Check on which side of this Rich lies this point
   *
   * @param point A point in the global coordinate system
   * @return The side for this point
   */
  virtual Rich::Side side( const Gaudi::XYZPoint& point ) const = 0;

  /**
   * Returns the nominal spherical mirror radius for this Rich
   *
   * @return The nominal spherical mirror radius
   */
  inline double sphMirrorRadius() const
  {
    return m_sphMirrorRadius;
  }

  /// Returns the Photon Detector config type
  inline Rich::RichPhDetConfigType RichPhotoDetConfig() const
  {
    return m_RichPhotoDetConfig;
  }

  /**
   * Returns a pointer to the tabulated property that holds the refractive
   * index for the gas window for this Rich
   *
   * @return Pointer to gas window refIndex
   */
  inline const RichTabulatedProperty1D* gasWinRefIndex() const
  {
    return m_gasWinRefIndex;
  }

  /**
   * Returns a pointer to the tabulated property that holds the absorption
   * length of the gas window for this Rich
   *
   * @return Pointer gas window absorption length
   */
  inline const RichTabulatedProperty1D* gasWinAbsLength() const
  {
    return m_gasWinAbsLength;
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal quantum
   * efficiency of an Photodetector
   *
   * @return Pointer to quantum efficiency (can be null)
   */
  inline const RichTabulatedProperty1D * nominalPDQuantumEff() const
  {
    if ( !m_nominalPDQuantumEff ) { loadNominalQuantumEff(); }
    return m_nominalPDQuantumEff;
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal reflectivity
   * of the spherical mirror.
   *
   * @return Pointer to nominal spherical mirror reflectivity
   */
  inline const RichTabulatedProperty1D* nominalSphMirrorRefl() const
  {
    return m_nominalSphMirrorRefl;
  }

  /**
   * Returns a pointer to the tabulated property that holds the nominal reflectivity
   * of the flat mirror.
   *
   * @return Pointer to nominal flat mirror reflectivity
   */
  inline const RichTabulatedProperty1D* nominalSecMirrorRefl() const
  {
    return m_nominalSecMirrorRefl;
  }

  /**
   * Method to find the row/column of a spherical mirror segment.  It can
   * be used to test if the mirror segment is at the edge or not
   * @return Position (row/column) for this spherical mirror segment
   */
  virtual RichMirrorSegPosition sphMirrorSegPos( const int mirrorNumber ) const;

  /**
   * Method to find the row/column of a flat mirror segment. It can be used to
   * test if the mirror segment is at the edge or not
   *
   * @return Position (row/column) for this flat mirror segment
   */
  virtual RichMirrorSegPosition secMirrorSegPos( const int mirrorNumber ) const;

  virtual StatusCode alignSphMirrors() = 0;
  virtual StatusCode alignSecMirrors() = 0;

  /// sensitive volume identifier for hpd version. to be phased out
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const;

protected:

  /// (Mis)align the mirrors
  StatusCode alignMirrors( const std::vector<const ILVolume*>& mirrorContainers,
                           const std::string& mirrorID,
                           const SmartRef<Condition>& mirrorAlignCond,
                           const std::string& Rvector ) const;

public:

  /// Access PD Panels on demand
  DeRichPDPanel * pdPanel( const Rich::Side panel ) const;

private:

  /// Access the name for a given panel
  virtual const std::string panelName( const Rich::Side panel ) const;

  /// Load on demand the nominal HPD Q.E.
  const Rich::TabulatedProperty1D * loadNominalHPDQuantumEff() const;

  /// Load on demand the nominal PMT Q.E.
  const Rich::TabulatedProperty1D * loadNominalPMTQuantumEff() const;

  /// Load on demand the nominal PD Q.E.
  void loadNominalQuantumEff() const;

protected:

  /// The nominal radius of the spherical mirror
  double m_sphMirrorRadius;

  // RICH PhotoDetector Configuration
  Rich::RichPhDetConfigType m_RichPhotoDetConfig;

  /// refractive index of the quartz gas window
  const Rich::TabulatedProperty1D* m_gasWinRefIndex;

  /// absorption length of the quartz gas window
  const Rich::TabulatedProperty1D* m_gasWinAbsLength;

  /// spherical mirror reflectivity
  const Rich::TabulatedProperty1D* m_nominalSphMirrorRefl;

  /// flat mirror reflectivity
  const Rich::TabulatedProperty1D* m_nominalSecMirrorRefl;

private: // data

  /// Pointers to the PD panels of this Rich detector
  mutable std::vector<DeRichPDPanel*> m_PDPanels;

  /// flag to test if the xml supports mirror position info
  bool m_positionInfo;

  int m_sphMirrorSegRows;  ///< number of spherical mirror rows
  int m_sphMirrorSegCols;  ///< number of spherical mirror columns
  int m_secMirrorSegRows;  ///< number of secondary mirror rows
  int m_secMirrorSegCols;  ///< number of secondary mirror columns

  /// PD quantum efficiency
  mutable const Rich::TabulatedProperty1D * m_nominalPDQuantumEff;

};

#endif    // RICHDET_DERICH_H
