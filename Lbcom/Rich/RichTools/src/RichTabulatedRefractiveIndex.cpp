
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.cpp
 *
 * Implementation file for class : RichTabulatedRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTabulatedRefractiveIndex.cpp,v 1.3 2004-07-26 18:03:05 jonrob Exp $
 * $Log: not supported by cvs2svn $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRefractiveIndex.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedRefractiveIndex>          s_factory ;
const        IToolFactory& RichTabulatedRefractiveIndexFactory = s_factory ;

// Standard constructor
RichTabulatedRefractiveIndex::RichTabulatedRefractiveIndex ( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
  : RichToolBase ( type, name, parent )
{

  declareInterface<IRichRefractiveIndex>(this);

  // Define job option parameters
  m_refLocations.push_back("/dd/Materials/RichMaterialTabProperties/AerogelIdealRIndexPT");
  m_refLocations.push_back("/dd/Materials/RichMaterialTabProperties/C4F10SellParamRIndexPT");
  m_refLocations.push_back("/dd/Materials/RichMaterialTabProperties/CF4RIndexPT");
  declareProperty( "RefIndexLocations", m_refLocations );

  // Quantum efficiency
  declareProperty( "QETableLocation", m_qeTableLoc =
                   "/dd/Materials/RichMaterialTabProperties/HpdQuantumEff" );

}

StatusCode RichTabulatedRefractiveIndex::initialize() 
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;
 
  // Get tools
  acquireTool( "RichDetParameters", m_detParams );

  // Get the refractive indices from the XML
  SmartDataPtr<TabulatedProperty> tabAero( detSvc(), m_refLocations[Rich::Aerogel] );
  if ( !tabAero ) {
    return Error( "Failed to load refractive index from "+m_refLocations[Rich::Aerogel] );
  }
  SmartDataPtr<TabulatedProperty> tabR1Gas( detSvc(), m_refLocations[Rich::Rich1Gas] );
  if ( !tabR1Gas ) {
    return Error( "Failed to load refractive index from "+m_refLocations[Rich::Rich1Gas] );
  }
  SmartDataPtr<TabulatedProperty> tabR2Gas( detSvc(), m_refLocations[Rich::Rich2Gas] );
  if ( !tabR2Gas ) {
    return Error( "Failed to load refractive index from "+m_refLocations[Rich::Rich2Gas] );
  }

  // Create tabulated property objects from XML information
  m_refIndex[Rich::Aerogel]  = new Rich1DTabProperty( tabAero  );
  m_refIndex[Rich::Rich1Gas] = new Rich1DTabProperty( tabR1Gas );
  m_refIndex[Rich::Rich2Gas] = new Rich1DTabProperty( tabR2Gas );

  // Acquire QE Curve from XML
  SmartDataPtr<TabulatedProperty> tabQE( detSvc(), m_qeTableLoc );
  if ( tabQE ) {
    m_QE = new Rich1DTabProperty( tabQE );
  } else {
    return Error( "Cannot retrieve QE from " + m_qeTableLoc );
  }

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedRefractiveIndex::finalize()
{
  // Tidy up
  for ( RefractiveIndices::iterator iRef = m_refIndex.begin();
        iRef != m_refIndex.end(); ++iRef ) {
    if ( *iRef ) { delete *iRef; *iRef = 0; }
  }
  if ( m_QE ) { delete m_QE; m_QE = 0; }

  // base class finalize
  return RichToolBase::finalize();
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energy )
{
  return (*m_refIndex[rad])[energy*eV];
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energyBot,
                                                      const double energyTop )
{
  return refractiveIndex( rad, m_QE->meanX(energyBot,energyTop)/eV );
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad )
{
  //const double meanEn = m_QE->meanX( m_detParams->minPhotonEnergy(rad), 
  //                                   m_detParams->maxPhotonEnergy(rad) ) / eV;
  return refractiveIndex( rad, m_detParams->meanPhotonEnergy(rad) );
}

