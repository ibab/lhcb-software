// $Id: ParticleTaggerCriterion.h,v 1.1.1.1 2002-05-23 23:25:50 gcorti Exp $
#ifndef PARTICLETAGGERCRITERION_H 
#define PARTICLETAGGERCRITERION_H 1

// Include files
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "DaVinciTools/IGeomDispCalculator.h"
#include "DaVinciTools/IFilterCriterion.h"

/** @class ParticleTaggerCriterion ParticleTaggerCriterion.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   14/03/2002
 */
class ParticleTaggerCriterion : public AlgTool, virtual public IFilterCriterion
{
public:
  /// Standard constructor
  ParticleTaggerCriterion( const std::string &type,
                           const std::string &name,
                           const IInterface *parent );

  virtual ~ParticleTaggerCriterion( ) {}; ///< Destructor

  StatusCode initialize();

  /// Test if filter is satisfied
  bool isSatisfied( const Particle * const &part );
  bool operator()( const Particle * const &part );

private:

  IParticlePropertySvc *m_ppSvc;

  /// Properties
  std::string m_ppSvcName;    //< Name of the particle property service to use
  std::string m_PartName;     //< Name of the particle to keep
  int m_PartCode;             //< ID of the particle to keep
  double m_CLMin;             //< Minmal confidence level of the id
  double m_CLMax;             //< Maximal confidence level of the id
  double m_PtMin;             //< Minimal transverse momentum
  double m_PtMax;             //< Maximal transverse momentum
  double m_PMin;              //< Minimal momentum
  double m_PMax;              //< Maximal momentum
  double m_ZMin;              //< Minimal Z position of first point on track
  double m_ZMax;              //< Maximal Z position of first point on track
};
#endif // PARTICLETAGGERCRITERION_H
