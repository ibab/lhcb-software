
//----------------------------------------------------------------------------
/** @file DeRichGasRadiator.h
 *
 *  Header file for detector description class : DeRichGasRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichGasRadiator.h,v 1.8 2009-12-10 16:47:42 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-03-02
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHGASRADIATOR_H
#define RICHDET_DERICHGASRADIATOR_H 1

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"

// External declarations
extern const CLID CLID_DeRichGasRadiator;

/** @class DeRichGasRadiator DeRichGasRadiator.h RichDet/DeRichGasRadiator.h
 *
 *  Detector element for gaseous radiator volumes
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-02
 */
class DeRichGasRadiator : public DeRichSingleSolidRadiator
{

public:

  /// Standard constructor
  DeRichGasRadiator( const std::string & name = "" );

  virtual ~DeRichGasRadiator( ); ///< Destructor

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

protected:

private:

  /// method to update the refractive index of the radiator
  StatusCode updateProperties();

  /// method to update the refractive index of the radiator for the HLT
  StatusCode updateHltProperties();

  /**
   * Generates and returns the refractive index of the radiator for use by the HLT
   * @return A pointer to the HLT refractive index interpolated function of the radiator
   * @retval NULL No interpolation function
   */
  virtual const Rich::TabulatedProperty1D* generateHltRefIndex() const;

  /// method for the calculation of the refractive index from the Sellmeir
  /// coeficients and update of the Tabulated Property
  StatusCode calcSellmeirRefIndex (const std::vector<double>& momVect,
                                   const TabulatedProperty* tabProp,
                                   SmartRef<Condition> gasParamCond ) const;

  /// method to use the old separate temperature and pressure conditions
  StatusCode setupOldGasConditions();

  // data

  /// Condition holding the current temperature of radiator
  SmartRef<Condition> m_temperatureCond;

  /// Condition holding the current pressure of radiator
  SmartRef<Condition> m_pressureCond;

  /// Condition holding the current pressure of radiator for use at the HLT
  SmartRef<Condition> m_gasParametersCond;

  /// Condition holding the current pressure of radiator for use at the HLT
  SmartRef<Condition> m_hltGasParametersCond;

  /// Condition holding the scale factor for the refractivity
  /// This conditions scales n-1 NOT n
  SmartRef<Condition> m_scaleFactorCond;
};
#endif // RICHDET_DERICHGASRADIATOR_H
