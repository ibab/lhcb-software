#ifndef _MBM_INSTALLER_H
#define _MBM_INSTALLER_H

#include "Manager.h"

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
    int   p_size;         // buffer size
    int   p_force;        // force deinstall
    int   p_bits;         // Block size 
    char  buff_id[16];    // Buffer identifier
  public:
    Installer(int argc, char **argv);
    bool startMonitor() const { return p_moni > 0;    }
    int  continueInstallation()  const { return p_continue!=0; }
    virtual int  optparse (const char* c);
    int  deinstall();
    int  install();
  };
}
#endif // _MBM_INSTALLER_H
