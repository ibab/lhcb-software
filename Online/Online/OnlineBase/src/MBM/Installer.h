#ifndef _MBM_INSTALLER_H
#define _MBM_INSTALLER_H

#include "MBM/Manager.h"

/*
 *    MBM Namespace declaration
 */
namespace MBM {
  /* @class MBM::Installer  Installer.h  MBM/Installer.h
   *
   * @author  M.Frank
   * @version 1.0
   * @date    10/01/2006
   */
  struct Installer : public Manager  {
  private:
    int   p_continue;     // Do not keep process alive; continue execution
    int   p_moni;         // Start monitor
    int   p_emax;         // maximum events allowed 
    int   p_umax;         // maximun users
    int   p_tmax;         // maximun server threads
    int   p_size;         // buffer size
    int   p_force;        // force deinstall
    int   p_bits;         // Block size 
    char  buff_id[128];   // Buffer identifier
  public:
    /// Standard constructor
    Installer(int argc, char **argv);
    /// Standard destructor
    virtual ~Installer();
    /// Flag to indicate the start of the monitor after installation
    bool startMonitor() const { return p_moni > 0; }
    /// Continue with installation by processing further options
    int  continueInstallation()  const { return p_continue==1; }
    /// Start the MBM server threads asynchronously
    int  startBlocking()  const { return p_continue==0; }
    /// Parse options
    virtual int  optparse (const char* c);
    /// Deinstall buffers
    int  deinstall();
    /// Install buffers
    int  install();
  };
}
#endif // _MBM_INSTALLER_H
