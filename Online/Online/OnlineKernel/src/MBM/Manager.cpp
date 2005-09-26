/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                       OS-9 BUFFER MANAGER  	                         */
/*                  INSTALLATION PROGRAM FOR THE AEB                     */
/*                                                                       */
/* Edition History                                                       */
/*                                                                       */
/*  #   Date    Comments                                       By        */
/* -- -------- ----------------------------------------------- ---       */
/*  0  28/09/88  Initial version                               PM        */
/*  1  11/11/88  Released version 1.0                          PM        */
/*  2  29/11/88  Spy and Multibuffer included                  PM        */
/*  3  15/12/88  Released version 2.0                          PM        */
/*  4  21/02/90  Now it can allocate buffer across memories    PM        */
/*-----------------------------------------------------------------------*/
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
  strcat(strcpy(ctrl_mod,"bm_ctrl_"),bm_id);
  strcat(strcpy(user_mod,"bm_user_"),bm_id);
  strcat(strcpy(event_mod,"bm_event_"),bm_id);
  strcat(strcpy(bitmap_mod,"bm_bitmap_"),bm_id);
  strcat(strcpy(buff_mod,"bm_buffer_"),bm_id);
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
  _CHECK( _mbm_map_section(m->ctrl_mod, sizeof(CONTROL), dsc->ctrl_add) )
  dsc->ctrl = dsc->ctrl_add[0];
  _CHECK( _mbm_map_section(m->event_mod, dsc->ctrl->p_emax*sizeof(EVENT), dsc->event_add) );
  dsc->event = dsc->event_add[0];
  _CHECK( _mbm_map_section(m->user_mod, dsc->ctrl->p_umax*sizeof(USER), dsc->user_add) );
  dsc->user = dsc->user_add[0];
  _CHECK( _mbm_map_section(m->bitmap_mod, dsc->ctrl->bm_size, dsc->bitm_add) );
  dsc->bitmap = dsc->bitm_add[0];
  _CHECK( _mbm_map_section(m->buff_mod, dsc->ctrl->buff_size, dsc->buff_add) );
  dsc->buffer_add = dsc->buff_add[0];
  dsc->bitmap_size = dsc->ctrl->bm_size;
  dsc->buffer_size = dsc->ctrl->buff_size;
  dsc->ctrl->buff_ptr = dsc->buffer_add;
  dsc->bitmap_size = dsc->ctrl->bm_size;
  dsc->buffer_size = dsc->ctrl->buff_size;
  return 1;
}
