# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'create_a_device_type.ui'
#
# Created: Fri Jul 13 17:38:00 2007
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class Form1(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        if not name:
            self.setName("Form1")


        Form1Layout = QGridLayout(self,1,1,11,6,"Form1Layout")
        spacingButtons = QSpacerItem(110,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        Form1Layout.addMultiCell(spacingButtons,9,9,0,1)

        self.buttonOk = QPushButton(self,"buttonOk")
        self.buttonOk.setAutoDefault(1)
        self.buttonOk.setDefault(1)

        Form1Layout.addMultiCellWidget(self.buttonOk,9,9,2,4)

        self.buttonCancel = QPushButton(self,"buttonCancel")
        self.buttonCancel.setAutoDefault(1)

        Form1Layout.addMultiCellWidget(self.buttonCancel,9,9,5,6)

        self.Description = QTextEdit(self,"Description")

        Form1Layout.addMultiCellWidget(self.Description,8,8,0,6)

        self.Inputnumber = QSpinBox(self,"Inputnumber")

        Form1Layout.addWidget(self.Inputnumber,6,1)
        spacinginputOutput = QSpacerItem(51,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        Form1Layout.addMultiCell(spacinginputOutput,6,6,2,3)

        self.outputLabel = QLabel(self,"outputLabel")

        Form1Layout.addMultiCellWidget(self.outputLabel,6,6,4,5)

        self.Outputnumber = QSpinBox(self,"Outputnumber")

        Form1Layout.addWidget(self.Outputnumber,6,6)

        self.addsybsystemLabel = QLabel(self,"addsybsystemLabel")

        Form1Layout.addMultiCellWidget(self.addsybsystemLabel,4,4,0,1)

        self.inputLabel = QLabel(self,"inputLabel")

        Form1Layout.addWidget(self.inputLabel,6,0)

        self.descriptionLabel = QLabel(self,"descriptionLabel")

        Form1Layout.addMultiCellWidget(self.descriptionLabel,7,7,0,1)
        spacingNameColor = QSpacerItem(31,20,QSizePolicy.Minimum,QSizePolicy.Minimum)
        Form1Layout.addItem(spacingNameColor,1,3)

        self.createinLabel = QLabel(self,"createinLabel")

        Form1Layout.addMultiCellWidget(self.createinLabel,2,2,0,1)

        self.devicetypenameEdit = QLineEdit(self,"devicetypenameEdit")

        Form1Layout.addMultiCellWidget(self.devicetypenameEdit,1,1,0,2)

        self.CreateinLineedit = QLineEdit(self,"CreateinLineedit")

        Form1Layout.addMultiCellWidget(self.CreateinLineedit,3,3,0,6)

        self.addsubsystemList = QListBox(self,"addsubsystemList")

        Form1Layout.addMultiCellWidget(self.addsubsystemList,5,5,0,6)

        self.pushButton16 = QPushButton(self,"pushButton16")

        Form1Layout.addMultiCellWidget(self.pushButton16,1,1,4,6)

        self.DeviceNameLabel = QLabel(self,"DeviceNameLabel")

        Form1Layout.addMultiCellWidget(self.DeviceNameLabel,0,0,0,1)

        self.colorLabel = QLabel(self,"colorLabel")

        Form1Layout.addMultiCellWidget(self.colorLabel,0,0,4,5)

        self.languageChange()

        self.resize(QSize(315,441).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("Form1"))
        self.buttonOk.setText(self.__tr("&OK"))
        self.buttonOk.setAccel(QString.null)
        self.buttonCancel.setText(self.__tr("&Cancel"))
        self.buttonCancel.setAccel(QString.null)
        self.outputLabel.setText(self.__tr("Output ports:"))
        self.addsybsystemLabel.setText(self.__tr("Add to subsystem:"))
        self.inputLabel.setText(self.__tr("Input ports:"))
        self.descriptionLabel.setText(self.__tr("Description :"))
        self.createinLabel.setText(self.__tr("Create in:"))
        self.addsubsystemList.clear()
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.addsubsystemList.insertItem(self.__tr("New Item"))
        self.pushButton16.setText(self.__tr("pushButton16"))
        self.DeviceNameLabel.setText(self.__tr("Device Type Name:"))
        self.colorLabel.setText(self.__tr("Choose color:"))


    def __tr(self,s,c = None):
        return qApp.translate("Form1",s,c)
