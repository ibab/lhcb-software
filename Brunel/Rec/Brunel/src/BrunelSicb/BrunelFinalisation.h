// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelFinalisation.h,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#ifndef BRUNEL_FINALISATION__H
#define BRUNEL_FINALISATION__H 1


// Include files
#include "GaudiKernel/Algorithm.h"

//------------------------------------------------------------------------------
//
// ClassName:   BrunelFinalisation
//  
// Description: Generic Brunel finalisation, independent of phase
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
//------------------------------------------------------------------------------


class BrunelFinalisation : public Algorithm {
public:
	// Construtor
	BrunelFinalisation( const std::string& name, ISvcLocator *svcloc );
	// Destructor
	virtual ~BrunelFinalisation();

    IAlgorithm* create(const std::string& name, ISvcLocator* loc);

	// Extend the default behaviour
	StatusCode BrunelFinalisation::initialize();
	StatusCode BrunelFinalisation::execute();
	StatusCode BrunelFinalisation::finalize();
private:

};

#endif    // BRUNEL_FINALISATION__H