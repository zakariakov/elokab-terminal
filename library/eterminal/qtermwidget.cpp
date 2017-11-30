/*  Copyright (C) 2008 e_k (e_k@users.sourceforge.net)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "qtermwidget.h"

// Qt
#include <QApplication>
#include <QTextCodec>
#include <QKeyEvent>
#include <QSettings>
#include <QDebug>
// Konsole
#include "Session.h"
#include "TerminalDisplay.h"

using namespace Konsole;

void *createTermWidget(int startnow, void *parent)
{
  return (void*)new QTermWidget(startnow, (QWidget*)parent);
}

struct TermWidgetImpl
{
  TermWidgetImpl(QWidget* parent = 0);

  TerminalDisplay *m_terminalDisplay;
  Session *m_session;

  Session* createSession();
  TerminalDisplay* createTerminalDisplay(Session *session, QWidget* parent);
};

TermWidgetImpl::TermWidgetImpl(QWidget* parent)
{
  m_session = createSession();
  m_terminalDisplay = createTerminalDisplay(m_session, parent);
}



Session *TermWidgetImpl::createSession()
{
  Session *session = new Session();

  session->setTitle(Session::NameRole, "QTermWidget");
  session->setProgram("/bin/bash");
  QStringList args("");
  session->setArguments(args);
  session->setAutoClose(true);

  session->setCodec(QTextCodec::codecForName("UTF-8"));

  session->setFlowControlEnabled(true);
  session->setHistoryType(HistoryTypeBuffer(1000));

  session->setDarkBackground(true);

  session->setKeyBindings("");
  return session;
}

TerminalDisplay *TermWidgetImpl::createTerminalDisplay(Session *session, QWidget* parent)
{
  TerminalDisplay* display = new TerminalDisplay(parent);

  display->setBellMode(TerminalDisplay::NotifyBell);
  display->setTerminalSizeHint(true);
  display->setTripleClickMode(TerminalDisplay::SelectWholeLine);
  display->setTerminalSizeStartup(true);

  display->setRandomSeed(session->sessionId() * 31);

  return display;
}


QTermWidget::QTermWidget(int startnow, QWidget *parent)
  :QWidget(parent)
{
// setWindowOpacity(0.0);
  setLayoutDirection(Qt::LeftToRight);
  m_impl = new TermWidgetImpl(this);

  init();

  if (startnow && m_impl->m_session)
    m_impl->m_session->run();

  setFocus( Qt::OtherFocusReason );
  m_impl->m_terminalDisplay->resize(size());

  setFocusProxy(m_impl->m_terminalDisplay);

}

void QTermWidget::startShellProgram()
{
  if ( m_impl->m_session->isRunning() ) return;

  m_impl->m_session->run();
}

bool QTermWidget::sessionIsruning()
{
    return m_impl->m_session->isRunning();
}

void QTermWidget::init()
{
  m_impl->m_terminalDisplay->setSize(80, 40);

  QFont font = QApplication::font();
  font.setFamily("Monospace");
  font.setPointSize(10);
  font.setStyleHint(QFont::TypeWriter);
  QSettings setting;
  QFont f=  setting.value("Font",font).value<QFont>();
  setTerminalFont(f);
 // setScrollBarPosition(NoScrollBar);

  m_impl->m_session->addView(m_impl->m_terminalDisplay);

  connect(m_impl->m_session, SIGNAL(finished()), this, SLOT(sessionFinished()));
  connect(m_impl->m_terminalDisplay, SIGNAL(selectionAvailable(bool)), this, SIGNAL(selectionAvailable(bool)));
 connect(m_impl->m_session, SIGNAL(titleChanged()), this, SLOT(changeTitle()));
}


QTermWidget::~QTermWidget()
{
    m_impl->m_session->close();
  emit destroyed();
}
void QTermWidget::changeTitle()
{

     if ( m_impl->m_session ){

         QString tit=m_impl->m_session->userTitle();
         m_title=tit;
          emit titleChanged(tit);
     }

}

void QTermWidget::setTerminalFont(QFont &font)
{
  if (!m_impl->m_terminalDisplay)
    return;
  m_impl->m_terminalDisplay->setVTFont(font);
}

void QTermWidget::setShellProgram(const QString &progname)
{
  if (!m_impl->m_session)
    return;
  m_impl->m_session->setProgram(progname);
}

void QTermWidget::setArgs(QStringList &args)
{
  if (!m_impl->m_session)
    return;
  m_impl->m_session->setArguments(args);
}

void QTermWidget::setTextCodec(QTextCodec *codec)
{
  if (!m_impl->m_session)
    return;
  m_impl->m_session->setCodec(codec);
}

void QTermWidget::setColorScheme(int scheme)
{
  switch(scheme)
  {
    case COLOR_SCHEME_WHITE_ON_BLACK:
      m_impl->m_terminalDisplay->setColorTable(whiteonblack_color_table);
      break;
    case COLOR_SCHEME_GREEN_ON_BLACK:
      m_impl->m_terminalDisplay->setColorTable(greenonblack_color_table);
      break;
    case COLOR_SCHEME_BLACK_ON_LIGHT_YELLOW:
      m_impl->m_terminalDisplay->setColorTable(blackonlightyellow_color_table);
      break;
    case COLOR_SCHEME_COSTUM:
      m_impl->m_terminalDisplay->setCostumColorTable();
    break;
    default: //do nothing
      break;
  };
}

void QTermWidget::setSize(int h, int v)
{
  if (!m_impl->m_terminalDisplay)
    return;
  m_impl->m_terminalDisplay->setSize(h, v);
}

void QTermWidget::setHistorySize(int lines)
{
  if (lines < 0)
    m_impl->m_session->setHistoryType(HistoryTypeFile());
  else
    m_impl->m_session->setHistoryType(HistoryTypeBuffer(lines));
}
void QTermWidget::setScrollBarPosition(int pos)
{
    switch (pos) {
    case 0:
        setScrollBarPosition(NoScrollBar);
        break;
    case 1:
         setScrollBarPosition(ScrollBarLeft);
        break;
    case 2:
         setScrollBarPosition(ScrollBarRight);
        break;
    default:
         setScrollBarPosition(NoScrollBar);
        break;
    }
}

void QTermWidget::setScrollBarPosition(ScrollBarPosition pos)
{

  if (!m_impl->m_terminalDisplay)
    return;
  m_impl->m_terminalDisplay->setScrollBarPosition((TerminalDisplay::ScrollBarPosition)pos);
}

void QTermWidget::sendText(QString &text)
{
  m_impl->m_session->sendText(text);
}

void QTermWidget::resizeEvent(QResizeEvent*)
{
  //qDebug("global window resizing...with %d %d", size().width(), size().height());
  m_impl->m_terminalDisplay->resize(size());
}

void QTermWidget::sessionFinished()
{
  emit finished();
}

void QTermWidget::setInitialWorkingDirectory(const QString& path)
{
    m_impl->m_session->setInitialWorkingDirectory(path);
}

QString QTermWidget::initialWorkingDirectory()
{
    if ( m_impl )
        return m_impl->m_session->initialWorkingDirectory();
    
    return QString("");
}

QString QTermWidget::getOutputFromCommand(const QString& cmd)
{
    if ( ! m_impl->m_session->isRunning() ) return "";
    
    receiveData(true);
    
    QString command = cmd;
    command += "\r";
    sendText(command);
    
    //while(m_dataReceived.isEmpty())
    //    qDebug() << m_dataReceived;
    
    QString data = m_dataReceived;
    m_dataReceived.clear();
    
    return data;
}

void QTermWidget::onDataReceived(const QString& text)
{
    m_dataReceived = text;
    if ( m_dataReceived.isEmpty() )
        m_dataReceived = " ";
    receiveData(false);
    
    emit receivedData(text);
}

void QTermWidget::receiveData(bool receive)
{
    if ( receive )
        connect(m_impl->m_session, SIGNAL(receivedData(const QString&)), this, SLOT(onDataReceived(const QString&)));
    else
        disconnect(m_impl->m_session, SIGNAL(receivedData(const QString&)), this, SLOT(onDataReceived(const QString&)));
}
//--------------------------
 void  QTermWidget::CopySelection()
 {
     m_impl->m_terminalDisplay->copyClipboard();
 }
//------------------------------
 void QTermWidget::clear()
 {
   //  m_impl->m_session->emulation();
     m_impl->m_session->refresh();
     m_impl->m_session->clearHistory();
 }

 void QTermWidget::setZoom(int step)
 {
     if (!m_impl->m_terminalDisplay)
         return;

     QFont font = m_impl->m_terminalDisplay->getVTFont();

     font.setPointSize(font.pointSize() + step);
     setTerminalFont(font);
 }

 void QTermWidget::zoomIn()
 {
     setZoom(1);
 }

 void QTermWidget::zoomOut()
 {
     setZoom(-1);
 }

 void QTermWidget::setTerminalOpacity(qreal level)
 {
     if (!m_impl->m_terminalDisplay)
         return;

     m_impl->m_terminalDisplay->setOpacity(level);
 }
