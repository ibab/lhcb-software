#ifndef VPLOTOPS_H
#define VPLOTOPS_H

#include <QtGui/QGroupBox>
#include "VPlot.h"
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QTextBrowser>
#include <QtGui/QScrollBar>
#include <QtGui/QMessageBox>
#include "TFile.h"

class VPlot;

class VPlotOps : public QWidget {
  Q_OBJECT
public:
  QGroupBox * m_opsHolder;
  VPlot * m_selPlot;
  QGridLayout * m_layout;
  QCheckBox * b_logx;
  QCheckBox * b_logy;
  QCheckBox * b_logz;
  QCheckBox * b_toggleRef;
  QCheckBox * b_toggleRefDiff;
  QCheckBox * b_toggleRefRatio;
  QPushButton * b_popout;
  QPushButton * b_ref;
  QPushButton * b_tip;
  bool m_firstTime;
  QWidget * m_statsBox;
  QComboBox * b_moduleSelector1;
  QComboBox * b_moduleSelector2;
  QComboBox * b_veloRunNumber;
  QComboBox * b_veloRunNumberRef;
  QGroupBox * m_moduleSelector;
  std::string * m_dataDir;
  QTextBrowser * m_textBrowser;
  void notify(std::string);
  bool m_waitSwitch;
  QCheckBox * b_showAllRefs;
  TFile * f_in;
  TFile * f_inReff;

  VPlotOps(QGroupBox*, std::string *);
  void newSelection(VPlot*, bool);
  std::string currentModuleStr() {
    std::stringstream ss;
    ss<< (b_moduleSelector2->count() * b_moduleSelector1->currentIndex() +
            b_moduleSelector2->currentIndex());
    return ss.str();
  }

  unsigned int currentModuleNum() {
    int val = b_moduleSelector2->count() * b_moduleSelector1->currentIndex() +
            b_moduleSelector2->currentIndex();
    return val;
  }

private slots:
	void showTip();
};



#endif // VPLOTOPS_H
