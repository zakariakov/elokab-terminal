#include "titlewidget.h"
#include <QAction>
#include <QSettings>
#include <QApplication>
TitleWidget::TitleWidget(QWidget *parent) : QWidget(parent),m_parent(parent)
{

    bGWidget=new QWidget;
    bGWidget->setObjectName("TitleWidget");
    QHBoxLayout *hlayoutmain=new QHBoxLayout(this);
    hlayoutmain->setMargin(0);

    hlayout=new QHBoxLayout(this);
    hlayout->setContentsMargins(6,0,6,0);
    hlayout->setSpacing(5);

    btnClose=new QToolButton(this);
    btnMaximize=new QToolButton(this);
    btnMininize=new QToolButton(this);

    btnClose->setAutoRaise(true);
    btnMaximize->setAutoRaise(true);
    btnMininize->setAutoRaise(true);

    btnClose->setMaximumSize(QSize(12,12));
    btnMaximize->setMaximumSize(QSize(12,12));
    btnMininize->setMaximumSize(QSize(12,12));

     connect(btnMininize,SIGNAL(clicked()),m_parent,SLOT(showMinimized()));
     connect(btnClose,SIGNAL(clicked()),m_parent,SLOT(close()));
    connect(btnMaximize,SIGNAL(clicked()),this,SLOT(ToggleMaxMinimize()));

    QAction *actionNewTab=new QAction(tr("New Tab"),this);

    actionNewTab->setToolTip(actionNewTab->text()+"\n"+actionNewTab->shortcut().toString());
    actionNewTab->setIcon(QIcon::fromTheme("tab-new",QIcon(":/icons/tab-new.png")));
    connect(actionNewTab,SIGNAL(triggered()),this,SIGNAL(addTab()));

    QToolButton *btnNewTab=new QToolButton(this);
    btnNewTab->setDefaultAction(actionNewTab);
    btnNewTab->setAutoRaise(true);
    btnNewTab->setIconSize(QSize(16,16));
    hlayout->addWidget(btnNewTab);
    hlayout->addStretch();

    hlayout->addWidget(btnMininize);
    hlayout->addWidget(btnMaximize);
    hlayout->addWidget(btnClose);

    bGWidget->setLayout(hlayout);
    hlayoutmain->addWidget(bGWidget);
    setLayout(hlayoutmain);
    loadSetting();
}

void TitleWidget::setTabWidget(QWidget *wtab)
{
    hlayout->insertWidget(0,wtab);

}

void TitleWidget::ToggleMaxMinimize()
{
    m_parent->isMaximized() ?  m_parent->showNormal() : m_parent->showMaximized();
}

void TitleWidget::loadSetting()
{
    QSettings setting(QApplication::organizationName(),QApplication::applicationDisplayName());
    int opacity =setting.value("Opacity",100).toInt();

    setting.beginGroup("colors");
    QString fcolor=setting.value("foreground","#fff").toString();
    QString bcolor=setting.value("background","#000").toString();

    QString  redcol=setting.value("color1","#ff0000").toString();
    QString  greencol=setting.value("color2","#1baa24").toString();
    QString  bluecol=setting.value("color4","#0055ff").toString();
    setting.endGroup();


    QString strStyle=QString("QToolButton{background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, "
                             "stop:0 %1, "
                             "stop:0.800 %1, "
                             "stop:1 transparent);"
                             "border: 0px ;}");

    btnClose->setStyleSheet(strStyle.arg(redcol));
    btnMaximize->setStyleSheet(strStyle.arg(greencol));
    btnMininize->setStyleSheet(strStyle.arg(bluecol));

    QColor col(bcolor);
    setStyleSheet(QString("QWidget#TitleWidget{background-color: rgba(%1, %2, %3, %4);}")
                  .arg(col.red())
                  .arg(col.green())
                  .arg(col.blue())
                  .arg(QString::number(opacity*2.55)));

}

//________________________________________________________________________________________________________
void TitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    QPoint p(cursor().pos()-QPoint(m_x,m_y));

    m_parent-> move(p);
}

//________________________________________________________________________________________________________
void TitleWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_x=cursor().pos().x()-m_parent->pos().x();
    m_y=cursor().pos().y()-m_parent->pos().y();
}
