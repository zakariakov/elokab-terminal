#include "mainwindow.h"
#include <QApplication>
//#include <ElokabApplication>
#include <QTranslator>
#include <QDebug>
#include <QLibraryInfo>
#include <QSettings>

void helpMe()
{
    printf("Usage: elokab-terminal [OPTION]\n");
    puts("OPTION:\n");
    puts(" -h  --help                            Print this help.\n");
    puts(" -w  --working-directory  <dir>        Start session with specified work directory.\n");
    puts(" -e, --execute            <command>    Execute command instead of shel\n");
    puts(" -b, --hide-border                     FramelessWindow no border\n");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    setenv("TERM", "xterm", 1); // TODO/FIXME: why?

        a.setApplicationName("elokab-terminal");
    a.setOrganizationName("elokab");
    a.setApplicationVersion("0.1");

    /// جلب ترجمة البرنامج من مجلد البرنامج

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

    QString p= dirPath+"/translations/"+locale+"/"+a.applicationName();

    QTranslator translator;
    //TODO
    translator.load(p);
    a.installTranslator(&translator);

    QIcon icon=QIcon::fromTheme("terminal",QIcon(":/icons/terminal.png"));
    a.setWindowIcon(icon);
    //------------------------------------argument-------------------------------------------------
    QString workdir= QDir::currentPath(),command;
    bool framless=false;
    QStringList args = a.arguments();

    if(args.count()>1)
    {



        QString arg = args.at(1);
        if (arg == "-h" || arg == "--help" ) {helpMe();return 0; }
        else if (arg == "-w" || arg == "--working-directory" )  {workdir=args.at(2);}
        else if (arg == "-e" ||arg == "-x" || arg == "--execute"|| arg == "--command" ) {command=args.at(2);}
        else if (arg == "-b" || arg == "--hide-border" ) {framless=true;}
        else {
            QDir dir(arg);
            if(dir.exists())
                workdir=arg;
            else
                 qWarning() << "echo Unknown option: " << args;  helpMe(); return 0;
               // command= "echo \"Unknown option: " + arg+"\n"+ help+"\"";
           }


    }
         //   qWarning() << "echo Unknown option: " << args;  qDebug()<<help; return 0;}
    qDebug()<<"=======main========="<<workdir<<command;

    MainWindow w(workdir,command,framless);

    w.show();

    return a.exec();
}
