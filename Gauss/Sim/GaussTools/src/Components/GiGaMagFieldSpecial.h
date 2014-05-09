#ifndef GIGA_GIGAMagFieldSpecial_H
#define GIGA_GIGAMagFieldSpecial_H 1

// GiGa
#include "GiGa/GiGaMagFieldBase.h"

/** @class GiGaMagFieldSpecial  GiGaMagFieldSpecial.h
 *
 *  Various magnetic fields for simulation of LHC magnets.
 *
*/

class GiGaMagFieldSpecial : public GiGaMagFieldBase {

 public:
  /** standard constructor
   *  @see GiGaMagFieldBase
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaMagFieldSpecial(const std::string& type, const std::string& name,
                      const IInterface* parent);

  /// Destructor
  virtual ~GiGaMagFieldSpecial();

  /** get the field value
   *  @see G4MagneticField
   *  @param point point
   *  @param B   value of magnetic filed in poitnt
   */
  virtual void GetFieldValue(const double partvec[4], double* B) const;

 private:
  GiGaMagFieldSpecial();                            ///< no default constructor!
  GiGaMagFieldSpecial(const GiGaMagFieldSpecial&);  ///< no copy
  GiGaMagFieldSpecial& operator=(const GiGaMagFieldSpecial&);  ///< no =

 private:
  double m_ByUniform, m_QuadGrad, m_LHCbFieldMax;
  enum FieldType {
    UniformInvertZ = 0,
    QuadGrad,
    LHCbDown,         // LHCb: mag DOWN By < 0
    LHCbUp,           // LHCb: mag UP By > 0
    QuadGradInvertZ,  // use negative sign for negative z; added at end to not
                      // interfere with existing options
  };

  int m_Type;
};

#endif  // GIGA_GIGAMagFieldSpecial_H
