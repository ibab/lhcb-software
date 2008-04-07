#ifndef FILTERBYBACKCAT_H 
#define FILTERBYBACKCAT_H 1

// base class
#include "Kernel/FilterCriterionBase.h"
#include "Kernel/IBackgroundCategory.h"

/** @class FilterByBackCat FilterByBackCat.h
 *  Returns a yes/no depending on the particle BackCat.
 *  @author V. Gligorov
 *  @date   30/03/2008
 */

class IBackgroundCategory;

class FilterByBackCat : public FilterCriterionBase {

public:

  /// Standard constructor
  FilterByBackCat( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Destructor
  virtual ~FilterByBackCat( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

protected: // methods

  /// Test if filter is satisfied.
  virtual bool testParticle( const LHCb::Particle* const & part );

private:

  IBackgroundCategory* m_bkg;  
  std::vector<long> m_bkgCat;     ///< Particle back cats allowed
  std::vector<long>::const_iterator m_bkgCatIterator;

};

#endif // KINFILTERCRITERION_H
