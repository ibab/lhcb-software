// $Id: DimCondCommand.cpp,v 1.1.1.1 2005-08-23 10:00:41 marcocle Exp $
// Include files 

#include <string>
#include <map>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/TimePoint.h"

#include "DetDesc/IUpdateManagerSvc.h"
#include "DetDesc/ICondDBAccessSvc.h"

// local
#include "DimCondCommand.h"
//#include "Utils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DimCondCommand
//
// 2005-06-17 : Marco Clemencic
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DimCondCommand::DimCondCommand(const std::string &name, IMessageSvc* msgsvc,
                               IUpdateManagerSvc* ums, ICondDBAccessSvc* db):
  DimCommand(name.c_str(),"C"), m_msgsvc(msgsvc), m_ums(ums), m_db(db), m_parser() {
  
  MsgStream log(m_msgsvc, "DimCondCommand");

  log << MSG::DEBUG << "DimCondCommand created with name \"" << name << "\"" <<endmsg;
  
}
//=============================================================================
// Destructor
//=============================================================================
DimCondCommand::~DimCondCommand() {
  MsgStream log(m_msgsvc, "DimCondCommand");
  log << MSG::DEBUG << "DimCondCommand deleted" <<endmsg;
}

//=========================================================================
//  
//=========================================================================
void DimCondCommand::commandHandler(){

  MsgStream log(m_msgsvc, "DimCondCommand");

  // Format:
  // "command [options] [; command [options] ...]"
  std::string command(getString());

  try {
    std::string::const_iterator pos = command.begin();
    std::vector<std::string> tokens;
    
    while ( pos != command.end() ){
      pos = m_parser.parse(pos,command.end(),tokens);
      if ( tokens.empty() ) continue; // skip empty statements
      
      if ( "add" == tokens[0] ) {
        if ( tokens.size() < 2 ) {
          log << MSG::ERROR << "syntax error: 'add' needs at least an extra argument" << endmsg;
          return;
        }
        if ( "folder" == tokens[1] ) {
          // ****************************** create-folder
          if ( tokens.size() < 3 ) {
            log << MSG::ERROR << "syntax error: 'add folder' needs the folder path" << endmsg;
            return;
          }
          StatusCode sc = m_db->cacheAddXMLFolder(tokens[2]);
          if ( !sc.isSuccess() ) {
            log << MSG::ERROR << "create-folder failed!" << endmsg;
            return;
          }
        } else if ( "object" == tokens[1] ) {
          // ****************************** add-object
          if ( tokens.size() < 5 ) {
            log << MSG::ERROR << "syntax error: 'add object' needs path, data, since and optionally until" << endmsg;
            return;
          }
          longlong since,until;
          std::istringstream is_since(tokens[4]);
          is_since >> since;
          if ( tokens.size() > 5 ) { // "until" is specified
            std::istringstream is_until(tokens[5]);
            is_until >> until;
          } else { // "until" not specified: use maximum
            until = time_absolutefuture.absoluteTime();
          }
          StatusCode sc = m_db->cacheAddXMLObject(tokens[2],since,until,tokens[3]);
          if ( !sc.isSuccess() ) {
            log << MSG::ERROR << "add-object failed! (couldn't insert into cache)" << endmsg;
            return;
          }
          // check if I have to change the values in UMS
          TimePoint old_since,old_until;
          if (m_ums->getValidity(tokens[2],old_since,old_until,true)){ // UMS knows the object
            if ( old_until.absoluteTime() > since ) { // it is the only possible change
              m_ums->setValidity(tokens[2],since,until,true);
            }
          }
        } else {
          log << MSG::ERROR << "syntax error: 'add' must be followed by 'folder' or 'object', what's '"
              << tokens[1] << "'?" << endmsg;
          return;
        }
      } else  if ( "dump" == tokens[0] ) {
        // ****************************** dump-cache
        m_db->dumpCache();
      } else {
        log << MSG::ERROR << "Unknown command: \"" << tokens[0] << "\"" << endmsg;
      }
    }
  }
  catch (ParserError &e) {
    log << MSG::ERROR << "ParserError: " << e.what() << endmsg; 
  }
}
//=============================================================================
