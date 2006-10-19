#include <map>
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
#include "MBM/mepdef.h"
#include "MDF/MEPEvent.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventPrintout.h"

namespace MBMDump {
  void shift_block_up(int nl_shift,char c[N_LINES][LINE_LENGTH],int n_max, int l_len) {
    for(int i=0; i < (n_max - nl_shift); i++)
      strncpy(&c[i][0],&c[i+nl_shift][0],l_len); 
  }
  void shift_block_down(int nl_shift,char c[N_LINES][LINE_LENGTH], int n_max, int len)  {
    for(int i=1; i < (n_max - nl_shift + 1); i++)
      strncpy(&c[n_max - i][0],&c[n_max - i- nl_shift][0],len); 
  }
  void shift_lines_up(char c[N_LINES][LINE_LENGTH],int n,int len) {
    for(int i=0; i < n-1; i++) strncpy(&c[i][0], &c[i+1][0],len);  
  }
  void shift_lines_down(char c[N_LINES][LINE_LENGTH],int n,int len)  {
    for(int i=n-1; i > 0; i--) strncpy(&c[i][0],&c[i-1][0],len);
  }
}
using namespace LHCb;
using namespace MBMDump;

DisplayMenu::DisplayMenu() : m_main(0), m_mepWindow(0), m_banksWindow(0) {
  Format stdFmt;
  m_bankData.start  = m_evtData.start  = 0;
  m_bankData.length = m_evtData.length = 0;
  strcpy(m_dispViewPort,"Top   ");
  // Set event and bank format to standard format
  for(int i=0;i<MAX_WORDS_PER_LINE;i++)
    stdFmt.fmt[i] = FMT_HEX08;
  stdFmt.words_per_line  = 4;
  stdFmt.column_one_flag = 1;
  stdFmt.ascii_flag      = true;
  m_fmtDataMenu.setFormat(stdFmt);
  m_fmtBankMenu.setFormat(stdFmt);
}

void DisplayMenu::buildMenu(BaseMenu* ptr, int menu_id,int cmd_id)    {
  static char *tb_list[2] = {"Top   ","Bottom"};
  setParent(ptr);
  m_main = dynamic_cast<MainMenu*>(ptr);
  ::upic_open_menu  (id(),menu_id,cmd_id,"Display menu","Define data view",procName());
  ::upic_add_command(C_GET,   "Get event and display    ","");
  ::upic_add_command(C_DMP,   "Display current event    ","");
  ::upic_add_command(C_CHECK, "Check event sanity       ","");
  ::upic_add_command(C_BL,    "Show bank list           ","");
  ::upic_add_command(C_MEP,   "Show MEP structure       ","");
  ::upic_add_command(C_FMT,   "Set display data format  ","");
  ::upic_add_command(C_FMT2,  "Edit bank format         ","");
  ::upic_add_comment(76,      "*************************","");
  ::upic_set_param  (m_dispViewPort,7,"%6s",tb_list[0],0,0,tb_list,2,1);
  ::upic_add_command(C_TB,    "^^^^^^                   ","");
  ::upic_add_command(C_UP,    "Scroll up                ","");
  ::upic_add_command(C_DN,    "Scroll down              ","");
  ::upic_add_command(C_PU,    "Page up                  ","");
  ::upic_add_command(C_PD,    "Page down                ","");
  ::upic_set_param(&m_dispReqOffset,2,"%5d",0,0,99999,0,0,0);
  ::upic_add_command(C_GO,    "Go to offset    ^^^^^    ","");
  //::upic_add_command(C_DHC,   "Create hardcopy          ","");
  ::upic_enable_action_routine(id(),C_GET, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_DMP, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_BL,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_STD, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_HC,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_TB,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_UP,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_DN,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_PU,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_PD,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_GO,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_MEP, Routine(BaseMenu::dispatch));
  ::upic_close_menu();
  m_window.buildMenu(this,0);
  // Cause format window to be made (and thus user format defined)
  m_fmtDataMenu.buildMenu(this,C_FMT);
  m_fmtBankMenu.buildMenu(this,C_FMT2);
  // Cause hardcopy menu to be made (and set pointer to print info)
  m_prtMenu.buildMenu(this,C_DHC);
}

void DisplayMenu::handleMenu(int cmd_id)    {
  int i;
  static int active_bank = 1;
  static unsigned int bank_name[4] = {0x656e6f4e,0x656e6f4e,0x656e6f4e,0x656e6f4e};   /* "None" in hex !! */
  static char up_title[LINE_LENGTH];
  static char down_title[LINE_LENGTH];
  static char lines[N_LINES][LINE_LENGTH];
  static int n_ftd_lines;
  static int n_read;
  static int offset_first_word;
  static int n_last_read;
  unsigned int partID;
  MEPEVENT* e;

  /* variable of file output */
  static struct print_block *pr; /* pointer to print info */
  static FILE *fp;
  int status, nl;
  switch(cmd_id){
    case C_BL:
      e = (MEPEVENT*)m_evtData.start;
      if ( e )  {
        MEPBankListWindow::Banks banks;
        if ( decodeMEP2Banks((MEPEvent*)e->data,partID,banks).isSuccess() )  {
          replace(m_banksWindow,new MEPBankListWindow(this,cmd_id,m_fmtBankMenu.fmt(),banks));
        }
      }
      break;
    case C_CHECK:

    case C_MEP:
      // MEP header menu
      replace(m_mepWindow,new MEPWindow(this, C_MEP, m_fmtBankMenu.fmt()));
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
      m_currFmt  = m_fmtDataMenu.fmt();
      sprintf(down_title,"   Evtype %3d    Trigger mask %08X %08X %08X %08X   Length %5d (words) \0",
              m_currData.number,m_currData.name[0],m_currData.name[1],m_currData.name[2],m_currData.name[3],m_currData.length);
      ::upic_write_message(down_title,"");
      handleMenu(C_TOP);
      break;
    case C_TB:
      if(strcmp(m_dispViewPort,"Top   ") == 0)  {
        handleMenu(C_TOP);
      }
      else{
        int temp_dispReqOffset = m_dispReqOffset;
        m_dispReqOffset = m_currData.length;
        handleMenu(C_GO);
        m_dispReqOffset = temp_dispReqOffset;
      }
      break;
    case C_TOP:
      handleMenu(C_RESET);
      handleMenu(C_DISPLAY);       
      handleMenu(C_WRITE);
      break;
    case C_UP:
      nl = shiftLinesUp(n_read,N_LINES);
      if(nl == 0)break;
      if(n_ftd_lines < N_LINES)break;
      offset_first_word += m_currFmt.words_per_line;
      shift_lines_up(lines,N_LINES,LINE_LENGTH);
      n_last_read = format_line(n_read,&lines[N_LINES-1][0]);
      n_read += n_last_read;
      if(n_last_read == 0)n_ftd_lines--;
      handleMenu(C_WRITE);
      break;
    case C_DN:
      if(offset_first_word <= 0)break;   /* < 0 => Fuck up */
      offset_first_word -= m_currFmt.words_per_line;
      n_read = offset_first_word + N_LINES*m_currFmt.words_per_line;
      shift_lines_down(lines,N_LINES,LINE_LENGTH);
      n_last_read = format_line(offset_first_word,&lines[0][0]);
      handleMenu(C_WRITE);
      break;
    case C_PU:
      nl = shiftLinesUp(n_read,N_LINES);
      if(nl == 0)break;   /* return ?? */
      offset_first_word += nl*m_currFmt.words_per_line;
      shift_block_up(nl,lines,N_LINES,LINE_LENGTH);
      n_ftd_lines = N_LINES-nl;
      handleMenu(C_DISPLAY);
      handleMenu(C_WRITE);
      break;
    case C_PD:
      nl = shiftLinesDown(offset_first_word,N_LINES);
      if(nl == 0)break;   /* return ?? */
      shift_block_down(nl,lines,N_LINES,LINE_LENGTH);
      offset_first_word -= nl*m_currFmt.words_per_line;
      n_ftd_lines = 0;
      n_read = offset_first_word; 
      while(n_ftd_lines < nl){
        n_last_read = format_line(n_read,&lines[n_ftd_lines][0]);
        n_ftd_lines++;
        n_read += n_last_read;
      }
      n_read += (N_LINES - nl)*m_currFmt.words_per_line;
      n_ftd_lines = N_LINES;
      handleMenu(C_WRITE);
      break;
    case C_GO:
      if(n_ftd_lines < N_LINES)break;
      nl = jumpToLine(m_dispReqOffset,N_LINES);
      offset_first_word = nl*m_currFmt.words_per_line;
      n_ftd_lines = 0;
      n_read = offset_first_word;
      handleMenu(C_DISPLAY);
      handleMenu(C_WRITE);
      break;
    case C_CMW:
      ::upic_write_message(" "," ");
      break;
    case C_RESET:
      n_read = 0;
      n_ftd_lines = 0;
      offset_first_word = 0;
      break;
    case C_DISPLAY:
      ::upic_change_titles(m_window.id(),up_title,down_title,"Display window");
      while(n_ftd_lines < N_LINES){
        n_last_read = format_line(n_read,&lines[n_ftd_lines][0]);
        if(n_last_read == 0)break;
        n_ftd_lines++;
        n_read += n_last_read;
      }
      for(i=n_ftd_lines; i < N_LINES; ++i) 
        lines[i][0] = 0;
      break;
    case C_WRITE:
      for(i = 0; i < N_LINES; i++)  {
        //::upic_write_message(lines[i],"");
        ::upic_replace_comment(m_window.id(),i+1,lines[i],"");
      }
      break;
    case C_WW:
      //for(i=0;i < N_LINES && i < pr->m_mxLines;i++)
      //  fprintf(fp,"%s\n",lines+i);
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
  tmp[0] = wrd[0] = '\0';
  // Column one : nothing, offset or row number
  if(m_currFmt.column_one_flag == 1)
    sprintf(tmp,"%4d  ",first_word);
  else if(m_currFmt.column_one_flag == 2)
    sprintf(tmp,"%4d  ",(first_word/m_currFmt.words_per_line)+1);
  // Main format
  i=0;
  while( (first_word + i < m_currData.length) && (i < m_currFmt.words_per_line) ){
    // Concatenate
    sprintf(wrd,m_currFmt.fmt[i],*(m_currData.start + first_word + i));
    strncat(tmp,wrd,10);
    // ascii representation
    if(m_currFmt.ascii_flag){
      rconv(wrd2,*(m_currData.start + first_word + i));
      asc[i*5]=' ';
      strncpy(asc+(i*5+1),wrd2,4);
      asc[i*5+1+4]='\0';
    }
    i++;    
  }
  if(m_currFmt.ascii_flag){    // Final formatting
    if(i < m_currFmt.words_per_line)
      for(j=i*10+6;j<46;j++)
        *(tmp+j)=' ';
  }
  strcpy(c,tmp);
  strcat(c,"   ");
  strcat(c,asc);
  /* Return no. words read */
  return i;
}

int DisplayMenu::shiftLinesUp(int first_word,int n_lines) {
  int n_shift = 0, i = 0;
  while( first_word + i < m_currData.length && n_shift < n_lines - 1){
    i += m_currFmt.words_per_line;
    n_shift++;
  }  
  return n_shift;
}

int DisplayMenu::shiftLinesDown(int first_word,int n_lines) {
  int n_shift = 0;
  while(first_word > 0 && n_shift < n_lines - 1){
    first_word -= m_currFmt.words_per_line;
    n_shift++;
  }
  return n_shift;
}

// This routine assumes that the screen is filled, i.e. that n_ftd_lines = N_LINES.                                    */
int DisplayMenu::jumpToLine(int req_offset,int n_lines)    {
  int n_shift = 0;
  // Do an integer division to get the first guess for n_shift
  n_shift = req_offset/m_currFmt.words_per_line;
  while( (n_shift+n_lines-1)*m_currFmt.words_per_line >= m_currData.length)
    n_shift--;
  return n_shift;
}


