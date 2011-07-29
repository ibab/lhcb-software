// $Id: $
#ifndef DECISIONMAP_H 
#define DECISIONMAP_H 1

// Include files
#include <memory>

// boost
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

/** @class DecisionMap DecisionMap.h
 *  
 *
 *  @author Roel Aaij
 *  @date   2010-03-26
 */
class DecisionMap {
public: 
   /// Standard constructor

   typedef std::pair< unsigned int, unsigned int > eventKey_t;

   DecisionMap( ); 

   virtual ~DecisionMap( ); ///< Destructor

   bool decision( const unsigned int runNumber,
                  const unsigned int eventNumber,
                  const std::string& lineName ) const;

   void addDecision( const unsigned int runNumber,
                     const unsigned int eventNumber,
                     const std::string& lineName,
                     const bool value );

   std::vector< eventKey_t, std::allocator< eventKey_t > > events() const;

   bool compare( const unsigned int runNumber,
                 const unsigned int eventNumber,
                 const std::string& first_line,
                 const std::string& second_line ) const;

private:

   typedef boost::unordered_set< std::string > decisions_t;
   typedef boost::unordered_map< eventKey_t, decisions_t > decisionMap_t;
   decisionMap_t* m_map;

   // caching
   mutable unsigned int m_runNumber;
   mutable unsigned int m_eventNumber;
   mutable decisionMap_t::iterator m_cached;

};
#endif // DECISIONMAP_H
