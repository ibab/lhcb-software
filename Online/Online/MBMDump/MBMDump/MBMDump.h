#ifndef MBMDUMP_H
#define MBMDUMP_H 1
#include <vector>
#include <utility>
#include "RTL/bits.h"
#include "MBM/bmdef.h"
#include "MBM/mepdef.h"

namespace LHCb {
  class RawBank;
  class MEPEvent;
  class MEPFragment;
  class MEPMultiFragment;
}

/*
 *  MBMDump namespace declaration
 */
namespace MBMDump  {
  class MainMenu;
  class Requirement;
  class PrintMenu;
  class DisplayMenu;
  class MEPWindow;
  class MEPBankWindow;
  class MEPFragmentWindow;
  class MEPMultiFragmentWindow;

  enum { MAX_WORDS_PER_LINE = 8 };

  /**@class Format MBMDump.h  MBMDump/MBMDump.h
    *
    * Purpose:
    * Define display format for data pages.
    *
    * @author  P.Maley/M.Frank
    * @version 1.0
    */
  struct Format{
    int column_one_flag;
    int words_per_line;
    int ascii_flag;
    const char *fmt[MAX_WORDS_PER_LINE];
    /// Standard constructor defining defaults
    Format();
  };

  /**@class DataBlock MBMDump.h  MBMDump/MBMDump.h
    *
    * Purpose:
    * Define event buffer info and MBM info for data display(s).
    *
    * @author  P.Maley/M.Frank
    * @version 1.0
    */
  struct DataBlock{
    int *start;
    int  length;
    int  number;
    unsigned int name[4];
    int header;
  };

  /**@class BaseMenu MBMDump.h  MBMDump/MBMDump.h
    *
    * Purpose:
    * Common base class for all MBMDump UPI menus
    *
    * @author  M.Frank
    * @version 1.0
    */
  class BaseMenu  {
  private:
    int m_ID;
    BaseMenu* m_parent;
  public:
    /// Standard constructor
    BaseMenu();
    /// Standard constructor with initialization
    BaseMenu(BaseMenu* par);
    /// Default destructor
    virtual ~BaseMenu();
    /// Access to the menu identifier
    int id() const { return m_ID; }
    /// Set menu parent
    void setParent(BaseMenu* par)  { m_parent = par;  }
    /// Access the menu patent
    BaseMenu& parent()  const;
    /// Run the handling event loop
    virtual int run();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id) = 0;
    /// Dispatch routine
    static int dispatch(int menu_id, int cmd_id, ...);
  };

  /**@class Requirement MBMDump.h  MBMDump/MBMDump.h
    *
    * Purpose:
    * Menu class handling MBM requirement I/O
    *
    * @author  M.Frank
    * @version 1.0
    */
  class Requirement : public BaseMenu  {
  public:
    typedef Bits::BitMask<4> Mask;
    /// Command labels for the different menu items
    enum { C_EVT = 1, 
           C_TMASK0, C_TMASK1, C_TMASK2, C_TMASK3, 
           C_VETO0,  C_VETO1,  C_VETO2,  C_VETO3, 
           C_RTYPE, 
           C_MODE, 
           C_ADD, 
           C_PTO
    }; 
  private:
    bool m_reqActive;
    BMID m_bmID;
    int  m_evType;
    Mask m_trMask;
    Mask m_veto;
    int  m_reqTyp;
    int  m_reqMode;
    char m_rtype_c[4];
    char m_mode_c[8];
  public:
    /// Standard constructor
    Requirement();
    /// Default destructor
    virtual ~Requirement() {}
    /// Set buffer ID to existing requirement
    void setBufferID(BMID bm);
    /// Build the menu
    void build(int pg, int menu_id, int cmd_id);
    /// Add MBM requirement
    void add();
    /// Remove MBM requirement
    void remove();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class PrintMenu : public BaseMenu  {
  private:
    enum { WINDOW=0, 
           C_DATA=1 
    };
    /// Command labels for the different menu items
    enum { C_PR =1, 
           C_NL,
           C_FN,
           C_AP
    };
    char m_fileName[255];
    int  print_flag;      /* 0 => window, 1 => current data */
    int  m_mxLines;
    char m_prtFlag[11];
    char m_fileMode[2];
  public:
    /// Standard constructor
    PrintMenu(BaseMenu* par, int cmd_id);
    /// Default destructor
    virtual ~PrintMenu() {}
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class DisplayWindow : public BaseMenu  {
  public:
    /// Standard constructor with initialization
    DisplayWindow(BaseMenu* parent, int cmd_id);
    /// Default destructor
    virtual ~DisplayWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class MEPBankWindow : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum { C_DISMISS = 9999999,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_DATA};
    /// Connected parent command identifier
    int m_parentCmd;
    /// Reference to display format specification(s)
    const Format& m_fmt;
  public:
    /// Standard constructor with initialization
    MEPBankWindow(BaseMenu* par,int cmd_id, const Format& f, const LHCb::RawBank* b);
    /// Default destructor
    virtual ~MEPBankWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class MEPFragmentWindow : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum { C_DISMISS   = 9999999,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_BANKS
    };
    /// Connected parent command identifier
    int m_parentCmd;
    /// Reference to display format specification(s)
    const Format& m_fmt;
    LHCb::MEPFragment* m_frag;
    /// Pointer to display of all raw banks
    MEPBankWindow* m_bankWindow;
  public:
    /// Standard constructor with initialization
    MEPFragmentWindow(BaseMenu* par,int cmd_id, const Format& f, LHCb::MEPFragment* frag);
    /// Default destructor
    virtual ~MEPFragmentWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class MEPBankListWindow : public BaseMenu  {
  public:
    typedef std::vector<std::pair<unsigned int,LHCb::RawBank*> > Banks;
  private:
    enum { C_DISMISS   = 9999999,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_BANKS
    };
    /// Connected parent command identifier
    int m_parentCmd;
    /// Reference to display format specification(s)
    const Format& m_fmt;
    Banks m_banks;
    /// Pointer to display of all raw banks
    MEPBankWindow* m_bankWindow;
  public:
    /// Standard constructor with initialization
    MEPBankListWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& banks);
    /// Default destructor
    virtual ~MEPBankListWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class MEPMultiFragmentWindow : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum { C_DISMISS   = 9999999,
           C_SHOWBANKS = 9999998,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_COM8,
           C_COM9,
           C_COM10,
           C_FRAGS
    };
    /// Connected parent command identifier
    int m_parentCmd;
    /// Reference to display format specification(s)
    const Format& m_fmt;
    /// Pointer to data fragment
    LHCb::MEPMultiFragment* m_frag;
    /// Pointer to fragment window (if invoked)
    MEPFragmentWindow* m_fragWindow;
    /// Pointer to display of all raw banks
    MEPBankListWindow* m_banksWindow;
  public:
    /// Standard constructor with initialization
    MEPMultiFragmentWindow(BaseMenu* par,int cmd_id, const Format& f, LHCb::MEPMultiFragment* mf);
    /// Default destructor
    virtual ~MEPMultiFragmentWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class MEPWindow : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum { C_DISMISS   = 9999999,
           C_SHOWBANKS = 9999998,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_COM4,
           C_COM5,
           C_COM6,
           C_COM7,
           C_COM8,
           C_MULTIFRAGS
           };
    /// Connected parent command identifier
    int m_parentCmd;
    /// Reference to display format specification(s)
    const Format& m_fmt;
    /// Pointer to display of all raw banks
    MEPBankListWindow* m_banksWindow;
    /// Pointer to display of MEP multi fragments
    MEPMultiFragmentWindow* m_multiFrags;
  public:
    /// Standard constructor with initialization
    MEPWindow(DisplayMenu* par, int cmd_id, const Format& f);
    /// Default destructor
    virtual ~MEPWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class FormatMenu : public BaseMenu {
  private:
    /// Command labels for the different menu items
    enum {C_STD = 1, 
          C_COL1,
          C_NW,
          C_FMT
    };
    /// Buffer for column 1 value in display
    char   m_col1Value[9];
    /// String buffer for display containing abbreviated format
    char   m_fmtString[MAX_WORDS_PER_LINE];
    /// Display format specification(s)
    Format m_fmt;
  public:
    /// Standard constructor
    FormatMenu(BaseMenu* parent,int cmd_id);
    /// Default destructor
    virtual ~FormatMenu() {}
    /// Access to edited data format
    const Format& fmt() const        { return m_fmt; }
    /// Virtual overload to handle menu interaction(s)
    void  handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class DisplayMenu : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum {C_COM0 = 123,
          C_COM1,
          C_COM2,
          C_COM3,
          C_COM4,
          C_COM5,
          C_COM6,
          C_COM7,
          C_COM8,
          C_FMT, 
          C_FMT2,
          C_GET,
          C_DMP,
          C_MEP,
          C_BLMEP, 
          C_CHECKMEP,
          C_RAW,
          C_BLRAW, 
          C_CHECKRAW,
          C_MDF,
          C_BLMDF, 
          C_CHECKMDF,
          C_DSC,
          C_BLDSC, 
          C_CHECKDSC,
          C_HC,
          C_STD,
          C_HDR,
          C_TOP,
          C_BOT,
          C_UP,
          C_DN,
          C_PU,
          C_PD,
          C_CMW,
          C_DHC,
          C_GO, 
          C_RESET,
          C_DISPLAY,
          C_WRITE,
          C_WW,
          C_WD
    };
    std::vector<char*> m_lines;
    DataBlock          m_evtData;
    DataBlock          m_bankData;
    DataBlock          m_currData;
    Format             m_fmt;
    int                m_dispOffset;
    MainMenu*          m_main;
    BaseMenu*          m_mepWindow;
    /// Pointer to window containing data formatting information
    FormatMenu*        m_fmtDataWindow;
    /// Pointer to window containing bank formatting information
    FormatMenu*        m_fmtBankWindow;
    /// Pointer to dump display
    DisplayWindow*     m_dispWindow;
    /// Pointer to output control window
    PrintMenu*         m_printWindow;
    /// Pointer to bank list window (if invoked)
    MEPBankListWindow* m_banksWindow;
    /// Pointer to start of MEP record (if showing MEP related data)
    int*               m_mepStart;
    int                m_bufType;
    int shiftWordsUp    (int first_word,int n_lines);
    int shiftWordsDown  (int first_word,int n_lines);
    void shiftBlockUp   (int nl_shift,int n_max, int l_len);
    void shiftBlockDown (int nl_shift,int n_max, int len);
    void shiftLinesUp   (int n, int len);
    void shiftLinesDown (int n, int len);
    int jumpToLine      (int req_offset,int n_lines);
    int format_line     (int first_word, char *c);
  public:
    /// Buffer type labels
    enum {B_UNKNOWN,
          B_MEP,
          B_RAW,
          B_DESC,
          B_MDF
    };
    /// Standard constructor with initialization
    DisplayMenu(BaseMenu* parent, int menu_id, int cmd_id);
    /// Default destructor
    virtual ~DisplayMenu();
    /// Access to event data block
    const DataBlock& evtData() const  { return m_evtData; }
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
    /// Update menu content according to input data type
    void update(int buf_typ);
    /// Show the menu
    void show();
  };

  class MainMenu : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum {C_COM1 = 1,
          C_COM2,
          C_COM3,
          C_COM4,
          C_COM5,
          C_COM6,
          C_COM7,
          C_COM8,
          C_PROC, 
          C_PART,
          C_BUF,
          C_TYP,
          C_RQS,
          C_INC_EXC,
          C_CMD,
          C_DISPLAY,
          C_DEBUG,
          C_EXIT
    };
    BMID  m_bmID;
    MEPID m_mepID;
    int   m_partID;
    char  m_name[12];
    char  m_buffName[12];
    char  m_buffType[12];
    int   m_mepFlags;
    int*  m_memory;
    Requirement m_req[8];
    DisplayMenu* m_dispMenu;
  public:
    /// Standard constructor
    MainMenu();
    /// Default destructor
    virtual ~MainMenu();
    /// Access to MEP identifier
    MEPID mepID() const { return m_mepID; }
    /// Build the menu
    void build();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
    /// Include into specified MBM buffer
    int includeMBM();
    /// Exclude from spcified MBM buffer
    int excludeMBM();
    /// Include into specified MEP buffer(s)
    int includeMEP();
    /// Exclude from spcified MEP buffer(s)
    int excludeMEP();
    /// Wait and get next event
    int getEvent(struct DataBlock *event);
  };

  struct Constants {
    static const int N_LINES = 23;
    static const int LINE_LENGTH = 255;
  };

  const char* procName();
  void rconv(char c[], int word);
  template<class T> inline T* drop(T*& ref)  {
    if ( ref != 0 ) delete ref;
    return ref = 0;
  }
  template<class T, class Q> inline T* replace(T*& ref, Q* rep)  {
    if ( ref != 0 ) delete ref;
    return ref = rep;
  }
};
#endif //  MBMDUMP_H
