//-----------------------------------------------------------------------
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// File and Version Information:
// EvtPFermi.hh 
//
// Description:
//   F(k+) = N (1-x)^a exp((1-a)x) ,x=k+/(mB-mb) 
//   the fermi motion distribution according to
//   hep-ph/9905351 v2
//   
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Sven Menke   
//
//-----------------------------------------------------------------------

#ifndef EVTPFERMI_HH
#define EVTPFERMI_HH

class EvtPFermi {

public:
  
  // Constructors

  EvtPFermi(const double &a, const double &mB, const double &mb);

  // Destructor

  virtual ~EvtPFermi( );

  // Operators

  // Selectors 

  // Modifiers

  // Methods

  double getFPFermi(const double &kplus);

protected:
  
  // Helper functions

private:

  // Friends
  
  // Data members

  double _a;
  double _mb;
  double _mB;
};


#endif // EVTPFERMI_HH


