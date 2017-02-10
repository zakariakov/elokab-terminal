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
MainWindow::MainWindow(const QString &wDir,
                       const QString &command,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),numTab(0)
{
    ui->setupUi(this);
    setupActions();
      QSettings setting;
    restoreGeometry(setting.value("Geometry").toByteArray());

    const QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard ,SIGNAL(dataChanged()),this,SLOT(clipboardChanged()));

    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
    clipboardChanged();
    addNewTab(wDir,command);

}

MainWindow::~MainWindow()
{
    QSettings setting;
    setting.setValue("Geometry",saveGeometry());

    delete ui;
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

    QMenu *menu=new QMenu(this);
    menu->addAction(actionNewTab);
    menu->addSeparator();
    menu->addAction(actionCopy);
    menu->addAction(actionPast);
    menu->addSeparator();
    menu->addAction(mZoomIn);
    menu->addAction(mZoomOut);
    menu->addSeparator();
    menu->addAction(mActSetting);
    menu->addSeparator();
    menu->addAction(mActAbout);
    menu->addAction(mActAboutQt);

    QToolButton *btnMenu=new QToolButton(this);
    btnMenu->setShortcut(QKeySequence("Ctrl+L"));
    btnMenu->setToolTip(tr("Menu")+"\n"+btnMenu->shortcut().toString());
    btnMenu->setIcon(QIcon::fromTheme("view-list-details",QIcon(":/icons/view-list-details.png")));
    btnMenu->setMenu(menu);
    btnMenu->setAutoRaise(true);
    btnMenu->setPopupMode(QToolButton::InstantPopup);
    ui->tabWidget->setCornerWidget(btnMenu,Qt::TopRightCorner);

    QToolButton *btnNewTab=new QToolButton(this);
    btnNewTab->setDefaultAction(actionNewTab);
    btnNewTab->setAutoRaise(true);
    ui->tabWidget->setCornerWidget(btnNewTab,Qt::TopLeftCorner);


}

void MainWindow::on_actionTest_triggered()
{
    termWidget()->clear();
termWidget()->zoomIn();
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
    QMenu menu;
    menu.addAction(actionCopy);
    menu.addAction(actionPast);
    menu.exec(QCursor::pos());
}

void MainWindow::addNewTab(const QString &wDir, const QString &command)
{
    numTab++;

    QTermWidget *terminaleWidget=new    QTermWidget(0);
    QFont font = QApplication::font();
    font.setFamily("Monospace");
    font.setPointSize(10);
    font.setStyleHint(QFont::TypeWriter);

    QSettings setting;
    QFont f=  setting.value("Font",font).value<QFont>();
    int color=setting.value("ColorSheme",0).toInt();
    int spos=setting.value("ScrollBar",0).toInt();
    QString shell=setting.value("Shell",QString()).toString();
    terminaleWidget-> setShellProgram(shell);
    terminaleWidget->setTerminalFont(f);
    terminaleWidget->setColorScheme(color+1);
    terminaleWidget->setScrollBarPosition(spos);
    terminaleWidget->setInitialWorkingDirectory(wDir);
    terminaleWidget->startShellProgram();

    if(!command.isEmpty())
    {
       // terminaleWidget-> setShellProgram(command);
         terminaleWidget->getOutputFromCommand(command);
    }

    terminaleWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QIcon icon=QIcon::fromTheme("terminal",QIcon(":/icons/terminal.png"));
    ui->tabWidget->addTab(terminaleWidget,icon,tr("Shell ")+QString::number(numTab));
    connect(terminaleWidget,SIGNAL(selectionAvailable(bool)),actionCopy,SLOT(setEnabled(bool)));
    connect(terminaleWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));


    ui->tabWidget->setCurrentWidget(terminaleWidget);
terminaleWidget->setFocus();
}

QTermWidget *MainWindow::termWidget()
{
    QTermWidget   *w= qobject_cast<QTermWidget *>( ui->tabWidget->currentWidget());
    if(w)
        return w;
    return 0;

}

void MainWindow::closeTab(int index)
{
    QTermWidget   *w= qobject_cast<QTermWidget *>( ui->tabWidget->widget(index));
//  if (w->sessionIsruning()){
//      QMessageBox msgBox;
//       msgBox.setText("The document has been modified.");
//       msgBox.setInformativeText("Do you want to save your changes?");
//       msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//       msgBox.setDefaultButton(QMessageBox::No);
//       int ret = msgBox.exec();
//       if(ret==QMessageBox::No)
//           return;

//  }
  w->close();
   delete w;
}

void MainWindow::tabChanged(int /*index*/)
{
  ui->tabWidget->setTabsClosable(ui->tabWidget->count()-1);
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
    if(dlg->exec()==QDialog::Accepted)
    {
        QFont font=dlg->getFont();
        int color=dlg->getColorSheme();
        int spos=dlg->getScrollBar();
        for (int i = 0; i < ui->tabWidget->count(); ++i)
       {
            QTermWidget *termWidget= qobject_cast<QTermWidget *>(ui->tabWidget->widget(i));

           if(termWidget)
           {
              termWidget->setTerminalFont(font);
              termWidget->setColorScheme(color);
              termWidget->setScrollBarPosition(spos);


           }

        }
    }
}

void MainWindow::aboutShow()
{
//    QMessageBox::about(this, QString("Elokab-terminal ") + QApplication::applicationVersion(), tr("A lightweight  terminal emulator"));
    QMessageBox::about(this, tr("About Elokab-terminal"),
                       tr("<p><b>Elokab-terminal %1</b></p><p>A lightweight  terminal emulator"
                          "<p> See <a href=\"https://elkirtasse.sourceforge.net/\">elkirtasse.sourceforge.net</a> for more information.</p><p>&copy; About Zakaria</p>").arg(QApplication::applicationVersion()));

}
