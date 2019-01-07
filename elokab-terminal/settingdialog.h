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
int getOpacity();

signals:
void settingsChanged();
private slots:
void saveSettings();

void on_themesComboBox_currentIndexChanged(int index);
void meAdjustSize();
void on_pushButton_clicked();
void loadColorShemes(const QString &fileNames);
void loadXresourceColorShemes();
void on_pushButtonXresources_clicked();

void on_buttonBox_clicked(QAbstractButton *button);

void on_checkBoxFont_toggled(bool checked);

private:
    Ui::SettingDialog *ui;

    ButtonColor *btnFColor;

    ButtonColor *btnBColor;
enum mi{mD=16};

    ButtonColor *btnColor[mD];

};

#endif // SETTINGDIALOG_H
