// $Id: KinFilterCriterion.h,v 1.2 2002-05-15 23:24:18 gcorti Exp $
#ifndef KINFILTERCRITERION_H 
#define KINFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

/** @class KinFilterCriterion KinFilterCriterion.h
 *  Returns a yes/no depending on the particle minimum momentum and pt.
 *  @author Paul Colrain
 *  @date   14/03/2002
 */
class KinFilterCriterion : public AlgTool, 
                           virtual public IFilterCriterion {

public:

  /// Standard constructor
  KinFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Destructor
  virtual ~KinFilterCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if kinematical filter (minimum momentum and pt) is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if kinematical filter (minimum momentum and pt) is satisfied.
  inline bool operator()( const Particle* const & part );

private:
  
  double     m_minMom;     ///< Minimum momentum
  double     m_minPt;      ///< Minimum pt

};

#endif // KINFILTERCRITERION_H
