// $Id: ERelations.h,v 1.7 2009-05-30 12:14:28 graven Exp $
#ifndef HLTBASE_RELATIONS_H 
#define HLTBASE_RELATIONS_H 1

#include "GaudiKernel/GaudiException.h"
#include <functional>
#include <algorithm>
#include <ostream>
#include <cmath>
#include <vector>
#include <map>


namespace zen
{

  /** Template base class to define relations between objects
   *  
   *  @author Jose Angel Hernando Morata
   *  @date   2006-10-04
   */
  
  template <class T1, class T2>
  class relation 
  {
  public:

    typedef typename std::pair<T2,double> target;
    typedef typename std::vector<target> targets;

    // clear the relation
    void clear() 
    {_relations.clear();_keys.clear();}
    
    // relate t2 to t1 with a weight
    void relate(const T1& t1, const T2& t2, double weight = 1.) {
      targets& tars = get_targets(t1);
      target& tar   = get_target(tars,t2);
      tar.second += weight;
      std::sort(tars.begin(),tars.end(),byWeight());
    }

    // return size of the relation
    size_t size() const {return _keys.size();}

    // return keys
    const std::vector<T1>& keys() const {return _keys;}
    
    // return true if the key has a relation
    bool has_key(const T1& t1) const {
      return (_relations.find(t1) != _relations.end());
    }

    // returns the relations of this key
    const targets& relations(const T1& t1) const {
      if (!has_key(t1)) 
          throw GaudiException( "relations: invalid key ", "", StatusCode::FAILURE );
      return ((_relations.find(t1))->second);
    }
    
    // return the best relation
    const T2& best_relation(const T1& t1, double& weight) const {
      const targets& tars = relations(t1);
      weight = tars[0].second;
      return (tars[0].first); 
    }

    relation<T2,T1> reverse() const {
      relation<T2,T1> rel;
      for (typename std::vector<T1>::const_iterator it = _keys.begin(); 
           it != _keys.end(); ++it) {
        const T1& t1 = *it;
        const targets& tars = _relations.find(t1)->second;
        for (typename targets::const_iterator it2 = tars.begin();
             it2 != tars.end(); ++it2) {
          const T2& t2 = (*it2).first;
          double w = (*it2).second;
          rel.relate(t2,t1,w);
        }
      }
      return rel;
    }

    void info() const {
      // std::cout << " size " << _keys.size() << std::endl;
      for (typename std::vector<T1>::const_iterator it = _keys.begin(); 
           it != _keys.end(); ++it) {
        const T1& t1 = *it;
        const targets& tars = _relations.find(t1)->second;
        // std::cout << " targets size " << tars.size() << std::endl;
        for (typename targets::const_iterator it2 = tars.begin();
             it2 != tars.end(); ++it2) {
          //const T2& t2 = (*it2).first;
          //double w = (*it2).second;
          // std::cout  << "    t1 " << t1 
          //           << " -->  t2 " << t2 << " w " << w << std::endl;
        }
      }
    }

  protected:
    
    class byWeight {
    public:
      bool operator() (const target& t1, const target& t2) const 
        {return (t1.second > t2.second);}
    };

    targets& get_targets(const T1& t1) {
      if (_relations.find(t1) == _relations.end()) {
        //        std::cout << " creating targets t1 " << t1 << std::endl;
        _relations[t1] = targets();
        _keys.push_back(t1);
      }
      return _relations[t1];
    }
    
    
    target& get_target(targets& tars, const T2& t2) {
      for (typename targets::iterator it = tars.begin(); it != tars.end(); ++it)
        if ((*it).first == t2) return (*it);
      // std::cout << " creating target t2 " << t2 << std::endl;
      tars.push_back( target(t2,0.));
      target& tar = tars.back();
      return tar;
    }      
    
  protected:
    std::vector<T1> _keys;
    std::map<T1, targets > _relations;
  };
  

  typedef relation<int,int> IntRelation;
  
}

  
#endif
