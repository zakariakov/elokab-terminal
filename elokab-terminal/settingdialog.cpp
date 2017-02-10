#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QSettings>
SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    QFont font = QApplication::font();
    font.setFamily("Monospace");
    font.setPointSize(10);
    font.setStyleHint(QFont::TypeWriter);

   QSettings setting;
  QFont f=  setting.value("Font",font).value<QFont>();
  int colorSheme=setting.value("ColorSheme",0).toInt();
int sPos=setting.value("ScrollBar",0).toInt();
QString txt=setting.value("Shell","/bin/bash").toString();
    ui->setupUi(this);
    ui->fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts
                                 | QFontComboBox::NonScalableFonts
                                 | QFontComboBox::ScalableFonts);

    ui->fontComboBox->setCurrentFont(f);
    ui->fontComboBox->setEditable(false);

    ui->sizeSpinBox->setValue(f.pointSize());
    ui->themesComboBox->setCurrentIndex(colorSheme);
    ui->ScrollBarComboBox->setCurrentIndex(sPos);
    ui->lineEditShell->setText(txt);

}

SettingDialog::~SettingDialog()
{
    delete ui;
}
QFont SettingDialog::getFont()
{
    QFont f= ui->fontComboBox->currentFont();
    f.setPointSize(ui->sizeSpinBox->value());
    return f;
}
int SettingDialog::getColorSheme()
{
    return ui->themesComboBox->currentIndex()+1;
}
int SettingDialog::getScrollBar()
{
    return ui->ScrollBarComboBox->currentIndex();
}

void SettingDialog::on_buttonBox_accepted()
{
    QSettings setting;
    QFont f=ui->fontComboBox->currentFont();
    f.setPointSize(ui->sizeSpinBox->value());
    setting.setValue("Font",f);
    setting.setValue("ColorSheme",ui->themesComboBox->currentIndex());
    setting.setValue("ScrollBar",ui->ScrollBarComboBox->currentIndex());
      setting.setValue("Shell",ui->lineEditShell->text());

}
