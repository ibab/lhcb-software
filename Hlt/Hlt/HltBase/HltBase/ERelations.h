// $Id: ERelations.h,v 1.1 2006-10-24 09:31:19 hernando Exp $
#ifndef HLTBASE_RELATIONS_H 
#define HLTBASE_RELATIONS_H 1

#include <functional>
#include <algorithm>
#include <ostream>
#include <cmath>
#include <vector>
#include <map>

// Include files

/** Template base class to define relationw between objects
 *  between them
 *  
 *  @author Jose Angel Hernando Morata
 *  @date   2006-10-04
 */

namespace Estd
{

  //--- Base classes
  
  template <class T1, class T2>
  class relation 
  {
  public:

    typedef typename std::pair<T2*,double> target;
    typedef typename std::vector<target> targets;

    void clear() 
    {_relations.clear();_keys.clear();}
    
    void relate(const T1& ct1, const T2& ct2, double weight = 1.) {
      T1* t1 = (T1*) &ct1;
      T2* t2 = (T2*) &ct2;
      targets& tars = get_targets(t1);
      target& tar   = get_target(tars,t2);
      tar.second += weight;
      std::sort(tars.begin(),tars.end(),byWeight());
    }

    size_t size() const {return _keys.size();}

    const std::vector<T1*>& keys() const {return _keys;}
    
    bool has_key(const T1& ct1) const {
      T1* t1 = (T1*) &ct1;
      return (_relations.find(t1) != _relations.end());
    }

    const targets& relations(const T1& ct1) const {
      T1* t1 = (T1*) &ct1; 
      if (has_key(ct1)) return ((_relations.find(t1))->second);
      std::cout << " ERROR no relations to " << t1 << std::endl;
    }
    
    const T2& best_relation(const T1& ct1, double& weight) const {
      const targets& tars = relations(ct1);
      weight = tars[0].second;
      return *(tars[0].first); 
    }

    relation<T2,T1> reverse() const {
      relation<T2,T1> rel;
      for (typename std::vector<T1*>::const_iterator it = _keys.begin(); 
           it != _keys.end(); ++it) {
        T1* t1 = *it;
        const targets& tars = _relations.find(t1)->second;
        for (typename targets::const_iterator it2 = tars.begin();
             it2 != tars.end(); ++it2) {
          T2* t2 = (*it2).first;
          double w = (*it2).second;
          rel.relate(*t2,*t1,w);
        }
      }
      return rel;
    }

    void info() const {
      std::cout << " size " << _keys.size() << std::endl;
      for (typename std::vector<T1*>::const_iterator it = _keys.begin(); 
           it != _keys.end(); ++it) {
        T1* t1 = *it;
        const targets& tars = _relations.find(t1)->second;
        std::cout << " targets size " << tars.size() << std::endl;
        for (typename targets::const_iterator it2 = tars.begin();
             it2 != tars.end(); ++it2) {
          T2* t2 = (*it2).first;
          double w = (*it2).second;
          std::cout  << "    t1 " << t1 
                     << " -->  t2 " << t2 << " w " << w << std::endl;
        }
      }
    }

  protected:
    
    class byWeight {
    public:
      bool operator() (const target& t1, const target& t2) const 
        {return (t1.second > t2.second);}
    };

    targets& get_targets(T1* t1) {
      if (_relations.find(t1) == _relations.end()) {
        //        std::cout << " creating targets t1 " << t1 << std::endl;
        _relations[t1] = targets();
        _keys.push_back(t1);
      }
      return _relations[t1];
    }
    
    
    target& get_target(targets& tars, T2* t2) {
      for (typename targets::iterator it = tars.begin(); it != tars.end(); ++it)
        if ((*it).first == t2) return (*it);
      // std::cout << " creating target t2 " << t2 << std::endl;
      tars.push_back( target(t2,0.));
      target& tar = tars.back();
      return tar;
    }      
    
  protected:
    std::vector<T1*> _keys;
    std::map<T1*, targets > _relations;
  };
  
}

  
#endif
