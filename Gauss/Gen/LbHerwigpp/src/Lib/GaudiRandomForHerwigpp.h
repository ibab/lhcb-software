/*
  This file is not used in the HerwigppProduction tool and is currently under
  development. The idea is to create a random number module that can be loaded
  into the ThePEG repository and called by Herwig++ instead of setting the
  random seed before generating each event. Currently the module compiles, but
  the Gaudi random number service needs to be initialized within the module.
 */
#ifndef ThePEG_GaudiRandomForHerwigpp_H
#define ThePEG_GaudiRandomForHerwigpp_H

// ThePEG
#include "ThePEG/Repository/RandomGenerator.h"
#include "ThePEG/Persistency/PersistentOStream.h"
#include "ThePEG/Persistency/PersistentIStream.h"

// Gaudi
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

namespace ThePEG {

class GaudiRandomForHerwigpp : public RandomGenerator{
      
 public:
  
  GaudiRandomForHerwigpp();
  virtual void setSeed(long seed);
  void persistentOutput(PersistentOStream & os) const;
  void persistentInput(PersistentIStream & is, int version);
  static void Init();

protected:

  virtual void fill();
  virtual IBPtr clone() const;
  virtual IBPtr fullclone() const;

private:

  // random number generator
  Rndm::Numbers m_random;

  static ClassDescription<GaudiRandomForHerwigpp> initGaudiRandomForHerwigpp;
  GaudiRandomForHerwigpp & operator=(const GaudiRandomForHerwigpp &);
};

/** This template specialization informs ThePEG about the base classes
 *  of GaudiRandomForHerwigpp. */
template <>
struct BaseClassTrait<GaudiRandomForHerwigpp,1>: public ClassTraitsType {
  // typedef of the first base class of GaudiRandomForHerwigpp
  typedef RandomGenerator NthBase;
};

// inform ThePEG about the GaudiRandomForHerwigpp class
template <>
struct ClassTraits<GaudiRandomForHerwigpp>: 
    public ClassTraitsBase<GaudiRandomForHerwigpp>
{
  static string className() { return "ThePEG::GaudiRandomForHerwigpp";}
};

}

#endif
