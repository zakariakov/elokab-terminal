#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include "buttonColor/buttoncolor.h"
namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();
QFont getFont();
int getColorSheme();
int getScrollBar();
QColor getBcolor();
QColor getFcolor();
private slots:
void on_buttonBox_accepted();

void on_themesComboBox_currentIndexChanged(int index);

private:
    Ui::SettingDialog *ui;

    ButtonColor *btnFColor;

    ButtonColor *btnBColor;

};

#endif // SETTINGDIALOG_H
