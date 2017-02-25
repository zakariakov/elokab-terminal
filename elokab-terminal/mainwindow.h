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
                        QWidget *parent = 0);
    ~MainWindow();
signals:
    void clipboardAvailable(bool);
private slots:
    void on_actionTest_triggered();

    void setupActions();



    void copySelectedText();

    void pastText();

    void clipboardChanged();

    void customContextMenu(QPoint);
    void addNewTab(const QString &wDir=QDir::homePath(),
                   const QString &command=QString());
    QTermWidget *termWidget();
    void closeTab(int index);
    void tabChanged(int);
    void zoomOut();
    void zoomIn();
    void settingShow();
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
    int numTab;
};

#endif // MAINWINDOW_H
