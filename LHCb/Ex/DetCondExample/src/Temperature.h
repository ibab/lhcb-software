//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/src/Temperature.h,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
#ifndef DETCONDEXAMPLE_TEMPERATURE_H
#define DETCONDEXAMPLE_TEMPERATURE_H 1

#include <string>

// Base class
#include "DetCond/ConditionData.h"

// Unique ID of the class
static const CLID CLID_Temperature = 197001;

///---------------------------------------------------------------------------
/** @class Temperature Temperature.h DetCond/Temperature.h

    A simple ConditionData class for temperature data.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class Temperature : public ConditionData

{

 public: 
  
  /// Default constructor 
  Temperature( );
  
  /// Constructor 
  Temperature( const double temperature );
  
  /// Destructor 
  virtual ~Temperature();
  
 public:

  // Re-implemented from DataObject

  /// Class ID of this instance
  inline virtual const CLID& clID   () const { return classID(); } 

  /// Class ID of this class
  inline static  const CLID& classID()       { return CLID_Temperature; };

 public: 

  // Re-implemented from ConditionData

  /// Printout to a message stream
  virtual MsgStream& printOut( MsgStream& os ) const;

 public: 

  /// Set temperature value
  void setTemperature( const double temperature );

  /// Retrieve temperature value
  const double temperature() const;

 private:

  /// Temperature value
  double m_temperature;
  
};

#endif // DETCONDEXAMPLE_TEMPERATURE_H


