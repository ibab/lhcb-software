#ifndef VTREE_H_
#define VTREE_H_

#include <QtGui/QWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QGridLayout>
#include "VTabContent.h"
#include <fstream>
#include "treemodel.h"
#include <QtGui/QTreeView>

class VTabContent;

class VTree : public QWidget {
  Q_OBJECT
public:
  VTree(std::string, std::string, VTabContent*);
  std::string m_fileName;
  QGridLayout * m_layout;
  VTabContent * m_tab;
  bool m_drawn;
  std::string m_title;

  // Methods___________________________________________________________________
  void draw();
  void setupLayout();
};

#endif // VTREE_H_
