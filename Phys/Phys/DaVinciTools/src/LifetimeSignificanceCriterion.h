// $Id: LifetimeSignificanceCriterion.h,v 1.1 2003-12-04 13:40:28 graven Exp $
#ifndef LIFETIMESIGNIFICANCEFILTERCRITERION_H 
#define LIFETIMESIGNIFICANCEFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"
#include "DaVinciTools/Particle2VertexAsct.h"
                                                                                                                                


// Forward declarations
class ILifetimeFitter;
class IGeomDispCalculator;
class IDataProviderSvc;


/** @class LifetimeSignificanceCriterion LifetimeSignificanceCriterion.h
 *  Returns a yes/no depending on the lifetime significance of a
 *  particle and the associated primary vertex
 *  @author Gerhard Raven
 *  @date   21/06/2003
 */
class LifetimeSignificanceCriterion : public AlgTool, 
                           virtual public IFilterCriterion {
public:

  /// Standard constructor
  LifetimeSignificanceCriterion( const std::string& type, 
                                 const std::string& name,
                                 const IInterface* parent);
  
  /// Destructor
  virtual ~LifetimeSignificanceCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if filter is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied.
  inline bool operator()( const Particle* const & part )
  { return isSatisfied(part); }

private:
  
  double m_minSig;
  double m_minLife;
  double m_maxRes;
  double m_maxChisq;
  double m_maxAssocWeight;
  double m_minAssocWeight;
  Particle2VertexAsct::IAsct* m_vAsct;
  ILifetimeFitter *m_iLife;
  IGeomDispCalculator* m_ipTool;
  IDataProviderSvc* m_EDS;
  bool m_useAssoc;

};

#endif // MASSFILTERCRITERION_H
