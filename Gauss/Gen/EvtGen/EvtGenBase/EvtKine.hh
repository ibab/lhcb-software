//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtKine.hh
//
// Description:routines to calc. decay angles.
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------

#ifndef EVTKINE_HH
#define EVTKINE_HH

class EvtVector4R;

double EvtDecayAngle(const EvtVector4R&, const EvtVector4R&,
		     const EvtVector4R&);

double EvtDecayAngleChi(const EvtVector4R&, const EvtVector4R&, 
			const EvtVector4R&, const EvtVector4R&, 
			const EvtVector4R& );

//
// This routine calculates the cosine of the angle between
// the normal of the decay plane and the flight direction of particle q
// in the parent frame.
//
double EvtDecayPlaneNormalAngle(const EvtVector4R& p,const EvtVector4R& q,
                          const EvtVector4R& d1,const EvtVector4R& d2);

#endif

