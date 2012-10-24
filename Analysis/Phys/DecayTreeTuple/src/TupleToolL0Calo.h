// $Id: TupleToolL0Calo.h, v1 2012-10-11$
#ifndef TUPLETOOLL0CALO_H
#define TUPLETOOLL0CALO_H 1

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

class IPart2Calo;

  // HCAL constants
  static const double HCAL_CellSize_Inner = 131.3;
  static const double HCAL_CellSize_Outer = 262.6;
  static const double HCAL_xMax_Inner = 2101;
  static const double HCAL_yMax_Inner = 1838;
  static const double HCAL_xMax_Outer = 4202;
  static const double HCAL_yMax_Outer = 3414;

  // ECAL constants
  static const double ECAL_CellSize_Inner = 40.4;
  static const double ECAL_CellSize_Middle = 60.6;
  static const double ECAL_CellSize_Outer = 121.2;
  static const double ECAL_xMax_Inner = 970;
  static const double ECAL_yMax_Inner = 725;
  static const double ECAL_xMax_Middle = 1940;
  static const double ECAL_yMax_Middle = 1210;
  static const double ECAL_xMax_Outer = 3880;
  static const double ECAL_yMax_Outer = 3150;


/** @class TupleToolL0Calo TupleToolL0Calo.h
 *
 * \brief Calorimeter information to compute L0 efficiencies
 *
 * The particle is projected on the selected calorimeter surface, and the projection information is retrieved.
 * Tuple columns:
 * - trueET: ET of the particle when arrived on the calorimeter
 * - xProjection: X dimension of the particle projection on the calorimeter
 * - yProjection: Y dimension of the particle projection on the calorimeter
 * - region: calorimeter region corresponding to the particle projection
 *    -1 = outside of calorimeter,
 *    HCAL: 0 = outer, 1 = inner
 *    ECAL: 0 = outer, 1 = middle, 2 = inner
 *
 *  @author Alexandra Martin Sanchez
 *  @date   2012-10-11
 */

class TupleToolL0Calo : public TupleToolBase,
                        virtual public IParticleTupleTool 
{

public:

  /// Standard constructor
  TupleToolL0Calo( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~TupleToolL0Calo( ){}; ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode fill( const LHCb::Particle* mother,
                           const LHCb::Particle* P,
                           const std::string& head,
                           Tuples::Tuple& tuple);

private :

  int isinside_HCAL(double x,
                    double y);
  int isinside_ECAL(double x,
                    double y);

  IPart2Calo* m_part2calo;
  std::string m_calo;

};
#endif // TUPLETOOLL0CALO_H
