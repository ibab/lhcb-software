
#ifndef RICHMARKOV_ALTERATIONS_H
#define RICHMARKOV_ALTERATIONS_H

#include <list>
#include <map>
#include <iostream>
#include <vector>

#include "RichPriors.h"

#include "CircleParams.fwd"

namespace RichMarkov {

  template <class Mode, const Mode & mode>
  class Alterations {
  public:
    typedef CircleParams<Mode,mode> CircleParamsT;
    typedef RichPriors<Mode,mode>   RichPriorsT;
  public:
    typedef std::vector<CircleParamsT> Circs;
  public:

    class Alteration {
    private:
      virtual void printMyInnardsTo(std::ostream & os) const = 0;

    public:
      inline void printMeTo(std::ostream & os) const {
        os << "Alteration[";
        printMyInnardsTo(os);
        os << "]";
      }
      virtual ~Alteration() {};
      virtual void modifyCacheVectorEtc(double & ans,
                                        std::vector<double> & cache,
                                        const Hep2Vector & hit,
                                        const Circs & circs
                                        ) const = 0;
    public:
      static inline double bitToBeCached(const Hep2Vector & hit, const CircleParamsT & circle) {
        const double mu = circle.meanNumberOfHits();
        const double bitToBeAdded = mu * RichPriorsT::priorProbabilityOfHitDueToCircle(hit, circle);
        return bitToBeAdded;
      }
    };

    class DidNothing : public Alteration {
    public:
      DidNothing() {}
    private:
      virtual void printMyInnardsTo(std::ostream & os) const {
        os << "Did nothing.";
      }

      virtual void modifyCacheVectorEtc(double &,
                                        std::vector<double> &,
                                        const Hep2Vector &,
                                        const Circs & ) const {
        // Do absolutely nothing!
        static bool first = true;
        if (first) {
          std::cerr << "Note that once things are working, a default implementation " 
                    << "can be provided for Alterations::Alteration::modifyCacheVectorEtc(...) "
                    << "eliminating the need for Alterations::DidNothing to "
                    << "implement a do-nothing function." << std::endl;
          first = false;
        }
      }

    };

    class AlterationsNecessitateCompleteRecalculation : public Alteration {

    public:
      AlterationsNecessitateCompleteRecalculation() {
      };
    private:
      virtual void printMyInnardsTo(std::ostream & os) const {
        os << "Made a change so big as to necessitate a complete recalculation.";
      };
      virtual void modifyCacheVectorEtc(double &,
                                        std::vector<double> &,
                                        const Hep2Vector &,
                                        const Circs & ) const {
        // This should NEVER be called.  There is something seriously wrong if it ever is called!
        assert(false);
      };
    };
    class DeletedCircleAtBack : public Alteration {
    public:
      DeletedCircleAtBack() {
      };
    private:
      virtual void printMyInnardsTo(std::ostream & os) const {
        os << "Deleted circle at back.";
      };
      virtual void modifyCacheVectorEtc(double & ans,
                                        std::vector<double> & cache,
                                        const Hep2Vector & /*hit*/,
                                        const Circs & /*circs*/) const {
        // We have no use for hit and circs args here!
        assert(!(cache.empty()));
        ans -= cache.back();
        cache.pop_back();
      };
    };
    class CreatedCircleAtBack : public Alteration {
    public:
      CreatedCircleAtBack() {
      };
    private:
      virtual void printMyInnardsTo(std::ostream & os) const {
        os << "Created a cicle at the back.";
      };
      virtual void modifyCacheVectorEtc(double & ans, std::vector<double> & cache, const Hep2Vector & hit, const Circs & circs) const {
        const unsigned int i = cache.size(); // number of circle to be created!  This is a bit flakey too (see below) ... should really make more transparent.
        const CircleParamsT & circle = circs[i];
        const double thing = bitToBeCached(hit, circle);
        cache.push_back(thing);
        ans += thing;
      };
    };
    class AlteredCircle : public Alteration {
    public:
      const unsigned int index;
      AlteredCircle(const unsigned int i) : index(i) {
      };
    private:
      virtual void printMyInnardsTo(std::ostream & os) const {
        os << "Altered circle number " << index << ".";
      };
      virtual void modifyCacheVectorEtc(double & ans, std::vector<double> & cache, const Hep2Vector & hit, const Circs & circs) const {
        assert(cache.size()>index);
        ans -= cache[index];
        const CircleParamsT & circle = circs[index]; // this is a bit flakey; it depends on no circles being exchanged in the same edit as this one ...
        const double thing = bitToBeCached(hit, circle);
        ans += (cache[index]=thing);
      };
    };
    class ExchangedCircles : public Alteration {
    public:
      const unsigned int a;
      const unsigned int b;
      ExchangedCircles(const unsigned int aIndex,
                       const unsigned int bIndex) :
        a(aIndex),
        b(bIndex) {
      };
    private:
      virtual void printMyInnardsTo(std::ostream & os) const {
        os << "Exchanged circles number " << a << " and " << b << ".";
      };
      virtual void modifyCacheVectorEtc(double & /*ans*/,
                                        std::vector<double> & cache,
                                        const Hep2Vector & /*hit*/,
                                        const Circs & /*circs*/) const {
        assert(cache.size()>a);
        assert(cache.size()>b);
        const double t = cache[a];
        cache[a]=cache[b];
        cache[b]=t;
        // No change to ans.
        // No use of hit or circs
      };
    };
  public:
    typedef std::list<const Alteration *> List;
    typedef typename List::const_iterator const_iterator;
  private:
    List alterationList;
    inline void record(const Alteration * alt) {
      alterationList.push_back(alt);
    };
  public:
    //inline const List & getAlterationList() const {
    //  return alterationList;
    //};
    inline const_iterator begin() const {
      return alterationList.begin();
    };
    inline const_iterator end() const {
      return alterationList.end();
    };
    inline const Alteration & front() const {
      return *(alterationList.front());
    };
    inline const Alteration & back() const {
      return *(alterationList.back());
    };
    inline const bool empty() const {
      return alterationList.empty();
    };
    inline void alterationsNecessitateCompleteRecalculation() {
      record(new AlterationsNecessitateCompleteRecalculation());
    };
    inline void deletedAtBack() {
      record(new DeletedCircleAtBack());
    };
    inline void altered(unsigned int i) {
      record(new AlteredCircle(i));
    };
    inline void createdAtBack() {
      record(new CreatedCircleAtBack());
    };
    inline void exchanged(unsigned int a,
                          unsigned int b) {
      record(new ExchangedCircles(a, b));
    };
    inline void didNothing() {
      assert(alterationList.empty());
      record(new DidNothing);
    };
    inline void clear() {
      while (!alterationList.empty()) {
        delete alterationList.back();
        alterationList.pop_back();
      };
    };
    Alterations() {};
    ~Alterations() {
      // free up allocated memory
      clear();
    };
    void printMeTo(std::ostream & os) const {
      os << "AlterationList[";
      typename List::const_iterator it = alterationList.begin();
      if (it!=alterationList.end()) {
        while(true) {
          (*it)->printMeTo(os);
          ++it;
          if (it!=alterationList.end()) {
            os <<", ";
          } else {
            break;
          };
        };
      };
      os << "]";
    };
  };

}

template <class Mode, const Mode & mode>
inline std::ostream & operator<<(std::ostream & os, const RichMarkov::Alterations<Mode,mode> & a) {
  a.printMeTo(os);
  return os;
};


#endif
