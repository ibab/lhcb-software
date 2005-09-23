#ifndef _MBM_MANAGER_H
#define _MBM_MANAGER_H

struct BMDESCRIPT;

namespace MBM {
  struct Manager  {
    BMDESCRIPT* m_bm;
    char ctrl_mod[32];
    char buff_mod[32];
    char user_mod[32];
    char event_mod[32];
    char bitmap_mod[32];
    char* bm_id;     /* Buffer id                   */
    Manager();
    virtual ~Manager();
    static int mapSections(Manager* m);
    virtual int  optparse (const char* c) = 0;
    void getOptions(int argc, char** argv);
    void setup(const char* id=0);
  };
}
#endif // _MBM_MANAGER_H
