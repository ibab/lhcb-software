
//----------------------------------------------------------------------------
/** @file DeRichAerogelRadiator.h
 *
 *  Header file for detector description class : DeRichAerogelRadiator
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-03-02
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHAEROGELRADIATOR_H
#define RICHDET_DERICHAEROGELRADIATOR_H 1

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"

/** @class DeRichAerogelRadiator RichDet/DeRichAerogelRadiator.h
 *
 *  Detector element for aerogel radiator volumes
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-02
 */
class DeRichAerogelRadiator : public DeRichSingleSolidRadiator 
{

public:

  /// Standard constructor
  DeRichAerogelRadiator( const std::string & name = "" );

  virtual ~DeRichAerogelRadiator( ); ///< Destructor

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  inline const CLID& clID() const
  {
    return classID();
  }

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

public:

  /** Access the Aerogel tile ID number
   *  @return The tile ID */
  inline int tileID() const { return m_tileNumber; }

private:

  /// method to update the refractive index of the radiator
  StatusCode updateProperties();

  /// method for the calculation of the refractive index from the Sellmeir
  /// coeficients and update of the Tabulated Property
  StatusCode calcSellmeirRefIndex (const std::vector<double>& momVect,
                                   const TabulatedProperty* tabProp );

  /// method for the calculation of Rayleigh scattering from
  /// clarity and update of the Tabulated Property
  StatusCode calcRayleigh(const std::vector<double>& momVect,
                          const TabulatedProperty* tabProp);

  /// method for the calculation of Absorption from const A
  /// and update of the Tabulated Property
  StatusCode calcAbsorption(const std::vector<double>& momVect,
                            const TabulatedProperty* tabProp);

  /// Access on demand the DeRich1 detector element
  DetectorElement* deRich1() const;

private:

  /// access to DeRich1 for user parameters
  mutable DetectorElement* m_deRich1;

  /// Condition holding the aerogel parameters
  SmartRef<Condition> m_AerogelCond;

  /// conertion from nm to eV
  double m_photMomWaveConv;

  /// Aerogel tile number
  int m_tileNumber;

};

#endif // RICHDET_DERICHAEROGELRADIATOR_H
