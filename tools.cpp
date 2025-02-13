#include "tools.h"
#include <QFile>
#include <QDir>

namespace Tools {
    bool copyFile(QString file, QString destinationDir, QString sourceDir) {
        bool result = true;

        QFile sdir(file);
        QDir ddir(destinationDir + "/" + file.replace(sourceDir,""));
        QFile fdir(ddir.path());

        // https://stackoverflow.com/questions/14935919/copy-file-even-when-destination-exists-in-qt
        if (QFile::exists(fdir.fileName()))
        {
            QFile::remove(fdir.fileName());
        }

        if(!QFile::copy(sdir.fileName(), fdir.fileName())) {
            result = false;
        }

        return result;
    }

    void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
}
