#define MBM_IMPLEMENTATION
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "bm_struct.h"
#include "Manager.h"
#define writeln(a,b,c) printf(b)
#define _CHECK( x )  { int sc = x ; if ( !(sc&1) ) { printf ( "Error in:%s, status=%d\n", #x , sc ); return sc; } }

/* default parameters values */
namespace MBM {
  struct Installer : public Manager  {
    Installer(int argc, char **argv)  {
      p_moni   = 0;           /* Start monitor               */
      p_emax   = 32;			    /* maximum events allowed	     */
      p_umax   = 5;			      /* maximun users		           */
      p_loc    = 0;	      		/* local flag			             */
      p_across = 0;		    	  /* Across memory flag		       */
      p_size   = 10;			    /* buffer size			           */
      p_base   = 0x1000000;		/* Event Buffer base address   */
      spy_base = 0;			      /* Spy Memory base address	   */
      p_force  = 0;			      /* force deinstall		         */
      getOptions(argc, argv);
      m_bm = new BMDESCRIPT;
      ::memset(m_bm,0,sizeof(BMDESCRIPT));
      ::strcpy(m_bm->bm_name,bm_id);
    }
    int p_moni;     /* Start monitor */
    int	p_emax;			/* maximum events allowed	      */
    int	p_umax;			/* maximun users		      */
    int	p_loc;			/* local flag			      */
    int	p_across;			/* Across memory flag		      */
    int	p_size;			/* buffer size			      */
    int	p_base;		/* Event Buffer base address	      */
    int	spy_base;			/* Spy Memory base address	      */
    int	p_force;			/* force deinstall		      */

    /* global variables          */

    char	*buff_ptr;
    char	*spy_ptr;
    char	*data_ptr;
    char	buff_id[16];
    int 	*spy_register;
    int 	*spy_on;
    int 	*spy_off;
    int	spy_set;

    bool startMonitor() const { return p_moni > 0; }
    virtual int  optparse (const char* c);
    int  deinstall();
    int  install();
  };
}

static void help()  {
  writeln(2,"Syntax: bm_init [<-opt>]\n",80);
  writeln(2,"Function: Buffer Manager Installation\n",80);
  writeln(2,"Options:\n",80);
  writeln(2,"    -s=<size> [10]      Buffer size (kbytes)\n",80);
  writeln(2,"    -e=<max>  [32]      Maximum number of events\n",80);
  writeln(2,"    -u=<max>  [5]       Maximum number of users\n",80);
  writeln(2,"    -b=<base> [1000000] Event Memory base address (hex)\n",80);
  writeln(2,"    -y=<base> [0000000] Spy Memory base address (hex)\n",80);
  writeln(2,"    -i=<id>   [ ]       Buffer Identifier \n",80);
  writeln(2,"    -l        [ ]       Local memory buffer (data module)\n",80);
  writeln(2,"    -x        [ ]       Allow allocation across memories\n",80);
  writeln(2,"    -f        [ ]       force deinstall\n",80);
  writeln(2,"    -m        [ ]       Start monitor after installer\n",80);
}
int MBM::Installer::optparse (const char* c)  {
  register int iret;
  switch (*c | 0x20)    {
  case 's':				/*      size of buffer        */	
    iret = sscanf(c+1,"=%d",&p_size);
    if( iret != 1 ) 	   {
      writeln(2,"Error reading buffer size parameter\n",80);
      exit(0);
    }
    p_size = ((p_size+1)>>1)<<1;
    break;
  case 'b':				/*      base address       */	
    iret = sscanf(c+1,"=%x",&p_base);
    if( iret != 1 ) 	   {
      writeln(2,"Error reading base address parameter\n",80);
      exit(0);
    }
    break;
  case 'y':				/*     spy base address       */	
    iret = sscanf(c+1,"=%x",&spy_base);
    if( iret != 1 ) 	   {
      writeln(2,"Error reading spy base address parameter\n",80);
      exit(0);
    }
    break;
  case 'e':				/*      maximum events        */	
    iret = sscanf(c+1,"=%d",&p_emax);
    if( iret != 1 ) 	   {
      writeln(2,"Error reading maximum events parameter\n",80);
      exit(0);
    }
    break;
  case 'u':				/*      maximum users        */	
    iret = sscanf(c+1,"=%d",&p_umax);
    if( iret != 1 )  {
      writeln(2,"Error reading maximum users parameter\n",80);
      exit(0);
    }
    if( p_umax > 128 )    {
      writeln(2,"Maximum users exeeded maximum (128)\n",80);
      exit(0);
    }
    break;
  case 'i':				/*      maximum users        */	
    iret = sscanf(c+1,"=%s",buff_id);
    if( iret != 1 ) 	   {
      writeln(2,"Error reading Buffer identifier parameter\n",80);
      exit(0);
    }
    bm_id = buff_id;
    ::strcpy(m_bm->bm_name,bm_id);
    break;
  case 'l':				/*      local flag            */	
    p_loc = 1;
    break;
  case 'x':				/*      local flag            */	
    p_across = 1;
    break;
  case 'f':				/*      local flag            */	
    p_force = 1;
    break;
  case 'm':				/*      local flag            */	
    p_moni = 1;
    break;
  case '?':
  case 'h':
  default:
    help();
    exit(0);
  }
  return 0;
}

int MBM::Installer::install()  {
  int icode = deinstall();
  if(icode == -1) exit(0);
  printf("++bm_init++ Commencing BM installation \n");
  int status = lib_rtl_create_section(ctrl_mod,sizeof(CONTROL),&m_bm->ctrl_add);
  if(!lib_rtl_is_success(status))   {	
    printf("Cannot create section %s. Exiting....",ctrl_mod);
    exit(status);
  }
  m_bm->ctrl = (CONTROL*)m_bm->ctrl_add->address;
  status = lib_rtl_create_section(user_mod,sizeof(USER)*p_umax ,&m_bm->user_add);
  if(!lib_rtl_is_success(status))   {	
    lib_rtl_delete_section(m_bm->ctrl_add);
    printf("Cannot create section %s. Exiting....",user_mod);
    exit(status);
  }
  m_bm->user = (USER*)m_bm->user_add->address;
  status = lib_rtl_create_section(event_mod,sizeof(EVENT)*p_emax ,&m_bm->event_add);
  if(!lib_rtl_is_success(status))   {	
    lib_rtl_delete_section(m_bm->ctrl_add);
    lib_rtl_delete_section(m_bm->user_add);
    printf("Cannot create section %s. Exiting....",event_mod);
    exit(status);
  }
  m_bm->event = (EVENT*)m_bm->event_add->address;
  status = lib_rtl_create_section(bitmap_mod,(p_size<<(Bits_p_kByte-1))>>3,&m_bm->bitm_add);
  if(!lib_rtl_is_success(status))   {	
    lib_rtl_delete_section(m_bm->ctrl_add);
    lib_rtl_delete_section(m_bm->user_add);
    lib_rtl_delete_section(m_bm->event_add);
    printf("Cannot create section %s. Exiting....",bitmap_mod);
    exit(status);
  }
  m_bm->bitmap = (char*)m_bm->bitm_add->address;
  status = lib_rtl_create_section(buff_mod,p_size<<10,&m_bm->buff_add);
  if(!lib_rtl_is_success(status))   {	
    lib_rtl_delete_section(m_bm->ctrl_add);
    lib_rtl_delete_section(m_bm->user_add);
    lib_rtl_delete_section(m_bm->event_add);
    lib_rtl_delete_section(m_bm->buff_add);
    printf("Cannot create section %s. Exiting....",buff_mod);
    exit(status);
  }
  m_bm->buffer_add = (char*)m_bm->buff_add->address;
  CONTROL* ctrl  = m_bm->ctrl;
  USER*    user  = m_bm->user;
  EVENT*   event = m_bm->event;
  memset(ctrl,0,sizeof(CONTROL));
  ctrl->p_umax       = p_umax;
  ctrl->p_emax       = p_emax;
  ctrl->buff_size    = p_size<<10; /* in bytes*/
  ctrl->tot_produced = 0;
  ctrl->tot_actual   = 0;
  ctrl->tot_seen     = 0;
  ctrl->i_events     = 0;
  ctrl->i_space      = p_size*Bits_p_kByte; /*in Bits*/
  ctrl->last_bit     = 0;
  ctrl->last_alloc   = 0;
  ctrl->bm_size      = p_size*Bits_p_kByte; /*in bits*/
  ctrl->spare1       = 0;
  ::memset(user,0,sizeof(USER)*p_umax);
  ::memset(event,0,sizeof(EVENT)*p_emax );
  ::memset(m_bm->bitmap,0,(p_size<<Bits_p_kByte)>>3);
  for (int i=0;i<p_umax;i++)  {
    user[i].block_id	= BID_USER;
    user[i].uid		= i;
  }
  for (int j=0;j<p_emax;j++)  {
    event[j].block_id	= BID_EVENT;
    event[j].eid	= j;
  }
  status = mbm_map_global_buffer_info(&bm_all);
  if(!lib_rtl_is_success(status))   {	
    lib_rtl_delete_section(m_bm->ctrl_add);
    lib_rtl_delete_section(m_bm->user_add);
    lib_rtl_delete_section(m_bm->event_add);
    lib_rtl_delete_section(m_bm->buff_add);
    ::printf("Cannot map global buffer information....\n");
    ::exit(status);
  }
  BUFFERS* buffs = (BUFFERS*)bm_all->address;
  for(int i=0; i<buffs->p_bmax; ++i)  {
    if ( 0 == buffs->buffers[i].used )  {
      buffs->buffers[i].used = 1;
      ::strcpy(buffs->buffers[i].name, m_bm->bm_name);
      buffs->nbuffer++;
      ::printf("++bm_init++ BM registration successful:%d buffers now \n",buffs->nbuffer);
      break;
    }
  }
  ::printf("++bm_init++ BM installation [%s] successfully. \n",m_bm->bm_name);
  return 1;
}

int MBM::Installer::deinstall()  {
  lib_rtl_gbl_t handle;
  printf("++bm_init++ Commencing BM deinstallation \n");
  int status = lib_rtl_map_section(ctrl_mod, sizeof(CONTROL), &handle);
  if( !lib_rtl_is_success(status)) 
    return 1;
  if (p_force != 1)  {
    m_bm->ctrl = (CONTROL*)handle->address;
    if( m_bm->ctrl->i_users > 0 )    {
      printf("++bm_init++ Unable to de-install BM (%d users still active)\n", m_bm->ctrl->i_users);
      return(-1);
    }
  }
  status = lib_rtl_delete_section(handle);
  if (!lib_rtl_is_success(status))  {
    printf("problem deleting section %s status %d\n",ctrl_mod,status);
  }
  status = lib_rtl_map_section(user_mod, sizeof(USER), &handle);
  status = lib_rtl_delete_section(handle);
  if (!lib_rtl_is_success(status))  {
    printf("problem deleting section %s status %d\n",user_mod,status);
  }
  status = lib_rtl_map_section(buff_mod, 1, &handle);
  status = lib_rtl_delete_section(handle);
  if (!lib_rtl_is_success(status))  {
    printf("problem deleting section %s status %d\n",buff_mod,status);
  }
  status = lib_rtl_map_section(event_mod, 1, &handle);
  status = lib_rtl_delete_section(handle);
  if (!lib_rtl_is_success(status))  {
    printf("problem deleting section %s status %d\n",event_mod,status);
  }
  status = lib_rtl_map_section(bitmap_mod, 1, &handle);
  status = lib_rtl_delete_section(handle);
  if (!lib_rtl_is_success(status))  {
    printf("problem deleting section %s status %d\n",bitmap_mod,status);
  }
  status = mbm_map_global_buffer_info(&bm_all);
  if(!lib_rtl_is_success(status))
    return 1;
  BUFFERS* buffs = (BUFFERS*)bm_all->address;
  for(int i=0; i<buffs->p_bmax; ++i)  {
    if ( ::strcmp(buffs->buffers[i].name,bm_id)==0 )  {
      buffs->buffers[i].used = 0;
      buffs->nbuffer--;
      ::memset(buffs->buffers[i].name,0,sizeof(buffs->buffers[i].name));
      break;
    }
  }
  mbm_unmap_global_buffer_info(bm_all);
  printf("++bm_init++ Old BM [%s] de-installed successfully\n",m_bm->bm_name);
  return(0);
}

int mbm_install(int argc , char** argv) {
  MBM::Installer inst(argc, argv);
  int sc = inst.install();
  if ( sc == 1 )  {
    if ( inst.startMonitor() )  {
      mbm_mon(0, argv); 
    }
    else  {
      while(1) lib_rtl_sleep(10000);
    }
  }
  return sc;
}

int mbm_deinstall(int argc , char** argv) {
  MBM::Installer inst(argc, argv);
  return inst.deinstall();
}
