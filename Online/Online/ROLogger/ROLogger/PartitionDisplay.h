// $Id: PartitionDisplay.h,v 1.5 2008-11-19 11:09:38 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/PartitionDisplay.h,v 1.5 2008-11-19 11:09:38 frankb Exp $
#ifndef ROLOGGER_PARTITIONDISPLAY_H
#define ROLOGGER_PARTITIONDISPLAY_H

// C++ include files
#include <map>
#include <vector>
#include <memory>

// Framework include files
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class PartitionDisplay PartitionDisplay.h ROLogger/PartitionDisplay.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class PartitionDisplay : public Interactor {

  protected:
    
    /// Partition definition
    typedef std::vector<std::string>    PVSSLogs;
    typedef std::vector<std::string>    Nodes;
    typedef std::vector<std::string>    Farms;
    typedef std::map<std::string,Nodes> Clusters;
    typedef std::map<int,std::pair<bool,std::string> >  Items;
    /// Menu id
    int         m_id;
    /// Menu cursor
    int         m_menuCursor;
    /// Partition name
    std::string m_name;
    /// Container of nodes in this cluster
    //Clusters    m_clusters;
    /// 
    Items       m_items;
    Nodes       m_nodes;
    Farms       m_farms;
    /// Storage slice used
    std::string m_storage;
    /// Monitoring slice in use
    std::string m_monitoring;
    /// Reconstruction slice in use
    std::string m_reconstruction;
    /// Parent object interactor
    Interactor* m_parent;
    /// Message logger interactor
    Interactor* m_msg;
    /// History logger interactor
    Interactor* m_history;

    /// Reference to output logger interactory
    std::auto_ptr<Interactor> m_clDisp;

    /// Match for wildcard node history match
    char        m_wildNode[64];
    /// Match for wildcard message history match
    char        m_wildMessage[64];
    /// Message window severity level
    char        m_msgSeverity[16];
    /// History window severity level
    char        m_histSeverity[16];
    /// Number of messages to display
    int         m_numMsg;

    /// Show processor cluster display
    void showCluster(int cmd);
    /// Show history according to node and message pattern match
    void showHistory(const char* node_match, const char* msg_match);

    /// Update farm content
    void updateFarms();

  public:
    /// Standard constructor with object setup through parameters
    PartitionDisplay(Interactor* parent, Interactor* msg, Interactor* hist, const std::string& name);
    /// Standard destructor
    virtual ~PartitionDisplay();
    /// Display callback handler
    virtual void handle(const Event& ev);
  };

}      /* End namespace ROLogger */
#endif /* ROLOGGER_PARTITIONDISPLAY_H */
