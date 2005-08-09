
#ifndef LESTER_ORDERED_PROBABILITY_SET_H
#define LESTER_ORDERED_PROBABILITY_SET_H

//fwd dec
#include "Utils/OrderedProbabilitySet.fwd"

//includes
#include "CLHEP/Random/RandFlat.h"
#include <set>
#include <map>


//defn
template<class T, class Less >
class OrderedProbabilitySet { 
private:
  typedef std::pair<T, double> Pair;
  struct ProbGreater {
    bool operator()(const Pair & a, const Pair & b) const {
      //return ((a.second) > (b.second));
      
      if ((a.second) > (b.second)) {
	return true;
      };
      if ((a.second) < (b.second)) {
	return false;
      };
      assert(((a.second) == (b.second)));
      return (a.first.get() > b.first.get() );
      
    };
  };
  typedef std::set<Pair, ProbGreater > Set;
  typedef typename Set::const_iterator IT;

  Set m_set;

  typedef std::map<T, IT> Map;

  Map m_map;

public:
  class TriedToShootFromEmptyOrderedProbabilitySet : public std::exception {
    virtual const char * what() const throw () {
      return "Tried to shoot from an empty OrderedProbabilitySet!";
    };
  };
  T largestT() const {
    if (m_set.empty()) {
      throw TriedToShootFromEmptyOrderedProbabilitySet();
    };
    return (m_set.begin())->first;
  };
  unsigned int size() const {
    assert (m_set.size() == m_map.size());
    return m_set.size();
  };
private:
  //void removeLargestT() {
  //  removeT(largestT());
  //};
public:
  void removeT(const T & t) {
    typename Map::iterator mit = m_map.find(t);
    if (mit != m_map.end()) {
      // we'd better delete this elt.
      const IT & sit = mit->second;
      assert((*sit).first == t);
      m_set.erase(sit);
      m_map.erase(mit);
      assert (m_set.size() == m_map.size());
      assert ( m_map.find(t) == m_map.end());
    } else {
      // user requested to remove non existent member.
      // Do nothing.
    };
  };
public:

  /// Precondition: do not call the following with a ordered pair (t, prob) which you know already to be in the set!!
  void insertTWithProb(const T & t, const double prob) {
    const Pair pair(t,prob);
    assert(m_set.count(pair) == 0);
    const std::pair<typename Set::iterator, bool> ans = m_set.insert(pair);
    assert(ans.second);
    assert(ans.first != m_set.end());
    assert(m_map.find(t) == m_map.end());
    m_map[t]=(ans.first);
    assert(m_map.find(t) != m_map.end());
  };
};





#endif
