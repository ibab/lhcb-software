// $Id: KinFilterCriterion.h,v 1.1 2002-03-27 20:35:00 gcorti Exp $
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
 *
 *  @author Paul Colrain
 *  @date   14/03/2002
 */
class KinFilterCriterion : public AlgTool, virtual public IFilterCriterion {

public:

  /// Standard constructor
  KinFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Desctructor
  virtual ~KinFilterCriterion( ){ }; 

  /// Initialize
  StatusCode initialize( );

  /// Test if filter is satisfied
  inline bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied
  inline bool operator()( const Particle* const & part );

private:

  /// Minimum momentum
  double     m_minMom;

  /// Minimum pt
  double     m_minPt;

};

#endif // KINFILTERCRITERION_H
