#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QSettings>
#include <QLayout>
#include <QTimer>
#include <QFileDialog>
#include <QApplication>
#include <QComboBox>
#include <QTextStream>
#include <QTextCodec>
#include <QDialogButtonBox>
#include <QDebug>
#include <QCheckBox>
#include <QFontDialog>
#include <QProcess>
#include <QMap>
#include <QFontDatabase>


SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    btnFColor=new ButtonColor;
    btnBColor=new ButtonColor;
    btnCursorColor=new ButtonColor;

    for (int i = 0; i < 16; ++i) {
        btnColor[i]=new ButtonColor;
        if(i<8){
            ui->gridLayoutBtn->addWidget(btnColor[i],2,i);
        }else{
            ui->gridLayoutBtn->addWidget(btnColor[i],3,i-8);
        }

    }

    //-----------------------------------------------------------

    QList<QFontDatabase::WritingSystem>listfonts;
    QFontDatabase ftnddata;
    listfonts=ftnddata.writingSystems() ;
    ui->comboBoxUnicode->addItem("Any",QFontDatabase::Any);
    foreach (QFontDatabase::WritingSystem wsFont, listfonts) {
        QString name= QFontDatabase::writingSystemName(wsFont);
        ui->comboBoxUnicode->addItem(name,wsFont);
    }

    //---------------------------------------------------------
    QFont font = QApplication::font();

    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
    //  QFont f=  setting.value("Font",font).value<QFont>();
    QString fontName=setting.value("FontName","Monospace").toString();
    mfontUnicod=setting.value("FontUnicodeName","Monospace").toString();
    int fontSize=setting.value("FontSize",11).toInt();
    bool italic=setting.value("FontItalic",false).toBool();
    bool bold=setting.value("FontBold",false).toBool();
    QString styleName=setting.value("FontStyle",font.styleName()).toString();

    font.setPointSize(fontSize);
    font.setFamily(fontName);
    font.setStyleName(styleName);
    font.setItalic(italic);
    font.setBold(bold);

    ui->pushButtonFont->setFont(font);

    QString boldStr=QString(),italicStr=QString();
    if(font.bold()==true)boldStr=":Bold";
    if(font.italic()==true)italicStr=":Italic";
    ui->pushButtonFont->setText(font.family()
                                +boldStr
                                +italicStr
                                +":"+QString::number(font.pointSize()));

    bool allfonts=setting.value("AllFonts",false).toBool();

    qDebug()<<"----------------------------"<<font;
    //int colorSheme=setting.value("ColorSheme",0).toInt();
    int sPos=setting.value("ScrollBar",0).toInt();
    QString txt =setting.value("Shell").toString();
    int opacity =setting.value("Opacity",100).toInt();
    bool confirm= setting.value("CloseMsg",true).toBool();
    int shape   =setting.value("CursorShape",0).toInt();
    int  margin = setting.value("Margin",5).toInt();

    bool blinking=setting.value("BlinkingCursor",false).toBool();
    bool  usecurscolor=setting.value("UseCursorColor",false).toBool();

    int linespacing=setting.value("LineSpacing",0).toInt();

    bool nativeWindow=setting.value("NativeWindow",false).toBool();
    setting.beginGroup("colors");

    QColor fcolor=setting.value("foreground",QColor(255,255,255)).value<QColor>();
    QColor bcolor=setting.value("background",QColor(0,0,0)).value<QColor>();
    QColor cursorcolor=setting.value("cursorColor",fcolor).value<QColor>();






    listColor
            <<"#000000" <<"#b21818" <<"#18b218"  <<"#b26818"
           <<"#1818b2" <<"#b218b2" <<"#18b2b2"  <<"#b2b2b2"
          <<"#686868" <<"#ff5454" <<"#54ff54"  <<"#ffff54"
         <<"#5454ff" <<"#ff54ff" <<"#54ffff"  <<"#ffffff";

    //    for (int i = 0; i < 16; ++i) {
    //        btnColor[i]->setColor(setting.value("color"+QString::number(i),listColor.at(i)).value<QColor>());
    //    }
    for (int i = 0; i < 16; ++i) {
        btnColor[i]->setColor(setting.value("color"+QString::number(i),listColor.at(i)).value<QColor>());
    }
    setting.endGroup();

    btnBColor->setColor(bcolor);
    btnFColor->setColor(fcolor);
    btnCursorColor->setColor(cursorcolor);

    ui->hLayoutBackColor->addWidget(btnBColor);
    ui->hLayoutForColor->addWidget(btnFColor);
    ui->hLayoutCursorColor->addWidget(btnCursorColor);

    ui->checkBoxFont->setChecked(allfonts);
    ui->checkBoxCursColor->setChecked(usecurscolor);
    ui->checkBoxBlinking->setChecked(blinking);
    ui->spinBoxLSpacing->setValue(linespacing);
    ui->MarginSpinBox->setValue(margin);
    ui->comboBoxCursor->setCurrentIndex(shape);
ui->checkBoxNativeWindow->setChecked(nativeWindow);
    //Color Shemes --------------------------------
    QDir appDir(QApplication::applicationDirPath());
    appDir.cdUp();
    QString mPath=  appDir.absolutePath()+"/share/elokab/color-shemes";
    QDir dirSheme(mPath);
    QStringList listShemes=dirSheme.entryList(QStringList() << "*.txt",QDir::Files);
    ui->themesComboBox->addItem("Xresources","Xresources");
    foreach (QString fileName, listShemes) {
        QFileInfo fi(mPath+"/"+fileName);
        ui->themesComboBox->addItem(fi.completeBaseName(),fi.filePath());
    }
    //END Color Shemes --------------------------------

    // ui->themesComboBox->setCurrentIndex(-1);
    ui->ScrollBarComboBox->setCurrentIndex(sPos);
    ui->lineEditShell->setText(txt);
    ui->spinBoxOPacity->setValue(opacity);
    //ui->widgetCostumColor->setVisible(colorSheme!=1);
    ui->checkBoxConfirmClosing->setChecked(confirm);
    ui->fontComboBox->setCurrentText(mfontUnicod);
    setupKeys();


}

void SettingDialog::xrdbquey()
{
    QProcess pr;
    pr.start("xrdb",QStringList()<<"-query");
    if (!pr.waitForStarted())
        return ;

    if (!pr.waitForFinished())
        return ;

    QString str= pr.readAllStandardOutput();
    //QString str2= pr.readAllStandardError();
    qDebug()<<"str="<<str;
    //qDebug()<<"str2="<<str2;

    QStringList list=str.split("\n");
    list.removeDuplicates();
    if(list.count()<15){
        loadXresourceColorShemes();
        return;
    }

QStringList listColor;
QMap<QString,QString>colorsMap;
    foreach (QString s, list) {

        QString key=s.section(":",0,0).trimmed();
        // background
        if(key.contains("background")){
            QString value=s.section(":",1,1).trimmed();
            if(value.startsWith("#")){
            qDebug()<< "background"<<value;
           colorsMap["background"]=value;
           btnBColor->setColor(value);
            }
        }
        // foreground
        if(key.contains("foreground")){
            QString value=s.section(":",1,1).trimmed();
           if(value.startsWith("#")){
            qDebug()<< "foreground"<<value;
            colorsMap["foreground"]=value;
            btnFColor->setColor(value);
           }
        }
        // color[num]
        for (int i = 0; i < 16; ++i) {
            if(key.endsWith("color"+QString::number(i))){
                QString value=s.section(":",1,1).trimmed();
                if(value.startsWith("#")){
                qDebug()<< key<<"color"+QString::number(i)<<value;
                colorsMap["color"+QString::number(i)]=value;
                if(i>-1 && i<16)
                    btnColor[i]->setColor(value);
                }
            }
        }

//qDebug()<<s<<key;

    }
}
void SettingDialog::setupKeys()
{
    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
    setting.beginGroup("Keys");

    QString addTab=QKeySequence(QKeySequence::AddTab).toString();
    QString item_0=((setting.value("AddTab",addTab).toString()));
    QString item_1=((setting.value("Copy","Ctrl+Shift+C").toString()));
    QString item_2=((setting.value("Paste","Ctrl+Shift+V").toString()));
    QString zoomIn=QKeySequence(QKeySequence::ZoomIn).toString();
    QString item_3=((setting.value("ZoomIn",zoomIn).toString()));
    QString zoomOut=QKeySequence(QKeySequence::ZoomOut).toString();
    QString item_4=((setting.value("ZoomOut",zoomOut).toString()));
    QString item_5=((setting.value("Preferences","Ctrl+O").toString()));
    QString quit=QKeySequence(QKeySequence::Quit).toString();
    QString item_6=((setting.value("Quit",quit).toString()));

    QString item_7=((setting.value("OpacityDecrase","Ctrl+=").toString()));
    QString item_8=((setting.value("OpacityIncrase","Ctrl+'").toString()));
    setting.endGroup();


    QTableWidgetItem *item0=  ui->tableWidget->item(0,0);
    item0->setText(item_0);
    QTableWidgetItem *item1=  ui->tableWidget->item(1,0);
    item1->setText(item_1);
    QTableWidgetItem *item2=  ui->tableWidget->item(2,0);
    item2->setText(item_2);
    QTableWidgetItem *item3=  ui->tableWidget->item(3,0);
    item3->setText(item_3);
    QTableWidgetItem *item4=  ui->tableWidget->item(4,0);
    item4->setText(item_4);
    QTableWidgetItem *item5=  ui->tableWidget->item(5,0);
    item5->setText(item_5);
    QTableWidgetItem *item6=  ui->tableWidget->item(6,0);
    item6->setText(item_6);

    QTableWidgetItem *item7=  ui->tableWidget->item(7,0);
    item7->setText(item_7);
    QTableWidgetItem *item8=  ui->tableWidget->item(8,0);
    item8->setText(item_8);
}

SettingDialog::~SettingDialog()
{
    delete ui;
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

void SettingDialog::saveSettings()
{

    QFont font = ui->pushButtonFont->font();


    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
    setting.setValue("FontName",    font.family());
    setting.setValue("FontUnicodeName", mfontUnicod);
    setting.setValue("FontSize",      font.pointSize());
    setting.setValue("FontItalic",    font.italic());
    setting.setValue("FontBold",      font.bold());
    setting.setValue("FontStyle",     font.styleName());

    setting.setValue("AllFonts",      ui->checkBoxFont->isChecked());

    setting.setValue("ColorSheme",    ui->themesComboBox->currentIndex());
    setting.setValue("ScrollBar",     ui->ScrollBarComboBox->currentIndex());
    setting.setValue("Shell",         ui->lineEditShell->text());
    setting.setValue("Opacity",       ui->spinBoxOPacity->value());
    setting.setValue("CloseMsg",      ui->checkBoxConfirmClosing->isChecked());
    setting.setValue("CursorShape",   ui->comboBoxCursor->currentIndex());

    setting.setValue("Margin",        ui->MarginSpinBox->value());
    setting.setValue("BlinkingCursor",ui->checkBoxBlinking->isChecked());
    setting.setValue("UseCursorColor",ui->checkBoxCursColor->isChecked());
    setting.setValue("NativeWindow"  ,ui->checkBoxNativeWindow->isChecked());

    setting.setValue("LineSpacing",   ui->spinBoxLSpacing->value());


    //colors-sheme
    setting.beginGroup("colors");

    setting.setValue("background",  btnBColor->color().name());
    setting.setValue("foreground",  btnFColor->color().name());
    setting.setValue("cursorColor", btnCursorColor->color().name());

    for (int i = 0; i < 16; ++i) {
        setting.setValue("color"+QString::number(i),btnColor[i]->color().name());
    }

    setting.endGroup();

    setting.beginGroup("Keys");

    setting.setValue("AddTab",      ui->tableWidget->item(0,0)->text());
    setting.setValue("Copy",        ui->tableWidget->item(1,0)->text());
    setting.setValue("Paste",       ui->tableWidget->item(2,0)->text());
    setting.setValue("ZoomIn",      ui->tableWidget->item(3,0)->text());
    setting.setValue("ZoomOut",     ui->tableWidget->item(4,0)->text());
    setting.setValue("Preferences", ui->tableWidget->item(5,0)->text());
    setting.setValue("Quit",        ui->tableWidget->item(6,0)->text());
    setting.setValue("OpacityDecrase", ui->tableWidget->item(7,0)->text());
    setting.setValue("OpacityIncrase", ui->tableWidget->item(8,0)->text());

    setting.endGroup();

}

void SettingDialog::on_themesComboBox_currentIndexChanged(int index)
{
     QString fileName=ui->themesComboBox->itemData(index).toString();
if(fileName=="Xresources")
    xrdbquey();
else
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



        btnBColor->setColor(bcolor);
        btnFColor->setColor(fcolor);
        for (int i = 0; i < 16; ++i) {
            btnColor[i]->setColor(QColor(listColor.at(i)));
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

void SettingDialog::loadXresourceColorShemes()
{
    QString xresourceFile=QDir::homePath()+"/.Xresources";
    if(!QFile::exists(xresourceFile))
        xresourceFile=QDir::homePath()+"/.Xdefaults";
    if(!QFile::exists(xresourceFile))
        return;

    QFile files(xresourceFile);
    if(!files.open( QFile::ReadOnly))
        return;

    QTextStream textStream(&files);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));
    QString line ;//premier line;

    while   (!textStream.atEnd()) {

        line = textStream.readLine().trimmed();
        if(line.startsWith("#"))continue;

        if(line.startsWith("!"))continue;

        if(!line.contains(":")) continue;


        if(line.startsWith("*") ){
            if(line.contains("background")||line.contains("foreground")||line.contains("color") ){


                QString key=line.section(":",0,0).trimmed();
                QString value=line.section(":",1,1).trimmed();

                if (key.isEmpty())  continue;

                if (value.isEmpty()) continue;

                // qDebug()<<key.section(".",1,1) <<value;
                //  QString color=key.section(".",1,1);
                key.remove("*");
                key.remove(".");
                QString color=key.trimmed();
                if(color=="background")     btnBColor->setColor(value);

                else if(color=="foreground") btnFColor->setColor(value);

                else if(color.startsWith("color")){

                    color.remove("color");

                    int num=color.toInt();
                    qDebug()<<num <<value;
                    if(num>-1 && num<16)
                        btnColor[num]->setColor(value);

                }

            }

        }
    }
    files.close();


}


void SettingDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->standardButton(button)==QDialogButtonBox::Apply){
        saveSettings();
        emit settingsChanged();

    }else if( ui->buttonBox->standardButton(button)==QDialogButtonBox::Ok){
        saveSettings();
        accept();
    }

}

void SettingDialog::on_pushButtonFont_clicked()
{
    QFontDialog::FontDialogOptions option;
    if(!ui->checkBoxFont->isChecked()){
        option=   QFontDialog::MonospacedFonts
                | QFontDialog::NonScalableFonts
                | QFontDialog::ScalableFonts ;
    }


    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->pushButtonFont->font(), this,"",option);
    if (ok) {
        qDebug()<<"fontstyle:"<<font;
        QString bold=QString(),italic=QString();
        if(font.bold())bold=":Bold";
        if(font.italic())italic=":Italic";
        ui->pushButtonFont->setFont(font);
        qDebug()<<"pushButtonFont:"<<ui->pushButtonFont->font().italic();
        ui->pushButtonFont->setText(font.family()
                                    +bold
                                    +italic
                                    +":"+QString::number(font.pointSize()));

    } else {
        // the user canceled the dialog; font is set to the initial
        // value, in this case Times, 12.
    }

}


void SettingDialog::on_comboBoxUnicode_currentIndexChanged(int index)
{
    QFontDatabase::WritingSystem ftdata=ui->comboBoxUnicode->itemData(index).value<QFontDatabase::WritingSystem>();

      ui->fontComboBox->setWritingSystem(ftdata);

}

void SettingDialog::on_fontComboBox_currentIndexChanged(const QString &arg1)
{
   mfontUnicod=arg1;
}
