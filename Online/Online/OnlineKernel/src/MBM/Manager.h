#ifndef _MBM_MANAGER_H
#define _MBM_MANAGER_H

struct BMDESCRIPT;

namespace MBM {
  struct Manager  {
    BMDESCRIPT* m_bm;
    char ctrl_mod[32];       // Control section name
    char buff_mod[32];       // Buffer section name
    char user_mod[32];       // Userdata section name
    char event_mod[32];      // Event data section name
    char bitmap_mod[32];     // Bitmap section name 
    const   char* bm_id;     // Buffer id
    lib_rtl_gbl_t bm_all;
    Manager();
    virtual ~Manager();
    virtual int mapSections();
    virtual int unmapSections();
    virtual int optparse (const char* c) = 0;
    virtual void getOptions(int argc, char** argv);
    virtual void setup(const char* id=0);
  };
}
#endif // _MBM_MANAGER_H
