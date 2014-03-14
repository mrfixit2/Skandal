#include <iostream>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <memory>
#include "app.h"
#include "appinfo.h"

namespace {
    bool matches_option(const QString& givenoption, const QString& expectedoption, int mindashes=1, int maxdashes=2) {
        int dashes = 0;
        if (givenoption.length() > 0) {
            while ((dashes<givenoption.length()) && (givenoption[dashes]=='-')){
                dashes++;
            }
        }
        if ((dashes < mindashes) || (dashes > maxdashes)) {
            return false;
        }
        
        QString substr=givenoption.right(givenoption.length()-dashes);
        return (expectedoption.compare(substr,Qt::CaseInsensitive) == 0);
    }
}

inline ostream& operator<<(ostream& out, const QString& str) {
    QByteArray a = str.toUtf8();
    out << a.constData();
    return out;
}

App::App(int argc, char* argv[]) : QApplication(argc,argv), _invocation(argv[0]), _gui(false) {
    
    /* enforce singleton property */
    if (_instance) {
        throw runtime_error("Only one instance of App allowed.");
    }
    
    /* set the singleton instance to this */
    _instance = this;
    
    /* set the application properties */
    setApplicationName(APPLICATION_NAME);
    setApplicationVersion(APPLICATION_VERSION_STRING);
    setOrganizationName(APPLICATION_VENDOR_NAME);
    setOrganizationDomain(APPLICATION_VENDOR_URL);
    
    /* remember if we are done */
    parseCommandline(argc, argv);
    
    if (_gui || argc == 1) {
        initGUI();
    } else {
        std::exit(EXIT_SUCCESS);
    }
}

App::~App() {
}

App* App::INSTANCE() {
    return _instance;
}

void App::parseCommandline(int argc, char* argv[]) {
    
    int idx = 1;
    while (idx < argc) {
        QString arg(argv[idx]);
        if (matches_option(arg, "help", 2) || matches_option(arg, "h") || matches_option(arg, "?", 0)) {
            printHelpMessage();
            std::exit(0);
        } else if (matches_option(arg, "version", 2) || matches_option(arg, "v")) {
            printVersionMessage();
            std::exit(EXIT_SUCCESS);
        } else if (matches_option(arg, "version-triplet")) {
            printVersionTripletMessage();
            std::exit(EXIT_SUCCESS);
        } else if (matches_option(arg, "dataset", 2) || matches_option(arg, "d")) {
            /* verify that there is another argument */
            if ((idx+1) >= argc) {
                std::exit(EXIT_FAILURE);
            }
            
            idx++;
            DataSet ds(argv[idx]);
            VoxelCarving vc(ds);
        } else if (matches_option(arg,"prefset")) {
            /* verify that there is another argument */
            if ((idx+1) >= argc) {
                std::exit(EXIT_FAILURE);
            }
            
            idx++;
            string param(argv[idx]);
            /* determine if there is an equals sign. If there is, set the preference.
             Otherwise, remove the preference */
            size_t eqidx = param.find('=');
            if (eqidx != string::npos){
                string key = param.substr(0,eqidx);
                string val = param.substr(eqidx+1);
                setPreference(key,val);
            } else {
                unsetPreference(param);
            }
        } else if (matches_option(arg,"prefdel")){
            /* verify that there is another argument */
            if ( (idx+1) >= argc ){
                std::exit(EXIT_FAILURE);
            }
            
            idx++;
            string param(argv[idx]);
            /* remove the preference */
            unsetPreference(param);
        } else if (matches_option(arg,"preflist")) {
            printAllPreferences();
        } else if (matches_option(arg,"prefget")) {
            /* verify that there is another argument */
            if ( (idx+1) >= argc ){
                std::exit(EXIT_FAILURE);
            }
            
            idx++;
            string param(argv[idx]);
            /* print the preference */
            printPreference(param);
        } else if ( matches_option(arg,"appid") || matches_option(arg,"application-identifier") ){
            printApplicationIdentifier();
            std::exit(EXIT_SUCCESS);
        } else if ( matches_option(arg,"gui") ) {
            _gui = true;
        }
        idx++;
    }
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
    auto_ptr<QLabel> label(new QLabel("Hello world!"));
    auto_ptr<QGridLayout> layout(new QGridLayout);
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

void App::printHelpMessage() {
    
    cout << "Usage: " << getProjectInvocation() << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "    --help                       Displays this help message." << endl;
    cout << "    --version                    Prints the program version." << endl;
    cout << "    --version-triplet            Prints the undecorated program version." << endl;
    cout << "    --appid                      Prints the unique application identifier." << endl;
    cout << "    --dataset                    Reconstructs 3d object with given dataset." << endl;
    cout << "    --prefset <key>=<val>        Sets the given preference." << endl;
    cout << "    --prefdel <key>              Unsets the given preference." << endl;
    cout << "    --prefget <key>              Prints the given preference." << endl;
    cout << "    --preflist                   Lists all preferences that are set." << endl;
    cout << "    --gui                        Run in graphical user interface mode." << endl;
}

void App::printVersionMessage() {
    
    cout << getProjectName() << " v" << getProjectVersion() << endl;
    cout << getProjectVendorName() << "; Copyright (C) " << getProjectCopyrightYears() << endl;
}

void App::printVersionTripletMessage() {
    cout << getProjectVersion() << endl;
}

void App::printApplicationIdentifier() {
    cout << getProjectID() << endl;
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

string App::getKeyName(const string& key) const {
    
    string result(key);
    for ( size_t i = 0; i < result.size(); i++ ){
        if ( (result[i]=='/') || (result[i]=='\\') ){
            result[i] = '.';
        }
    }
    return result;
}

string App::getKeyRepr(const string& key) const {
    string result(key);
    for ( size_t i = 0; i < result.size(); i++ ){
        if ( (result[i]=='/') || (result[i]=='\\') ){
            result[i] = '/';
        }
    }
    return result;
}

void App::setPreference(const string& key, const string& val) {
    QSettings settings;
    string keyrep(getKeyRepr(key));
    QString qkeyrep(keyrep.c_str());
    QString qval(val.c_str());
    settings.setValue(qkeyrep,qval);
    settings.sync();
}

void App::unsetPreference(const string& key) {
    QSettings settings;
    string keyrep(getKeyRepr(key));
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

void App::printPreference(const string& key) const {
    QSettings settings;
    string keyrep(getKeyRepr(key));
    QString qkeyrep(keyrep.c_str());
    QString result="undefined";
    if ( settings.contains(qkeyrep) ){
        result=settings.value(qkeyrep,QString("undefined")).toString();
    }
    cout << result << endl;
}

void App::printAllPreferences() const {
    QSettings settings;
    QStringList keys = settings.allKeys();
    for ( QStringList::const_iterator it = keys.begin(); it != keys.end(); ++it ){
        QString qkeystr = *it;
        QString qvalstr = settings.value(qkeystr).toString();
        
        if ( ! qvalstr.isEmpty() ){
            string key=getKeyName(convert(qkeystr));
            cout << key << "=" << qvalstr << endl;
        }
    }
}

string App::convert(const QString& str) const {
    QByteArray data = str.toUtf8();
    string result(data.constData());
    return result;
}

QString App::convert(const string& str) const {
    QString result(str.c_str());
    return result;
}

App* App::_instance = 0;
