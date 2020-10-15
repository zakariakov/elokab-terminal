#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#include "../lib/qtermwidget.h"
#include <QTermWidget>
#include <QMainWindow>
#include <QDir>
#include <QFileSystemWatcher>
#include <QTabBar>
#include "titlewidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &wDir=QDir::homePath(),
                        const QString &command=QString(),
                        bool framless=false,
                        const QString &geometry=QString(),
                        bool ontop=false,
                        int opacity=-1, bool full=false,
                        QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
signals:
    void clipboardAvailable(bool);
private slots:


    void setupActions();

   void setupKeys();

    void copySelectedText();

    void pastText();

    void clipboardChanged();

    void customContextMenu(QPoint);
    void addNewTab(const QString &wDir=QDir::homePath(),
                   const QString &command=QString());

    void changeTitle(const QString &txt);

    QTermWidget *termWidget();
    void closeTab(int index);
    void tabClose();
    void tabChanged(int index);
    void zoomOut();
    void zoomIn();
    void opacityOut();
    void opacityIn();

    void settingShow();
    void applySettings();
    void aboutShow();
private:
    Ui::MainWindow *ui;
QTabBar *m_tabBar;
    QTermWidget *terminaleWidget;
TitleWidget *mTitleWidget;
    QFileSystemWatcher *mFileSystemWatcher;
    QAction *actionCopy;
    QAction *actionPast;
    QAction *actionNewTab;

    QAction *mZoomOut;
    QAction *mZoomIn;

    QAction *mOpacityOut;
    QAction *mOpacityIn;

    QAction *mActSetting;
    QAction *mActAbout;
    QAction *mActAboutQt;
    QAction *mActQuit;

    QMenu *mMenu;
bool mSaveGeometry=true;
bool mNativeWin=false;
int mOpacity;
int curOpacity;
};

#endif // MAINWINDOW_H
