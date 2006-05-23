//	====================================================================
//  L1MEP.cpp
//	--------------------------------------------------------------------
//
//	Package   : L1MEP
//
//	Author    : Niko Neufeld
//
//	====================================================================

// Framework include files
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "L1MEP.h"

// Event Model related classes
#include "Event/L1Buffer.h"
#include "Event/L1Event.h"
#include "Event/L1Bank.h"

#include "GaudiKernel/System.h"
#include <sys/types.h>

static const AlgFactory<L1MEP>  Factory;
const IFactory& L1MEPFactory = Factory;


L1MEP::L1MEP(const std::string& name, ISvcLocator* pSvcLocator)  
    : Algorithm(name, pSvcLocator) { 
    memset(m_l1fd, 0, 300 * sizeof(void *));
    memset(m_l1hh, 0, 300 * sizeof(void *));
    declareProperty("L1PFX", m_l1pfx = "");
    declareProperty("L1HPFX", m_l1hpfx = "l1");
    declareProperty("L1NTELL1", m_l1ntell1 = 137);
    declareProperty("MarkusFile", m_l1bufout = "");
    m_nomep = 0;
 }

StatusCode L1MEP::initialize() {  

    m_rmout = -1;
    if (m_l1bufout.compare("")) { 
	if ((m_rmout = open(m_l1bufout.c_str(), O_RDWR | O_CREAT | O_TRUNC, 
			    S_IRWXU | S_IROTH | S_IRGRP)) == -1) {
	    *m_log << MSG::ERROR << "open: " << sys_errlist[errno] << endmsg;
	    return StatusCode::FAILURE;
	}
    }
    if (!m_l1pfx.compare("")) m_nomep = 1;
    m_eventid = 0;
    memset(m_fgwritten, 0, sizeof(long long) * 300);
    m_log = new MsgStream(msgSvc(), name());
    return StatusCode::SUCCESS;
}

#if 0
void 
dumpl1(l1_int *b, int n)
{
    for (int i = 0; i < n; i+=8) {
	printf("%04x: ", i);
	for (int j = i; j < n && j < i + 8; ++j) 
	    printf("%04x ", (u_int16_t) b[j]);
	printf("\n");
    }
}
#endif

typedef union {
    u_int16_t hdr;
    struct {
	u_int16_t size:  7;
	u_int16_t srcid: 6;
	u_int16_t type:  3;
    } bits ;
} l1_hdr_t;
	    
char *l1typstr[] = { "VR", "VP", "TT", "L0", "DQ", "##" , "##", "##" };


int
L1MEP::l1tell1id(int type, int src)
{
    switch (type) {
    case L1Buffer::VeloR:   
	if (src < 42) return src;
	*m_log << MSG::ERROR << "Severe: illegal src in L1 VR " << src 
	       << endmsg;
	break;
    case L1Buffer::VeloPhi: 
	if (src < 42) return 42 + src;
	*m_log << MSG::ERROR << "Severe: illegal src in L1 VP " << src 
	       << endmsg;
	break;
    case L1Buffer::TT: 
	if (src < 12) return 84 + src;
	if (src >= 16 && src < 28) return 84 + 11 + (src - 16);
	if (src >= 32 && src < 45) return 84 + 22 + (src - 32);
	if (src >= 48 && src < 61) return 84 + 35 + (src - 48);
	*m_log << MSG::ERROR << "Severe: illegal src in L1 TT: " << src 
	       << endmsg;
	break;
    case L1Buffer::L0:  // srcids from 1 to 3  
	if (src < 4) return 84 + 48 + src - 1;
	*m_log << MSG::ERROR << "Severe: illegal src in L1 L0: " << src 
	       << endmsg;
	break;
    case L1Buffer::DAQ: // srcids from 1 to 2
	if (src < 3) return 84 + 48 + 3 + src - 1;		
	*m_log << MSG::ERROR << "Severe: illegal src in L1 DQ: " << src 
	       << endmsg;
	break;
    }
    return -1;
}

#if 0
static int l1typ2det[17]  = {       0,      1,      2,      3,      4,      2, 
			     3,      4,      5,      6,      7,      8, 
                             9,     10,      11,     12,    13};

static int  l1maxsrc[] =  {        1,      1,      8,     10,      4,     84,   
			    48,      11,    24,     24,      1,      1,      
			     1};   

static char *l1subdetstr[14]  = { "L0CA", "L0DU", "PRSH", "ECAL", "HCAL", 
				  "VELO", "RICH",   "TT",   "IT",   "OT", 
				  "MUON", "L0PU",  "DAQ",  "TFC" };
#endif /* if 0 */

void
L1MEP::decodel1(l1_int *b, int n)
{
    l1_hdr_t *hdr;

    for (int i = 0; i < n; ) {
	hdr = ((l1_hdr_t *) &b[i]);
	*m_log << "Type " << l1typstr[hdr->bits.type] << " Src " << std::hex 
	       << std::setw(2) << hdr->bits.srcid  << " Len "  << 
	    2 * hdr->bits.size << endmsg;
	i += hdr->bits.size;
    }
}

typedef union {
    u_int32_t word;
    struct {
	u_int16_t evtid;
	u_int16_t len;
    } part;
} efg_t;


static u_int8_t frgbuf[300][0x10000];
static int bankpresent[300];


StatusCode
L1MEP::writel1(l1_int *b, int n)
{
    static int eventid = 0;
    char srcstr[12];
    int len, tell1id;
    int wlen = 0;
    int nbytes = sizeof(l1_int) *n;
    
    memset(bankpresent, 0, 300 * sizeof(int));
    if (m_rmout != -1) wlen = write(m_rmout, &nbytes, 4);
    for (int i = 0; i < 300; ++i) {
	efg_t *fgh = (efg_t *) &frgbuf[i][0];
	fgh->part.evtid = eventid;
	fgh->part.len = 0;
    }
    for (int i = 0; i < n; i+= len) {
	l1_hdr_t *hdr = (l1_hdr_t *) &b[i];
	int type, src;
	efg_t *fgh;
    
	type = hdr->bits.type;
	src  = hdr->bits.srcid;
	len  = hdr->bits.size;
	if ((tell1id = l1tell1id(type, src)) == -1) 
	    return StatusCode::FAILURE;
	bankpresent[tell1id]++;
	fgh = (efg_t *) &frgbuf[tell1id][0];
	if (len == 0) {
	    *m_log << MSG::ERROR << "Severe error 0-size frag" << endmsg;
	    return StatusCode::FAILURE;
	}
	sprintf(srcstr, "%03d", src);
	if (!m_nomep && !m_l1fd[tell1id]) {
	    std::string filename = m_l1pfx + "-" + 
		l1typstr[type] + "-" + srcstr + ".mep"; 
	    *m_log << MSG::INFO << filename << " TELL1 " << tell1id <<endmsg;	
	    if (!(m_l1fd[tell1id] = fopen(filename.c_str(), "w"))) {
		*m_log << MSG::ERROR << "fopen: " << sys_errlist[errno] << 
		    endmsg;
		return StatusCode::FAILURE;
	    }
	}
	if (m_l1hh[tell1id] == NULL) {
	    std::string histname = m_l1hpfx + "-" + l1typstr[type] + "-" +
		srcstr;
	    *m_log << MSG::DEBUG << histname << endmsg;
	    if ((m_l1hh[tell1id] = histoSvc()->book(histname, histname, 256, 
						    0.0, 4095.0)) == NULL) {
		*m_log << MSG::ERROR << "booking histogram" << endmsg;
		return StatusCode::FAILURE;
	    }
	}
	int nb = sizeof(l1_int) * len;
	memcpy(&frgbuf[tell1id][4 + fgh->part.len], &b[i], nb);
	fgh->part.len += nb;
	fgh->part.evtid = eventid;
    }
    unsigned int total = 0;
    for (int i = 0; i < m_l1ntell1; ++i) {
	if (!bankpresent[i]) {
	    *m_log << MSG::WARNING << "Missing bank from TELL1: " << i 
		   << endmsg;
	    if (!m_nomep && !m_l1fd[i]) {
		*m_log << MSG::ERROR << "File has not been created: !"
		       << endmsg;
		return StatusCode::FAILURE;
	    }
	}
	if (bankpresent[i] > 1) {
	    *m_log << MSG::WARNING << bankpresent[i] << " banks from TELL1: " << i 
		   << endmsg;
	}
	efg_t *fgh = (efg_t *) &frgbuf[i][0];
	total += fgh->part.len;
	if (!m_nomep) {
	    if (fwrite(&frgbuf[i][0], sizeof(efg_t) + fgh->part.len, 1, m_l1fd[i]) 
		!= 1) {
		*m_log << MSG::ERROR << "fwrite: " << sys_errlist[errno] << 
		    endmsg;
		return StatusCode::FAILURE;
	    }
	    m_fgwritten[i]++;
	}
	if (m_rmout != -1) 
	    wlen += write(m_rmout, &frgbuf[i][4], fgh->part.len);
//	    *m_log << MSG::DEBUG << "wrote event " << eventid << endmsg;
	
	if (m_l1hh[i]) m_l1hh[i]->fill((double) fgh->part.len);	    
    }  
    if (n * sizeof(l1_int) != total) { /* should not happen :-) */
      *m_log << MSG::ERROR << "mep len " << total << "!= l1 len" << 
	  n * sizeof(l1_int) << endmsg;
      return StatusCode::FAILURE;
    }
    if (m_rmout != -1 && wlen != (n *sizeof(l1_int) + 4)) {
	*m_log << MSG::ERROR << "raw write len mismatch!" << endmsg;
	return StatusCode::FAILURE;
    }
    ++eventid;
    return StatusCode::SUCCESS;     
}

//--------------------------------------------------------------------
// Execute
//--------------------------------------------------------------------
StatusCode L1MEP::execute() {
    StatusCode sc = StatusCode::SUCCESS;
    SmartDataPtr<L1Buffer> l1(eventSvc(), L1BufferLocation::Default);
 
    if ( l1 != 0 )  {
	//sc = decodel1(l1->buffer(), l1->currentSize());
	sc = writel1(l1->buffer(), l1->currentSize());
	if (sc != StatusCode::SUCCESS) return sc;
	m_eventid++;
	return sc;
    }
    *m_log << MSG::ERROR << "Unable to retrieve Event object" << endreq;
    return StatusCode::FAILURE;
}

StatusCode
L1MEP::finalize() 
{
    unsigned long long written;
    int locked = 0, err = 0;

    for (int i = 0; i < m_l1ntell1; ++i) {
	if (m_l1fd[i]) fclose(m_l1fd[i]);
	*m_log << MSG::INFO << "TELL1: " << i << "wrote " << m_fgwritten[i] << endmsg;
	if (!locked) {
	    locked = 1;
	    written = m_fgwritten[i];
	} else if ((! m_nomep) && written != m_fgwritten[i]) err = 1;
    }
    if (err) *m_log << MSG::ERROR << "SEVERE: mismatch in number of written MEP fragments!";
    return StatusCode::SUCCESS;

}
