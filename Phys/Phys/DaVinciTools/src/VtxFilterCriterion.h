// $Id: VtxFilterCriterion.h,v 1.1 2003-04-29 18:38:55 gcorti Exp $
#ifndef VTXFILTERCRITERION_H 
#define VTXFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

/** @class VtxFilterCriterion VtxFilterCriterion.h
 *  Returns a yes/no depending on the particle vertex
 *  @author Gerhard Raven
 *  @date   24/06/2002
 */
class VtxFilterCriterion : public AlgTool, 
                           virtual public IFilterCriterion {
public:

  /// Standard constructor
  VtxFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Destructor
  virtual ~VtxFilterCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if filter is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied.
  inline bool operator()( const Particle* const & part )
  { return isSatisfied(part); }

private:
  
  double     m_maxChi2;     ///< Maximum Chisquared
  double     m_maxZPos,m_minZPos; ///< Max and Min Zposition

};

#endif // VTXFILTERCRITERION_H
