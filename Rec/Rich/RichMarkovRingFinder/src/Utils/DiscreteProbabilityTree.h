
#ifndef LESTER_DISCRETE_PROBABILITY_TREE_H
#define LESTER_DISCRETE_PROBABILITY_TREE_H

//fwd dec
#include "Utils/DiscreteProbabilityTree.fwd"

//includes
#include "CLHEP/Random/RandFlat.h"
#include "Utils/SampleAndProbabilityModule.h"
#include <map>


//defn
template<class T/*, class Baggage*/>
class DiscreteProbabilityTree : public SampleAndProbabilityModule<T> { 
public:
  /*
    class TriedToDeleteRootElement : public std::exception {
    virtual char * what() const throw () {
    return "Tried to delete root element of DiscreteProbabilityTree!";
    };
    };
  */
  class TriedToShootFromEmptyTree : public std::exception {
    virtual const char * what() const throw () {
      return "Tried to shoot from an empty DiscreteProbabilityTree!";
    };
  };
  typedef DiscreteProbabilityTree<T> MDDPT;
  /// "shoot()" just calls "sample()" and is only here for historical reasons to support old code!
  T shoot() const {
    return sample();
  };
  /// "T sample() const" implements one of the two pure virtual methods of the base class "SampleAndProbabilityModule<T>".
  T sample() const {
    if (m_size==0) {
      throw TriedToShootFromEmptyTree();
    };
    if (m_branch1) {
      assert(m_branch2);
      const double p1=m_branch1->prob();
      const double p2=m_branch2->prob();
      const double pt=p1+p2;
      const double p=RandFlat::shoot(pt);
      if (p<p1) {
	return m_branch1->shoot();
      } else {
	return m_branch2->shoot();
      };
    } else {
      return m_t;
    };
  };
  /// "double probabilityOf(const T &) const" implements one of the two pure virtual methods of the base class "public SampleAndProbabilityModule<T>"
  double probabilityOf(const T & t) const {
    if (m_prob==0) {
      return 0;
    } else {
      return propToProbabilityOf(t)/m_prob;
    };
  };
  /// "double propToProbabilityOf(const T &) const" overrides one of the virtual methods of the base class "public SampleAndProbabilityModule<T>"
  double propToProbabilityOf(const T & t) const {
     const typename Map::iterator it = m_map->find(t);
     if (it == m_map->end()) {
       return 0;
     } else {
       const MDDPT * const br = it->second;
       return br->m_prob;
     };
  };
  double logOfProbabilityOf(const T & t) const {
    return DEFAULT_logOfProbabilityOf(t);
  };
  double almostLogOfProbabilityOf(const T & t) const {
    return DEFAULT_almostLogOfProbabilityOf(t);
  };
  double prob() const {
    return m_prob;
  };
  unsigned int size() const {
    return m_size;
  };
  void removeT(const T & t) {
    const typename Map::iterator it = m_map->find(t);
    const bool elementExists = (it != m_map->end());
    if (elementExists) {
      // Find element
      MDDPT * const leafP = it->second;
      assert(leafP);
      MDDPT * const leafPsParent = leafP->m_parent;
      m_map->erase(it);
      if (leafPsParent) {
	//std::cout << "I am " << this << " and I'm reporting the death of myself to my parent who is " << 
	leafPsParent->reportDeathOf(leafP);
      } else {
	// we must be deleting an element from the root
	assert(leafP->m_root);
	assert(m_size==1);
	assert(!m_branch1);
	assert(!m_branch2);
	assert(m_map->empty()); // the erase has already been done!
	// it is not permitted to delete the root element, as you can't have a probability distribution without a domain!
	// nevertheless, we relax this requirement, insisting only that shoot throws an exception if the user tries to shoot in this state!
	m_size = 0;
	assert(m_size==0);

	m_prob = 0;
	m_t = T();
	
      };
    } else {
      // element didn't exist: do nothing!
    };
  };
public:
  void insertTWithProb(const T & t, const double prob) {
    const typename Map::const_iterator it = m_map->find(t);
    if (it == m_map->end()) {
      // we are inserting a NEW domain member:
      insertNewTWithProb(t, prob);
    } else {
      // we are altering an existing domain member:
      it->second->alterProb(prob);
    };
  };
  /// works like filling a histogram.  If t doesn't already exist it gets created, otherwise we add to its probability.
  void fillTWithProb(const T & t, const double prob) {
    const typename Map::const_iterator it = m_map->find(t);
    if (it == m_map->end()) {
      // we are inserting a NEW domain member:
      insertNewTWithProb(t, prob);
    } else {
      // we are altering an existing domain member:
      const double existingProb = it->second->m_prob;
      const double newProb = existingProb + prob;
      it->second->alterProb(newProb);
    };
  };
private:
  void alterProb(const double prob) {
    assert(!m_branch1);
    assert(!m_branch2);
    m_prob = prob;
    if (m_parent) {
      m_parent->notifyOfAlterationsBelow();
    };
  }; 
  void insertNewTWithProb(const T & t, const double prob) {
    assert(m_map->find(t) == m_map->end());

    m_size += 1;
    if (m_branch1) {
      assert(m_branch2);
      if (m_branch1->distasteForNewPoint() > m_branch2->distasteForNewPoint()) {
	m_branch2->insertTWithProb(t, prob);
      } else {
	m_branch1->insertTWithProb(t, prob);
      };
      // I know that I ought to be able to just do m_prob += prob, but I do the following as it likely to be more numerically stable:
      m_prob = m_branch1->prob() + m_branch2->prob();
    } else {
      assert(!m_branch2);
      if (m_root && size()==1) {
	assert(m_prob==0);
	assert(m_map->empty());
	assert(m_root);
	assert(!m_branch1);
	assert(!m_branch2);
	assert(!m_parent);

	m_t = t;
	m_prob = prob;
	assert(size() == 1);
	
	(*m_map)[t] = this;
	assert(m_map->size()==1);
	
      } else {
	//std::cout << "m_size == " << m_size << std::endl;
	assert(m_map->find(m_t) != m_map->end());
	//MDDPT * const upcast = dynamic_cast<MDDPT*>(this);
	assert(m_map->find(m_t)->second == this);
	m_map->erase(m_t);
	m_branch1 = new MDDPT(m_t, m_prob, m_map, this);
	m_branch2 = new MDDPT(  t,   prob, m_map, this);
	m_prob += prob;
      };
    };
  };
public:
  DiscreteProbabilityTree() : 
    m_t(),
    m_prob(),
    m_size(0), 
    m_branch1(0),
    m_branch2(0),
    m_parent(0),
    m_map(new Map),
    m_root(true) {
    assert(m_map->empty());
  };
public:
  DiscreteProbabilityTree(const T & t, 
			  const double prob) : 
    m_t(t),
    m_prob(prob),
    m_size(1), 
    m_branch1(0),
    m_branch2(0),
    m_parent(0),
    m_map(new Map),
    m_root(true) {
    assert(m_map);
    assert(m_map->empty());
    (*m_map)[t] = this;//dynamic_cast<MDDPT*>(this);
  };
public:
  ~DiscreteProbabilityTree() {
    delete m_branch1;
    delete m_branch2;
    if (m_root) {
      delete m_map;
    };
  };
private:
  void reportDeathOf(MDDPT * const sickling) {
    assert(sickling);
    assert(sickling == m_branch1 || sickling == m_branch2);
    assert(m_branch1 != m_branch2);

    {
      MDDPT * brother = 
	(sickling == m_branch1 ? m_branch2 : m_branch1);
      
      assert(brother);
      
      if (brother->m_branch1) {
	// brother is a node
	assert(brother->m_branch2);
	m_branch1 = brother->m_branch1;
	m_branch2 = brother->m_branch2;
	brother->m_branch1=0;
	brother->m_branch2=0;
	m_size = brother->m_size;
	assert(m_size>=2);
	m_prob = brother->m_prob;      
	//	MDDPT * const upcast = dynamic_cast<MDDPT*>(this);
	m_branch1->m_parent = this;//upcast;
	m_branch2->m_parent = this;//upcast;

      } else {
	// brother is a leaf
	assert(!(brother->m_branch2));
	m_branch1=0;
	m_branch2=0;
	assert(brother->m_size == 1);
	m_size = 1;
	m_t = brother->m_t;
	m_prob = brother->m_prob;
  
	assert(m_map->find(m_t) != m_map->end());
	assert(m_map->find(m_t)->second == brother);
	//MDDPT * const upcast = dynamic_cast<MDDPT*>(this);
	(*m_map)[m_t]=this;//upcast;
      };
      
      delete (brother);
      assert((brother=0)==0);
    };

    if (m_parent) {
      m_parent->notifyOfDeletionBelow();
    };
  };
  void notifyOfAlterationsBelow() {
    assert(m_size>=2);
    assert(m_branch1 && m_branch2);
    assert(m_size==m_branch1->m_size + m_branch2->m_size);
    m_prob = m_branch1->m_prob + m_branch2->m_prob;
    if (m_parent) {
      assert(m_parent != this);
      m_parent->notifyOfAlterationsBelow();
    };
  };
  void notifyOfDeletionBelow() {
    assert(m_size>=2);
    m_size-=1;
    assert(m_branch1 && m_branch2);
    assert(m_size==m_branch1->m_size + m_branch2->m_size);
    m_prob = m_branch1->m_prob + m_branch2->m_prob;
    if (m_parent) {
      assert(m_parent != this);
      m_parent->notifyOfDeletionBelow();
    };
  };
protected:
  typedef std::map<const T, MDDPT *> Map;
  DiscreteProbabilityTree(const T & t, 
			  const double prob,
			  Map * const map,
			  MDDPT * const parent) : 
    m_t(t),
    m_prob(prob),
    m_size(1), 
    m_branch1(0),
    m_branch2(0),
    m_parent(parent),
    m_map(map),
    m_root(false) {
    assert(m_map);
    assert(m_map->find(t) == m_map->end());
    //MDDPT * const upcast = dynamic_cast<MDDPT*>(this);
    (*m_map)[t] = this;//upcast;
  };
private:
  DiscreteProbabilityTree operator=(const MDDPT & other); // deliberately not implemented!
  //typedef unsigned int Distaste;
  typedef double Distaste;
  Distaste distasteForNewPoint() const {
    return m_size;
    //return m_prob;
  };
  T m_t;
  double m_prob;  // either totalProb or probT depending on status!
  unsigned int m_size;
  MDDPT * m_branch1;
  MDDPT * m_branch2;
  MDDPT * m_parent;
  Map * const m_map;
  const bool m_root; // LESTER: could probably do away with this variable, relying instead on a test of the nullness of m_parent.
  template<class TT>
  void sendN(std::ostream & os, unsigned int n, const TT & s) const {
    for (unsigned int i=0; i<n; ++i) {
      os << s;
    };
  };
public:
  void printMeIndentedTo(std::ostream & os, unsigned int indent=0) const {
    if (m_branch1) {
      assert(m_branch2);
      sendN(os, indent, ". ");
      os << " at " << this << " size=" <<m_size<<" totProb=" << m_prob << " and parent " << m_parent<< " and b12=" << m_branch1 << " " << m_branch2<<std::endl;
      m_branch1->printMeIndentedTo(os, indent+1);
      m_branch2->printMeIndentedTo(os, indent+1);
    } else {
	sendN(os, indent, ". ");
      if (m_size==0) {
	os << "EmptyDiscreteProbabilityTree" << std::endl;
      } else {
	os << " at " << this << " " << m_t << " has prob " << m_prob << " and parent " << m_parent<< std::endl;
      };
    };
  };
private:
  //Baggage m_baggage;
};





#endif
