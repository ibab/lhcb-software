// $Id: IJetTagTool.h,v 1.1 2011/06/28 11:22:33 aphan Exp $

//-----------------------------------------------------------------------------
//
// Abstract interface for Jet Tag Tools
//
//-----------------------------------------------------------------------------

#ifndef IJETTAGTOOL_H
#define IJETTAGTOOL_H 1

// ============================================================================
// Includes

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/Particle.h"
#include "Event/RecVertex.h"


// ============================================================================
// Declaration

class GAUDI_API IJetTagTool : virtual public IAlgTool
{

  public:
    // Interface machinery
    DeclareInterfaceID(IJetTagTool, 2, 0);

    // Method that calculates a jet property
    //  Takes a jet (1st parameter)
    //  Returns the jet property (2nd parameter)
    virtual bool  calculateJetProperty
    (
      const LHCb::Particle *jet,
      std::map <std::string,double> &jetWeight

    ) = 0;

  protected:

  private:

};

#endif
