// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/DisplayMenu.cpp,v 1.11 2008-09-26 09:51:36 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DisplayMenu.cpp,v 1.11 2008-09-26 09:51:36 frankb Exp $
//
// C++ include files
#include <map>
#include "MBMDump/MBMDump.h"
#include "MBM/mepdef.h"
#include "MDF/MEPEvent.h"
#include "MDF/MDFHeader.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventDescriptor.h"

#define N_LINES        Constants::N_LINES
#define LINE_LENGTH    Constants::LINE_LENGTH

using namespace LHCb;
using namespace MBMDump;

DisplayMenu::DisplayMenu(BaseMenu* ptr, int cmd_id) 
: BaseMenu(ptr), m_main(0), m_mepWindow(0), m_banksWindow(0), m_bufType(B_UNKNOWN)
{
  m_bankData.start  = m_evtData.start  = 0;
  m_bankData.length = m_evtData.length = 0;
  m_main = dynamic_cast<EventInput*>(ptr);
  for(int j=0; j<N_LINES; ++j)
    m_lines.push_back(new char[LINE_LENGTH+1]);
  openMenu(parent().id(),cmd_id,"Display menu","Define data view",procName());
  addComment(C_COM1,  "___________________________");
  addComment(C_COM2,  "                           ");
  addComment(C_COM3,  " Buffer type: Unknown      ");
  addComment(C_COM4,  "___________________________");
  addCommand(C_GET,   "Get event and display      ");
  addCommand(C_DMP,   "Display current event      ");
  addCommand(C_FMT,   "Set display data format    ");
  addCommand(C_FMT2,  "Edit bank format           ");
  addComment(C_COM5,  "___________________________");
  addComment(C_COM6,  "                           ");
  addCommand(C_TOP,   "Goto top                   ");
  addCommand(C_BOT,   "Goto bottom                ");
  addCommand(C_UP,    "Scroll up                  ");
  addCommand(C_DN,    "Scroll down                ");
  addCommand(C_PU,    "Page up                    ");
  addCommand(C_PD,    "Page down                  ");
  setParam(&m_dispOffset,2,"%5d",0,0,99999,0,0,0);
  addCommand(C_GO,    "Go to offset    ^^^^^      ");
  //addCommand(C_DHC,   "Create hardcopy          ");
  closeMenu();
  // Create display window
  m_dispWindow    = new DisplayWindow(this,0);
  // Cause format window to be made (and thus user format defined)
  m_fmtDataWindow = new FormatMenu(this,C_FMT);
  m_fmtBankWindow = new FormatMenu(this,C_FMT2);
  // Cause hardcopy menu to be made (and set pointer to print info)
  m_printWindow   = new PrintMenu(this,C_DHC);
}

DisplayMenu::~DisplayMenu() {
  deleteMenu();
  for(int j=0; j<N_LINES; ++j)
    delete [] m_lines[j];
  drop(m_dispWindow);
  drop(m_fmtDataWindow);
  drop(m_fmtBankWindow);
  drop(m_printWindow);
}

void DisplayMenu::update(int buf_type) {
  if ( m_bufType == B_MEP )  {
    deleteCommand(C_MEP);
    deleteCommand(C_BLMEP);
    deleteCommand(C_CHECKMEP);
  }
  else if ( m_bufType == B_RAW )  {
    //deleteCommand(C_RAW);
    deleteCommand(C_BLRAW);
    deleteCommand(C_BTRAW);
    deleteCommand(C_CHECKRAW);
  }
  else if ( m_bufType == B_MDF )  {
    //deleteCommandC_MDF);
    deleteCommand(C_BLMDF);
    deleteCommand(C_BTMDF);
    deleteCommand(C_CHECKMDF);
  }
  else if ( m_bufType == B_DESC )  {
    //deleteCommand(C_DSC);
    deleteCommand(C_BLDSC);
    deleteCommand(C_BTDSC);
    deleteCommand(C_CHECKDSC);
  }
  m_bufType = buf_type;
  if ( m_bufType == B_MEP )  {
    replaceComment(      C_COM3,     " Buffer type: MEP structure");
    insertCommand(C_FMT, C_MEP,      "Show MEP structure       ");
    insertCommand(C_FMT, C_BLMEP,    "Show bank list           ");
    insertCommand(C_FMT, C_CHECKMEP, "Check event sanity       ");
  }
  else if ( m_bufType == B_RAW )  {
    replaceComment(      C_COM3,     " Buffer type: RawEvent   ");
    insertCommand(C_FMT, C_BLRAW,    "Show bank list           ");
    insertCommand(C_FMT, C_BTRAW,    "Show bank type list      ");
    insertCommand(C_FMT, C_CHECKRAW, "Check event sanity       ");
  }
  else if ( m_bufType == B_MDF )  {
    replaceComment(      C_COM3,     " Buffer type: MDF record ");
    insertCommand(C_FMT, C_BLMDF,    "Show bank list           ");
    insertCommand(C_FMT, C_BTMDF,    "Show bank type list      ");
    insertCommand(C_FMT, C_CHECKMDF, "Check event sanity       ");
  }
  else if ( m_bufType == B_DESC )  {
    replaceComment(      C_COM3,     " Buffer type: Descriptor ");
    insertCommand(C_FMT, C_BLDSC,    "Show bank list           ");
    insertCommand(C_FMT, C_BTDSC,    "Show bank type list      ");
    insertCommand(C_FMT, C_CHECKDSC, "Check event sanity       ");
  }
  else  {
    replaceComment( C_COM3,  " Buffer type: Unknown    ");
  }
}

void DisplayMenu::show() {
  setCursor( C_GET, 1);
}

void DisplayMenu::handleMenu(int cmd_id)    {
  int i, status, nl;
  const char* ptr;
  static char down_title[LINE_LENGTH];
  static int n_ftd_lines;
  static int n_read;
  static int offset_first_word;
  static int n_last_read;
  unsigned int partID;
  static int evt = 0;
  MEPEVENT* e;

  switch(cmd_id){
    case C_MEP:
      // MEP header menu
      replace(m_mepWindow,new MEPWindow(this, C_MEP, m_fmtBankWindow->fmt()));
      break;
    case C_BLMEP:
      if ( (e=(MEPEVENT*)m_evtData.start) )  {
        BankListWindow::Banks banks;
        if ( decodeMEP2Banks((MEPEvent*)e->data,partID,banks).isSuccess() )  {
          if ( cmd_id == C_BTMEP )
            replace(m_banksWindow,new BankTypesWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
          else
            replace(m_banksWindow,new BankListWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
          return;
        }
      }
      output("Invalid MEP event .... failed to create bank list.");
      break;
    case C_CHECKMEP:
      e=(MEPEVENT*)m_evtData.start;
      if(checkMEPEvent((MEPEvent*)e->data,true,false)) {
        output("Sanity check completed successfully.");
        return;
      }
      output("Invalid MEP event .... failed to check data.");
      break;
    case C_RAW:
    case C_BLRAW:
    case C_BTRAW:
      ptr = (const char*)m_evtData.start;
      if ( ptr )  {
        std::vector<RawBank*> b;
        BankListWindow::Banks banks;
        if ( decodeRawBanks(ptr,ptr+m_evtData.length*sizeof(int),b).isSuccess() )  {
          for(std::vector<RawBank*>::iterator j=b.begin(); j!=b.end(); ++j)
            banks.push_back(std::make_pair(0,*j));
          if ( cmd_id == C_BTRAW )
            replace(m_banksWindow,new BankTypesWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
          else
            replace(m_banksWindow,new BankListWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
          return;
        }
      }
      break;
    case C_CHECKRAW:
      ptr = (const char*)m_evtData.start;
      checkRawBanks(ptr,ptr+m_evtData.length*sizeof(int),true,false);
      output("Sanity check completed successfully.");
      break;
    case C_MDF:
    case C_BLMDF: 
    case C_BTMDF: 
      ptr = (const char*)m_evtData.start;
      if ( ptr )  {
        std::vector<RawBank*> b;
        BankListWindow::Banks banks;
	size_t len = m_evtData.length*sizeof(int);
        if ( decodeRawBanks(ptr,ptr+len,b).isSuccess() )  {
          for(std::vector<RawBank*>::iterator j=b.begin(); j!=b.end(); ++j)
            banks.push_back(std::make_pair(0,*j));
          if ( cmd_id == C_BTMDF )
            replace(m_banksWindow,new BankTypesWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
          else
            replace(m_banksWindow,new BankListWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
          return;
        }
      }
      break;
    case C_CHECKMDF:
      ptr = (const char*)m_evtData.start;
      checkMDFRecord((MDFHeader*)(ptr+2*sizeof(int)),(m_evtData.length-2)*sizeof(int),true,false);
      output("Sanity check completed successfully.");
      break;
    case C_DSC:
    case C_BLDSC:
    case C_BTDSC:
    case C_CHECKDSC:
      if ( m_evtData.start && m_main->mepID() != MBM_INV_DESC )  {
        RawEventDescriptor dsc;
        std::vector<RawBank*> b;
        BankListWindow::Banks banks;
        dsc.setPartitionID(0);
        dsc.setTriggerMask(m_evtData.mask);
        dsc.setEventType(EVENT_TYPE_EVENT);
        dsc.setHeader(m_evtData.start);
        dsc.setSize(m_evtData.length);
        dsc.setMepBuffer((void*)m_evtData.start);
        for(int j=0, n=dsc.numberOfFragments(); j<n; ++j)  {
          MEPFragment* f = dsc.fragment(j);
          if (cmd_id == C_CHECKDSC) checkFragment(f);
          else                      decodeFragment(f, b);
        }
        if ( cmd_id == C_CHECKDSC )  {
          output("Sanity check completed successfully.");
          return;
        }
        for(std::vector<RawBank*>::iterator j=b.begin(); j!=b.end(); ++j)
          banks.push_back(std::make_pair(0,*j));
        if ( cmd_id == C_BTDSC )
          replace(m_banksWindow,new BankTypesWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
        else
          replace(m_banksWindow,new BankListWindow(this,cmd_id,m_fmtBankWindow->fmt(),banks));
        return;
      }
      output("You are not configured to analyse event descriptors. MEP buffer is not mapped!!!");
      break;
    case C_GET:
      drop(m_mepWindow);
      drop(m_banksWindow);
      status = m_main->getEvent(&m_evtData);
      if(status == MBM_NORMAL){
        handleMenu(C_DMP);  // display event
      }
      break;
    case C_DMP: // For displaying the EVENT
      m_currData = m_evtData;
      m_fmt  = m_fmtDataWindow->fmt();
      ::snprintf(down_title,sizeof(down_title),
		 "   Evtype %3d    Trigger mask %08X %08X %08X %08X   Length %5d (words) ",
		 m_currData.number,m_currData.mask[0],m_currData.mask[1],m_currData.mask[2],
		 m_currData.mask[3],m_currData.length);
      output("Got event....   %d --> %s",++evt,down_title);
      handleMenu(C_TOP);
      break;
    case C_TOP:
      handleMenu(C_RESET);
      handleMenu(C_DISPLAY);       
      handleMenu(C_WRITE);
      break;
    case C_BOT:
      i = m_dispOffset;
      m_dispOffset = m_currData.length;
      handleMenu(C_GO);
      m_dispOffset = i;
      break;
    case C_DN:
      nl = shiftWordsUp(n_read,N_LINES);
      if(nl == 0)break;
      if(n_ftd_lines < N_LINES)break;
      offset_first_word += m_fmt.words_per_line;
      shiftLinesUp(N_LINES,LINE_LENGTH);
      n_last_read = format_line(n_read,&m_lines[N_LINES-1][0]);
      n_read += n_last_read;
      if(n_last_read == 0)n_ftd_lines--;
      handleMenu(C_WRITE);
      break;
    case C_UP:
      if(offset_first_word <= 0)break;   /* < 0 => Fuck up */
      offset_first_word -= m_fmt.words_per_line;
      n_read = offset_first_word + N_LINES*m_fmt.words_per_line;
      shiftLinesDown(N_LINES,LINE_LENGTH);
      n_last_read = format_line(offset_first_word,&m_lines[0][0]);
      handleMenu(C_WRITE);
      break;
    case C_PD:
      nl = shiftWordsUp(n_read,N_LINES);
      if(nl == 0)break;   /* return ?? */
      offset_first_word += nl*m_fmt.words_per_line;
      shiftBlockUp(nl,N_LINES,LINE_LENGTH);
      n_ftd_lines = N_LINES-nl;
      handleMenu(C_DISPLAY);
      handleMenu(C_WRITE);
      break;
    case C_PU:
      nl = shiftWordsDown(offset_first_word,N_LINES);
      if(nl == 0)break;   /* return ?? */
      shiftBlockDown(nl,N_LINES,LINE_LENGTH);
      offset_first_word -= nl*m_fmt.words_per_line;
      n_ftd_lines = 0;
      n_read = offset_first_word; 
      while(n_ftd_lines < nl){
        n_last_read = format_line(n_read,&m_lines[n_ftd_lines][0]);
        n_ftd_lines++;
        n_read += n_last_read;
      }
      n_read += (N_LINES - nl)*m_fmt.words_per_line;
      n_ftd_lines = N_LINES;
      handleMenu(C_WRITE);
      break;
    case C_GO:
      if(n_ftd_lines < N_LINES)break;
      nl = jumpToLine(m_dispOffset,N_LINES);
      offset_first_word = nl*m_fmt.words_per_line;
      n_ftd_lines = 0;
      n_read = offset_first_word;
      handleMenu(C_DISPLAY);
      handleMenu(C_WRITE);
      break;
    case C_CMW:
      //      output("","");
      break;
    case C_RESET:
      n_read = 0;
      n_ftd_lines = 0;
      offset_first_word = 0;
      break;
    case C_DISPLAY:
      m_dispWindow->changeTitles("",down_title,"Display window");
      while(n_ftd_lines < N_LINES){
        n_last_read = format_line(n_read,m_lines[n_ftd_lines]);
        if(n_last_read == 0)break;
        n_ftd_lines++;
        n_read += n_last_read;
      }
      for(i=n_ftd_lines; i < N_LINES; ++i) 
        m_lines[i][0] = 0;
      break;
    case C_WRITE:
      for(i = 0; i < N_LINES; i++)
        m_dispWindow->replaceComment(i+1,m_lines[i]);
      break;
    case C_WW:
      //for(i=0;i < N_LINES && i < pr->m_mxLines;i++)
      //  fprintf(fp,"%s\n",m_lines+i);
      //break;
    case C_WD:
      //for(int nl=0, print_offset=0; nl++ < m_print->m_mxLines; ) {
      //  n_last_read = format_line(print_offset,print_line);
      //  if(n_last_read == 0)break;
      //  fprintf(fp,"%s\n",print_line);
      //  print_offset += n_last_read;
      //}
      break;
    default:
      break;
  }
}

int DisplayMenu::format_line(int first_word, char *c)  {
  int i, j;
  char tmp[LINE_LENGTH], asc[32], wrd[32], wrd2[4];
  tmp[0] = wrd[0] = 0;
  // Column one : nothing, offset or row number
  if(m_fmt.column_one_flag == 1)
    ::snprintf(tmp,sizeof(tmp),"%4d  ",first_word);
  else if(m_fmt.column_one_flag == 2)
    ::snprintf(tmp,sizeof(tmp),"%4d  ",(first_word/m_fmt.words_per_line)+1);
  // Main format
  for(i=0; (first_word + i < m_currData.length) && (i < m_fmt.words_per_line); ++i){
    // Concatenate
    ::snprintf(wrd,sizeof(wrd),m_fmt.fmt[i],*(m_currData.start + first_word + i));
    ::strncat(tmp,wrd,10);
    if(m_fmt.ascii_flag){   // ascii representation
      rconv(wrd2,*(m_currData.start + first_word + i));
      asc[i*5]=' ';
      strncpy(asc+(i*5+1),wrd2,4);
      asc[i*5+1+4]='\0';
    }
  }
  if(m_fmt.ascii_flag){     // Final formatting
    if(i < m_fmt.words_per_line)
      for(j=i*10+6;j<46;j++) *(tmp+j)=' ';
  }
  asc[sizeof(asc)-1]=0;
  ::strncpy(c,tmp,sizeof(tmp));
  ::strncat(c,"   ",4);
  ::strncat(c,asc,sizeof(asc)-1);
  return i;                 // Return no. words read
}

void DisplayMenu::shiftBlockUp(int nl_shift,int n_max, int l_len) {
  for(int i=0; i < (n_max - nl_shift); i++)
    ::strncpy(m_lines[i],m_lines[i+nl_shift],l_len); 
}

void DisplayMenu::shiftBlockDown(int nl_shift,int n_max, int len)  {
  for(int i=1; i < (n_max - nl_shift + 1); i++)
    ::strncpy(m_lines[n_max - i],m_lines[n_max - i- nl_shift],len); 
}

void DisplayMenu::shiftLinesUp(int n,int len) {
  for(int i=0; i < n-1; i++) ::strncpy(m_lines[i], m_lines[i+1],len);  
}

void DisplayMenu::shiftLinesDown(int n,int len)  {
  for(int i=n-1; i > 0; i--) ::strncpy(m_lines[i],m_lines[i-1],len);
}

int DisplayMenu::shiftWordsUp(int first_word,int n_lines) {
  int n_shift = 0, i = 0;
  while( first_word + i < m_currData.length && n_shift < n_lines - 1){
    i += m_fmt.words_per_line;
    n_shift++;
  }  
  return n_shift;
}

int DisplayMenu::shiftWordsDown(int first_word,int n_lines) {
  int n_shift = 0;
  while(first_word > 0 && n_shift < n_lines - 1){
    first_word -= m_fmt.words_per_line;
    n_shift++;
  }
  return n_shift;
}

// This routine assumes that the screen is filled, i.e. that n_ftd_lines = N_LINES.                                    */
int DisplayMenu::jumpToLine(int req_offset,int n_lines)    {
  int n_shift = 0;
  // Do an integer division to get the first guess for n_shift
  n_shift = req_offset/m_fmt.words_per_line;
  while( (n_shift+n_lines-1)*m_fmt.words_per_line >= m_currData.length)
    n_shift--;
  return n_shift;
}
