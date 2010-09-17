//
#include "GaudiUPI/AbstractDisplay.h"
#include "GaudiUPI/DisplayFactory.h"
#include <algorithm>
using namespace std;

namespace {
  static AbstractDisplayFactory* s_instance = 0;
  template <class Func, class Type> struct Apply   {
    Func m_pmf; 
    Apply ( Func f ) : m_pmf (f) { }
    void operator ( ) ( Type* p ) const    {   (p->*m_pmf)();                 }
  };
  template <class R, class T>
  Apply<R (T::*)(),T> apply(R (T::*pmf)()) { return Apply<R (T::*)(),T>(pmf); }
}

void AbstractDisplayFactory::setInstance(AbstractDisplayFactory* s) {
  s_instance = s;
}

AbstractDisplayFactory* AbstractDisplayFactory::instance() {          
  return s_instance;
}

bool AbstractSubDisplay::operator==(const AbstractSubDisplay& cmp) const {
  return this == &cmp;
}

int AbstractSubDisplay::init (size_t num_col,size_t num_row,AbstractMainDisplay& par) {
  m_x = num_col;
  m_y = num_row;
  m_parent = &par;
  parent()->children().push_back(this);
  return DISPLAY_SUCCESS;
}

int AbstractSubDisplay::map() {
  return DISPLAY_SUCCESS;
}

int AbstractSubDisplay::close() {
  return DISPLAY_SUCCESS;
}

int AbstractSubDisplay::flush() {
  string text(width(),' ');
  while ( m_currLine < height() ) putString(text, DISPLAY_NORMAL);
  return DISPLAY_SUCCESS;
}

int AbstractSubDisplay::repaint() {
  m_currLine = 0;
  return DISPLAY_SUCCESS;
}

int AbstractSubDisplay::endUpdate() {
  m_currLine = 0;
  return DISPLAY_SUCCESS;
}

int AbstractSubDisplay::beginUpdate() {
  m_currLine = 0;
  return DISPLAY_SUCCESS;
}

int AbstractSubDisplay::label (const string& text,int where,int flag) {
  string label(text.substr(0,width()));
  label.insert(label.length(),width()-label.length(),' ');
  m_label.Set(label,flag,where);
  return DISPLAY_SUCCESS;
}

int AbstractMainDisplay::close() {
  for_each(children().begin(),children().end(),apply(&AbstractSubDisplay::close));
  return DISPLAY_SUCCESS;
}

int AbstractMainDisplay::flush() {
  for_each(children().begin(),children().end(),apply(&AbstractSubDisplay::flush));
  return DISPLAY_SUCCESS;
}

int AbstractMainDisplay::map() {
  for_each(children().begin(),children().end(),apply(&AbstractSubDisplay::map));
  return DISPLAY_SUCCESS;
}

int AbstractMainDisplay::beginUpdate() {
  for_each(children().begin(),children().end(),apply(&AbstractSubDisplay::beginUpdate));
  return DISPLAY_SUCCESS;
}

int AbstractMainDisplay::endUpdate() {
  for_each(children().begin(),children().end(),apply(&AbstractSubDisplay::endUpdate));
  return DISPLAY_SUCCESS;
}

int AbstractMainDisplay::repaint() {
  for_each(children().begin(),children().end(),apply(&AbstractSubDisplay::repaint));
  return DISPLAY_SUCCESS;
}

int AbstractMainDisplay::SetOutOfBandAsts (int /* ast_map */, AbstractMainDisplay::HandlerType handle,Interactor* actor) {
  m_actor = actor;
  m_handler = handle;
  return DISPLAY_SUCCESS;
}

AbstractSubDisplay* AbstractSubDisplay::create (const AbstractDisplay& data) {
  return AbstractDisplayFactory::instance()->createSubDisplay(data);
}

AbstractMainDisplay* AbstractMainDisplay::create (const AbstractDisplay& data) {
  return AbstractDisplayFactory::instance()->createMainDisplay(data);
}

DisplayText& DisplayText::pad(const char c) {
  m_board.insert(m_board.length(),width()-m_board.length(),c);
  m_board.c_str();
  return *this;
}

DisplayText& DisplayText::set(const string& txt) {
  m_board = txt;
  return *this;
}

DisplayBoard::DisplayBoard ( int width, int video) 
:  DisplayText (width), m_video(video), m_change(true)  {
}

DisplayBoard::DisplayBoard ( const DisplayBoard& cpy ) 
:  DisplayText (cpy.width()), m_video(cpy.video()), m_change(cpy.changed())  
{
}

DisplayBoard::~DisplayBoard() {
}
DisplayBoard& DisplayBoard::set(const string& txt, int video) {
  m_video = video;
  if ( txt == m_board ) return *this;
  DisplayText::set(txt);
  m_change = true;
  return *this;
}
