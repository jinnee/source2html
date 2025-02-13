#ifndef TOOLS_H
#define TOOLS_H

#include <QString>

namespace Tools {
    bool copyFile(QString file, QString destinationDir, QString sourceDir);
    void replaceAll(std::string& str, const std::string& from, const std::string& to);
}

#endif // TOOLS_H
