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
#include "VCombatContent.h"
#include "VKeplerContent.h"
#include "VContentGetter.h"
#include "VPlotOps.h"
#include <QtGui/QCompleter>


namespace Ui {
class veloview;
}

class veloview : public QMainWindow {
  Q_OBJECT
public:
  int m_printOption;
  VTabContent * m_content;
  bool m_welcomeTabSet;
  bool m_ran;
  int m_runMode;
  std::string m_logoText;
  bool m_combatFileOpen;
  bool m_keplerFileOpen;
  VPlotOps * m_plotOps;
  std::string m_VVinterfaceScript;
  QSortFilterProxyModel * m_runProxy;

  // Methods___________________________________________________________________
  explicit veloview(int, QWidget *parent = 0);
  ~veloview();
  void VPrint(std::string); // Overloaded.
  void VPrint(int);
  void VPrint(double);
  void VPrintContentDetails(VTabContent*);
  void completeTabs(std::vector<VTabContent*> &, QWidget*, QGridLayout*);
  void addWelcomeTab(QTabWidget*);
  void loadOptionsFile();
  void setOptionsWidg();
  void setCombatOptionsWidg();
  void setVeloOptionsWidg();
  void setKeplerOptionsWidg();
  std::string combatFileName() {return b_combatFileName->text().toStdString();}
  int combatPlaneNum() {return b_combatPlaneNum->value();}
  std::string keplerFileName() {return b_keplerFileName->text().toStdString();}
  int keplerPlaneNum() {return b_keplerPlaneNum->value();}
  TFile * combatFile();
  TFile * keplerFile();
  void addModuleSelector();

  // Optional settings ________________________________________________________
  QLineEdit * b_combatFileName;
  QSpinBox * b_combatPlaneNum;
  QLineEdit * b_keplerFileName;
  QSpinBox * b_keplerPlaneNum;
  QComboBox * b_veloRunNumber;
  QSpinBox * b_veloRunNumberLow;
  QSpinBox * b_veloRunNumberUp;
  std::vector<TFile*> m_rootFiles;

private:
  Ui::veloview * ui;

private slots:
  void setContent();
  void on_b_quit_clicked();
  void moduleChanged();
  void on_b_selector3_clicked();
  void on_b_selector4_clicked();
  void filterWildcard(QString);
};

#endif // VELOVIEW_H
