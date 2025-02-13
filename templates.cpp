#include "templates.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "tools.h"

Templates::Templates()
{
    indexTemplate = "";
    fileTemplate = "";
    jsCssUrl = "";
}

bool Templates::loadTemplate(TEMPLATE_FILES_TYPES file_type) {
    QString file = INDEX_TEMPLATE_NAME;

    if (file_type == FILE_TEMPLATE_TYPE) {
        file = FILE_TEMPLATE_NAME;
    }

    QFile ifile(file);
    if(!ifile.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }

    QTextStream in(&ifile);

    if (file_type == FILE_TEMPLATE_TYPE) {
        fileTemplate = in.readAll();
    } else if (file_type == INDEX_TEMPLATE_TYPE) {
        indexTemplate = in.readAll();
    }

    ifile.close();

    return true;

}

bool Templates::indexExport(QString destinationDir, QString treeArrayData) {
    QString html = indexTemplate.replace("!treeArray!", treeArrayData);

    QFile ofile(destinationDir + "index.html");

    if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&ofile);
    out << html;
    ofile.close();

    return true;
}

bool Templates::htmlExport(QString file, QString destinationDir, QString sourceDir) {
    QFile ifile(file);
    QFile sdir(file);
    QFileInfo fileInfo(sdir.fileName());

    if (!ifile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString mStr(file);

    QString t = mStr.replace(sourceDir,"").replace(fileInfo.fileName(), "");

    QString path_to_jscss = "";

    if (jsCssUrl.isEmpty()) {
        int cntd = t.count("/") - 1;

        for (int i = 0; i < cntd; i++ ) {
            path_to_jscss += "../";
        }
    } else {
        path_to_jscss = jsCssUrl;
    }


    QTextStream in(&ifile);
    QString ftemp(fileTemplate);
    QString codeText = in.readAll();
    std::string codeTextStd = codeText.toStdString();

    Tools::replaceAll(codeTextStd, "<", "&lt;");
    Tools::replaceAll(codeTextStd, ">", "&gt;");
    codeText = QString::fromUtf8(codeTextStd.c_str());;

    QString html = ftemp.replace("!code!", codeText)
            .replace("!title!",fileInfo.fileName())
            .replace("!css!",path_to_jscss+"prism.css")
            .replace("!js!",path_to_jscss+"prism.js");

    QDir ddir(destinationDir + "/" + file.replace(sourceDir,""));
    QFile fdir(ddir.path());

    QFile ofile(fdir.fileName() + ".html");
//    qDebug() << "ofile: " << fdir.fileName();

    if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&ofile);
    out << html;

    ifile.close();
    ofile.close();

    return true;
}

void Templates::setJsCssUrl(QString url) {
    this->jsCssUrl = url;
}
