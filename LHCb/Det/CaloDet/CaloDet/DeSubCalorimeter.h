#ifndef   CALODET_DESUBCALORIMETER_H
#define   CALODET_DESUBCALORIMETER_H   1


// Include files
#include "DetDesc/DetectorElement.h"

#include "CaloDet/CLIDDeSubCalorimeter.h"

class MsgStream;


//------------------------------------------------------------------------------
//
// ClassName:   DeSubCalorimeter 
//  
// Description: Example of a specific detector element class.
//
//------------------------------------------------------------------------------


class DeSubCalorimeter: public DetectorElement {
  
public:

// ** Constructors

  DeSubCalorimeter( const std::string& name    = ""  );

// ** Destructor

  ~DeSubCalorimeter();

// ** Retrieve refrence to class identifier

  virtual const CLID& clID   () const { return classID()             ; } 
  static  const CLID& classID()       { return CLID_DeSubCalorimeter ; } 

  virtual std::ostream& printOut( std::ostream& os = std::cerr ) const ; 
  virtual MsgStream&    printOut( MsgStream&                   ) const ; 

// ** Get/Set size

  inline double size   ()            const    { return m_size; };
  inline void   setSize( double s )           { m_size = s;    };

private:

  double    m_size;      // ** Cell dimension

};
///
#include "CaloDet/DeSubCalorimeter.icpp"
///

#endif    //   CALODET_DESUBCALORIMETER_H
