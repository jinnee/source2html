#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QVector>
#include <QTimer>
#include <QDirIterator>
#include <QDebug>
#include <memory>

#include"tools.h"
#include"templates.h"

static QString destinationDir;
static QString sourceDir;
static QCommandLineParser parser;

void processFiles(const QString path, QString& stream, const std::unique_ptr<Templates> &templates, const QStringList extensionsOptionList) {

    QString tmpPath(path);
    QString tmpStr = destinationDir + tmpPath.replace(sourceDir, "");
    QDir newDestDir(tmpStr);

    if (!newDestDir.mkpath(tmpStr)) {
        qDebug() << tmpStr;
        qDebug() << "Error by creating destination directory!";
    }

    QDirIterator itDirs(path, QDir::Dirs);

    while (itDirs.hasNext()) {
        itDirs.next();

        if (itDirs.fileName() == "." || itDirs.fileName() == "..")
          continue;

        QFileInfo info = itDirs.fileInfo();
        processFiles(path + "/" + info.fileName(), stream, templates, extensionsOptionList);
    }

    QDirIterator itFiles(path, QDir::Files);
    while (itFiles.hasNext()) {
        itFiles.next();

        QFileInfo info = itFiles.fileInfo();
        // process only files described in -e option
        if (!extensionsOptionList.contains(info.suffix())) {
            continue;
        }

        stream += "treeArray.push('/" + path + "/" + info.fileName() + "');";
        templates->htmlExport(info.absoluteFilePath(), destinationDir, sourceDir);
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("source2html");
    QCoreApplication::setApplicationVersion("0.1");
    QStringList paths;

    parser.setApplicationDescription("Convert source files to html files");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption sourceDirectoryOption(QStringList() << "s" << "source",
               QCoreApplication::translate("main", "Copy all source files into destination directory."),
               QCoreApplication::translate("main", "directory"));

    QCommandLineOption destinationDirectoryOption(QStringList() << "d" << "destination",
               QCoreApplication::translate("main", "Target directory that will contain the converted output files from the source directory."),
               QCoreApplication::translate("main", "directory"));

    QCommandLineOption extensionsOption(QStringList() << "e" << "extensions",
               QCoreApplication::translate("main", "Which files can be exported to html. Default value \"cpp;c;h\""),
                                        QCoreApplication::translate("main", "directory"));

    QCommandLineOption jsCssUrlOption(QStringList() << "u" << "url",
               QCoreApplication::translate("main", "Fixed location for .js and .css files."),
                                        QCoreApplication::translate("main", "directory"));


    extensionsOption.setDefaultValue("cpp;c;h");
    jsCssUrlOption.setDefaultValue("");


    parser.addOption(sourceDirectoryOption);
    parser.addOption(destinationDirectoryOption);
    parser.addOption(extensionsOption);
    parser.addOption(jsCssUrlOption);

    parser.process(a);


    if (!parser.isSet("source")) {
        qDebug() << "Please, specify the source directory!\n";
        parser.showHelp(0);
    }

    if (!parser.isSet("destination")) {
        qDebug() << "Please, specify the destination directory!\n";
        parser.showHelp(0);
    }

    sourceDir = parser.value(sourceDirectoryOption);
    destinationDir = parser.value(destinationDirectoryOption);
    QString jsCssUrl = parser.value(jsCssUrlOption);

    QDir sdir(sourceDir);
    QDir ddir(destinationDir);

    if (sourceDir == destinationDir) {
        qDebug() << "The source directory " << sourceDir << " and destionation directory must be different\n";
        parser.showHelp(0);
    }

    if (!sdir.exists()) {
        qDebug() << "The source directory " << sourceDir << " not exist!\n";
        parser.showHelp(0);
    }

    if (!ddir.exists()) {
        qDebug() << "The destionation directory " << destinationDir << " not exist!\n";

        if(!ddir.mkpath(destinationDir)) {
            qDebug() << "Error by creating destination directory!";
            parser.showHelp(0);
        } else {
            qDebug() << "Creating destination directory!";
        }

    }

    QDirIterator it(sourceDir, QDirIterator::Subdirectories);

    if (jsCssUrl.isEmpty()) {
        if(!Tools::copyFile("prism.css", ddir.path(), sdir.path())) {
            qDebug() << "Cannot find prism.css!";
            qDebug() << sdir.path();
            qDebug() << ddir.path();
        }
        if(!Tools::copyFile("prism.js", ddir.path(), sdir.path())) {
            qDebug() << "Cannot find prism.js!";
        }
    }

    std::unique_ptr<Templates> templates = std::make_unique<Templates>();

    if(!templates) {
        parser.showHelp(0);
    }

    templates->setJsCssUrl(jsCssUrl);

    if(!templates->loadTemplate(FILE_TEMPLATE_TYPE)){
        qDebug() << "Cannot load file.html template!";
    }

    if(!templates->loadTemplate(INDEX_TEMPLATE_TYPE)){
        qDebug() << "Cannot load index.html template!";
    }


    QString str;
    processFiles(sdir.path(), str, templates, parser.value(extensionsOption).split(";"));
    std::string textStd = str.toStdString();
    Tools::replaceAll(textStd, sdir.path().toStdString(), "Fifteen");
    QString res = QString::fromUtf8(textStd.c_str());

    templates->indexExport(ddir.path() + QDir::separator(), res);

    qDebug() << "The conversion is ended!";

    QTimer::singleShot(0,[](){
         QCoreApplication::exit(0);
    });

    return a.exec();
}
