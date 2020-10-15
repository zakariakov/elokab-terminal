﻿#include "mainwindow.h"
#include <QApplication>
//#include <ElokabApplication>
#include <QTranslator>
#include <QDebug>
#include <QLibraryInfo>
#include <QSettings>
#include <QDateTime>

void helpMe()
{

    printf("Usage: elokab-terminal [OPTION][Execution]\n");
    puts("elokab-terminal v: 0.3 \n" );
    puts("OPTION:\n");
    puts(" -h  --help                                             Print this help.\n");
    puts(" -w  --working-directory  <dir>                         Start session with specified work directory.\n");
    puts(" -b, --hide-border                                      FramelessWindow no border\n");
    puts(" -t, --on-top                                           On top hint\n");
    puts(" -g, --geometry           <left,top,width,height>       Run in specific dimensions ex: 0,0,800,600 \n");
    puts(" -o, --opacity            <int>                        Window opacity 0 to 100 \n");
    puts(" -n, --name               <int>                        change  instance name \n");
    puts(" -r, --reload                                          reload configuration and exit \n");
    puts(" -f, --fullscreen                                      show fullscreen \n");
    puts("Execution:\n");
    puts(" -e, --execute            <command>                     Execute command instead of shel\n");

}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    setenv("TERM", "xterm", 1); // TODO/FIXME: why?
    //  setenv("TERM", "rxvt", 1); // TODO/FIXME: why?

    a.setApplicationName("elokab-terminal");
    a.setOrganizationName("elokab");
    a.setApplicationDisplayName("elokab-terminal");
    a.setApplicationVersion("0.3");

    //------------------------------------argument-------------------------------------------------
    QString workdir,command,geometry;
    bool framless=false;
    bool ontop=false;
    bool fullscreen=false;
    QStringList args = a.arguments();
    int opacity=-1;
    if(args.count()>1)
    {

        for (int i = 0; i < args.count(); ++i) {

            QString arg = args.at(i);
            if (arg == "-h" || arg == "--help" ) {helpMe();return 0; }

            else if (arg == "-r" || arg == "--reload" ) {
                QDateTime dateTime(QDate::currentDate(),QTime::currentTime());
                QString date=dateTime.toString("dd.MM.yyyy/hh:mm:ss");
                QSettings s("elokab","terminal");
                s.setValue("Reload",date);
                puts("elokab-terminal v: 0.3 reloaded\n" );
                return 0;
            }
            else if (arg == "-w" || arg == "--working-directory" )  {

                if(i+1>args.count()-1){helpMe();return 0;}
                QDir dir(args.at(i+1));
                if(dir.exists()) workdir=args.at(i+1);

            }

            else if (arg == "-n" || arg == "--name" ) {
                if(i+1>args.count()-1){helpMe();return 0;}
                QString name=args.at(i+1);
                if(name.isEmpty()) break;
                a.setApplicationName(name);
            }

            else if (arg == "-e" ||arg == "-x" || arg == "--execute"|| arg == "--command" ) {
                if(i+1>args.count()-1){helpMe();return 0;}
                for (int r = i+1; r < args.count(); ++r) {
                    command+=" "+args.at(r)+" ";
                }
                // command= command.replace("\"","'");
                // command=args.at(i+1);
                break;
            }

            else if (arg == "-b" || arg == "--hide-border" ) {framless=true;}

            else if (arg == "-g" || arg == "--geometry" ) {
                if(i+1>args.count()-1){helpMe();return 0;}
                geometry=args.at(i+1);
            }

            else if (arg == "-t" || arg == "--on-top" ) {ontop=true;}

            else if (arg == "-f" || arg == "--fullscreen" ) {fullscreen=true;}

            else if (arg == "-o" || arg == "--opacity" ) {
                if(i+1>args.count()-1){helpMe();return 0;}
                opacity=args.at(i+1).toInt();

            }




            // qWarning() << "echo Unknown option: " << args;  helpMe(); return 0;
            // command= "echo \"Unknown option: " + arg+"\n"+ help+"\"";


        }

        if(workdir.isEmpty())
        {

            QDir dir(args.at(1));
            if(dir.exists())
                workdir=args.at(1);
        }



    }
    //-------------------------------------------------------------------------------
    /// جلب اعدادات اللغة
    QSettings globalSetting(a.organizationName(),"elokabsettings");
    globalSetting.beginGroup("Language");
    QString locale=globalSetting.value("Name","Default").toString();
    globalSetting.endGroup();

    /// اذاكانت اللغة الافتراضية
    if(locale=="Default")
        locale = QLocale::system().name().section("_",0,0);
    /// اللغة الحالية لجميع البرنامج
    QLocale::setDefault(QLocale(locale));
    /// جلب ترجمات كيوتي
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += locale;
    QTranslator *translatorsys = new QTranslator;
    if (translatorsys->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(translatorsys);
    QDir appDir(a.applicationDirPath());
    appDir.cdUp();
    QString dirPath=  appDir.absolutePath()+"/share/"+a.organizationName();

    QString p= dirPath+"/translations/"+locale+"/"+a.applicationDisplayName();

    QTranslator translator;
    //TODO
    translator.load(p);
    a.installTranslator(&translator);
    QLocale lx=QLocale(locale);

    a.setLayoutDirection(lx.textDirection());
    QIcon icon=QIcon::fromTheme("terminal",QIcon(":/icons/terminal.png"));
    a.setWindowIcon(icon);

    if(workdir.isEmpty())
    { workdir=QDir::currentPath();}

    qWarning() <<"=======Main========="
              <<"\n Workdir: "<<workdir
             <<"\n Command: "<<command
            <<"\n Framless:"<<framless
           <<"\n Geometry:"<<geometry
          <<"\n Opacity:"<<opacity
         <<"\n Name:"<<a.applicationName()
        <<"\n DisplayName:"<<a.applicationDisplayName()
    <<"\n====================";
    MainWindow w(workdir,command,framless,geometry,ontop,opacity);

    if(fullscreen){ w.showFullScreen();}

    else{ w.show();}

    return a.exec();
}
