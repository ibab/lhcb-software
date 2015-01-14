#ifndef VTABLE_H
#define VTABLE_H

#include <QtGui/QWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QGridLayout>
#include "VTabContent.h"
#include <fstream>

class VTabContent;

class VTable : public QWidget {
  Q_OBJECT
public:
  explicit VTable(std::string, VTabContent*);
  std::string m_fileName;
  QTableWidget * m_table;
  QGridLayout * m_layout;
  VTabContent * m_tab;
  bool m_drawn;

  // Methods___________________________________________________________________
  void draw();
  void setupLayout();
  QTableWidget * getTable(bool);
  void addButtons();
  void fillTable(QTableWidget*, std::ifstream &);

signals:

public slots:
  void makePopUp();

};

#endif // VTABLE_H
