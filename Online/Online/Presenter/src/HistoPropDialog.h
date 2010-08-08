// $Id: HistoPropDialog.h,v 1.12 2010-08-08 15:13:33 robbep Exp $
#ifndef HISTOPROPDIALOG_H_
#define HISTOPROPDIALOG_H_

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#ifdef WIN32
#pragma warning( pop )
#endif

#include "presenter.h"

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class TGTextBuffer;
class TGTextEntry;
class TGTextEdit;
class TGedPatternSelect;
class TGLineWidthComboBox;
class TGColorSelect;
class TGLineStyleComboBox;

class HistoPropDialog : public TGTransientFrame
{
  public:
    HistoPropDialog(PresenterMainFrame* gui, int width,
		    int height, pres::MsgLevel v);
    virtual ~HistoPropDialog();

    void ok();
    void build();
    void CloseWindow();

  private:
    PresenterMainFrame *m_mainFrame;
    TGTextButton       *m_okButton;
    TGTextButton       *m_cancelButton;
    pres::MsgLevel      m_verbosity;
    int                 m_msgBoxReturnCode;

    TGComboBox *m_1DRootDrawOptionComboBox;
    TGComboBox *m_2DRootDrawOptionComboBox;
    TGComboBox *m_genericRootDrawOptionComboBox;
    TGComboBox *m_statOptionComboBox;

    TGTextEntry        *m_xAxisLabelTextEntry;
    TGTextEntry        *m_yAxisLabelTextEntry;
    TGTextEntry        *m_drawPatternTextEntry;
    TGTextEdit         *m_histogramDescriptionField;

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
