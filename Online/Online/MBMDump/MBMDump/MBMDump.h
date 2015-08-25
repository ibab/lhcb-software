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
  class MDFIOHelper;
  class MBMMainMenu;
  class FileMenu;
  class FileMainMenu;
  class Requirement;
  class PrintMenu;
  class DisplayMenu;
  class MEPWindow;
  class BankWindow;
  class FragmentWindow;
  class MultiFragmentWindow;

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
    /// Flag to steer content of first (index) column
    int column_one_flag;
    /// Number of data words per display line
    int words_per_line;
    /// Flag to show ASCII dump of columns
    int ascii_flag;
    /// Columns wise format specification
    const char *fmt[MAX_WORDS_PER_LINE];
    /// Standard constructor defining defaults
    Format();
    /// Access to HEX data format
    static const char* FMT_HEX08();
    /// Access to decimal data format
    static const char* FMT_DEC10();
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
    /// Start of data block
    int *start;
    /// Length of data block in long words (32 bit)
    int  length;
    /// Event number
    int  number;
    /// Trigger mask
    unsigned int mask[4];
    /// Header word
    int header;
  };

  /**@class EventInput MBMDump.h  MBMDump/MBMDump.h
    *
    * Purpose:
    * Request for new event
    *
    * @author  M.Frank
    * @version 1.0
    */
  class EventInput  {
  public:
    /// Access MEP identifier (if configured)
    virtual BMID mepID() const { return 0; }
    /// Wait and get next event
    virtual int getEvent(struct DataBlock *event) = 0;
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
    /// Quit menu
    int quit();
    /// open new menu
    void openMenu(int par_id, int cmd_id, const char* up, const char* tit, const char* down);
    /// open new menu
    void openDetached(int par_id, int cmd_id, const char* up, const char* tit, const char* down);
    /// Change menu titles
    void changeTitles(const char* up, const char* tit, const char* down);
    /// Close menu
    void closeMenu();
    /// Delete menu
    void deleteMenu();
    /// Set cursor position
    void setCursor(int cmd_id,int par_id=1);
    /// Set cursor position
    void setCursor(int menu_id, int cmd_id,int par_id);
    /// Add comment line
    void addComment(int id, const char* txt);
    /// Replace comment line
    void replaceComment(int com_id, const char* txt);
    /// Add command line
    void addCommand(int id, const char* txt, bool enable=true);
    /// Delete command item
    void deleteCommand(int cmd_id);
    /// Replace command line
    void replaceCommand(int com_id, const char* txt);
    /// Enable command item
    void enableCommand(int cmd_id);
    /// Insert command item
    void insertCommand(int pos_id, int cmd_id, const char* txt, bool enable=true);
    void enableCommands(int n, ...);
    void disableCommands(int n, ...);
    void hideMenu();
    void openWindow();
    void openOldWindow();
    void setParam(char* var, int par_id, const char* fmt, const char* def, const char* mi, const char* ma, const char** lst,int len,  bool flg);
    void setParam(int* var, int par_id, const char* fmt, const int def, const int mi, const int ma, const int* lst,int len,  bool flg);
    void setParam(unsigned int* var, int par_id, const char* fmt, const unsigned int def, const unsigned int mi, const unsigned int ma, const unsigned int* lst,int len,  bool flg);
    void setParam(float* var, int par_id, const char* fmt, const float def, const float mi, const float ma, const float* lst,float len,  bool flg);

    /// Output message to message window
    void output(const char* txt, ...);
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

  /** @class PrintMenu MBMDump.h MBMDump/MBMDump.h
    *
    * Purpose:
    * Offer the settings to create a hardcopy.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class PrintMenu : public BaseMenu  {
  private:
    enum { WINDOW=0, 
           C_DATA=1 
    };
    /// Command labels for the different menu items
    enum { C_PR =1, 
           C_NL,
           C_COM1,
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

  /** @class DisplayWindow MBMDump.h MBMDump/MBMDump.h
    *
    * Purpose:
    * Show the raw data dump for a given event.
    *
    * @author  M.Frank
    * @version 1.0
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

  /** @class BankWindow MBMDump.h MBMDump/MBMDump.h
    *
    * Purpose:
    * Display a raw bank structure.
    * Show all data words for the bank.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class BankWindow : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum { C_DISMISS  = 9999999,
           C_DISMISS2 = 9999998,
           C_COM1=1,
           C_COM2,
           C_COM3,
           C_DATA};
    /// Connected parent command identifier
    int   m_parentCmd;
    /// Reference to display format specification(s)
    const Format& m_fmt;
  public:
    /// Standard constructor with initialization
    BankWindow(BaseMenu* par,int cmd_id, const Format& f, const LHCb::RawBank* b);
    /// Default destructor
    virtual ~BankWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /** @class FragmentWindow MBMDump.h MBMDump/MBMDump.h
    *
    * Purpose:
    * Display a MEP fragment.
    * Offer the possibility to expand the list of banks.
    * Offer the possibility to expand for each bank
    * the attached data word.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class FragmentWindow : public BaseMenu  {
  private:
    /// Command labels for the different menu items
    enum { C_DISMISS   = 9999999,
           C_DISMISS2  = 9999998,
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
    int                 m_parentCmd;
    /// Reference to display format specification(s)
    const Format&       m_fmt;
    /// Pointer to MEP fragment
    LHCb::MEPFragment*  m_frag;
    /// Pointer to display of all raw banks
    BankWindow*         m_bankWindow;
  public:
    /// Standard constructor with initialization
    FragmentWindow(BaseMenu* par,int cmd_id, const Format& f, LHCb::MEPFragment* frag);
    /// Default destructor
    virtual ~FragmentWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /** @class BankListWindow MBMDump.h MBMDump/MBMDump.h
    *
    * Purpose:
    * Display from a vector of raw banks all containees.
    * Offer the possibility to expand for each bank
    * the attached data word.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class BankListWindow : public BaseMenu  {
  public:
    typedef std::vector<std::pair<unsigned int,LHCb::RawBank*> > Banks;
  protected:
    /// Command labels
    enum { C_DISMISS   = 9999999,
           C_DISMISS2  = 9999998,
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
    int           m_parentCmd;
    /// Reference to display format specification(s)
    const Format& m_fmt;
    /// Container with all banks
    Banks         m_banks;
    /// Pointer to display of all raw banks
    BankWindow*   m_bankWindow;
  public:
    /// Standard constructor with initialization
    BankListWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& banks, bool bld=true);
    /// Default destructor
    virtual ~BankListWindow();
    /// Build the menu
    void build();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /** @class MEPBankTypesWindow MBMDump.h MBMDump/MBMDump.h
    *
    * Purpose:
    * Extract and display from a vector of raw banks the different types
    * and display them. Offer the possibility to expand for each type
    * the list of banks connected.
    * Offer the possibility to expand for each banks
    * the attached data word.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class BankTypesWindow : public BankListWindow  {
  protected:
    typedef std::map<std::pair<int,int>,std::pair<const LHCb::RawBank*,int> > BankMap;
    /// Extension of command labels from the base class BankListWindow
    enum { C_TYPES = C_BANKS+100000 };
    /// Map of known bank types
    BankMap         m_map;
    /// Bank buffer with same type (for display only)
    Banks           m_banksOfType;
    /// Pointer to display of all raw banks
    BankListWindow* m_bankWindow2;
  public:
    /// Standard constructor with initialization
    BankTypesWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& banks);
    /// Default destructor
    virtual ~BankTypesWindow();
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
  };

  /**
    *
    *
    */
  class MultiFragmentWindow : public BaseMenu  {
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
    FragmentWindow* m_fragWindow;
    /// Pointer to display of all raw banks
    BankListWindow* m_banksWindow;
  public:
    /// Standard constructor with initialization
    MultiFragmentWindow(BaseMenu* par,int cmd_id, const Format& f, LHCb::MEPMultiFragment* mf);
    /// Default destructor
    virtual ~MultiFragmentWindow();
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
    BankListWindow* m_banksWindow;
    /// Pointer to display of MEP multi fragments
    MultiFragmentWindow* m_multiFrags;
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
    /// Display format specification(s)
    Format m_fmt;
    /// Buffer for column 1 value in display
    char   m_col1Value[9];
    /// String buffer for display containing abbreviated format
    char   m_fmtString[MAX_WORDS_PER_LINE];
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
          C_BTMEP, 
          C_CHECKMEP,
          C_RAW,
          C_BLRAW, 
          C_BTRAW, 
          C_CHECKRAW,
          C_MDF,
          C_BLMDF, 
          C_BTMDF, 
          C_CHECKMDF,
          C_DSC,
          C_BLDSC, 
          C_BTDSC, 
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
    EventInput*        m_main;
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
    BankListWindow* m_banksWindow;
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
    DisplayMenu(BaseMenu* parent, int cmd_id);
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

  class MBMMainMenu : public BaseMenu, virtual public EventInput  {
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
    int   m_partID;
    char  m_name[64];
    char  m_buffName[64];
    char  m_buffType[64];
    int*  m_memory;
    /// Buffer to requirements menus
    Requirement m_req[8];
    /// Pointer to display menu
    DisplayMenu* m_dispMenu;
  public:
    /// Standard constructor
    MBMMainMenu();
    /// Default destructor
    virtual ~MBMMainMenu();
    /// Access to MEP identifier
    virtual BMID mepID() const { return m_bmID; }
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
    /// Include into specified MBM buffer
    int includeMBM();
    /// Exclude from spcified MBM buffer
    int excludeMBM();
    /// Wait and get next event
    virtual int getEvent(struct DataBlock *event);
  };

  class FileMenu : public BaseMenu {
  private:
    /// Buffer for file name
    char          m_fileName[255];
    /// Invoking parent command
    int           m_parentCmd;
    /// Reference to true parent
    FileMainMenu* m_file;
  public:
    /// Standard constructor
    FileMenu(FileMainMenu* par,int cmd_id);
    /// Default destructor
    virtual ~FileMenu();
    /// Access to file name buffer
    char* fileName()  {  return m_fileName;  }
    /// Virtual overload to handle menu interaction(s)
    void handleMenu(int cmd_id);
  private:
    /// Private copy constructor
    FileMenu(const FileMenu&) : BaseMenu() {}
    /// Private assignment operator
    FileMenu& operator=(const FileMenu&) { return *this; }
  };

  class FileMainMenu : public BaseMenu, virtual public EventInput  {
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
    /// Buffer type
    char         m_buffType[32];
    /// File name buffer
    char         m_name[255];
    /// Pointer to MDF IO object
    MDFIOHelper* m_io;
    /// Pointer to file name dialog
    FileMenu*    m_fileMenu;
    /// Pointer to display menu
    DisplayMenu* m_dispMenu;
  public:
    /// Standard constructor
    FileMainMenu();
    /// Default destructor
    virtual ~FileMainMenu();
    /// Access to file name buffer
    const char* fileName() const   {  return m_name; }
    /// Virtual overload to handle menu interaction(s)
    virtual void handleMenu(int cmd_id);
    /// Wait and get next event
    virtual int getEvent(struct DataBlock *event);
    /// On accept new file name
    void acceptFile(const char* fnam);
  private:
    /// Private copy constructor
    FileMainMenu(const FileMainMenu&) : BaseMenu() {}
    /// Private assignment operator
    FileMainMenu& operator=(const FileMainMenu&) { return *this; }
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
}
#endif //  MBMDUMP_H
