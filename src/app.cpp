#include <iostream>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <memory>
#include "app.h"
#include "appinfo.h"

namespace {
    bool matches_option(const QString& givenoption, const QString& expectedoption, int mindashes=1, int maxdashes=2){
        int dashes = 0;
        if ( givenoption.length() > 0 ) {
            while ((dashes<givenoption.length())&&(givenoption[dashes]=='-')){
                dashes++;
            }
        }
        if ( (dashes < mindashes) || (dashes > maxdashes) ) {
            return false;
        }
        QString substr=givenoption.right(givenoption.length()-dashes);
        return (expectedoption.compare(substr,Qt::CaseInsensitive)==0);
    }
}

inline std::ostream& operator<<(std::ostream& out, const QString& str) {
    QByteArray a = str.toUtf8();
    out << a.constData();
    return out;
}

App::App(int& argc, char** argv) : QApplication(argc,argv), _invocation(argv[0]), _console(false), _gui(false), _interactive(false) {
    
    /* enforce singleton property */
    if ( _instance ){
        throw std::runtime_error("Only one instance of App allowed.");
    }
    
    /* remember if we are done */
    bool done = false;
    
    /* set the singleton instance to this */
    _instance = this;
    
    /* set the application properties */
    setApplicationName(APPLICATION_NAME);
    setApplicationVersion(APPLICATION_VERSION_STRING);
    setOrganizationName(APPLICATION_VENDOR_NAME);
    setOrganizationDomain(APPLICATION_VENDOR_URL);
    
    /* parse the commandline */
    int idx = 1;
    while ( idx < argc ) {
        QString arg(argv[idx]);
        if ( matches_option(arg,"help",0) || matches_option(arg,"h") || matches_option(arg,"?",0) ){
            printHelpMessage();
            std::exit(0);
        } else if ( matches_option(arg,"version",0) ) {
            printVersionMessage();
            std::exit(0);
        } else if ( matches_option(arg,"version-triplet") ) {
            printVersionTripletMessage();
            std::exit(0);
        } else if ( matches_option(arg,"prefset") ) {
            /* verify that there is another argument */
            if ( (idx+1) >= argc ){
                std::exit(1);
            }
            
            /* increment the index */
            idx++;
            
            /* get the next parameter */
            std::string param(argv[idx]);
            
            /* determine if there is an equals sign. If there is, set the preference;
             Otherwise, remove the preference */
            size_t eqidx = param.find('=');
            if ( eqidx != std::string::npos ){
                std::string key = param.substr(0,eqidx);
                std::string val = param.substr(eqidx+1);
                setPreference(key,val);
            } else {
                unsetPreference(param);
            }
            done = true;
        } else if ( matches_option(arg,"prefdel") ){
            /* verify that there is another argument */
            if ( (idx+1) >= argc ){
                std::exit(1);
            }
            
            /* increment the index */
            idx++;
            
            /* get the next parameter */
            std::string param(argv[idx]);
            
            /* remove the preference */
            unsetPreference(param);
            done = true;
        } else if ( matches_option(arg,"preflist") ) {
            printAllPreferences();
            done = true;
        } else if ( matches_option(arg,"prefget") ) {
            /* verify that there is another argument */
            if ( (idx+1) >= argc ){
                std::exit(1);
            }
            
            /* increment the index */
            idx++;
            
            /* get the next parameter */
            std::string param(argv[idx]);
            
            /* print the preference */
            printPreference(param);
            done = true;
        } else if ( matches_option(arg,"loglevel") ) {
            /* verify that there is another argument */
            if ( (idx+1) >= argc ){
                std::exit(1);
            }
            
            /* increment the index */
            idx++;
            
            /* get the next parameter */
            std::string param(argv[idx]);
            
            /* determine if there is an equals sign and act accordingly */
            size_t eqidx = param.find('=');
            if ( eqidx != std::string::npos ) {
                std::string logger = param.substr(0,eqidx);
                std::string level  = param.substr(eqidx+1);
            }
        } else if ( matches_option(arg,"appid") || matches_option(arg,"application-identifier") ){
            printApplicationIdentifier();
            std::exit(0);
        } else if ( matches_option(arg,"gui") ) {
            if ( _interactive ){
                std::exit(1);
            }
            if ( _gui ){
            }
            _gui = true;
        } else if ( matches_option(arg,"interactive") ) {
            if ( _gui ){
                std::exit(1);
            }
            if ( _interactive ){
            }
            _interactive = true;
        }
        idx++;
    }
    
    if ( done ) {
        std::exit(0);
    }
    
    if (_console) {
        consoleMain();
        std::exit(EXIT_SUCCESS);
    } else if (_interactive) {
        interactiveMain();
        std::exit(EXIT_SUCCESS);
    } else {
        initGUI();
    }
}

App::~App() {
}

App* App::INSTANCE() {
    return _instance;
}

void App::initGUI() {
    
    /* construct the main window */
    _mainwindow.reset(new QMainWindow);
    _mainwindow->setCentralWidget(new QWidget);
    
    /* setup the central widget */
    QWidget* centralwidget = _mainwindow->centralWidget();
    QDesktopWidget* desktopwidget = desktop();
    int preferredwidth = 800;
    int preferredheight = 600;
    int leftmargin = (desktopwidget->width()-preferredwidth)/2;
    int topmargin  = (desktopwidget->height()-preferredheight)/2;
    centralwidget->setWindowTitle(getProjectName());
    centralwidget->setFixedSize(preferredwidth,preferredheight);
    std::auto_ptr<QLabel> label(new QLabel("Hello world!"));
    std::auto_ptr<QGridLayout> layout(new QGridLayout);
    layout->addWidget(label.release(),0,0,Qt::AlignCenter);
    centralwidget->setLayout(layout.release());
    
    /* setup the toolbars */
    // ...
    
    
    /* setup the icons */
    // ...
    
    /* display the main window */
    _mainwindow->setVisible(true);
    _mainwindow->move(leftmargin,topmargin);
}

void App::interactiveMain() {
    std::cout << "Hello, " << getProjectName().toStdString() << "!" << std::endl;
}

void App::consoleMain() {
    std::cout << "Hello, " << getProjectName().toStdString() << "!" << std::endl;
}

void App::printHelpMessage() {
    
    std::cout << "Usage: " << getProjectInvocation() << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    --help                       Displays this help message." << std::endl;
    std::cout << "    --version                    Prints the program version." << std::endl;
    std::cout << "    --version-triplet            Prints the undecorated program version." << std::endl;
    std::cout << "    --appid                      Prints the unique application identifier." << std::endl;
    std::cout << "    --prefset <key>=<val>        Sets the given preference." << std::endl;
    std::cout << "    --prefdel <key>              Unsets the given preference." << std::endl;
    std::cout << "    --prefget <key>              Prints the given preference." << std::endl;
    std::cout << "    --preflist                   Lists all preferences that are set." << std::endl;
    std::cout << "    --loglevel <level>           Sets the current logging level." << std::endl;
    std::cout << "    --loglevel <logger>=<level>  Sets the logging level for the given logger." << std::endl;
    std::cout << "    --gui                        Run in graphical user interface mode." << std::endl;
    std::cout << "    --interactive                Run in interactive commandline mode." << std::endl;
}

void App::printVersionMessage() {
    
    std::cout << getProjectName() << " v" << getProjectVersion() << std::endl;
    std::cout << getProjectVendorName() << "; Copyright (C) " << getProjectCopyrightYears();
}

void App::printVersionTripletMessage() {
    std::cout << getProjectVersion() << std::endl;
}

void App::printApplicationIdentifier() {
    std::cout << getProjectID() << std::endl;
}

QString App::getProjectName() {
    return APPLICATION_NAME;
}


QString App::getProjectCodeName() {
    return APPLICATION_CODENAME;
}

QString App::getProjectVendorID() {
    return APPLICATION_VENDOR_ID;
}

QString App::getProjectVendorName() {
    return APPLICATION_VENDOR_NAME;
}

QString App::getProjectID() {
    return APPLICATION_ID;
}

int App::getProjectMajorVersion() {
    return APPLICATION_VERSION_MAJOR;
}

int App::getProjectMinorVersion() {
    return APPLICATION_VERSION_MINOR;
}

int App::getProjectPatchVersion() {
    return APPLICATION_VERSION_PATCH;
}

QString App::getProjectVersion() {
    return APPLICATION_VERSION_STRING;
}

QString App::getProjectCopyrightYears() {
    return APPLICATION_COPYRIGHT_YEARS;
}

QString App::getProjectInvocation() {
    return _invocation;
}

std::string App::getKeyName(const std::string& key) const {
    
    std::string result(key);
    for ( size_t i = 0; i < result.size(); i++ ){
        if ( (result[i]=='/') || (result[i]=='\\') ){
            result[i] = '.';
        }
    }
    return result;
}

std::string App::getKeyRepr(const std::string& key) const {
    std::string result(key);
    for ( size_t i = 0; i < result.size(); i++ ){
        if ( (result[i]=='/') || (result[i]=='\\') ){
            result[i] = '/';
        }
    }
    return result;
}

void App::setPreference(const std::string& key, const std::string& val) {
    QSettings settings;
    std::string keyrep(getKeyRepr(key));
    QString qkeyrep(keyrep.c_str());
    QString qval(val.c_str());
    settings.setValue(qkeyrep,qval);
    settings.sync();
}

void App::unsetPreference(const std::string& key) {
    QSettings settings;
    std::string keyrep(getKeyRepr(key));
    QString qkeyrep(keyrep.c_str());
    settings.beginGroup(qkeyrep);
    if ( (settings.childGroups().length()!=0) || (settings.childKeys().length()!=0) ){
        settings.setValue("","");
    }else{
        settings.remove("");
    }
    settings.endGroup();
    settings.sync();
}

void App::printPreference(const std::string& key) const {
    QSettings settings;
    std::string keyrep(getKeyRepr(key));
    QString qkeyrep(keyrep.c_str());
    QString result="undefined";
    if ( settings.contains(qkeyrep) ){
        result=settings.value(qkeyrep,QString("undefined")).toString();
    }
    std::cout << result << std::endl;
}

void App::printAllPreferences() const {
    QSettings settings;
    QStringList keys = settings.allKeys();
    for ( QStringList::const_iterator it = keys.begin(); it != keys.end(); ++it ){
        QString qkeystr = *it;
        QString qvalstr = settings.value(qkeystr).toString();
        
        if ( ! qvalstr.isEmpty() ){
            std::string key=getKeyName(convert(qkeystr));
            std::cout << key << "=" << qvalstr << std::endl;
        }
    }
}

std::string App::convert(const QString& str) const {
    QByteArray data = str.toUtf8();
    std::string result(data.constData());
    return result;
}

QString App::convert(const std::string& str) const {
    QString result(str.c_str());
    return result;
}

App* App::_instance = 0;
