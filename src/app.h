#ifndef HEADER_SRC_APP_H_INCLUDED
#define HEADER_SRC_APP_H_INCLUDED

#ifndef Q_MOC_RUN // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <boost/program_options.hpp>
#endif

#include <boost/shared_ptr.hpp>
#include <QtCore>
#include <QtGui>

#include "reconstruction/dataset.h"
#include "reconstruction/voxelcarving.h"

using namespace std;
namespace po = boost::program_options;

class App : public QApplication {
    
    Q_OBJECT
public:
    App(int argc, char* argv[]);
    ~App();
    
    static App* INSTANCE();
    
    QString getProjectName();
    QString getProjectCodeName();
    QString getProjectVendorID();
    QString getProjectVendorName();
    QString getProjectID();
    int getProjectMajorVersion();
    int getProjectMinorVersion();
    int getProjectPatchVersion();
    QString getProjectVersion();
    QString getProjectCopyrightYears();
    QString getProjectInvocation();
    bool inVerboseMode();
    bool inVerboseAsyncMode();
    
private:
    void initGUI();
    void setupCmdParser(int argc, char *argv[], po::variables_map &vm, po::options_description &desc);
    void printVersionMessage();
    void printVersionTripletMessage();
    void printApplicationIdentifier();
    void setPreference(const std::string& key, const std::string& val);
    void unsetPreference(const std::string& key);
    void printPreference(const std::string& key)const;
    void printAllPreferences() const;
    std::string getKeyName(const std::string& key)const;
    std::string getKeyRepr(const std::string& key)const;
    std::string convert(const QString& str)const;
    QString convert(const std::string& str)const;
    
    static App* _instance;
    QString _invocation;
    bool _gui;
    bool _verbose;
    bool _verboseAsync;
    boost::shared_ptr<QMainWindow> _mainwindow;
};

#endif
