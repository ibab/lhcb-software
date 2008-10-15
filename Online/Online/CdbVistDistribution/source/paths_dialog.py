# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'paths_dialog.ui'
#
# Created: Tue Jun 12 10:21:17 2007
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class Paths(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        if not name:
            self.setName("Paths")

        self.setMinimumSize(QSize(200,0))
        self.setFocusPolicy(QDialog.TabFocus)
        self.setModal(0)

        PathsLayout = QGridLayout(self,1,1,2,6,"PathsLayout")

        self.listBox1 = QListBox(self,"listBox1")
        self.listBox1.setFrameShadow(QListBox.Sunken)
        self.listBox1.setLineWidth(1)
        self.listBox1.setMargin(0)
        self.listBox1.setVScrollBarMode(QListBox.Auto)

        PathsLayout.addWidget(self.listBox1,0,0)

        self.languageChange()

        self.resize(QSize(200,376).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("Paths View"))
        self.listBox1.clear()
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))
        self.listBox1.insertItem(self.__tr("New Item"))


    def __tr(self,s,c = None):
        return qApp.translate("Paths",s,c)
