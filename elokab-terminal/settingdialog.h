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
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

int getColorSheme();
int getScrollBar();
QColor getBcolor();
QColor getFcolor();
int getOpacity();

signals:
void settingsChanged();
private slots:
void setupKeys();
void saveSettings();

void on_themesComboBox_currentIndexChanged(int index);
void meAdjustSize();
void on_pushButton_clicked();
void loadColorShemes(const QString &fileNames);
void loadXresourceColorShemes();


void on_buttonBox_clicked(QAbstractButton *button);

void on_pushButtonFont_clicked();
void xrdbquey();


void on_comboBoxUnicode_currentIndexChanged(int index);

void on_fontComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::SettingDialog *ui;

    ButtonColor *btnFColor;

    ButtonColor *btnBColor;

    ButtonColor *btnCursorColor;

enum mi{mD=16};

    ButtonColor *btnColor[mD];

    QString mfontUnicod;
     QStringList listColor;
};

#endif // SETTINGDIALOG_H
