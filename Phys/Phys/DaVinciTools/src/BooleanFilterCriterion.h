// $Id: BooleanFilterCriterion.h,v 1.1 2004-03-23 17:30:39 pkoppenb Exp $
#ifndef BOOLEANFILTERCRITERION_H 
#define BOOLEANFILTERCRITERION_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

class Particle;

/** @class BooleanFilterCriterion BooleanFilterCriterion.h
 *  Returns a yes/no depending on the boolean combination of a set of other cuts:
 *   i) at last one cut in the 'orList' should pass (if the 'orList' is non-empty)
 *  ii) all cuts in the 'andList' should pass (if the 'andList' is non-empty)
 *  @author Gerhard Raven
 *  @date   18/03/2004
 */
class BooleanFilterCriterion : public AlgTool, 
                               virtual public IFilterCriterion {

public:

  /// Standard constructor
  BooleanFilterCriterion( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);
  
  /// Destructor
  virtual ~BooleanFilterCriterion( ){ }; 


  /// Initialize: init...
  StatusCode initialize( );

  /// Finalize: clean up
  StatusCode finalize( );

  /// Test if filter is satisfied.
  bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied.
  bool operator()( const Particle* const & part ) { return isSatisfied(part); }

private:
  
  std::vector<std::string>     m_andListNames;     ///< List of 'and' criteria
  std::vector<std::string>     m_orListNames;     ///< List of 'or' criteria

  std::vector<IFilterCriterion*>     m_andList;     ///< List of 'and' criteria
  std::vector<IFilterCriterion*>     m_orList;     ///< List of 'or' criteria

  bool                          m_flipResult;     ///< negate the result

  StatusCode getCriteria( const std::vector<std::string>&, 
                          std::vector<IFilterCriterion*>&,
                          const char*) const;

};

#endif // BOOLEANFILTERCRITERION_H
