
//----------------------------------------------------------------------------
/** @file DeRichGasRadiator.h
 *
 *  Header file for detector description class : DeRichGasRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichGasRadiator.h,v 1.4 2007-02-28 18:31:07 marcocle Exp $
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
  DeRichGasRadiator( );

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

  /// method for the calculation of the refractive index from the Sellmeir
  /// coeficients and update of the Tabulated Property
  StatusCode calcSellmeirRefIndex (const std::vector<double>& momVect,
                                   const TabulatedProperty* tabProp );

  /// Condition holding the current temperature of radiator
  SmartRef<Condition> m_temperatureCond;

  /// Condition holding the current pressure of radiator
  SmartRef<Condition> m_pressureCond;

};
#endif // RICHDET_DERICHGASRADIATOR_H
