#ifndef DDEXAMPLE_DECALORIMETER_H
#define DDEXAMPLE_DECALORIMETER_H 1


// Include files
#include "DetDesc/DetectorElement.h"


// External declarations
extern const CLID& CLID_DECalorimeter;


//------------------------------------------------------------------------------
//
// ClassName:   DeCalorimeter 
//  
// Description: Example of a specific detector element class.
//
//------------------------------------------------------------------------------


class DeCalorimeter: public DetectorElement
{
public:
  // Constructors
  DeCalorimeter();
  DeCalorimeter( double s );

  // Destructor
  ~DeCalorimeter();

  const CLID& clID() const {
    return classID();
  }

  // Retrieve reference to class identifier
  static const CLID& classID();

  // Get/Set size
  double size();
  void   setSize( double s );

private:

  double    m_size;      // Cell dimension
};

#endif    // DDEXAMPLE_DECALORIMETER_H
