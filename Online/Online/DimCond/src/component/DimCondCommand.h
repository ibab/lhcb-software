// $Id: DimCondCommand.h,v 1.1.1.1 2005-08-23 10:00:41 marcocle Exp $
#ifndef DIMCONDCOMMAND_H 
#define DIMCONDCOMMAND_H 1

// Include files
#include "dis.hxx"
#include "dic.hxx"

#include "Parser.h"

class ISvcLocator;
class IMessageSvc;
class IUpdateManagerSvc;
class ICondDBAccessSvc;


/** @class DimCondCommand DimCondCommand.h
 *  
 *  Specialized version of DimCommand that can be used to feed conditions to an Online job.
 *
 *  The format of the command is "C" (a string).
 *  The string is expected to be of the form
 *  <quote>command [options] [; command [options] ...]</quote>
 *  Possible commands are:<dl>
 *  <dt>add folder "/folder/path"
 *  <dd>Add to the CondDBCache an XML folder at the given path
 *  <dt>add object "/folder/path" "data string" <em>since</em> [<em>until</em>]
 *  <dd>Add to the given folder a new object containing the data "data string", the validity is
 *  from <em>since</em> to <em>until</em> or to +inf if <em>until</em> is not privided
 *  <dt>dump
 *  <dd>Tell the server to print out the content of the cache (mainly for debug purposes)
 *  </dl>
 *  
 *  @author Marco Clemencic
 *  @date   2005-06-17
 */
class DimCondCommand: public DimCommand {
public: 
  /// Standard constructor
  DimCondCommand(const std::string &name, IMessageSvc* msgsvc, IUpdateManagerSvc *ums, ICondDBAccessSvc *db);

  virtual ~DimCondCommand(); ///< Destructor

  /// Main method
  void commandHandler();

protected:

private:

  /// Pointer to the an instance of IMessageSvc
  IMessageSvc        *m_msgsvc;

  /// Pointer to the UpdateManagerSvc
  IUpdateManagerSvc  *m_ums;
  
  /// Pointer to the CondDBAccessSvc
  ICondDBAccessSvc   *m_db;

  /// Instance of the class used to parse the command.
  Parser             m_parser;
  
};
#endif // DIMCONDCOMMAND_H
