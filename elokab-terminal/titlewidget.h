#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QHBoxLayout>
class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = nullptr);
    void setTabWidget(QWidget *wtab);
    void loadSetting();
  void mouseMoveEvent(QMouseEvent *event);
 void mousePressEvent(QMouseEvent *event);
signals:
void addTab();

private slots:
void ToggleMaxMinimize();

private:

   QWidget *m_parent;
   QWidget *bGWidget;
   QHBoxLayout *hlayout;
   QToolButton *btnClose;
   QToolButton *btnMaximize;
   QToolButton *btnMininize;

   int m_x;
   int m_y;
};

#endif // TITLEWIDGET_H
