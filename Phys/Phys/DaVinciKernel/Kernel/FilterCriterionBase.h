// $Id: FilterCriterionBase.h,v 1.1 2005-06-28 15:32:27 lazzecri Exp $
#ifndef FILTERCRITERIONBASE_H 
#define FILTERCRITERIONBASE_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "Kernel/IFilterCriterion.h"
#include "Kernel/IFilterCriteriaTESTool.h"

/** @class FilterCriterionBase FilterCriterionBase.h
 *  
 *  Base class for all filter tools
 *
 *  @author Cristina Lazzeroni
 *  @date   2005-06-22
 */

class FilterCriterionBase : public GaudiTool, 
                            virtual public IFilterCriterion
{

public: 
  
  /// Standard constructor
  FilterCriterionBase( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent ); 
  
  virtual ~FilterCriterionBase( ) = 0; ///< Destructor

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if kinematical filter (minimum momentum and pt) is satisfied.
  virtual bool isSatisfied( const Particle* const & part );

  /// Test if kinematical filter (minimum momentum and pt) is satisfied.
  virtual bool operator()( const Particle* const & part );
  
protected:

  /// The method which actually makes the decision
  /// Must be re-implemented in each real derived filter tool
  virtual bool testParticle( const Particle* const & part ) = 0; 

private: // methods

  /// Return a pointer to the FilterCriteriaTES Tool
  /// On first access retrieves tool pointer from tool service
  inline const IFilterCriteriaTESTool * tesTool() const
  {
    if ( !m_tesTool ) 
    { 
      m_tesTool = tool<IFilterCriteriaTESTool>( "FilterCriteriaTESTool" ); 
    }
    return m_tesTool;
  }

private: // data

  /// Pointer to FilterCriteriaTESTool
  mutable IFilterCriteriaTESTool * m_tesTool;

  /// Flag to turn on saving of filter results to TES
  bool m_saveResults;

  /// Turn on flagging mode, save result but always return true
  bool m_flagMode;

};

#endif // FILTERCRITERIONBASE_H
