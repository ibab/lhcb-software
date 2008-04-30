// $Id: ClusterDisplay.h,v 1.1 2008-04-30 08:39:24 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/ClusterDisplay.h,v 1.1 2008-04-30 08:39:24 frankb Exp $
#ifndef ROLOGGER_CLUSTERDISPLAY_H
#define ROLOGGER_CLUSTERDISPLAY_H

// C++ include files
#include <vector>

// Framework include files
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class ClusterDisplay ClusterDisplay.h ROLogger/ClusterDisplay.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class ClusterDisplay : public Interactor {

  protected:
    
    /// Cluster definition
    typedef std::vector<std::string> Nodes;

    /// Menu id
    int         m_id;
    /// Cluster name
    std::string m_name;
    /// Container of nodes in this cluster
    Nodes       m_nodes;
    /// Parent object interactor
    Interactor* m_parent;
    /// Reference to output logger interactory
    Interactor* m_logger;

  public:
    /// Standard constructor with object setup through parameters
    ClusterDisplay(Interactor* parent, Interactor* log, const std::string& name, const Nodes& nodes);
    /// Standard destructor
    virtual ~ClusterDisplay();
    /// Display callback handler
    virtual void handle(const Event& ev);
  };

}      /* End namespace ROLogger */
#endif /* ROLOGGER_CLUSTERDISPLAY_H */
