#include "GaudiRandomForHerwigpp.h"
#include "ThePEG/Interface/ClassDocumentation.h"

using namespace ThePEG;

GaudiRandomForHerwigpp::GaudiRandomForHerwigpp()
{
  // initialize random number service
  IRndmGenSvc* i(0);
  // FIXME
  /*
    This module should fail when used within Herwig++. The random number
    generator service is NOT initialized properly. This needs looking into.
   */
  StatusCode sc = m_random.initialize(i, Rndm::Flat(0,1));
}

IBPtr GaudiRandomForHerwigpp::clone() const
{
  return new_ptr(*this);
}

IBPtr GaudiRandomForHerwigpp::fullclone() const
{
  return new_ptr(*this);
}

void GaudiRandomForHerwigpp::setSeed(long seed)
{
  if ( seed == -1 ) seed = 19940801;
}

void GaudiRandomForHerwigpp::fill()
{
  for (int i = 0, N = theNumbers.size(); i < N; ++i)
    {
      theNumbers[i] = m_random.shoot();
    }
  nextNumber = theNumbers.begin();
}

ClassDescription<GaudiRandomForHerwigpp> GaudiRandomForHerwigpp::initGaudiRandomForHerwigpp;

void GaudiRandomForHerwigpp::Init()
{
  
  static ClassDocumentation<GaudiRandomForHerwigpp> documentation
    ("Interface to the GAUDI random number generator.");
}

