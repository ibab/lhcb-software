// $Id: MassFilterCriterion.h,v 1.1.1.1 2004-08-24 06:35:15 pkoppenb Exp $
#ifndef MASSFILTERCRITERION_H 
#define MASSFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

// Forward declarations
class IParticlePropertySvc;

/** @class MassFilterCriterion MassFilterCriterion.h
 *  Returns a yes/no depending on the particle vertex
 *  @author Gerhard Raven
 *  @date   24/06/2002
 */
class MassFilterCriterion : public GaudiTool, 
                            virtual public IFilterCriterion {
public:

  /// Standard constructor
  MassFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Destructor
  virtual ~MassFilterCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if filter is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied.
  inline bool operator()( const Particle* const & part )
  { return isSatisfied(part); }

private:
  
  double     m_massWindow;     ///< Mass Window
  double     m_upperMassWindow;     ///< Mass Window
  double     m_lowerMassWindow;     ///< Mass Window
  /// Pointer to the ParticlePropertySvc
  IParticlePropertySvc* m_ppSvc;   

};

#endif // MASSFILTERCRITERION_H
