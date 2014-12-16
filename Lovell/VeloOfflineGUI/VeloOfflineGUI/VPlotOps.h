#ifndef VPLOTOPS_H
#define VPLOTOPS_H

#include <QGroupBox>
#include "VPlot.h"
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QGridLayout>

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
  QPushButton * b_popout;
  QPushButton * b_ref;
  bool m_firstTime;
  QWidget * m_statsBox;
  QComboBox * b_moduleSelector1;
  QComboBox * b_moduleSelector2;
  QComboBox * b_veloRunNumber;
  QGroupBox * m_moduleSelector;

  VPlotOps(QGroupBox*);
  void newSelection(VPlot*, bool);
  std::string currentModuleStr() {
    std::stringstream ss;
    ss<< (b_moduleSelector2->count() * b_moduleSelector1->currentIndex() +
            b_moduleSelector2->currentIndex());
    return ss.str();
  }
};



#endif // VPLOTOPS_H
