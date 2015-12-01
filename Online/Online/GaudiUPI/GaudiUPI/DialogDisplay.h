// DECLARATIONS FOR OMT MODULE dialogdisplay
#ifndef _DIALOGDISPLAY_H
#define _DIALOGDISPLAY_H 1

#include "GaudiUPI/AbstractDisplay.h"
#include "CPP/Interactor.h"

class DialogDisplayFactory;
class DialogMainDisplay;

/**@class DialogMainDisplay DialogDisplay.h Gaudi/DialogDisplay.h
 *
 *  Class representing application sub-window.
 *
 * @author   M.Frank
 * @version  1.0
 * @date     20/04/2006
 */
class DialogSubDisplay : public AbstractSubDisplay   {
  friend class DialogDisplayFactory;
  friend class DialogMainDisplay;
 public:
  /// Default destructor
  virtual ~DialogSubDisplay();
  int map();
  int close();
  int repaint();
  int endUpdate();
  int beginUpdate();
  int putString(const std::string& text,int video);
  virtual int init(size_t num_col,size_t num_row, AbstractMainDisplay& parent);
 protected:
  DialogSubDisplay (AbstractMainDisplay& source);
  DialogSubMenu  *window() {     return m_window;   }
  DisplayBoard& buffer(size_t i) { 
    return i < height() ? *m_board[i] : *m_board[0]; 
  }
  DisplayLabel& Overflow() {    return m_overflow;  }
 private:
  /// Private copy constructor
  DialogSubDisplay(const DialogSubDisplay&) {}
  /// Private assignment operator
  DialogSubDisplay& operator=(const DialogSubDisplay&) { return *this; }
 protected:
  DisplayLabel   m_overflow;
  DisplayBoard** m_board;
  DialogSubMenu* m_window;
};

/**@class DialogMainDisplay DialogDisplay.h Gaudi/DialogDisplay.h
 *
 *  Class representing main application window.
 *
 * @author   M.Frank
 * @version  1.0
 * @date     20/04/2006
 */
class DialogMainDisplay : public AbstractMainDisplay, public CPP::Interactor   {
  friend class DialogDisplayFactory;
  friend class DialogSubDisplay;
 public:
  /// Default destructor
  virtual ~DialogMainDisplay();
  int print (const std::string& printer);
  int endUpdate();
  DialogMainDisplay (DialogMenu *parent);
 protected:
  DialogMainDisplay (const AbstractMainDisplay& source);
  void handle (const CPP::Event& ev);
  DialogMenu* window() {     return m_window;   }
 private:
  /// Private copy constructor
  DialogMainDisplay(const DialogMainDisplay&) {}
  /// Private assignment operator
  DialogMainDisplay& operator=(const DialogMainDisplay&) { return *this; }
 protected:
  DialogItem *m_par;
  DialogMenu *m_window;
};
#endif // _DIALOGDISPLAY_H
