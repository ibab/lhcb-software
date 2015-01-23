// $Id: LokiTopoTag.h,v 1.1 2011-06-24 10:10:10 aphan Exp $


//-----------------------------------------------------------------------------
//
// bjet identification based on muons
//
//-----------------------------------------------------------------------------

#ifndef LOKITOPOTAG_H
#define LOKITOPOTAG_H 1

// ============================================================================
// Includes

// Kernel
#include "Kernel/ITriggerTisTos.h"

#include "Kernel/IJetTagTool.h"

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IHistogramSvc.h"


// ============================================================================
// Declaration
namespace LoKi
{
  class TopoTag
    : public virtual IJetTagTool
    , public GaudiTool
  {
    friend class ToolFactory<LoKi::TopoTag>;  // friend factory

    public:

      StatusCode initialize(); // standard initialise
      StatusCode finalize();   // standard finalise

      // Method that calculates the tag weight of a jet
      bool calculateJetProperty
      (
        const LHCb::Particle *jet,
	std::map <std::string,double > &jetWeight
      );

  protected:

      // standard constructor
      TopoTag
      (
        const std::string &type,
        const std::string &name,
        const IInterface *parent
      );

       // standard destructor
      ~TopoTag();

    private:

      TopoTag();                          // disable default constructor
      TopoTag(const TopoTag&);            // disable copy constructor
      TopoTag& operator=(const TopoTag&); // disable assignement operator

      ITriggerTisTos* m_TriggerTisTosTool;
      std::string m_TLine; // algorithm mode - PtRel or IPSig



  };
}

#endif
