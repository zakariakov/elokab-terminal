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
                       bool ontop, int opacity, bool full,
                       QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),mTitleWidget(nullptr)
{

    this->setAttribute(Qt::WA_TranslucentBackground,true);

    mOpacity=opacity;
    setAutoFillBackground(true);
    ui->setupUi(this);
    m_tabBar=new QTabBar;
    m_tabBar->setAutoHide(true);
    m_tabBar->setTabsClosable(true);

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
    }else if(!full){
        QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
        restoreGeometry(setting.value("Geometry").toByteArray());
        mSaveGeometry=true;
    }

    const QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard ,SIGNAL(dataChanged()),this,SLOT(clipboardChanged()));

    m_tabBar->setAutoFillBackground(true);
    connect(m_tabBar,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(m_tabBar,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));

    clipboardChanged();

    mFileSystemWatcher=new QFileSystemWatcher(this);
    QSettings s("elokab","terminal");
    mFileSystemWatcher->addPath(s.fileName());
    connect(mFileSystemWatcher,SIGNAL(fileChanged(QString)),this,SLOT(applySettings()));



    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
mNativeWin=setting.value("NativeWindow",false).toBool();

    if(mNativeWin){
        mTitleWidget=new TitleWidget(this);
        mTitleWidget->setTabWidget(m_tabBar);
        ui->verticalLayout->insertWidget(0,mTitleWidget);
        connect(mTitleWidget,SIGNAL(addTab()),this,SLOT(addNewTab()));
        setWindowFlags( Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);
    }else{
        ui->verticalLayout->insertWidget(0,m_tabBar);
    }
addNewTab(wDir,command);
}

MainWindow::~MainWindow()
{
    if(mSaveGeometry && !isFullScreen()){
        QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
        setting.setValue("Geometry",saveGeometry());
    }

    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
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

    actionNewTab->setToolTip(actionNewTab->text()+"\n"+actionNewTab->shortcut().toString());
    actionNewTab->setIcon(QIcon::fromTheme("tab-new",QIcon(":/icons/tab-new.png")));
    connect(actionNewTab,SIGNAL(triggered()),this,SLOT(addNewTab()));

    actionCopy=new QAction(tr("Copy"),this);
    actionCopy->setIcon(QIcon::fromTheme("edit-copy",QIcon(":/icons/edit-copy.png")));
    connect(actionCopy,SIGNAL(triggered()),this,SLOT(copySelectedText()));

    actionPast=new QAction (tr("Past"),this);
    actionPast->setIcon(QIcon::fromTheme("edit-paste",QIcon(":/icons/edit-paste.png")));
    connect(actionPast,SIGNAL(triggered()),this,SLOT(pastText()));
    connect(this,SIGNAL(clipboardAvailable(bool)),actionPast,SLOT(setEnabled(bool)));

    mZoomIn=new QAction(tr("Zoom in"),this);
    mZoomIn->setIcon(QIcon::fromTheme("zoom-in",QIcon(":/icons/zoom-in.png")));
    connect(mZoomIn,SIGNAL(triggered()),this,SLOT(zoomIn()));

    mZoomOut=new QAction(tr("Zoom out"),this);
    mZoomOut->setIcon(QIcon::fromTheme("zoom-out",QIcon(":/icons/zoom-out.png")));
    connect(mZoomOut,SIGNAL(triggered()),this,SLOT(zoomOut()));

    mOpacityIn=new QAction(this);
    connect(mOpacityIn,SIGNAL(triggered()),this,SLOT(opacityIn()));

    mOpacityOut=new QAction(this);
    connect(mOpacityOut,SIGNAL(triggered()),this,SLOT(opacityOut()));

    mActSetting=new QAction(tr("Settings"),this);
    mActSetting->setIcon(QIcon::fromTheme("document-properties",QIcon(":/icons/document-properties.png")));
    connect(mActSetting,SIGNAL(triggered()),this,SLOT(settingShow()));

    mActAbout=new QAction(tr("About"),this);
    mActAbout->setIcon(QIcon::fromTheme("info",QIcon(":/icons/info.png")));
    connect(mActAbout,SIGNAL(triggered()),this,SLOT(aboutShow()));

    mActAboutQt=new QAction(tr("About Qt..."),this);
    connect(mActAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

    mActQuit=new QAction(tr("Exit"),this);
    mActQuit->setIcon(QIcon::fromTheme("application-exit",QIcon(":/icons/application-exit")));
    connect(mActQuit,SIGNAL(triggered()),this,SLOT(close()));

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

    addActions(mMenu->actions());
    addAction(mOpacityIn);
    addAction(mOpacityOut);

    QAction *act=new QAction;
        connect(act,SIGNAL(triggered()),this,SLOT(tabClose()));
        act->setShortcut(QKeySequence("Ctrl+D"));
        addAction(act);
    setupKeys();
}

void MainWindow::setupKeys()
{

    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
    setting.beginGroup("Keys");

    QString addTab=QKeySequence(QKeySequence::AddTab).toString();
    actionNewTab->setShortcut(QKeySequence(setting.value("AddTab",addTab).toString()));

    actionCopy->setShortcut(QKeySequence(setting.value("Copy","Ctrl+Shift+C").toString()));

    actionPast->setShortcut(QKeySequence(setting.value("Past","Ctrl+Shift+V").toString()));

    QString zoomIn=QKeySequence(QKeySequence::ZoomIn).toString();
    mZoomIn->setShortcut(QKeySequence(setting.value("ZoomIn",zoomIn).toString()));

    QString zoomOut=QKeySequence(QKeySequence::ZoomOut).toString();
    mZoomOut->setShortcut(QKeySequence(setting.value("ZoomOut",zoomOut).toString()));

    mOpacityOut->setShortcut(QKeySequence(setting.value("OpacityDecrase","Ctrl+'").toString()));
    mOpacityIn->setShortcut(QKeySequence(setting.value("OpacityIncrase","Ctrl+=").toString()));

    mActSetting->setShortcut(QKeySequence(setting.value("Preferences","Ctrl+O").toString()));

    QString quit=QKeySequence(QKeySequence::Quit).toString();
    mActQuit->setShortcut(QKeySequence(setting.value("Quit",quit).toString()));

    setting.endGroup();

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
    mMenu->exec(QCursor::pos());
}

void MainWindow::addNewTab(const QString &wDir, const QString &command)
{

    QTermWidget *terminaleWidget=new    QTermWidget(0,this);

    terminaleWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tabBar->addTab(terminaleWidget->title());
    m_tabBar->setCurrentIndex(m_tabBar->count()-1);

    connect(terminaleWidget,SIGNAL(selectionAvailable(bool)),actionCopy,SLOT(setEnabled(bool)));
    connect(terminaleWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));

    connect(terminaleWidget,SIGNAL(titleChanged(QString)),this,SLOT(changeTitle(QString)));

    ui->stackedWidget->addWidget(terminaleWidget);
    ui->stackedWidget->setCurrentWidget(terminaleWidget);
    terminaleWidget->setFocus();

    terminaleWidget->setInitialWorkingDirectory(wDir);
    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
    QString shell=setting.value("Shell",QString()).toString();
    terminaleWidget-> setShellProgram(shell);
    terminaleWidget->startShellProgram();

    qDebug()<<wDir;

    if(!command.isEmpty())
    {
        terminaleWidget->getOutputFromCommand(command);
    }

    QTimer::singleShot(5,this,SLOT(applySettings()))  ;

}

void MainWindow::changeTitle(const QString &txt)
{
    // qDebug("%s %d %s %s", __FILE__, __LINE__,"titleChanged", txt);
    qDebug()<<__FILE__<<__LINE__<<"titleChanged"<<txt;
    setWindowTitle(txt);
    int index=ui->stackedWidget->currentIndex();
    if(termWidget()){
        QString title=termWidget()->title();

        m_tabBar->setTabToolTip(index,QString());
        if(title.length()>20){
            m_tabBar->setTabToolTip(index,title);
            title=title.left(19)+"…";
        }

        m_tabBar->setTabText(index,title);
        qDebug()<<termWidget()->sessionIsruning();
    }
    if(txt=="exit"){
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(index));

        if(!termWidget->sessionIsruning()){
            termWidget->close();
            ui->stackedWidget->removeWidget(termWidget);
            delete termWidget;
            m_tabBar->removeTab(index);

        }
 qDebug()<<"sessionIsruning:"<<termWidget->sessionIsruning();
        //                if(ui->stackedWidget->count()>1){

        //                    termWidget->close();
        //                    ui->stackedWidget->removeWidget(termWidget);
        //                    delete termWidget;
        //                    m_tabBar->removeTab(index);


        //                }else{
        //                    qApp->quit();
        //                }

    }



}

QTermWidget *MainWindow::termWidget()
{
    QTermWidget   *w= qobject_cast<QTermWidget *>( ui->stackedWidget->currentWidget());
    if(w)
        return w;
    return nullptr;

}

void MainWindow::tabClose()
{
    int index=m_tabBar->currentIndex();
    QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(index));

    if(termWidget){
        termWidget->close();
        delete termWidget;

    }
   m_tabBar->removeTab(index);
   if(m_tabBar->count()<1)
       qApp->quit();
}
void MainWindow::closeTab(int index)
{
   // Q_UNUSED(index)
    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
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

    QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(index));

    if(termWidget){
        termWidget->close();
        delete termWidget;

    }
   m_tabBar->removeTab(index);

   if(m_tabBar->count()<1)
       qApp->quit();
}

void MainWindow::tabChanged(int index)
{
    Q_UNUSED(index)
    ui->stackedWidget->setCurrentIndex(index);
    //  ui->tabWidget->setTabsClosable(ui->tabWidget->count()-1);
    QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(index));


    m_tabBar->setTabsClosable(m_tabBar->count()-1);
    if(termWidget){
        setWindowTitle(termWidget->title());
    }

}

void MainWindow::zoomIn()
{
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(i));


        if(termWidget)
        {
            termWidget->zoomIn();
        }

    }
}

void MainWindow::zoomOut()
{
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(i));

        if(termWidget)
        {
            termWidget->zoomOut();
        }

    }
}

void MainWindow::opacityOut()
{
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(i));

        curOpacity+=10;
        if(curOpacity>100)curOpacity=100;
        if(termWidget)
        {
            termWidget->setTerminalOpacity(qreal(curOpacity)/100);

        }

    }
}
void MainWindow::opacityIn()
{
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(i));

        curOpacity-=10;
        if(curOpacity<0)curOpacity=0;
        if(termWidget)
        {
            termWidget->setTerminalOpacity(qreal(curOpacity)/100);

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

    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
mNativeWin=setting.value("NativeWindow",false).toBool();
    QString fontName=setting.value("FontName","Monospace").toString();
    QString fontUnicode=setting.value("FontUnicodeName","Monospace").toString();
    int fontSize=setting.value("FontSize",11).toInt();
    bool italic=setting.value("FontItalic",false).toBool();
    QString styleName=setting.value("FontStyle",this->font().styleName()).toString();
    bool bold=setting.value("FontBold",false).toBool();

    QFont font = QApplication::font();
    font.setPointSize(fontSize);
    font.setFamily(fontName);
    font.setItalic(italic);
    font.setBold(bold);
    font.setStyleName(styleName);
    font.setFamilies(QStringList()<<fontName<<fontUnicode);
    qDebug()<<"font.families"<<font.families();
    int spos=setting.value("ScrollBar",0).toInt();
    if(mOpacity==-1){
        mOpacity=setting.value("Opacity",100).toInt();
        curOpacity=mOpacity;
    }

    int     margin=setting.value("Margin",5).toInt();
    int     cursorShape=setting.value("CursorShape",0).toInt();
    bool    blinking=setting.value("BlinkingCursor",false).toBool();
    bool    usecurscolor=setting.value("UseCursorColor",false).toBool();
    uint    linespacing=setting.value("LineSpacing",0).toUInt();


    setting.beginGroup("colors");
    QColor fcolor=setting.value("foreground",QColor(255,255,255)).value<QColor>();
    QColor bcolor=setting.value("background",QColor(0,0,0)).value<QColor>();
    QColor cursorcolor=setting.value("cursorColor",fcolor).value<QColor>();
    setting.endGroup();


    for (int i = 0; i < ui->stackedWidget->count(); ++i)
    {
        QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->stackedWidget->widget(i));

        if(termWidget)
        {
            termWidget->setTerminalMargin(margin);

            termWidget->setLineSpacing(linespacing);

            termWidget->setTerminalFont(font);

            termWidget->setScrollBarPosition(spos);

            termWidget->updateColorScheme();

            termWidget->setTerminalOpacity(qreal(mOpacity)/100);

            termWidget->setKeyboardCursorShape(cursorShape);

            termWidget->setBlinkingCursor(blinking);

            termWidget->setKeyboardCursorColor(!usecurscolor,cursorcolor);
        }

    }

    //--------------------------------------------------------
    QString mst=QString(
                " QTabBar::close-button {"
                "image: url(:/icons/application-exit.png)"
                "}"

                "QTabBar::tab:selected {"
                "background-color: %1 ;"
                " border-top: 2px solid palette(highlight);"
                "border-top-color:palette(highlight);"
                "color:%3;"
                "margin-top: 2px; "
                "}"
                "QTabBar::tab:!selected {"
                "background-color: %2;"
                "color:%4"

                "}").arg(bcolor.name())
            .arg(bcolor.darker(150).name())
            .arg(fcolor.name())
            .arg(fcolor.darker().name())
            ;

    m_tabBar->  setStyleSheet(mst);
    //-------------------------------------------------------
    setupKeys();

    if(mTitleWidget){
        mTitleWidget->loadSetting();
    }

    QSettings s("elokab","terminal");
    mFileSystemWatcher->addPath(s.fileName());
    mOpacity=-1;
}

void MainWindow::aboutShow()
{
    QMessageBox::about(this, tr("About Elokab-terminal"),
                       tr("<p><b>Elokab-terminal %1</b></p><p>A lightweight  terminal emulator"
                          "<p> See <a href=\"https://github.com/zakariakov/elokab-terminal/\">github</a> for more information.</p><p>&copy; Abouzakaria</p>").arg(QApplication::applicationVersion()));

}
