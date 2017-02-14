/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef BUTTONCOLOR_H
#define BUTTONCOLOR_H

#include <QLabel>
#include <QColorDialog>
#include <QWidget>
#include <QToolButton>
#include <QMouseEvent>
class ButtonColor : public QWidget
{
          Q_OBJECT
public:

    explicit ButtonColor(const QString &text=QString(),
                               bool showAlphaChannel= false,
                               QColor color=QColor("black"),
                               QWidget *parent = 0);

signals:
          void colorChanged() ;

protected:

     //!
     void mouseReleaseEvent(QMouseEvent *event);

public slots:
        void setText(const QString &text){mLabel->setText(text);}
          void setColor(QColor color);
          QColor color(){return m_color;}



private:
          QColorDialog *dlgc;
          bool m_showAlphaChannel;
           bool isDlgRunning;

         QToolButton *tButtonColor ;
         QToolButton *tButtonPicker ;
QLabel *mLabel;
          QColor m_color;
          bool mCapturing;

private slots:
          void getColor();

          void changeColor(QColor color);
          void dlgRejected();
          void pickMode();
};

#endif // BUTTONCOLOR_H
