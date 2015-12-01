// $Id: ErrShowDisplay.h,v 1.2 2008-05-27 18:42:03 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/ErrShowDisplay.h,v 1.2 2008-05-27 18:42:03 frankb Exp $
#ifndef ROLOGGER_ERRSHOWDISPLAY_H
#define ROLOGGER_ERRSHOWDISPLAY_H

// Framework include files
#include "CPP/Interactor.h"

// C++ include files
#include <string>
#include <vector>

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class ErrShowDisplay ErrShowDisplay.h ROLogger/ErrShowDisplay.h
   *
   *   Display to dump message history
   *
   *   @author M.Frank
   */
  class ErrShowDisplay : public CPP::Interactor {

  protected:
    typedef std::vector<std::string> Files;

    /// Menu id
    int         m_id;
    /// Parent object interactor
    Interactor* m_parent;
    /// Message logger interactor
    Interactor* m_msg;

    /**@Search criteria                         */
    /// Partition name
    char        m_name[16];

    /// Start time of message search
    char        m_startTime[32];
    /// End time of message search
    char        m_endTime[32];
    /// Message window severity level
    char        m_severity[16];
    /// Match for wildcard node history match
    char        m_node[32];
    /// Match for wildcard node history match
    char        m_process[32];
    /// Match for wildcard node history match
    char        m_component[32];
    /// Match for wildcard message history match
    char        m_message[64];
    /// Directory with log files
    char        m_logDir[128];
    /// File name for output file.
    char        m_outFileName[128];
    /// Number of messages to display
    int         m_numMsg;


    /// Retrieve all files corresponding to this request
    void getFiles(Files& files);
    /// Show all files corresponding to this request
    void showFiles();
    /// Save all messages corresponding to this request
    void saveMessages();
    /// Show all messages corresponding to this request
    void showMessages(FILE* output);
    /// Process messages from single file
    void processFile(const std::string& fname, FILE* output);

  public:
    /// Standard constructor with object setup through parameters
    ErrShowDisplay(CPP::Interactor* parent, CPP::Interactor* msg, const std::string& part="");
    /// Standard destructor
    virtual ~ErrShowDisplay();
    /// Display callback handler
    virtual void handle(const CPP::Event& ev);
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_ERRSHOWDISPLAY_H */
