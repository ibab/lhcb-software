// $Id: PrintDuplicates.h,v 1.1 2012-10-11 11:56:34 elsasser Exp $
#ifndef PrintDuplicates_H
#define PrintDuplicates_H 1

#include <algorithm>
#include <string>
#include <map>
#include <sstream>
#include <set>

// Kernel
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/LHCbID.h"
#include "Kernel/HashIDs.h"
#include "Kernel/IPrintDecay.h"

// Boost
#include <boost/math/special_functions/round.hpp>
#include "boost/functional/hash.hpp"
#include "boost/cstdint.hpp"

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

  typedef boost::uint32_t     Hash32;
  typedef boost::uint64_t     Hash64;
  typedef std::vector<Hash32> Hashes32;
  typedef std::vector<Hash64> Hashes64;
  typedef std::vector<LHCb::Particle::ConstVector> FilteredDups;

private:

  /// Get TES location for an object
  template<class TYPE>
  inline std::string tesLocation( const TYPE * obj ) const
  {
    return ( obj && obj->parent() && obj->parent()->registry() ?
             obj->parent()->registry()->identifier() : "NotInTES" );
  }

  /// Access print tool on demand
  inline IPrintDecay * printDecay() const
  {
    if ( !m_printDecay )
    {
      m_printDecay = tool<IPrintDecay>( "PrintDecayTreeTool/PrintDuplicateDecays" );
    }
    return m_printDecay;
  }

  /// Get the hash for a Particle
  inline Hash32 getLHCbIDsHash( const LHCb::Particle * p ) const
  {
    return (Hash32) LHCb::HashIDs::hashID( p );
  }

  /// Peform a deep check on the given particles
  void deepHashCheck( const LHCb::Particle::ConstVector & parts,
                      FilteredDups& filtDups ) const;

  /// get deeep check hashes vector
  void getDeepHashes( const LHCb::Particle * p,
                      Hashes64& hashes,
                      unsigned int depth = 0 ) const;

  /// Report a given vector at a given TES location as duplicates
  void report( const std::string & loc,
               const LHCb::Particle::ConstVector & parts );

private:

  mutable IPrintDecay * m_printDecay; ///< Tool to print the decay tree
  unsigned int m_maxPrints;   ///< Max number of times to print the decay tree.
  bool m_deepCheck;           ///< Perform a deep check of duplicates
  std::map< std::string, unsigned int > m_countPerLoc; ///< Printout count per TES location

};

#endif // REMOVEDUPLICATES_H
