#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#include "../lib/qtermwidget.h"
#include <QTermWidget>
#include <QMainWindow>
#include <QDir>
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
                        int opacity=-1,
                        QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
signals:
    void clipboardAvailable(bool);
private slots:


    void setupActions();



    void copySelectedText();

    void pastText();

    void clipboardChanged();

    void customContextMenu(QPoint);
    void addNewTab(const QString &wDir=QDir::homePath(),
                   const QString &command=QString(), int opacity=-1);

    void changeTitle(const QString &txt);

    QTermWidget *termWidget();
    void closeTab(int index);
    void tabChanged(int index);
    void zoomOut();
    void zoomIn();
    void settingShow();
    void applySettings();
    void aboutShow();
private:
    Ui::MainWindow *ui;

    QTermWidget *terminaleWidget;

    QAction *actionCopy;
    QAction *actionPast;
    QAction *actionNewTab;

    QAction *mZoomOut;
    QAction *mZoomIn;

    QAction *mActSetting;
    QAction *mActAbout;
    QAction *mActAboutQt;
    QAction *mActQuit;

    QMenu *mMenu;
bool mSaveGeometry=true;

};

#endif // MAINWINDOW_H
