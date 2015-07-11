#ifndef __VTABCONTENT_H_INCLUDED__
#define __VTABCONTENT_H_INCLUDED__

#include <vector>
#include "VPlot.h"
#include <QtGui/qwidget.h>
#include <QtGui/QGridLayout>
#include "VTable.h"

// VTabContent - an object containing one tabs worth of content (be it plots or 
// tabs). Note that tabs and plots is not currently supported.
// Author: Daniel Saunders


class VPlot;
class VTable;

class VTabContent : public QWidget {
public:
  // Set of configurations of sub-tabs that are displayed in this tab.
  std::vector<VTabContent*> m_subContents;
  std::vector<VPlot*> m_plots; // Plots to be shown in this tab - up to 9.
  std::vector<VTable*> m_tables; // Tables to be shown in this tab.
  std::string m_title; // This tabs name.
  int m_plotFillDirection; // 0 for filling rows first, 1 for columns. TODO
  int m_subTabViewMethod; // 0 to draw as tabs, 1 for drop down list. TODO
  QWidget * m_widget; // Pointer to its own widget in the GUI.
  QGridLayout * m_plotLayout;
  VTabContent * m_parentTab;
  int m_ID;
  int m_parentID;
  bool m_drawn;
  int m_depth;
  QTabWidget * m_qtab;
  unsigned int m_qtabID;
  int m_layoutX;
  int m_layoutY;
  

  // Methods __________________________________________________________________
  VTabContent() : m_subContents(),
                  m_plots(),
                  m_title("Default Tab Name"),
                  m_plotFillDirection(0),
                  m_subTabViewMethod(0),
                  m_widget(NULL),
                  m_plotLayout(NULL),
                  m_parentTab(NULL),
                  m_ID(0),
									m_drawn(false),
									m_qtab(NULL),
									m_layoutX(2),
									m_layoutY(2) {} // Top tab constructor.

  VTabContent(std::string title) :
                  m_subContents(),
                  m_plots(),
                  m_plotFillDirection(0),
                  m_subTabViewMethod(0),
                  m_widget(NULL),
                  m_plotLayout(NULL),
                  m_parentTab(NULL),
                  m_ID(0),
									m_drawn(false),
									m_qtab(NULL),
									m_layoutX(2),
								  m_layoutY(2) {
                    m_title = title;
                  } // Top tab constructor.


  VTabContent(std::string title,
              VTabContent * parentTab) :
                m_subContents(),
                m_plots(),
                m_plotFillDirection(0),
                m_subTabViewMethod(0),
                m_widget(NULL),
                m_plotLayout(NULL),
								m_drawn(false),
								m_qtab(NULL),
								m_layoutX(2),
								m_layoutY(2) {
                  m_title = title;
                  m_parentTab = parentTab;
                  m_parentTab->m_subContents.push_back(this);
                }

  VTabContent(std::string title,
              VTabContent * parentTab, int ID) :
                m_subContents(),
                m_plots(),
                m_plotFillDirection(0),
                m_subTabViewMethod(0),
                m_widget(NULL),
                m_plotLayout(NULL),
								m_drawn(false),
								m_qtab(NULL),
								m_layoutX(2),
								m_layoutY(2) {
                  m_title = title;
                  m_parentTab = parentTab;
                  m_parentTab->m_subContents.push_back(this);
                  m_ID = ID;
                }

  void setPlotLayout(int x, int y) {
  	m_layoutX = x;
  	m_layoutY = y;
  }



  //___________________________________________________________________________

  void showEvent(QShowEvent *) {
    if (m_plots.size() > 0 && !m_drawn) drawPlots();
    if (m_tables.size() > 0 && !m_drawn) drawTables();
    m_drawn = true;
  } // Re-implemented from QWidget.

  void drawPlots();
  void drawTables();


  //___________________________________________________________________________
};

#endif
