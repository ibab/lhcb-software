// $Id: PrintDuplicates.h,v 1.1 2012-10-11 11:56:34 elsasser Exp $
#ifndef PrintDuplicates_H
#define PrintDuplicates_H 1

#include "Kernel/DaVinciAlgorithm.h"

#include <algorithm>
#include <string>
#include <map>
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Kernel/LHCbID.h"
#include "Kernel/HashIDs.h"

#include "Kernel/IPrintDecay.h"

/** @class PrintDuplicates PrintDuplicates.h
 *
 *  Finds and print duplicates.
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

  virtual StatusCode execute();    ///< Algorithm execution

private:

  /// Get TES location for an object
  template<class TYPE>
  std::string tesLocation( const TYPE * obj ) const
  {
    return ( obj && obj->parent() && obj->parent()->registry() ?
             obj->parent()->registry()->identifier() : "NotInTES" );
  }

private:

  /// Access print tool on demand
  IPrintDecay * printDecay()
  {
    if ( ! m_printDecay )
    {
      m_printDecay = tool<IPrintDecay>( "PrintDecayTreeTool/PrintDecay", this ) ;
    }
    return m_printDecay;
  }

private:

  typedef std::map< std::pair<std::size_t,double>, LHCb::Particle::ConstVector > PartHashMap;

  typedef std::map< std::string, PartHashMap > LocHashMap;

private:

  IPrintDecay * m_printDecay;

};

#endif // REMOVEDUPLICATES_H
