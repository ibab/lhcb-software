#define MBM_IMPLEMENTATION
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include "bm_struct.h"
#include "bm_internals.h"
#include "Installer.h"

static void help()  {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Syntax: mbm_install/mbm_deinstall [<-opt>]                            \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Function: Buffer Manager Installation                                 \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Options:                                                              \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s=<size> [10]      Buffer size (kbytes)                          \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -e=<max>  [32]      Maximum number of events                      \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -u=<max>  [5]       Maximum number of users                       \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -i=<id>   [ ]       Buffer Identifier                             \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -b=<nbits>[10]      2**nbits block size                           \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -f        [ ]       force deinstall                               \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -m        [ ]       Start monitor after installer                 \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -c        [ ]       Do not keep process alive; continue execution \n");
}

static int _mbm_installer_shutdown(void* param) {
  std::pair<void*,int>* p = (std::pair<void*,int>*)param;
  BUFFERS* b = (BUFFERS*)p->first;
  int id = p->second;
  if ( b->buffers[id].used != 0 )
    b->nbuffer--;
  ::memset(&b->buffers[id],0,sizeof(BUFFERS::BUFF));
  delete p;
  return 1;
}

MBM::Installer::Installer(int argc, char **argv)  {
  p_continue = 0;
  p_moni   = 0;
  p_emax   = 32;
  p_umax   = 5;
  p_size   = 10;
  p_force  = 0;
  p_bits   = 10;
  m_bm = new BMDESCRIPT();
  getOptions(argc, argv);
}

MBM::Installer::~Installer()  {
  // We do NOT want to unload the global sections!
  m_bm = 0;
}

int MBM::Installer::optparse (const char* c)  {
  int iret;
  switch (*c | 0x20)    {
  case 's':            /*      size of buffer        */   
    iret = sscanf(c+1,"=%d",&p_size);
    if( iret != 1 )       {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error reading buffer size parameter\n");
      throw std::runtime_error("Error reading buffer size parameter");
    }
    p_size = (p_size>>1)<<1;
    break;
  case 'e':            /*      maximum events        */   
    iret = sscanf(c+1,"=%d",&p_emax);
    if( iret != 1 )       {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error reading maximum events parameter\n");
      throw std::runtime_error("Error reading  maximum events parameter");
    }
    break;
  case 'u':            /*      maximum users        */   
    iret = sscanf(c+1,"=%d",&p_umax);
    if( iret != 1 )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error reading maximum users parameter\n");
      throw std::runtime_error("Error reading  maximum users parameter");
    }
    if( p_umax > 128 )    {
      ::lib_rtl_output(LIB_RTL_ERROR,"Maximum users exeeded maximum (128)\n");
      throw std::runtime_error("Maximum users exeeded maximum (128)");
    }
    break;
  case 'b':            /*      block size          */   
    iret = sscanf(c+1,"=%d",&p_bits);
    if( iret != 1 )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error reading block size parameter\n");
      throw std::runtime_error("Error reading  block size parameter");
    }
    if( p_bits > 20 )    {
      ::lib_rtl_output(LIB_RTL_ERROR,"Block size exeeded maximum (20)\n");
      throw std::runtime_error("Block size exeeded maximum (20)");
    }
    break;
  case 'i':            /*      maximum users        */   
    iret = sscanf(c+1,"=%s",buff_id);
    if( iret != 1 )       {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error reading Buffer identifier parameter\n");
      throw std::runtime_error("Error reading Buffer identifier parameter");
    }
    ::strcpy(m_bm->bm_name,buff_id);
    bm_id = buff_id;
    break;
  case 'f':
    p_force = 1;
    break;
  case 'm':
    p_moni = 1;
    break;
  case 'c':
    p_continue = 1;
    break;
  case '?':
  case 'h':
    help();
    throw std::runtime_error("Help only requested");
  default:
    help();
    throw std::runtime_error("Bad parameter supplied!");
  }
  return MBM_NORMAL;
}

int MBM::Installer::install()  {
  int len, icode = deinstall();
  if(icode != MBM_NORMAL) return icode;
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ Commencing BM installation \n");
  int status = ::lib_rtl_create_section(ctrl_mod,sizeof(CONTROL),&m_bm->ctrl_add);
  if(!::lib_rtl_is_success(status))   {   
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot create section %s. Exiting....",ctrl_mod);
    return status;
  }
  m_bm->ctrl = (CONTROL*)m_bm->ctrl_add->address;
  ::memset(m_bm->ctrl,0,sizeof(CONTROL));
  m_bm->ctrl->shift_p_Bit = p_bits;
  m_bm->ctrl->bytes_p_Bit = (1<<p_bits);
  ::lib_rtl_output(LIB_RTL_INFO,"Control: %p  %08X             [%d Bytes]\n",(void*)m_bm->ctrl,
           ((char*)m_bm->ctrl)-((char*)m_bm->ctrl), sizeof(CONTROL));

  len = sizeof(USERDesc)+sizeof(USER)*(p_umax-1);
  status = ::lib_rtl_create_section(user_mod, len ,&m_bm->user_add);
  if(!::lib_rtl_is_success(status))   {   
    ::lib_rtl_delete_section(m_bm->ctrl_add);
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot create section %s. Exiting....",user_mod);
    return status;
  }
  m_bm->usDesc = (USERDesc*)m_bm->user_add->address;
  m_bm->user   = m_bm->usDesc->users;
  ::memset(m_bm->usDesc,0,len);
  ::memset(m_bm->user,0,sizeof(USER)*p_umax);
  ::lib_rtl_output(LIB_RTL_INFO,"User:    %p  %08X  %p   [%d Bytes]\n",(void*)m_bm->user,
           ((char*)m_bm->user)-((char*)m_bm->ctrl),(void*)m_bm->usDesc, len);

  len = sizeof(EVENTDesc)+sizeof(EVENT)*(p_emax-1);
  status = ::lib_rtl_create_section(event_mod, len ,&m_bm->event_add);
  if(!::lib_rtl_is_success(status))   {   
    ::lib_rtl_delete_section(m_bm->ctrl_add);
    ::lib_rtl_delete_section(m_bm->user_add);
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot create section %s. Exiting....",event_mod);
    return status;
  }
  m_bm->evDesc = (EVENTDesc*)m_bm->event_add->address;
  m_bm->event = m_bm->evDesc->events;
  ::memset(m_bm->evDesc,0,len);
  ::lib_rtl_output(LIB_RTL_INFO,"Event:   %p  %08X  %p   [%d Bytes]\n",(void*)m_bm->event,
           ((char*)m_bm->event)-((char*)m_bm->ctrl),(void*)m_bm->evDesc, len);

  len = (((p_size<<10)/m_bm->ctrl->bytes_p_Bit)<<3);
  status = ::lib_rtl_create_section(bitmap_mod,len,&m_bm->bitm_add);
  if(!::lib_rtl_is_success(status))   {
    ::lib_rtl_delete_section(m_bm->ctrl_add);
    ::lib_rtl_delete_section(m_bm->user_add);
    ::lib_rtl_delete_section(m_bm->event_add);
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot create section %s. Exiting....",bitmap_mod);
    return status;
  }
  len = ((p_size<<10)/m_bm->ctrl->bytes_p_Bit)<<3;
  m_bm->bitmap = (char*)m_bm->bitm_add->address;
  ::memset(m_bm->bitmap,0,len);
  ::lib_rtl_output(LIB_RTL_INFO,"Bitmap:  %p  %08X             [%d Bytes]\n",(void*)m_bm->bitmap,
           ((char*)m_bm->bitmap)-((char*)m_bm->ctrl), len);

  len = p_size<<10;
  status = ::lib_rtl_create_section(buff_mod,len,&m_bm->buff_add);
  if(!::lib_rtl_is_success(status))   {   
    ::lib_rtl_delete_section(m_bm->ctrl_add);
    ::lib_rtl_delete_section(m_bm->user_add);
    ::lib_rtl_delete_section(m_bm->event_add);
    ::lib_rtl_delete_section(m_bm->buff_add);
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot create section %s. Exiting....",buff_mod);
    return status;
  }
  m_bm->buffer_add = (char*)m_bm->buff_add->address;
  ::memset(m_bm->buffer_add,0xDD,len);
  ::lib_rtl_output(LIB_RTL_INFO,"Buffer:  %p  %08X             [%d Bytes]\n",(void*)m_bm->buffer_add,
           ((char*)m_bm->buffer_add)-((char*)m_bm->ctrl), len);

  CONTROL* ctrl  = m_bm->ctrl;
  USER*    user  = m_bm->user;
  EVENT*   event = m_bm->event;
  ctrl->p_umax       = p_umax;
  ctrl->p_emax       = p_emax;
  ctrl->buff_size    = p_size<<10; /* in bytes*/
  ctrl->tot_produced = 0;
  ctrl->tot_actual   = 0;
  ctrl->tot_seen     = 0;
  ctrl->i_events     = 0;
  ctrl->i_space      = (p_size<<10)/ctrl->bytes_p_Bit; /*in Bits*/
  ctrl->last_bit     = 0;
  ctrl->last_alloc   = 0;
  ctrl->bm_size      = (p_size<<10)/ctrl->bytes_p_Bit; /*in bits*/
  ctrl->spare1       = 0;
  for (int i=0;i<p_umax;i++)  {
    user[i].block_id = BID_USER;
    user[i].uid      = i;
  }
  for (int j=0;j<p_emax;j++)  {
    event[j].block_id = BID_EVENT;
    event[j].eid      = j;
  }
  status = mbm_map_global_buffer_info(&bm_all,true);
  if(!::lib_rtl_is_success(status))   {   
    ::lib_rtl_delete_section(m_bm->ctrl_add);
    ::lib_rtl_delete_section(m_bm->user_add);
    ::lib_rtl_delete_section(m_bm->event_add);
    ::lib_rtl_delete_section(m_bm->buff_add);
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot map global buffer information....\n");
    return status;
  }

  BUFFERS* buffs = (BUFFERS*)bm_all->address;
  for(int i=0; i<buffs->p_bmax; ++i)  {
    if ( ::strcmp(buffs->buffers[i].name,bm_id)==0 )  {
      buffs->buffers[i].used = 1;
      ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ BM installation [%s] finished successfully.\n",m_bm->bm_name);
      lib_rtl_declare_exit (_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      lib_rtl_declare_rundown(_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      return MBM_NORMAL;
    }
  }
  for(int i=0; i<buffs->p_bmax; ++i)  {
    if ( 0 == buffs->buffers[i].used )  {
      buffs->buffers[i].used = 1;
      ::strcpy(buffs->buffers[i].name, m_bm->bm_name);
      buffs->nbuffer++;
      lib_rtl_declare_exit (_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      lib_rtl_declare_rundown(_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ BM registration successful:%d buffers now\n",buffs->nbuffer);
      break;
    }
  }
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ BM installation [%s] finished successfully.\n",m_bm->bm_name);
  return MBM_NORMAL;
}

int MBM::Installer::deinstall()  {
  ::lib_rtl_gbl_t handle = 0;
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ Commencing BM deinstallation \n");
  int status = ::lib_rtl_map_section(ctrl_mod, sizeof(CONTROL), &handle);
  if (::lib_rtl_is_success(status) && p_force != 1)  {
    m_bm->ctrl = (CONTROL*)handle->address;
    if( m_bm->ctrl->i_users > 0 )    {
      ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ Unable to de-install BM (%d users still active)\n", m_bm->ctrl->i_users);
      return MBM_ERROR;
    }
  }
  if (::lib_rtl_is_success(status))
    status = ::lib_rtl_delete_section(handle);
  if (!::lib_rtl_is_success(status))  {
    ::lib_rtl_output(LIB_RTL_INFO,"problem deleting section %s status %d\n",ctrl_mod,status);
  }
  status = ::lib_rtl_map_section(user_mod, sizeof(USER), &handle);
  if (::lib_rtl_is_success(status))
    status = ::lib_rtl_delete_section(handle);
  if (!::lib_rtl_is_success(status))  {
    ::lib_rtl_output(LIB_RTL_INFO,"problem deleting section %s status %d\n",user_mod,status);
  }
  status = ::lib_rtl_map_section(buff_mod, 1, &handle);
  if (::lib_rtl_is_success(status))
    status = ::lib_rtl_delete_section(handle);
  if (!::lib_rtl_is_success(status))  {
    ::lib_rtl_output(LIB_RTL_INFO,"problem deleting section %s status %d\n",buff_mod,status);
  }
  status = ::lib_rtl_map_section(event_mod, 1, &handle);
  if (::lib_rtl_is_success(status))
    status = ::lib_rtl_delete_section(handle);
  if (!::lib_rtl_is_success(status))  {
    ::lib_rtl_output(LIB_RTL_INFO,"problem deleting section %s status %d\n",event_mod,status);
  }
  status = ::lib_rtl_map_section(bitmap_mod, 1, &handle);
  if (::lib_rtl_is_success(status))
    status = ::lib_rtl_delete_section(handle);
  if (!::lib_rtl_is_success(status))  {
    ::lib_rtl_output(LIB_RTL_INFO,"problem deleting section %s status %d\n",bitmap_mod,status);
  }
  status = mbm_map_global_buffer_info(&bm_all,true);
  if(!::lib_rtl_is_success(status))
    return 1;
  BUFFERS* buffs = (BUFFERS*)bm_all->address;
  for(int i=0; i<buffs->p_bmax; ++i)  {
    if ( ::strcmp(buffs->buffers[i].name,bm_id)==0 )  {
      buffs->buffers[i].used = 0;
      buffs->nbuffer =  (buffs->nbuffer>0) ? buffs->nbuffer - 1 : 0;
      ::memset(buffs->buffers[i].name,0,sizeof(buffs->buffers[i].name));
      break;
    }
  }
  mbm_unmap_global_buffer_info(bm_all);
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ Old BM [%s] de-installed successfully\n",m_bm->bm_name);
  return MBM_NORMAL;
}

int mbm_install(int argc , char** argv) {
  try  {
    MBM::Installer inst(argc, argv);
    int sc = inst.install();
    if ( sc == MBM_NORMAL )  {
      if ( inst.startMonitor() )  {
        lib_rtl_sleep(1000);
        mbm_mon(0, argv); 
      }
      else if ( !inst.continueInstallation() ) {
        for(;;) ::lib_rtl_sleep(10000);
      }
    }
    ::lib_rtl_output(LIB_RTL_INFO,"All done.\n");
    return sc;
  }
  catch (std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_ERROR,"MBM initialization failed: %s\n",e.what());
  }
  return MBM_ERROR;
}

int mbm_deinstall(int argc , char** argv) {
  MBM::Installer inst(argc, argv);
  return inst.deinstall();
}

extern "C" int mbm_remove(int argc, char** argv) {
  MBM::Installer inst(argc,argv);
  return inst.deinstall();
}

extern "C" int mbm_install_qmtest(int argc , char** argv) {
  int ret = mbm_install(argc,argv);
  return (ret == 1) ? 0 : ret;
}

