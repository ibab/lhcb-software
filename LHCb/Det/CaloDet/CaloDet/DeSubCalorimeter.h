#ifndef DDEXAMPLE_DESUBCALORIMETER_H
#define DDEXAMPLE_DESUBCALORIMETER_H 1


// Include files
#include "DetDesc/DetectorElement.h"


#include  "CaloDet/CLIDDeSubCalorimeter.h"



//------------------------------------------------------------------------------
//
// ClassName:   DeSubCalorimeter 
//  
// Description: Example of a specific detector element class.
//
//------------------------------------------------------------------------------


class DeSubCalorimeter: public DetectorElement {
  
public:

  //*** Constructors

  DeSubCalorimeter( const std::string& name    = ""  );

  //*** Destructor

  ~DeSubCalorimeter();

  //*** Retrieve refrence to class identifier

  virtual const CLID& clID   () const { return classID()             ; } 
  static  const CLID& classID()       { return CLID_DeSubCalorimeter ; } 

  //*** Get/Set size

  inline double size   ()            const    { return m_size; };
  inline void   setSize( double s )           { m_size = s;    };

private:

  double    m_size;      // Cell dimension

};

#endif    // DDEXAMPLE_DESUBCALORIMETER_H
