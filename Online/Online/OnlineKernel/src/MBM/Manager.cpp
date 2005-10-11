#define MBM_IMPLEMENTATION
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "bm_struct.h"
#include "Manager.h"

MBM::Manager::Manager()  {
  bm_id = (char*)getenv("BM_ID");
  if(!bm_id) bm_id = "0";
  m_bm = new BMDESCRIPT;
  ::memset(m_bm,0,sizeof(BMDESCRIPT));
}

MBM::Manager::~Manager()  {
  delete m_bm;
}

void MBM::Manager::setup(const char* id)  {
  if ( id ) bm_id = (char*)id;
  strcpy(m_bm->bm_name,bm_id);
  strcat(strcpy(ctrl_mod,  "bm_ctrl_"),bm_id);
  strcat(strcpy(user_mod,  "bm_user_"),bm_id);
  strcat(strcpy(event_mod, "bm_event_"),bm_id);
  strcat(strcpy(bitmap_mod,"bm_bitmap_"),bm_id);
  strcat(strcpy(buff_mod,  "bm_buffer_"),bm_id);
}

void MBM::Manager::getOptions(int argc, char** argv)    {
  while (--argc > 0) {                                  /* process options  */
    const char* cptr = *++argv;
    if ( *cptr == '-' || *cptr == '/' )
      optparse (cptr+1);
  }
  setup();
}

int MBM::Manager::mapSections(Manager* m)  {
  BMDESCRIPT* dsc = m->m_bm;
  _CHECK( _mbm_map_section(m->ctrl_mod, sizeof(CONTROL), &dsc->ctrl_add) )
  dsc->ctrl = (CONTROL*)dsc->ctrl_add->address;
  _CHECK( _mbm_map_section(m->event_mod, dsc->ctrl->p_emax*sizeof(EVENT), &dsc->event_add) );
  dsc->event = (EVENT*)dsc->event_add->address;
  _CHECK( _mbm_map_section(m->user_mod, dsc->ctrl->p_umax*sizeof(USER), &dsc->user_add) );
  dsc->user = (USER*)dsc->user_add->address;
  _CHECK( _mbm_map_section(m->bitmap_mod, dsc->ctrl->bm_size, &dsc->bitm_add) );
  dsc->bitmap = (char*)dsc->bitm_add->address;
  _CHECK( _mbm_map_section(m->buff_mod, dsc->ctrl->buff_size, &dsc->buff_add) );
  dsc->buffer_add = (char*)dsc->buff_add->address;
  dsc->bitmap_size = dsc->ctrl->bm_size;
  dsc->buffer_size = dsc->ctrl->buff_size;
  dsc->ctrl->buff_ptr = dsc->buffer_add;
  dsc->bitmap_size = dsc->ctrl->bm_size;
  dsc->buffer_size = dsc->ctrl->buff_size;
  return 1;
}
