#include <iostream>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <memory>
#include "app.h"
#include "appinfo.h"

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
    
    /* parsing command line arguments */
    po::variables_map vm;
    po::options_description desc("Skandal Command Line Options");
    setupCmdParser(argc, argv, vm, desc);
    
    if (argc == 1 || vm.count("gui")) {
        _gui = true;
        initGUI();
    }
    
    if (vm.count("help")) {
        cout << desc << endl;
        std::exit(EXIT_SUCCESS);
    }
    
    if (vm.count("version")) {
        printVersionMessage();
        std::exit(EXIT_SUCCESS);
    }
    
    if (vm.count("version-triplet")) {
        printVersionTripletMessage();
        std::exit(EXIT_SUCCESS);
    }
    
    if (vm.count("appid")) {
        printApplicationIdentifier();
        std::exit(EXIT_SUCCESS);
    }
    
    if (vm.count("dataset")) {
        DataSet ds(vm["dataset"].as<string>());
        VoxelCarving vc(ds, vm["voxeldim"].as<int>());
        vc.exportAsPly(vm["output"].as<string>());
    }
    
    if (vm.count("prefset")) {
        size_t eqidx = vm["prefset"].as<string>().find('=');
        if (eqidx != string::npos) {
            string key = vm["prefset"].as<string>().substr(0,eqidx);
            string val = vm["prefset"].as<string>().substr(eqidx+1);
            setPreference(key, val);
        } else {
            unsetPreference(vm["prefset"].as<string>());
        }
    }
    
    if (vm.count("prefdel")) {
        unsetPreference(vm["prefdel"].as<string>());
    }
    
    if (vm.count("prefget")) {
        printPreference(vm["prefget"].as<string>());
    }
    
    if (vm.count("preflist")) {
        printAllPreferences();
        std::exit(EXIT_SUCCESS);
    }
   
    /* if no gui is running, we're finished now */
    if (!_gui) {
        std::exit(EXIT_SUCCESS);
    }
}

App::~App() {
}

App* App::INSTANCE() {
    return _instance;
}

void App::setupCmdParser(int argc, char *argv[], po::variables_map &vm, po::options_description &desc) {
    
    desc.add_options()
    ("help,h",          "Display this help message")
    ("version,v",       "Display the version number")
    ("version-triplet", "Display the undecorated program version")
    ("appid",           "Display the unique application identifier")
    ("dataset,d",       po::value<string>(), "Reconstruct 3d model with given dataset path")
    ("voxeldim",        po::value<int>()->default_value(32), "Set the voxelgrid dimension (value must be power of two)")
    ("output,o",        po::value<string>()->default_value("export.ply"), "Set the output file name of the 3D reconstruction")
    ("prefset",         po::value<string>(), "Set the given preference")
    ("prefdel",         po::value<string>(), "Unset the given preference")
    ("prefget",         po::value<string>(), "Display the given preference")
    ("preflist",        "List all preferences that are set")
    ("gui",             "Run in graphical user interface mode");
    
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);
    } catch (po::error &e) {
        cerr << e.what() << endl;
        cerr << desc << endl;
        std::exit(EXIT_FAILURE);
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
