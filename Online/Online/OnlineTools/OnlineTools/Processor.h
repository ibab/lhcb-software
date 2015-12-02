// $Id: Processor.h,v 1.1 2008-09-18 13:08:22 frankb Exp $
//====================================================================
//  OnlineTools
//--------------------------------------------------------------------
//
//  Package    : OnlineTools
//
//  Description: 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineTools/OnlineTools/Processor.h,v 1.1 2008-09-18 13:08:22 frankb Exp $
#ifndef ONLINETOOLS_PROCESSOR_H
#define ONLINETOOLS_PROCESSOR_H

// C++ include files
#include <map>
#include <string>
#include <vector>

// Framework include files
#include "CPP/Interactor.h"

/*
 *   OnlineTools namespace declaration
 */
namespace OnlineTools {


  class NodeManipulator : public CPP::Interactor {
  protected:
    typedef std::string _S;
    _S          m_name;
    CPP::Interactor* m_exec;
  public:
    typedef std::vector<NodeManipulator*> Children;
    NodeManipulator(const _S& n, CPP::Interactor* i) : m_name(n), m_exec(i) {}
    virtual ~NodeManipulator()               {                    }
    const _S& name() const                   { return m_name;     }
    void setName(const _S& n)                { m_name = n;        }
    virtual bool hasChildren()  const        { return false;      }
    virtual size_t numChildren() const       { return 0;          }
    virtual Children children()              { return Children(); }
    virtual int doPing() = 0;
    virtual int doReset() = 0;
    virtual int doStatus() = 0;
    virtual int doSwitchON() = 0;
    virtual int doSwitchOFF() = 0;
    virtual int add(NodeManipulator*,const _S&,const _S&,const _S&,const _S&) {  return 1; }
  };

  class ProcessorGroup : public NodeManipulator  {
  public:
    typedef std::map<std::string,NodeManipulator*> SubGroups;
  protected:
    SubGroups m_groups;
    template <class T> int _exec(T pmf)  {
      for(SubGroups::iterator i=m_groups.begin(); i != m_groups.end(); ++i)
	((*i).second->*pmf)();
      return 1;
    }
  public:
    ProcessorGroup(const std::string& n,CPP::Interactor* i) : NodeManipulator(n,i) {}
    virtual ~ProcessorGroup() {}
    virtual size_t numChildren() const       { return m_groups.size();        }
    virtual bool hasChildren()  const        { return m_groups.size()>0;      }
    virtual Children children();
    virtual int doPing()      {   return _exec(&NodeManipulator::doPing); }
    virtual int doReset()     {   return _exec(&NodeManipulator::doReset); }
    virtual int doStatus()    {   return _exec(&NodeManipulator::doStatus); }
    virtual int doSwitchON()  {   return _exec(&NodeManipulator::doSwitchON); }
    virtual int doSwitchOFF() {   return _exec(&NodeManipulator::doSwitchOFF); }
    virtual int add(NodeManipulator* item,
		    const std::string& a1, 
		    const std::string& a2="", 
		    const std::string& a3="", 
		    const std::string& a4="");
  };


  /**@class Processor Processor.h OnlineTools/Processor.h
   *
   *   @author M.Frank
   */
  class Processor : public NodeManipulator {
  public:
    /// Standard constructor with object setup through parameters
    Processor(const std::string& name, CPP::Interactor* i);
    /// Standard destructor
    virtual ~Processor();
    virtual int doPing();
    virtual int doReset();
    virtual int doStatus();
    virtual int doSwitchON();
    virtual int doSwitchOFF();
    int _send(const std::string& m, const std::string& d);
  };

}      /* End namespace OnlineTools */
#endif /* ONLINETOOLS_PROCESSOR_H */
