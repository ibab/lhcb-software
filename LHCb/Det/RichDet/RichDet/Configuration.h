#ifndef RICHDET_CONFIGURATION_H
#define RICHDET_CONFIGURATION_H 1

#include <cassert>
#include <vector>
//#include "RichGlobal/Definitions.h"

using namespace std;

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

  // Additional Public Declarations

protected:

  // Additional Protected Declarations

private:
  Configuration(const Configuration &right);

  Configuration & operator=(const Configuration &right);

  // Additional Private Declarations

private:
  // Data Members for Associations

  vector<Rich*> rich_;

  // Additional Implementation Declarations

};


// Class Rich::Configuration


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
