// $Id: IFilterCriteriaTESTool.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCITOOLS_IFILTERCRITERIATESTOOL_H
#define DAVINCITOOLS_IFILTERCRITERIATESTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/FilterCriterionResult.h"
// from DaVinciTools
class IFilterCriterion;

// Event
namespace LHCb
{
  class Particle;
}

/** @class IFilterCriteriaTESTool IFilterCriteriaTESTool.h DaVinciTools/IFilterCriteriaTESTool.h
 *
 *  Interface to tool to save filter results to TES and provides access methods
 *  to retrieve the results for analyses.
 *
 *  @author Cristina Lazzeroni
 *  @date   2005-06-23
 */

class GAUDI_API IFilterCriteriaTESTool : virtual public IAlgTool {

public: // definitions

  typedef std::vector<const LHCb::FilterCriterionResult *> ResultsVector;

public:

  DeclareInterfaceID(IFilterCriteriaTESTool, 2, 0);
  
  /// Save the given filter result to the TES container
  virtual bool saveFilterResult( IFilterCriterion * critTool,
                                 LHCb::Particle * part,
                                 const bool result ) const = 0;

  /// Access to the vector of all filter results
  virtual const LHCb::FilterCriterionResults * criterionResults() const = 0;

  /// Returns a vector of all filter results that either failed or succeeded
  virtual bool criterionResults( ResultsVector & results,
                                 const bool result = true ) const = 0;

  /// Returns all filter results that are associated to the given name
  virtual bool criterionResults( ResultsVector & results,
                                 std::string & name ) const = 0;

  /// Returns all filter results that were applied to a given particle
  virtual bool criterionResults( ResultsVector & results,
                                 const LHCb::Particle * part ) const = 0;

};

#endif // DAVINCITOOLS_IFILTERCRITERIATESTOOL_H
