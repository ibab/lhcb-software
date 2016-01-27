/*
 * ipmilight.c
 *
 *  Created on: Jan 18, 2016
 *      Author: beat
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <ipmitool/ipmilight.h>
#include <ipmitool/helper.h>
#include <ipmitool/log.h>
#include <ipmitool/ipmi.h>
#include <ipmitool/ipmi_intf.h>
#include <ipmitool/ipmi_session.h>
#include <ipmitool/ipmi_sdr.h>
#include <ipmitool/ipmi_gendev.h>
#include <ipmitool/ipmi_sel.h>
#include <ipmitool/ipmi_fru.h>
#include <ipmitool/ipmi_sol.h>
#include <ipmitool/ipmi_isol.h>
#include <ipmitool/ipmi_lanp.h>
#include <ipmitool/ipmi_chassis.h>
#include <ipmitool/ipmi_mc.h>
#include <ipmitool/ipmi_firewall.h>
#include <ipmitool/ipmi_sensor.h>
#include <ipmitool/ipmi_channel.h>
#include <ipmitool/ipmi_session.h>
#include <ipmitool/ipmi_event.h>
#include <ipmitool/ipmi_user.h>
#include <ipmitool/ipmi_raw.h>
#include <ipmitool/ipmi_pef.h>
#include <ipmitool/ipmi_oem.h>
#include <ipmitool/ipmi_ekanalyzer.h>
#include <ipmitool/ipmi_picmg.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef ENABLE_ALL_OPTIONS
# define OPTION_STRING	"I:hVvcgsEKYao:H:d:P:f:U:p:C:L:A:t:T:m:z:S:l:b:B:e:k:y:O:R:N:D:"
#else
# define OPTION_STRING	"I:hVvcH:f:U:p:d:S:D:"
#endif

extern int verbose;
extern int csv_output;
extern const struct valstr ipmi_privlvl_vals[];
extern const struct valstr ipmi_authtype_session_vals[];
void ipmi_catch_sigint();


struct ipmi_intf_support * sup;
int privlvl = 0;
uint8_t target_addr = 0;
uint8_t target_channel = 0;

uint8_t transit_addr = 0;
uint8_t transit_channel = 0;
uint8_t target_lun     = 0;
uint8_t arg_addr = 0;
uint8_t addr = 0;
uint16_t my_long_packet_size=0;
uint8_t my_long_packet_set=0;
uint8_t lookupbit = 0x10;	/* use name-only lookup by default */
int retry = 0;
uint32_t timeout = 0;
int authtype = -1;
char * tmp_pass = NULL;
char * tmp_env = NULL;
char * hostname = NULL;
char * username = NULL;
char * password = NULL;
char * intfname = NULL;
char * progname = NULL;
char * oemtype  = NULL;
char * sdrcache = NULL;
unsigned char * kgkey = NULL;
char * seloem   = NULL;
int port = 0;
int devnum = 0;
int cipher_suite_id = 3; /* See table 22-19 of the IPMIv2 spec */
int argflag, i, found;
int rc = -1;
char sol_escape_char = SOL_ESCAPE_CHARACTER_DEFAULT;
char * devfile  = NULL;

struct ipmi_intf *ipmi_light_init_intf(    char *hname,
    char *uname,
    char *pword,
struct ipmi_cmd * cmdlist)
{
  intfname = strdup("lan");
  username = strdup(uname);
  hostname = strdup(hname);
  password = strdup(pword);
  struct ipmi_intf *ipmi_main_intf,*IntfPatt;
  IntfPatt = malloc(sizeof(struct ipmi_intf));
//  if (ipmi_main_intf == 0)
//  {
//     return (void*)-1;
//  }
//  if (hostname != NULL && password == NULL &&
//      (authtype != IPMI_SESSION_AUTHTYPE_NONE || authtype < 0)) {
//#ifdef HAVE_GETPASSPHRASE
//    tmp_pass = getpassphrase("Password: ");
//#else
//    tmp_pass = getpass("Password: ");
//#endif
//    if (tmp_pass != NULL) {
//      password = strdup(tmp_pass);
//      tmp_pass = NULL;
//      if (password == NULL) {
//        lprintf(LOG_ERR, "%s: malloc failure", progname);
//        goto out_free;
//      }
//    }
//  }

  /* if no interface was specified but a
  * hostname was then use LAN by default
  * otherwise the default is hardcoded
  * to use the first entry in the list
  */
  if (intfname == NULL && hostname != NULL) {
    intfname = strdup("lan");
    if (intfname == NULL) {
      lprintf(LOG_ERR, "%s: malloc failure", progname);
      ipmi_light_Cleanup(ipmi_main_intf);
      return (void*)-1;
    }
  }

  if (password != NULL && intfname != NULL) {
    if (strcmp(intfname, "lan") == 0 && strlen(password) > 16) {
      lprintf(LOG_ERR, "%s: password is longer than 16 bytes.", intfname);
      rc = -1;
      ipmi_light_Cleanup(ipmi_main_intf);
       return (void*)-1;
    } else if (strcmp(intfname, "lanplus") == 0 && strlen(password) > 20) {
      lprintf(LOG_ERR, "%s: password is longer than 20 bytes.", intfname);
      rc = -1;
      ipmi_light_Cleanup(ipmi_main_intf);
       return (void*)-1;
    }
  } /* if (password != NULL && intfname != NULL) */

  /* load interface */
  ipmi_main_intf = ipmi_intf_load(intfname);
  if (ipmi_main_intf == NULL) {
    lprintf(LOG_ERR, "Error loading interface %s", intfname);
    ipmi_light_Cleanup(IntfPatt);
     return (void*)-1;
  }
  /* setup log */
  log_init(progname, 0, verbose);

  /* run OEM setup if found */
  if (oemtype != NULL &&
      ipmi_oem_setup(ipmi_main_intf, oemtype) < 0) {
    lprintf(LOG_ERR, "OEM setup for \"%s\" failed", oemtype);
    ipmi_light_Cleanup(ipmi_main_intf);
     return (void*)-1;
  }

  /* set session variables */
  if (hostname != NULL)
    ipmi_intf_session_set_hostname(ipmi_main_intf, hostname);
  if (username != NULL)
    ipmi_intf_session_set_username(ipmi_main_intf, username);
  if (password != NULL)
    ipmi_intf_session_set_password(ipmi_main_intf, password);
  if (kgkey != NULL)
    ipmi_intf_session_set_kgkey(ipmi_main_intf, kgkey);
  if (port > 0)
    ipmi_intf_session_set_port(ipmi_main_intf, port);
  if (authtype >= 0)
    ipmi_intf_session_set_authtype(ipmi_main_intf, (uint8_t)authtype);
  if (privlvl > 0)
    ipmi_intf_session_set_privlvl(ipmi_main_intf, (uint8_t)privlvl);
  else
    ipmi_intf_session_set_privlvl(ipmi_main_intf,
        IPMI_SESSION_PRIV_ADMIN);	/* default */
  /* Adding retry and timeout for interface that support it */
  if (retry > 0)
    ipmi_intf_session_set_retry(ipmi_main_intf, retry);
  if (timeout > 0)
    ipmi_intf_session_set_timeout(ipmi_main_intf, timeout);

  ipmi_intf_session_set_lookupbit(ipmi_main_intf, lookupbit);
  ipmi_intf_session_set_sol_escape_char(ipmi_main_intf, sol_escape_char);
  ipmi_intf_session_set_cipher_suite_id(ipmi_main_intf, cipher_suite_id);

  ipmi_main_intf->devnum = devnum;

  /* setup device file if given */
  ipmi_main_intf->devfile = devfile;

  /* Open the interface with the specified or default IPMB address */
  ipmi_main_intf->my_addr = arg_addr ? arg_addr : IPMI_BMC_SLAVE_ADDR;
  if (ipmi_main_intf->open != NULL) {
    if (ipmi_main_intf->open(ipmi_main_intf) < 0) {
      ipmi_light_Cleanup(ipmi_main_intf);
       return (void*)-1;
    }
  }
  /*
  * Attempt picmg discovery of the actual interface address unless
  * the users specified an address.
  *	Address specification always overrides discovery
  */
  if (picmg_discover(ipmi_main_intf) && !arg_addr) {
    lprintf(LOG_DEBUG, "Running PICMG Get Address Info");
    addr = ipmi_picmg_ipmb_address(ipmi_main_intf);
    lprintf(LOG_INFO,  "Discovered IPMB-0 address 0x%x", addr);
  }

  /*
  * If we discovered the ipmb address and it is not the same as what we
  * used for open, Set the discovered IPMB address as my address if the
  * interface supports it.
  */
  if (addr != 0 && addr != ipmi_main_intf->my_addr &&
            ipmi_main_intf->set_my_addr) {
    /*
    * Only set the interface address on interfaces which support
    * it
    */
    (void) ipmi_main_intf->set_my_addr(ipmi_main_intf, addr);
  }

  /* If bridging addresses are specified, handle them */
  if (target_addr > 0) {
    ipmi_main_intf->target_addr = target_addr;
    ipmi_main_intf->target_lun = target_lun ;
    ipmi_main_intf->target_channel = target_channel ;
  }
  if (transit_addr > 0) {
    /* sanity check, transit makes no sense without a target */
    if ((transit_addr != 0 || transit_channel != 0) &&
      ipmi_main_intf->target_addr == 0) {
      lprintf(LOG_ERR,
        "Transit address/channel %#x/%#x ignored. "
        "Target address must be specified!",
        transit_addr, transit_channel);
      ipmi_light_Cleanup(ipmi_main_intf);
       return (void*)-1;
    }

    ipmi_main_intf->transit_addr    = transit_addr;
    ipmi_main_intf->transit_channel = transit_channel;
  }
  if (ipmi_main_intf->target_addr > 0) {
    /* must be admin level to do this over lan */
    ipmi_intf_session_set_privlvl(ipmi_main_intf, IPMI_SESSION_PRIV_ADMIN);
    /* Get the ipmb address of the targeted entity */
    ipmi_main_intf->target_ipmb_addr =
          ipmi_picmg_ipmb_address(ipmi_main_intf);
    lprintf(LOG_DEBUG, "Specified addressing     Target  %#x:%#x Transit %#x:%#x",
            ipmi_main_intf->target_addr,
            ipmi_main_intf->target_channel,
            ipmi_main_intf->transit_addr,
            ipmi_main_intf->transit_channel);
    if (ipmi_main_intf->target_ipmb_addr) {
      lprintf(LOG_INFO, "Discovered Target IPMB-0 address %#x",
            ipmi_main_intf->target_ipmb_addr);
    }
  }

  lprintf(LOG_DEBUG, "Interface address: my_addr %#x "
        "transit %#x:%#x target %#x:%#x "
        "ipmb_target %#x\n",
      ipmi_main_intf->my_addr,
      ipmi_main_intf->transit_addr,
      ipmi_main_intf->transit_channel,
      ipmi_main_intf->target_addr,
      ipmi_main_intf->target_channel,
      ipmi_main_intf->target_ipmb_addr);

  /* parse local SDR cache if given */
  if (sdrcache != NULL) {
    ipmi_sdr_list_cache_fromfile(ipmi_main_intf, sdrcache);
  }
  /* Parse SEL OEM file if given */
  if (seloem != NULL) {
    ipmi_sel_oem_init(seloem);
  }

  /* Enable Big Buffer when requested */
  if ( my_long_packet_size != 0 ) {
    /* Enable Big Buffer when requested */
    if (!ipmi_oem_active(ipmi_main_intf, "kontron") ||
      ipmi_kontronoem_set_large_buffer(ipmi_main_intf,
          my_long_packet_size ) == 0) {
      printf("Setting large buffer to %i\n",my_long_packet_size);
      my_long_packet_set = 1;
      ipmi_intf_set_max_request_data_size(ipmi_main_intf,
          my_long_packet_size);
    }
  }

  ipmi_main_intf->cmdlist = cmdlist;
  memcpy(IntfPatt,ipmi_main_intf,sizeof (struct ipmi_intf));
  return IntfPatt;
}

int ipmi_light_do_it(struct ipmi_intf *ipmi_main_intf,int argc, char ** argv)
{
  /* save program name */
  progname = strrchr(argv[0], '/');
  progname = ((progname == NULL) ? argv[0] : progname+1);
  signal(SIGINT, ipmi_catch_sigint);

  /* now we finally run the command */
  if (argc > 0)
    rc = ipmi_cmd_run(ipmi_main_intf, argv[1], argc-1,
        &(argv[2]));
  else
    rc = ipmi_cmd_run(ipmi_main_intf, NULL, 0, NULL);
  return rc;
}

int ipmi_light_Cleanup(struct ipmi_intf *ipmi_main_intf)
{
  /* clean repository caches */
  ipmi_cleanup(ipmi_main_intf);

  /* call interface close function if available */
  if (ipmi_main_intf->opened > 0 && ipmi_main_intf->close != NULL)
    ipmi_main_intf->close(ipmi_main_intf);

  out_free:
  log_halt();

  if (intfname != NULL) {
    free(intfname);
    intfname = NULL;
  }
  if (hostname != NULL) {
    free(hostname);
    hostname = NULL;
  }
  if (username != NULL) {
    free(username);
    username = NULL;
  }
  if (password != NULL) {
    free(password);
    password = NULL;
  }
  if (oemtype != NULL) {
    free(oemtype);
    oemtype = NULL;
  }
  if (seloem != NULL) {
    free(seloem);
    seloem = NULL;
  }
  if (kgkey != NULL) {
    free(kgkey);
    kgkey = NULL;
  }
  if (sdrcache != NULL) {
    free(sdrcache);
    sdrcache = NULL;
  }
  if (devfile) {
    free(devfile);
    devfile = NULL;
  }
  if (ipmi_main_intf)
  {
    free(ipmi_main_intf);
    ipmi_main_intf = 0;
  }
  return 0;
}
