#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingdialog.h"
#include <QDir>
#include <QClipboard>
#include <QMimeData>
#include <QShortcut>
#include <QToolButton>
#include <QMenu>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QApplication>
#include <QCheckBox>
#include <QTimer>
//#include <QGraphicsBlurEffect>
MainWindow::MainWindow(const QString &wDir,
                       const QString &command,
                       bool framless,
                       const QString &geometry,
                       bool ontop,int opacity,
                       QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->setAttribute(Qt::WA_TranslucentBackground,true);
//    QGraphicsBlurEffect *efect=new QGraphicsBlurEffect;
//    efect->setBlurRadius(5.0);
//        this->setGraphicsEffect(efect);
mOpacity=opacity;
   setAutoFillBackground(true);
    ui->setupUi(this);
    setupActions();
    //Framless Border
    if(framless && ontop)
        setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    else if(framless && !ontop)
        setWindowFlags( Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);
    else if(!framless && ontop)
        setWindowFlags( /*Qt::FramelessWindowHint |*/ Qt::WindowStaysOnTopHint);


    //Geometry
    QStringList list=geometry.split(",");
    if(!geometry.isEmpty()&& list.count()==4){
        int _x=QString(list.at(0)).toInt();
        int _y=QString(list.at(1)).toInt();
        int _w=QString(list.at(2)).toInt();
        int _h=QString(list.at(3)).toInt();
        qDebug()<<"MainWindow Geometry:"<<_x<<_y<<_w<<_h;
        setGeometry(_x,_y,_w,_h);
        mSaveGeometry=false;
    }else{
        QSettings setting;
        restoreGeometry(setting.value("Geometry").toByteArray());
           mSaveGeometry=true;
    }




    const QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard ,SIGNAL(dataChanged()),this,SLOT(clipboardChanged()));
    ui->tabWidget->setAutoFillBackground(true);
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
    clipboardChanged();
    addNewTab(wDir,command);

}

MainWindow::~MainWindow()
{
    if(mSaveGeometry){
        QSettings setting;
        setting.setValue("Geometry",saveGeometry());
    }

    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings setting;
    bool checked= setting.value("CloseMsg",true).toBool();
    if(!checked){
        event->accept();
        return;
    }

    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to exit ?."));

    QCheckBox *cb=new QCheckBox(tr("Do not ask again"));
    msgBox.setCheckBox(cb);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    checked=msgBox.checkBox()->isChecked();
    setting.setValue("CloseMsg",!checked);


    if(ret==QMessageBox::No)
        event->ignore();
    else
        event->accept();


}

void MainWindow::setupActions()
{

    actionNewTab=new QAction(tr("New Tab"),this);
    actionNewTab->setShortcut(QKeySequence::AddTab);
    actionNewTab->setToolTip(actionNewTab->text()+"\n"+actionNewTab->shortcut().toString());
    actionNewTab->setIcon(QIcon::fromTheme("tab-new",QIcon(":/icons/tab-new.png")));
    connect(actionNewTab,SIGNAL(triggered()),this,SLOT(addNewTab()));

    actionCopy=new QAction(tr("Copy"),this);
    actionCopy->setIcon(QIcon::fromTheme("edit-copy",QIcon(":/icons/edit-copy.png")));
    actionCopy->setShortcut(QKeySequence("Ctrl+Shift+C"));
    connect(actionCopy,SIGNAL(triggered()),this,SLOT(copySelectedText()));

    actionPast=new QAction (tr("Past"),this);
    actionPast->setIcon(QIcon::fromTheme("edit-paste",QIcon(":/icons/edit-paste.png")));
    actionPast->setShortcut(QKeySequence("Ctrl+Shift+V"));
    connect(actionPast,SIGNAL(triggered()),this,SLOT(pastText()));
    connect(this,SIGNAL(clipboardAvailable(bool)),actionPast,SLOT(setEnabled(bool)));

    mZoomIn=new QAction(tr("Zoom in"),this);
    mZoomIn->setIcon(QIcon::fromTheme("zoom-in",QIcon(":/icons/zoom-in.png")));
    mZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(mZoomIn,SIGNAL(triggered()),this,SLOT(zoomIn()));

    mZoomOut=new QAction(tr("Zoom out"),this);
    mZoomOut->setIcon(QIcon::fromTheme("zoom-out",QIcon(":/icons/zoom-out.png")));
    mZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(mZoomOut,SIGNAL(triggered()),this,SLOT(zoomOut()));


    mActSetting=new QAction(tr("Settings"),this);
    mActSetting->setIcon(QIcon::fromTheme("document-properties",QIcon(":/icons/document-properties.png")));
    mActSetting->setShortcut(QKeySequence::Preferences);
    connect(mActSetting,SIGNAL(triggered()),this,SLOT(settingShow()));

    mActAbout=new QAction(tr("About"),this);
    mActAbout->setIcon(QIcon::fromTheme("info",QIcon(":/icons/info.png")));
    mActAbout->setShortcut(QKeySequence::HelpContents);
    connect(mActAbout,SIGNAL(triggered()),this,SLOT(aboutShow()));

    mActAboutQt=new QAction(tr("About Qt..."),this);
    connect(mActAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

    mActQuit=new QAction(tr("Exit"),this);
    mActQuit->setIcon(QIcon::fromTheme("application-exit",QIcon(":/icons/application-exit")));
    connect(mActQuit,SIGNAL(triggered()),this,SLOT(close()));
    mActQuit->setShortcut(QKeySequence::Quit);

    mMenu=new QMenu(this);
    mMenu->addAction(actionNewTab);
    mMenu->addSeparator();
    mMenu->addAction(actionCopy);
    mMenu->addAction(actionPast);
    mMenu->addSeparator();
    mMenu->addAction(mZoomIn);
    mMenu->addAction(mZoomOut);
    mMenu->addSeparator();
    mMenu->addAction(mActSetting);
    mMenu->addSeparator();
    mMenu->addAction(mActAbout);
    mMenu->addAction(mActAboutQt);
    mMenu->addSeparator();
    mMenu->addAction(mActQuit);

    QToolButton *btnMenu=new QToolButton(this);
    btnMenu->setShortcut(QKeySequence("Ctrl+M"));
    btnMenu->setToolTip(tr("Menu")+"\n"+btnMenu->shortcut().toString());
    btnMenu->setIcon(QIcon::fromTheme("view-list-details",QIcon(":/icons/view-list-details.png")));
    btnMenu->setMenu(mMenu);
    btnMenu->setAutoRaise(true);
    btnMenu->setPopupMode(QToolButton::InstantPopup);
    ui->tabWidget->setCornerWidget(btnMenu,Qt::TopRightCorner);

    QToolButton *btnNewTab=new QToolButton(this);
    btnNewTab->setDefaultAction(actionNewTab);
    btnNewTab->setAutoRaise(true);
    ui->tabWidget->setCornerWidget(btnNewTab,Qt::TopLeftCorner);


}



void MainWindow::copySelectedText()
{
    termWidget()->CopySelection();
}

void MainWindow::pastText()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasText()) {
        QString text =mimeData->text();
        termWidget()->sendText(text);
    }
}

void MainWindow::clipboardChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData->hasText()){
        if(!mimeData->text().isEmpty())
            emit clipboardAvailable(true);
        else
            emit clipboardAvailable(false);
    }else{
        emit clipboardAvailable(false);
    }
}

void MainWindow::customContextMenu(QPoint)
{
    //    QMenu menu;
    //    menu.addAction(actionCopy);
    //    menu.addAction(actionPast);
    mMenu->exec(QCursor::pos());
}

void MainWindow::addNewTab(const QString &wDir, const QString &command)
{


    QTermWidget *terminaleWidget=new    QTermWidget(0,this);




    terminaleWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QIcon icon=QIcon::fromTheme("terminal",QIcon(":/icons/terminal.png"));
    ui->tabWidget->addTab(terminaleWidget,icon,terminaleWidget->title());
    connect(terminaleWidget,SIGNAL(selectionAvailable(bool)),actionCopy,SLOT(setEnabled(bool)));
    connect(terminaleWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));

    connect(terminaleWidget,SIGNAL(titleChanged(QString)),this,SLOT(changeTitle(QString)));

    ui->tabWidget->setCurrentWidget(terminaleWidget);
    terminaleWidget->setFocus();




//    QFont font = QApplication::font();
//    font.setFamily("Monospace");
//    font.setPointSize(10);
//    QString fontName=setting.value("FontFamily",font.family()).toString();
//    int fontSize=setting.value("FontSize",font.pointSize()).toInt();
//    font.setPointSize(fontSize);
//    font.setFamily(fontName);


//    int spos=setting.value("ScrollBar",0).toInt();
//    qDebug()<<"opacity"<<opacity;

//    if(opacity==-1)
//        opacity=setting.value("Opacity",100).toInt();


 terminaleWidget->setInitialWorkingDirectory(wDir);
//    int cursorShape=setting.value("CursorShape",0).toInt();
    QSettings setting;
    QString shell=setting.value("Shell",QString()).toString();
    terminaleWidget-> setShellProgram(shell);
   terminaleWidget->startShellProgram();
    //TODO FIXME
//      terminaleWidget->setTerminalFont(font);
//    terminaleWidget->setColorScheme(4);
//    terminaleWidget->setKeyboardCursorShape(cursorShape);
//    terminaleWidget->setScrollBarPosition(spos);


qDebug()<<wDir;

  //  terminaleWidget->setTerminalOpacity(qreal(opacity)/100);


    if(!command.isEmpty())
    {
        terminaleWidget->getOutputFromCommand(command);
    }


  //  terminaleWidget->setTerminalFont(font);
   QTimer::singleShot(10,this,SLOT(applySettings()))  ;

}

void MainWindow::changeTitle(const QString &txt)
{
    // qDebug("%s %d %s %s", __FILE__, __LINE__,"titleChanged", txt);
   qDebug()<<__FILE__<<__LINE__<<"titleChanged"<<txt;
    setWindowTitle(txt);
    int index=ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(index,termWidget()->title());
    //qDebug()<<termWidget()->sessionIsruning();
    //if(txt=="exit"){

        if(!termWidget()->sessionIsruning()){
            if(ui->tabWidget->count()>1){
                termWidget()->close();
                delete termWidget();
            }else{
                qApp->quit();
            }

        }

    //}

}

QTermWidget *MainWindow::termWidget()
{
    QTermWidget   *w= qobject_cast<QTermWidget *>( ui->tabWidget->currentWidget());
    if(w)
        return w;
    return nullptr;

}

void MainWindow::closeTab(int index)
{
    Q_UNUSED(index);
    QSettings setting;
    bool checked= setting.value("CloseMsg",true).toBool();
    if(checked){
        QMessageBox msgBox;
        msgBox.setText(tr("Do you want to exit this tab?"));
        QCheckBox *cb=new QCheckBox(tr("Do not ask again"));
        msgBox.setCheckBox(cb);

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        checked=msgBox.checkBox()->isChecked();
        setting.setValue("CloseMsg",!checked);

        if(ret==QMessageBox::No)
            return;
    }




    termWidget()->close();
    delete termWidget();
}

void MainWindow::tabChanged(int index)
{
    ui->tabWidget->setTabsClosable(ui->tabWidget->count()-1);
    setWindowTitle(termWidget()->title());
    ui->tabWidget->setTabText(index,termWidget()->title());
}

void MainWindow::zoomIn()
{
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->tabWidget->widget(i));


        if(termWidget)
        {
            termWidget->zoomIn();
        }

    }
}

void MainWindow::zoomOut()
{
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->tabWidget->widget(i));


        if(termWidget)
        {
            termWidget->zoomOut();
        }

    }
}

void MainWindow::settingShow()
{
    SettingDialog *dlg=new SettingDialog;

    connect(dlg,SIGNAL(settingsChanged()),this,SLOT(applySettings()));

    if(dlg->exec()==QDialog::Accepted)
    {
        applySettings();
    }
    delete dlg;
}

void MainWindow::applySettings()
{
    QSettings setting;

    QFont font = QApplication::font();
    font.setFamily("Monospace");
    font.setPointSize(11);
    QString fontName=setting.value("FontFamily",font.family()).toString();
    int fontSize=setting.value("FontSize",font.pointSize()).toInt();
    font.setPointSize(fontSize);
    font.setFamily(fontName);
//    QString shell=setting.value("Shell",QString()).toString();
//    terminaleWidget-> setShellProgram(shell);
// terminaleWidget->startShellProgram();

    //QFont font=  setting.value("Font",font).value<QFont>();
    int spos=setting.value("ScrollBar",0).toInt();
    if(mOpacity==-1){
         mOpacity=setting.value("Opacity",100).toInt();
    }

    // QString shell=setting.value("Shell",QString()).toString();
       int cursorShape=setting.value("CursorShape",0).toInt();

    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->tabWidget->widget(i));

        if(termWidget)
        {
            termWidget->setTerminalFont(font);

            termWidget->setScrollBarPosition(spos);

            termWidget->setColorScheme(4);

            termWidget->setTerminalOpacity(qreal(mOpacity)/100);

            termWidget->setKeyboardCursorShape(cursorShape);
        }

    }
mOpacity=-1;
}

void MainWindow::aboutShow()
{
    //    QMessageBox::about(this, QString("Elokab-terminal ") + QApplication::applicationVersion(), tr("A lightweight  terminal emulator"));
    QMessageBox::about(this, tr("About Elokab-terminal"),
                       tr("<p><b>Elokab-terminal %1</b></p><p>A lightweight  terminal emulator"
                          "<p> See <a href=\"https://github.com/zakariakov/elokab-terminal/\">github</a> for more information.</p><p>&copy; Abouzakaria</p>").arg(QApplication::applicationVersion()));

}
