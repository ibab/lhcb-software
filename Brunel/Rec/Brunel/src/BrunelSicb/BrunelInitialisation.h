// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/BrunelSicb/BrunelInitialisation.h,v 1.1.1.1 2001-03-14 16:06:46 cattaneb Exp $
#ifndef BRUNEL_INITIALISATION__H
#define BRUNEL_INITIALISATION__H 1


// Include files
#include "GaudiKernel/Algorithm.h"

//------------------------------------------------------------------------------
//
// ClassName:   BrunelInitialisation
//  
// Description: Generic Brunel initialisation, independent of phase
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
//------------------------------------------------------------------------------


class BrunelInitialisation : public Algorithm {
public:
	// Construtor
	BrunelInitialisation( const std::string& name, ISvcLocator *svcloc );
	// Destructor
	virtual ~BrunelInitialisation();

    IAlgorithm* create(const std::string& name, ISvcLocator* loc);

	// Extend the default behaviour
	StatusCode BrunelInitialisation::initialize();
	StatusCode BrunelInitialisation::execute();
	StatusCode BrunelInitialisation::finalize();
private:

};

#endif    // BRUNEL_INITIALISATION__H