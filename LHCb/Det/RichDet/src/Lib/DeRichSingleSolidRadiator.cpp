
/** @file DeRichSingleSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichSingleSolidRadiator
 *
 *  $Id: DeRichSingleSolidRadiator.cpp,v 1.28 2009-11-11 17:27:30 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// MathCore files
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

/// Detector description classes
#include "DetDesc/SolidBoolean.h"

//----------------------------------------------------------------------------

const CLID CLID_DeRichSingleSolidRadiator = 12040;  // User defined

// Standard Constructor
DeRichSingleSolidRadiator::DeRichSingleSolidRadiator(const std::string & name)
  : DeRichRadiator(name) {}

// Standard Destructor
DeRichSingleSolidRadiator::~DeRichSingleSolidRadiator() { }

// Retrieve Pointer to class defininition structure
const CLID& DeRichSingleSolidRadiator::classID()
{
  return CLID_DeRichSingleSolidRadiator;
}

StatusCode DeRichSingleSolidRadiator::initialize()
{
  const StatusCode initSC =  DeRichRadiator::initialize();
  if ( initSC.isFailure() ) return initSC;

  debug() << "Starting initialisation for DeRichSingleSolidRadiator "
          << name() << endmsg;

  m_solid = geometry()->lvolume()->solid();

  m_material = geometry()->lvolume()->material();
  const Material::Tables& myTabProp = m_material->tabulatedProperties();
  Material::Tables::const_iterator matIter;
  for ( matIter = myTabProp.begin(); matIter!=myTabProp.end(); ++matIter )
  {
    if ( (*matIter) )
    {
      if ( (*matIter)->type() == "RINDEX" )
      {
        m_refIndexTabProp = (*matIter);
        debug() << "Found TabProp " << m_refIndexTabProp->name() << " type "
                << m_refIndexTabProp->type() << endmsg;
      }
      if ( (*matIter)->type() == "RAYLEIGH" )
      {
        m_rayleighTabProp = (*matIter);
        debug() << "Found TabProp " << m_rayleighTabProp->name() << " type "
                << m_rayleighTabProp->type() << endmsg;
      }
      if ( (*matIter)->type() == "CKVRNDX" )
      {
        m_chkvRefIndexTabProp = (*matIter);
        debug() << "Found TabProp " << m_chkvRefIndexTabProp->name() << " type "
                << m_chkvRefIndexTabProp->type() << endmsg;
      }
      if ( (*matIter)->type() == "ABSLENGTH" )
      {
        m_absorptionTabProp = (*matIter);
        debug() << "Found TabProp " << m_absorptionTabProp->name() << " type "
                << m_absorptionTabProp->type() << endmsg;
      }
    }
  }

  if (!m_refIndexTabProp)
  {
    error() << "Radiator " << name() << " without refractive index"
            << endmsg;
    return StatusCode::FAILURE;
  }

  const Gaudi::XYZPoint zero(0.0, 0.0, 0.0);
  debug() <<" Centre " << geometry()->toGlobal(zero) << endmsg;

  return initSC;
}

//=========================================================================
// prepareMomentumVector
//=========================================================================
StatusCode DeRichSingleSolidRadiator::
prepareMomentumVector ( std::vector<double>& photonMomentumVect,
                        double min,
                        double max,
                        unsigned int nbins) const
{

  // check parameters are sane
  if( max <= min ) {
    error() << "Inadmissible photon energy limits "
            << max << " " << min << endmsg;
    return StatusCode::FAILURE;
  }
  if ( nbins <= 0  ) {
    error() << "Inadimissible photon energy num bins "
            << nbins << endmsg;
    return StatusCode::FAILURE;
  }

  photonMomentumVect.reserve( nbins );

  // fill momentum vector
  double photonEnergyStep = ( max - min )/ nbins;
  for ( unsigned int ibin = 0; ibin<nbins; ++ibin )
  {
    photonMomentumVect.push_back( min + photonEnergyStep*ibin );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  nextIntersectionPoint
//=========================================================================
StatusCode
DeRichSingleSolidRadiator::nextIntersectionPoint( const Gaudi::XYZPoint&  pGlobal,
                                                  const Gaudi::XYZVector& vGlobal,
                                                  Gaudi::XYZPoint& returnPoint ) const
{
  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks) {
    return StatusCode::FAILURE;
  }

  returnPoint = geometry()->toGlobal( pLocal + ticks[0] * vLocal );
  return StatusCode::SUCCESS;
}

//=========================================================================
// intersectionPoints
//=========================================================================
StatusCode
DeRichSingleSolidRadiator::intersectionPoints( const Gaudi::XYZPoint&  position,
                                               const Gaudi::XYZVector& direction,
                                               Gaudi::XYZPoint& entryPoint,
                                               Gaudi::XYZPoint& exitPoint ) const
{
  const Gaudi::XYZPoint pLocal( geometry()->toLocal(position) );
  Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*direction );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks)  return StatusCode::FAILURE;

  entryPoint = geometry()->toGlobal( pLocal + ticks[0] * vLocal );
  exitPoint  = geometry()->toGlobal( pLocal + ticks[noTicks-1] * vLocal );
  return StatusCode::SUCCESS;
}


//=========================================================================
//
//=========================================================================
unsigned int
DeRichSingleSolidRadiator::intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                               const Gaudi::XYZVector& vGlobal,
                                               std::vector<Gaudi::XYZPoint>&
                                               points) const
{
  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (noTicks != 0) {
    for (ISolid::Ticks::iterator tick_it = ticks.begin();
         tick_it != ticks.end();
         ++tick_it) {
      points.push_back( geometry()->toGlobal( pLocal + (*tick_it) * vLocal) );
    }
  }
  return noTicks;
}


//=========================================================================
//  intersections
//=========================================================================
unsigned int DeRichSingleSolidRadiator::
intersections( const Gaudi::XYZPoint& pGlobal,
               const Gaudi::XYZVector& vGlobal,
               std::vector<RichRadIntersection>& intersections ) const
{
  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  const Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (noTicks != 0) {
    for ( unsigned int tick=0; tick<noTicks; tick += 2 ) {
      intersections.push_back(RichRadIntersection
                              (geometry()->toGlobal(pLocal + ticks[tick]*vLocal),
                               vGlobal,
                               geometry()->toGlobal(pLocal + ticks[tick+1]*vLocal),
                               vGlobal,
                               this ) );
    }
  }
  return (noTicks/2);
}

//=========================================================================
// Refractive Index
//=========================================================================
double
DeRichSingleSolidRadiator::refractiveIndex( const double energy, bool hlt ) const
{
  return (*(this->refIndex(hlt)))[energy*Gaudi::Units::eV];
}
