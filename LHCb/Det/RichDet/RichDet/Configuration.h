#ifndef RICHDET_CONFIGURATION_H
#define RICHDET_CONFIGURATION_H 1

#include <cassert>
#include <vector>

#include "RichDet/Rich.h"
#include "RichDet/RichParameters.h"


// Has the overview on the RICH assembly.

class Configuration {

public:
  Configuration();

  virtual ~Configuration();

  virtual void update (const RichParameters &parameter);

  // Returns number of RICH detectors attached to a
  // particular configuration
  int nrRich () const;

  // Returns reference to a given rich
  const Rich & rich (Rich::RichID id) const;

private:
  Configuration(const Configuration &right);

  Configuration & operator=(const Configuration &right);

  std::vector<Rich*> rich_;

};


// ====================================================


inline int Configuration::nrRich () const
{
  return rich_.size();
}

inline const Rich & Configuration::rich (Rich::RichID id) const
{
  assert( rich_[id] != NULL );
  return *rich_[id];
}


#endif // RICHDET_CONFIGURATION_H
