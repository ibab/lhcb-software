
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.cpp
 *
 * Implementation file for class : RichTabulatedRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTabulatedRefractiveIndex.cpp,v 1.17 2009-04-17 11:07:27 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRefractiveIndex.h"

// From Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, TabulatedRefractiveIndex );

// Standard constructor
Rich::TabulatedRefractiveIndex::
TabulatedRefractiveIndex ( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_riches     ( Rich::NRiches ),
    m_detParams  ( NULL ),
    m_deRads     ( Rich::NRadiatorTypes    ),
    m_refRMS     ( Rich::NRadiatorTypes, 0 ),
    m_hltMode    ( false                   )
{
  // interface
  declareInterface<IRefractiveIndex>(this);
}

StatusCode Rich::TabulatedRefractiveIndex::initialize()
{
  // Initialise base class
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get tools
  acquireTool( "RichDetParameters", m_detParams );

  // Get the RICH radiators
  m_deRads[Rich::Aerogel]  = getDet<DeRichRadiator>( DeRichLocations::Aerogel  );
  m_deRads[Rich::Rich1Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich1Gas );
  m_deRads[Rich::Rich2Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich2Gas );

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocations::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocations::Rich2 );

  // Register dependencies on DeRichRadiator objects to UMS

  // aerogel
  updMgrSvc()->registerCondition( this,
                                  DeRichLocations::Aerogel,
                                  &TabulatedRefractiveIndex::updateAerogelRefIndex );

  // Rich1Gas
  updMgrSvc()->registerCondition( this,
                                  DeRichLocations::Rich1Gas,
                                  &TabulatedRefractiveIndex::updateRich1GasRefIndex );
  // Rich2Gas
  updMgrSvc()->registerCondition( this,
                                  DeRichLocations::Rich2Gas,
                                  &TabulatedRefractiveIndex::updateRich2GasRefIndex );

  // HLT mode or not ?
  m_hltMode = contextContains("HLT");

  // force first updates
  sc = updMgrSvc()->update(this);
  if (sc.isFailure()) return Error ( "Failed first UMS update", sc );

  return sc;
}

StatusCode Rich::TabulatedRefractiveIndex::updateAerogelRefIndex()
{
  return updateRefIndex(Rich::Aerogel);
}

StatusCode Rich::TabulatedRefractiveIndex::updateRich1GasRefIndex()
{
  return updateRefIndex(Rich::Rich1Gas);
}

StatusCode Rich::TabulatedRefractiveIndex::updateRich2GasRefIndex()
{
  return updateRefIndex(Rich::Rich2Gas);
}

StatusCode
Rich::TabulatedRefractiveIndex::updateRefIndex( const Rich::RadiatorType rad )
{
  /// @todo Figure out how to fix this hack properly ...

  // RMS values
  //m_refRMS[rad] = m_deRads[rad]->refIndex()->rms( m_deRads[rad]->refIndex()->minX(),
  //                                                m_deRads[rad]->refIndex()->maxX(),
  //                                                100 );
  // temp hack (to be removed)
  m_refRMS[Rich::Aerogel]  = 0.488e-3;
  m_refRMS[Rich::Rich1Gas] = 0.393e-4;
  m_refRMS[Rich::Rich2Gas] = 0.123e-4;
  debug() << "RMS hack : " << rad << " : " << m_refRMS[rad];

  return StatusCode::SUCCESS;
}

double Rich::TabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                        const double energy ) const
{
  return m_deRads[rad]->refractiveIndex(energy);
}

double Rich::TabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                        const double energyBot,
                                                        const double energyTop ) const
{
  const Rich::DetectorType rich = ( rad == Rich::Rich2Gas ? Rich::Rich2 : Rich::Rich1 );
  return refractiveIndex( rad,
                          m_riches[rich]->nominalHPDQuantumEff()->meanX(energyBot,energyTop) /
                          Gaudi::Units::eV );
}

double Rich::TabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad ) const
{
  return refractiveIndex( rad,
                          m_detParams->meanPhotonEnergy(rad) );

}

double
Rich::TabulatedRefractiveIndex::refractiveIndexRMS ( const Rich::RadiatorType rad ) const
{
  // CRJ : Need to find a better way to calculate this quantity
  return m_refRMS[rad];
}

double
Rich::TabulatedRefractiveIndex::
refractiveIndex ( const RichRadIntersection::Vector & intersections,
                  const double energy ) const
{
  // loop over all radiator intersections and calculate the weighted average
  // according to the path length in each radiator
  double refIndex(0), totPathL(0);
  for ( RichRadIntersection::Vector::const_iterator iR = intersections.begin();
        iR != intersections.end(); ++iR )
  {
    const double pLength = (*iR).pathLength();
    refIndex += pLength * (*iR).radiator()->refractiveIndex(energy,m_hltMode);
    totPathL += pLength;
  }
  return ( totPathL>0 ? refIndex/totPathL : refIndex );
}

double
Rich::TabulatedRefractiveIndex::
refractiveIndex ( const RichRadIntersection::Vector & intersections ) const
{
  // loop over all radiator intersections and calculate the weighted average
  // according to the path length in each radiator
  double refIndex(0), totPathL(0);
  for ( RichRadIntersection::Vector::const_iterator iR = intersections.begin();
        iR != intersections.end(); ++iR )
  {
    const double energy = m_detParams->meanPhotonEnergy((*iR).radiator()->radiatorID());
    const double pLength = (*iR).pathLength();
    refIndex += pLength * (*iR).radiator()->refractiveIndex(energy,m_hltMode);
    totPathL += pLength;
  }
  return ( totPathL>0 ? refIndex/totPathL : refIndex );
}
