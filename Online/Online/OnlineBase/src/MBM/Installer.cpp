// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#define MBM_IMPLEMENTATION
#include "MBM/bmstruct.h"
#include "MBM/bmserver.h"
#include "MBM/bmmessage.h"
#include "bm_internals.h"
#include "Installer.h"
#include <stdexcept>

static void help()  {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Syntax: mbm_install/mbm_deinstall [<-opt>]                            \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Function: Buffer Manager Installation                                 \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"Options:                                                              \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s=<size> [10]      Buffer size (kbytes)                          \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -e=<max>  [32]      Maximum number of events                      \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -u=<max>  [5]       Maximum number of users                       \n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -t=<max>  [1]       Maximum number of server threads              \n");
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
  p_moni     = 0;
  p_emax     = 32;
  p_umax     = 5;
  p_tmax     = 1;
  p_size     = 10;
  p_force    = 0;
  p_bits     = 10;
  _mbm_connections_use_fifos(m_bm->communication);
  getOptions(argc, argv);
}

MBM::Installer::~Installer()  {
  // We do NOT want to unload the global sections!
  if ( m_bm ) delete m_bm;
  m_bm = 0;
}

int MBM::Installer::optparse (const char* c)  {
  int iret;
  char text[1024];
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
  case 't':            /*      maximum threads       */   
    iret = sscanf(c+1,"=%d",&p_tmax);
    if( iret != 1 )       {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error reading maximum thread parameter\n");
      throw std::runtime_error("Error reading  maximum thread parameter");
    }
    if ( p_tmax > BM_MAX_THREAD ) {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error: The maximal number of server threads may not exceed %d\n",BM_MAX_THREAD);
      throw std::runtime_error("Error: Maximal number of server threads too large.");
    }
    break;
  case 'u':            /*      maximum users        */   
    iret = sscanf(c+1,"=%d",&p_umax);
    if( iret != 1 )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"Error reading maximum users parameter\n");
      throw std::runtime_error("Error reading  maximum users parameter");
    }
    if( p_umax > (int)UserMask::digits() )    {
      ::sprintf(text,"Maximum users exeeded maximum (%d)\n",UserMask::digits());
      ::lib_rtl_output(LIB_RTL_ERROR,text);
      ::lib_rtl_output(LIB_RTL_ERROR,"Recompilation with larger BM_MASK_SIZE required!\n");
      throw std::runtime_error(text);
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
  case 'a':
    p_continue = 2;
    break;
  case 'x':
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Using boost::asio connections....\n");
    _mbm_connections_use_asio(m_bm->communication);
    break;
  case 'y':
    _mbm_connections_use_fifos(m_bm->communication);
    break;
  case 'z':
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Using boost::asio IPC connections....\n");
    _mbm_connections_use_unix(m_bm->communication);
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
  int bytes_per_bit, icode = deinstall();
  if(icode != MBM_NORMAL) return icode;
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ Commencing BM installation \n");
  size_t ctrl_len = mbm_section_length(sizeof(CONTROL));
  size_t evnt_len = mbm_section_length(sizeof(EVENTDesc)+sizeof(EVENT)*(p_emax-1));
  size_t user_len = mbm_section_length(sizeof(USERDesc)+sizeof(USER)*(p_umax-1));
  size_t bitm_len = mbm_section_length((((p_size<<10)/(1<<p_bits))<<3));
  size_t buff_len = p_size<<10;
  size_t len = ctrl_len+evnt_len+user_len+bitm_len;

  int status = ::lib_rtl_create_section(ctrl_mod,len,&m_bm->gbl_add);
  if(!::lib_rtl_is_success(status))   {   
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot create section %s. Exiting....\n",ctrl_mod);
    return status;
  }
  m_bm->gbl  = (char*)m_bm->gbl_add->address;
  ::memset(m_bm->gbl,0,len);
  m_bm->ctrl = (CONTROL*)m_bm->gbl;
  m_bm->ctrl->shift_p_Bit = p_bits;
  m_bm->ctrl->bytes_p_Bit = (1<<p_bits)-1;
  bytes_per_bit = m_bm->ctrl->bytes_p_Bit + 1;
  ::lib_rtl_output(LIB_RTL_INFO,"Control: %p  0x%08X             [0x%06x Bytes]\n",(void*)m_bm->ctrl,
                   ((char*)m_bm->ctrl)-((char*)m_bm->ctrl), sizeof(CONTROL));

  m_bm->usDesc = (USERDesc*)(m_bm->gbl+ctrl_len);
  m_bm->user   = m_bm->usDesc->users;
  ::lib_rtl_output(LIB_RTL_INFO,"User:    %p  0x%08X  %p [0x%06x Bytes]\n",(void*)m_bm->user,
                   ((char*)m_bm->user)-((char*)m_bm->ctrl),(void*)m_bm->usDesc, len);

  m_bm->evDesc = (EVENTDesc*)(m_bm->gbl+ctrl_len+user_len);
  m_bm->event  = m_bm->evDesc->events;
  ::lib_rtl_output(LIB_RTL_INFO,"Event:   %p  0x%08X  %p [0x%06x Bytes]\n",(void*)m_bm->event,
                   ((char*)m_bm->event)-((char*)m_bm->ctrl),(void*)m_bm->evDesc, len);

  m_bm->bitmap = m_bm->gbl+ctrl_len+user_len+evnt_len;
  ::lib_rtl_output(LIB_RTL_INFO,"Bitmap:  %p  0x%08X             [0x%06x Bytes]\n",(void*)m_bm->bitmap,
                   ((char*)m_bm->bitmap)-((char*)m_bm->ctrl), len);

  len = p_size<<10;
  status = ::lib_rtl_create_section(buff_mod,buff_len,&m_bm->buff_add);
  if(!::lib_rtl_is_success(status))   {
    ::lib_rtl_delete_section(m_bm->gbl_add);
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot create section %s. Exiting....\n",buff_mod);
    return status;
  }
  m_bm->buffer_add = (char*)m_bm->buff_add->address;
  ::memset(m_bm->buffer_add,0xDD,len);
  ::lib_rtl_output(LIB_RTL_INFO,"Buffer:  %p  0x%08X             [0x%06x Bytes]\n",(void*)m_bm->buffer_add,
                   ((char*)m_bm->buffer_add)-((char*)m_bm->ctrl), len);

  CONTROL* ctrl  = m_bm->ctrl;
  USER*    user  = m_bm->user;
  EVENT*   event = m_bm->event;
  ctrl->p_umax       = p_umax;
  ctrl->p_emax       = p_emax;
  ctrl->p_tmax       = p_tmax;
  
  ctrl->buff_size    = p_size<<10; /* in bytes*/
  ctrl->tot_produced = 0;
  ctrl->tot_actual   = 0;
  ctrl->tot_seen     = 0;
  ctrl->i_events     = 0;
  ctrl->i_space      = (p_size<<10)/bytes_per_bit; /*in Bits*/
  ctrl->last_bit     = 0;
  ctrl->last_alloc   = 0;
  ctrl->bm_size      = (p_size<<10)/bytes_per_bit; /*in bits*/
  for (int i=0;i<p_umax;i++)  {
    user[i].block_id = BID_USER;
    user[i].uid      = i;
  }
  for (int j=0;j<p_emax;j++)  {
    event[j].block_id = BID_EVENT;
    event[j].eid      = j;
  }
  status = ::mbmsrv_map_global_buffer_info(&bm_all,true);
  if(!::lib_rtl_is_success(status))   {   
    ::lib_rtl_delete_section(m_bm->gbl_add);
    ::lib_rtl_delete_section(m_bm->buff_add);
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot map global buffer information....\n");
    return status;
  }
  BUFFERS* buffs = (BUFFERS*)bm_all->address;
  for(int i=0; i<buffs->p_bmax; ++i)  {
    if ( ::strcmp(buffs->buffers[i].name,bm_id)==0 )  {
      buffs->buffers[i].used = 1;
      buffs->buffers[i].size = ctrl->buff_size;
      ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ BM installation [%s] finished successfully.\n",m_bm->bm_name);
      lib_rtl_declare_exit (_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      lib_rtl_declare_rundown(_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      return MBM_NORMAL;
    }
  }
  for(int i=0; i<buffs->p_bmax; ++i)  {
    if ( 0 == buffs->buffers[i].used )  {
      ::snprintf(buffs->buffers[i].name,sizeof(buffs->buffers[i].name),"%s",m_bm->bm_name);
      buffs->buffers[i].size = ctrl->buff_size;
      buffs->buffers[i].used = 1;
      buffs->nbuffer++;
      lib_rtl_declare_exit (_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      lib_rtl_declare_rundown(_mbm_installer_shutdown, new std::pair<void*,int>(buffs,i));
      ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ BM registration successful:%d buffers now\n",buffs->nbuffer);
      break;
    }
  }
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ BM installation [%s] finished successfully.\n",m_bm->bm_name);
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ CONTROL structure size:%4u [%04X] bytes..\n",sizeof(CONTROL),sizeof(CONTROL));
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ EVENT   structure size:%4u [%04X] bytes..\n",sizeof(EVENT),sizeof(EVENT));
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ USER    structure size:%4u [%04X] bytes..\n",sizeof(USER),sizeof(USER));
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ MESSAGE structure size:%4u [%04X] bytes..\n",sizeof(MBMMessage),sizeof(MBMMessage));
  return MBM_NORMAL;
}

int MBM::Installer::deinstall()  {
  ::lib_rtl_gbl_t handle = 0;
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ Commencing BM deinstallation \n");
  int status = ::lib_rtl_map_section(ctrl_mod, 0, &handle);
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
  status = ::lib_rtl_map_section(buff_mod, 1, &handle);
  if (::lib_rtl_is_success(status))
    status = ::lib_rtl_delete_section(handle);
  if (!::lib_rtl_is_success(status))  {
    ::lib_rtl_output(LIB_RTL_INFO,"problem deleting section %s status %d\n",buff_mod,status);
  }
  status = ::mbmsrv_map_global_buffer_info(&bm_all,true);
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
  ::mbmsrv_unmap_global_buffer_info(bm_all);
  ::lib_rtl_output(LIB_RTL_INFO,"++bm_init++ Old BM [%s] de-installed successfully\n",m_bm->bm_name);
  return MBM_NORMAL;
}

ServerBMID mbm_install_server(int argc , char** argv) {
  try  {
    MBM::Installer inst(argc, argv);
    int sc = inst.install();
    if ( sc == MBM_NORMAL )  {
      if ( inst.startMonitor() )  {
        lib_rtl_sleep(1000);
        mbm_mon(0, argv); 
        ::lib_rtl_output(LIB_RTL_INFO,"++mbm_install++ All done.\n");
        return 0;
      }
      else if ( inst.startBlocking() ) {
        ServerBMID bmid = inst.bmid();
        ::mbmsrv_dispatch_nonblocking(bmid);
        ::mbmsrv_wait_dispatch(bmid);
        ::lib_rtl_output(LIB_RTL_INFO,"++mbm_install++ All done.\n");
        return 0;
      }
      else if ( inst.continueInstallation() ) {
        ServerBMID bmid = inst.releaseBMID();
        ::mbmsrv_dispatch_nonblocking(bmid);
        ::lib_rtl_output(LIB_RTL_INFO,"++mbm_install++ All done.\n");
        return bmid;
      }
    }
    ::lib_rtl_output(LIB_RTL_INFO,"++mbm_install++ MBM installation failed.\n");
    return 0;
  }
  catch (std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_ERROR,"++mbm_install++ MBM initialization failed: %s\n",e.what());
  }
  return 0;
}

extern "C" int mbm_deinstall(int argc , char** argv) {
  MBM::Installer inst(argc, argv);
  return inst.deinstall();
}

extern "C" int mbm_remove(int argc, char** argv) {
  MBM::Installer inst(argc,argv);
  return inst.deinstall();
}

extern "C" int mbm_install_qmtest(int argc , char** argv) {
  ServerBMID bm = mbm_install_server(argc,argv);
  return bm ? 0 : 1;
}

std::map<std::string,ServerBMID> mbm_multi_install(int argc , char** argv) {
  std::vector<char*> opts;
  std::map<std::string,ServerBMID> bmids;
  static char type[64] = "mbm_install";

  opts.push_back(type);
  for(size_t i=0; i<size_t(argc); ++i)  {
    char c0 = argv[i][0];
    char c1 = ::toupper(argv[i][1]);
    opts.push_back(argv[i]);
    if ( (c0 == '-' || c0 == '/') && (c1 == 'C' || c1 == 'A') ) {
      ServerBMID bmid = mbm_install_server(opts.size(), &opts[0]);
      if ( !bmid )  {
        ::lib_rtl_output(LIB_RTL_ERROR,"Unable to install MBM buffers...\n");
        throw std::runtime_error("Unable to install MBM buffers...");
      }
      bmids[bmid->bm_name] = bmid;
      opts.clear();
      opts.push_back(type);
    }
  }
  if ( opts.size() > 1 )  {
    ServerBMID bmid = mbm_install_server(opts.size(), &opts[0]);
    if ( !bmid )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"Unable to install MBM buffers...\n");
      throw std::runtime_error("Unable to install MBM buffers...");
    }
    bmids[bmid->bm_name] = bmid;
  }
  return bmids;
}
