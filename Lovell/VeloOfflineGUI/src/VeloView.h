#ifndef VELOVIEW_H
#define VELOVIEW_H

#include <QtGui/QMainWindow>
#include <iostream>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QStringListModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStringListModel>

#include "TFile.h"
#include "VContentGetter.h"
#include "VPlotOps.h"
#include <QtGui/QCompleter>


namespace Ui {
class veloview;
}

class veloview : public QMainWindow {
  Q_OBJECT
private:
  Ui::veloview * ui;
public:
  int m_printOption;
  VTabContent * m_content;
  bool m_welcomeTabSet;
  bool m_ran;
  int m_runMode;
  std::string m_logoText;
  VPlotOps * m_plotOps;
  std::string m_VVinterfaceScript;
  QSortFilterProxyModel * m_runProxy;
  QSortFilterProxyModel * m_runProxyRef;
  bool m_verbose;
  std::vector<std::string> * m_ops;
  std::string m_dataDir;
  TFile * f_in;
  TFile * f_inReff;

  // Methods___________________________________________________________________
  explicit veloview(int, std::vector<std::string> *, QWidget *parent = 0);
  ~veloview();
  void VPrint(std::string); // Overloaded.
  void VPrint(int);
  void VPrint(double);
  void VPrintContentDetails(VTabContent*);
  void completeTabs(std::vector<VTabContent*> &, QWidget*, QGridLayout*);
  void addWelcomeTab(QTabWidget*);
  void loadOptionsFile();
  void setOptionsWidg();
  void setVeloOptionsWidg();
  void addModuleSelector();
  void setOps();
  void setupInFiles(bool ref);

  // Optional settings ________________________________________________________
  QComboBox * b_veloRunNumber;
  QComboBox * b_veloRunNumberRef;
  QSpinBox * b_veloRunNumberLow;
  QSpinBox * b_veloRunNumberUp;
  std::vector<TFile*> m_rootFiles;
  QCheckBox * b_showAllRefs;

private slots:
  void setContent();
  void on_b_quit_clicked();
  void moduleChanged();
  void on_b_selector3_clicked();
  void on_b_selector4_clicked();
  void filterWildcard(QString);
  void filterWildcardRef(QString);
};

#endif // VELOVIEW_H
