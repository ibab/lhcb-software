// $Id: PrintDuplicates.h,v 1.1 2012-10-11 11:56:34 elsasser Exp $
#ifndef PrintDuplicates_H
#define PrintDuplicates_H 1

#include <algorithm>
#include <string>
#include <map>
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Kernel
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/LHCbID.h"
#include "Kernel/HashIDs.h"
#include "Kernel/IPrintDecay.h"

// Boost
#include <boost/math/special_functions/round.hpp>

/** @class PrintDuplicates PrintDuplicates.h
 *
 *  Finds and print duplicate decay trees.
 *
 *  @author Chris Jones
 *  @date   2012-10-11
 */
class PrintDuplicates : public DaVinciAlgorithm
{

public:

  /// Standard constructor
  PrintDuplicates( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrintDuplicates( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  /// Get TES location for an object
  template<class TYPE>
  inline std::string tesLocation( const TYPE * obj ) const
  {
    return ( obj && obj->parent() && obj->parent()->registry() ?
             obj->parent()->registry()->identifier() : "NotInTES" );
  }

  /// Access print tool on demand
  inline IPrintDecay * printDecay()
  {
    if ( !m_printDecay )
    {
      m_printDecay = tool<IPrintDecay>( "PrintDecayTreeTool/PrintDuplicateDecays" );
    }
    return m_printDecay;
  }

private:

  unsigned int m_dpPrec;      ///< Number of d.p. precision to compare energy values to
  IPrintDecay * m_printDecay; ///< Tool to print the decay tree
  unsigned int m_maxPrints;   ///< Max number of times to print the decay tree.
  std::map< std::string, unsigned int > m_countPerLoc; ///< Printout count per TES location

};

#endif // REMOVEDUPLICATES_H
