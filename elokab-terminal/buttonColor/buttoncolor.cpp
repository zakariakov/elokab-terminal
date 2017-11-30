#include "buttoncolor.h"
#include <QColorDialog>
#include <QPainter>
#include <QDebug>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

ButtonColor::ButtonColor(const QString &text, bool showAlphaChannel, QColor color, QWidget *parent) :
    QWidget(parent),dlgc(0),
    m_showAlphaChannel(showAlphaChannel),
    isDlgRunning(0)
{
    mLabel=new QLabel(this);
    tButtonColor =new QToolButton(this);
    tButtonPicker =new QToolButton(this);
    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->setMargin(0);
    layout->setSpacing(0);
    tButtonPicker->setText("Pick");
    tButtonPicker->setToolTip("Pick a color");
    tButtonColor->setToolTip("click to get color");
    #ifndef QT_NO_SHORTCUT
    mLabel->setBuddy(tButtonColor);

#endif
//     mLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
    mLabel->setText(text);
    tButtonColor->setFocusPolicy(Qt::StrongFocus);
    tButtonPicker->setAutoRaise(true);
    tButtonPicker->setIcon(QIcon::fromTheme("color-picker",QIcon(":/icons/color-picker")));
    tButtonPicker->setIconSize(QSize(16,16));
    tButtonColor->setIconSize(QSize(24,16));
    tButtonColor->setMinimumWidth(36);

    setLayout(layout);
    layout->addWidget(mLabel);
    layout->addWidget(tButtonColor);
    layout->addWidget(tButtonPicker);

    connect( tButtonColor,SIGNAL(clicked()),this,SLOT(getColor()));
    connect(tButtonPicker, SIGNAL(released()), this, SLOT(pickMode()));
 //   setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setColor( color);

}


void ButtonColor::setColor(QColor color)
{

    m_color=color;
    QPixmap pix(26,16);
    pix.fill(Qt::transparent);
    QPainter p;

    p.begin(&pix);

    p.fillRect(pix.rect(),m_color);
    p.setPen(this->palette().shadow().color());
    p.drawRect(pix.rect());
    p.setPen(this->palette().light().color());
    p.drawRect(-1,-1,26,16);
    // pix.fill(m_color);
     tButtonColor->setIcon(pix);
}
//________________________________________________________________________________________________________
void ButtonColor::pickMode()
{

  grabMouse(Qt::CrossCursor);
    mCapturing = true;
}

//________________________________________________________________________________________________________
void ButtonColor::mouseReleaseEvent(QMouseEvent* event)
{
    if (!mCapturing)
        return;

    WId id = QApplication::desktop()->winId();
    QPixmap pixmap = QPixmap::grabWindow(id, event->globalX(), event->globalY(), 1, 1);

    QImage img = pixmap.toImage();
    QColor col = QColor(img.pixel(0,0));

    //mColorViewer->setColor(col);
    setColor(col);
    emit colorChanged() ;
    mCapturing = false;
    releaseMouse();
}

void ButtonColor::getColor()
{

    if(dlgc){
        dlgc->activateWindow();
        return;
    }
    dlgc=new QColorDialog(m_color);


    if(m_showAlphaChannel==true)
        dlgc->setOption(QColorDialog::ShowAlphaChannel);

    connect(dlgc,SIGNAL(colorSelected(QColor)),this,SLOT(changeColor(QColor)));
    connect(dlgc,SIGNAL(rejected()),this,SLOT(dlgRejected()));

    dlgc->setModal(true);
    dlgc->show();

}
void ButtonColor::changeColor(QColor color)
{
    if(dlgc){
        dlgc=0;
        delete dlgc;

    }
    isDlgRunning=false;
    setColor(color);
    emit colorChanged() ;
}
void ButtonColor::dlgRejected()
{
    if(dlgc){
        dlgc=0;
        delete dlgc;

    }

}
