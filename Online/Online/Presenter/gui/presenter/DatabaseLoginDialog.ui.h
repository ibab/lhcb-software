/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
****************************************************************************/
#include "TSystem.h"
#include "OnlineHistDB/OnlineHistDBEnv.h"

  std::string defaultUserPassword = "xxx";

void DatabaseLoginDialog::init()
{
  UserGroup_t *userInfo = gSystem->GetUserInfo();
  if (userInfo) {
    std::string userName;
    userName = userInfo->fUser;
    histDbUsernameComboBox->insertItem(userName);
  }
  delete userInfo;
  
  histDbUsernameComboBox->insertItem(OnlineHistDBEnv_constants::ACCOUNT);
  histDbNameComboBox->insertItem(OnlineHistDBEnv_constants::DB);
}

void DatabaseLoginDialog::sendHistogramDatabaseCredentials()
{
  emit login(histDbPasswordLineEdit->text(),
             histDbUsernameComboBox->currentText(),
             histDbNameComboBox->currentText());  
}

void DatabaseLoginDialog::setDummyPassword()
{
  if( OnlineHistDBEnv_constants::ACCOUNT == histDbUsernameComboBox->
                                              currentText()) {
    histDbPasswordLineEdit->setText(defaultUserPassword);
  } else {
    histDbPasswordLineEdit->setText("");
  }
}
