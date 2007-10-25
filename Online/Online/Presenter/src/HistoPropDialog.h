#ifndef HISTOPROPDIALOG_H_
#define HISTOPROPDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

using namespace pres;

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class TGTextBuffer;
class TGTextEntry;
class TGedPatternSelect;
class TGLineWidthComboBox;
class TGColorSelect;
class TGLineStyleComboBox;

class HistoPropDialog : public TGTransientFrame
{
  public:
    HistoPropDialog(PresenterMainFrame* gui, Int_t Width, Int_t Height,
      MsgLevel v);
    virtual ~HistoPropDialog();

    void ok();
    void build();

  private:

    PresenterMainFrame *m_mainFrame;
    TGTextButton       *m_okButton;
    TGTextButton       *m_cancelButton;
    MsgLevel            m_verbosity;
    int                 m_msgBoxReturnCode;

    TGComboBox *m_1DRootDrawOptionComboBox;
    TGComboBox *m_2DRootDrawOptionComboBox;
    TGComboBox *m_genericRootDrawOptionComboBox;
    TGComboBox *m_statOptionComboBox;

    TGTextEntry        *m_xAxisLabelTextEntry;
    TGTextEntry        *m_yAxisLabelTextEntry;

    TGLineWidthComboBox*  m_lineWidthComboBox;
    TGedPatternSelect*  m_fillStylePatternSelector;
    TGColorSelect*  m_fillColourSelector;
    TGLineStyleComboBox *m_lineStyleComboBox;
    TGColorSelect*  m_lineColourSelector;

    GCValues_t m_fillStyle;
    GCValues_t m_lineStyle;

    ClassDef(HistoPropDialog, 0)
};

#endif /*HISTOPROPDIALOG_H_*/
