// $Id: FilterDisplay.h,v 1.1 2008-05-13 07:56:02 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/FilterDisplay.h,v 1.1 2008-05-13 07:56:02 frankb Exp $
#ifndef ROLOGGER_FILTERDISPLAY_H
#define ROLOGGER_FILTERDISPLAY_H

// C++ include files
#include <map>
#include <vector>
#include <memory>

// Framework include files
#include "CPP/Interactor.h"
#include "ROLogger/Filter.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class FilterDisplay FilterDisplay.h ROLogger/FilterDisplay.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class FilterDisplay : public Interactor {
  public:
  protected:
    typedef std::vector<std::pair<int,Filter> > Filters;
    
    /// Menu id
    int         m_id;
    /// Parent object interactor
    Interactor* m_parent;
    /// Message logger interactor
    Interactor* m_msg;
    /// History logger interactor
    Interactor* m_history;

    char m_node[3][64];
    char m_utgid[3][64];
    char m_comp[3][64];
    char m_mess[3][64];
    char m_file[64];
    char m_buff[64];
    /// Known Filters
    Filters m_filters;

    /// Add all commands to build filters
    void addCommands();
    /// Remove all commands to build filters
    void removeCommands();
    /// Build filter object and add it to display
    void addFilter();
    /// Add filter object
    void addFilter(const Filter& f);
    /// Remove a filter
    void removeFilter(int which);
    /// Remove all filters
    void removeFilters();
    /// Save filers to file
    void saveFilters(const std::string& fname);
    /// Load filers to file
    void loadFilters(const std::string& fname);

  public:

    /// Standard constructor with object setup through parameters
    FilterDisplay(Interactor* parent, Interactor* msg, Interactor* hist);
    /// Standard destructor
    virtual ~FilterDisplay();
    /// Display callback handler
    virtual void handle(const Event& ev);
  };

}      /* End namespace ROLogger */
#endif /* ROLOGGER_FILTERDISPLAY_H */
