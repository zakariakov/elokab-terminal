#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QSettings>
#include <QLayout>
#include <QTimer>
#include <QFileDialog>
#include <QApplication>
#include <QComboBox>
SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    btnFColor=new ButtonColor;
    btnBColor=new ButtonColor;

    for (int i = 0; i < 16; ++i) {
        btnColor[i]=new ButtonColor;
        if(i<8){
            ui->gridLayoutBtn->addWidget(btnColor[i],1,i);
        }else{
            ui->gridLayoutBtn->addWidget(btnColor[i],2,i-8);
        }

    }

    QFont font = QApplication::font();
    font.setFamily("Monospace");
    font.setPointSize(10);
    font.setStyleHint(QFont::TypeWriter);

    QSettings setting;
    QFont f=  setting.value("Font",font).value<QFont>();
    int colorSheme=setting.value("ColorSheme",0).toInt();
    int sPos=setting.value("ScrollBar",0).toInt();
    QString txt=setting.value("Shell","/bin/bash").toString();
    int opacity=setting.value("Opacity",100).toInt();


    setting.beginGroup("colors");
    QColor fcolor=setting.value("foreground",QColor(255,255,255)).value<QColor>();
    QColor bcolor=setting.value("background",QColor(0,0,0)).value<QColor>();

    QList<QColor> listColor;
    listColor<<QColor(0,   0,   0)<<QColor(178,  24,  24)
            <<QColor(    24, 178,  24)<<QColor(   178, 104,  24)
           <<QColor(    24,  24, 178)<<QColor(   178,  24, 178)
          <<QColor(    24, 178, 178)<<QColor(   178, 178, 178)
         <<QColor(  104, 104, 104)<<QColor(  255,  84,  84)
        <<QColor(   84, 255,  84)<<QColor(  255, 255,  84)
       <<QColor(   84,  84, 255)<<QColor(  255,  84, 255)
      <<QColor(   84, 255, 255) <<QColor(  255, 255, 255);


    for (int i = 0; i < 16; ++i) {
        btnColor[i]->setColor(setting.value("color"+QString::number(i),listColor.at(i)).value<QColor>());
    }

    setting.endGroup();

    btnBColor->setColor(bcolor);
    btnFColor->setColor(fcolor);
    ui->hLayoutBackColor->addWidget(btnBColor);
    ui->hLayoutForColor->addWidget(btnFColor);
    //    ui->hLayoutForColor


    ui->fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts
                                     | QFontComboBox::NonScalableFonts
                                     | QFontComboBox::ScalableFonts);

    ui->fontComboBox->setCurrentFont(f);
    ui->fontComboBox->setEditable(false);

    //Color Shemes --------------------------------
    QDir appDir(QApplication::applicationDirPath());
    appDir.cdUp();
    QString mPath=  appDir.absolutePath()+"/share/elokab/color-shemes";
   QDir dirSheme(mPath);
   QStringList listShemes=dirSheme.entryList(QStringList() << "*.txt",QDir::Files);
   foreach (QString fileName, listShemes) {
       QFileInfo fi(mPath+"/"+fileName);
       ui->themesComboBox->addItem(fi.completeBaseName(),fi.filePath());
   }
    //END Color Shemes --------------------------------
    ui->sizeSpinBox->setValue(f.pointSize());
   // ui->themesComboBox->setCurrentIndex(-1);
    ui->ScrollBarComboBox->setCurrentIndex(sPos);
    ui->lineEditShell->setText(txt);
    ui->spinBoxOPacity->setValue(opacity);
    //ui->widgetCostumColor->setVisible(colorSheme!=1);

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
QColor SettingDialog::getBcolor()
{
    return btnBColor->color();
}

QColor SettingDialog::getFcolor()
{
     return btnFColor->color();
}

int SettingDialog::getOpacity()
{
     return ui->spinBoxOPacity->value();
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
        setting.setValue("Opacity",ui->spinBoxOPacity->value());

    //colors-sheme
    setting.beginGroup("colors");

    setting.setValue("background",btnBColor->color().name());
    setting.setValue("foreground",btnFColor->color().name());
    for (int i = 0; i < 16; ++i) {
        setting.setValue("color"+QString::number(i),btnColor[i]->color().name());
    }

    setting.endGroup();
}

void SettingDialog::on_themesComboBox_currentIndexChanged(int index)
{
//      ui->widgetCostumColor->setVisible(index!=0);
//      resize(100,100);
//      adjustSize();
//QTimer::singleShot(10,this,SLOT(meAdjustSize() ));
QString fileName=ui->themesComboBox->itemData(index).toString();

loadColorShemes(fileName);
}
void SettingDialog::meAdjustSize()
{
     adjustSize();
}

void SettingDialog::on_pushButton_clicked()
{

    QString fileName=   QFileDialog::getOpenFileName(this,"Open",QDir::homePath(),"*");
    if(fileName.isEmpty())return;

   // ui->themesComboBox->setCurrentIndex(0);

 loadColorShemes(fileName);


}

void SettingDialog::loadColorShemes(const QString &fileNames)
{
    if(fileNames.isEmpty()){
        QColor fcolor=QColor(255,255,255);
        QColor bcolor=QColor(0,0,0);

        QList<QColor> listColor;
        listColor<<QColor(0,   0,   0)<<QColor(178,  24,  24)
                <<QColor(    24, 178,  24)<<QColor(   178, 104,  24)
               <<QColor(    24,  24, 178)<<QColor(   178,  24, 178)
              <<QColor(    24, 178, 178)<<QColor(   178, 178, 178)
             <<QColor(  104, 104, 104)<<QColor(  255,  84,  84)
            <<QColor(   84, 255,  84)<<QColor(  255, 255,  84)
           <<QColor(   84,  84, 255)<<QColor(  255,  84, 255)
          <<QColor(   84, 255, 255) <<QColor(  255, 255, 255);

        btnBColor->setColor(bcolor);
        btnFColor->setColor(fcolor);
        for (int i = 0; i < 16; ++i) {
            btnColor[i]->setColor(listColor.at(i));
        }
    }else{
        QSettings setting(fileNames,QSettings::IniFormat);

        setting.beginGroup("colors");
        QColor fcolor=setting.value("foreground",QColor(255,255,255)).value<QColor>();
        QColor bcolor=setting.value("background",QColor(0,0,0)).value<QColor>();
        btnBColor->setColor(bcolor);
        btnFColor->setColor(fcolor);
        for (int i = 0; i < 16; ++i) {
            btnColor[i]->setColor(setting.value("color"+QString::number(i),btnColor[i]->color()).value<QColor>());
        }

        setting.endGroup();
    }

}

