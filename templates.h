#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <QString>
#include "constants.h"

class Templates
{
public:
    Templates();
    bool indexExport(QString destinationDir, QString treeArrayData);
    bool loadTemplate(TEMPLATE_FILES_TYPES file);
    bool htmlExport(QString file, QString destinationDir, QString sourceDir);
    void setJsCssUrl(QString url);
private:
    QString indexTemplate;
    QString fileTemplate;
    QString jsCssUrl;

    bool hasChildId();
};

#endif // TEMPLATES_H
