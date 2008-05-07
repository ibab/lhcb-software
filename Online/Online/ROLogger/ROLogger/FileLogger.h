// $Id: FileLogger.h,v 1.1 2008-05-07 17:58:53 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/FileLogger.h,v 1.1 2008-05-07 17:58:53 frankb Exp $
#ifndef ROLOGGER_FILELOGGER_H
#define ROLOGGER_FILELOGGER_H

// Framework include files
#include "ROLogger/MessageLogger.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {
  // Forward declarations
  class PartitionListener;

  /**@class FileLogger FileLogger.h ROLogger/FileLogger.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class FileLogger : public MessageLogger  {
  protected:
    /// Menu id
    int                m_id;
    /// Reference to partition listener
    PartitionListener* m_listener;
    /// Output directory
    std::string        m_outdir;

    /// Open new output file
    FILE* openOutput();

  public:
    /// Standard constructor with object setup through parameters
    FileLogger(int argc, char** argv);
    /// Standard destructor
    virtual ~FileLogger();
    /// Interactor callback handler
    virtual void handle(const Event& ev);
  };
}
#endif /* ROLOGGER_FILELOGGER_H */
