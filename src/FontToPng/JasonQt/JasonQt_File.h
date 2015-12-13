/*
    This file is part of JasonQt

    Copyright: Jason

    Contact email: 188080501@qq.com

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 2.1 or version 3 as published by the Free
    Software Foundation and appearing in the file LICENSE.LGPLv21 and
    LICENSE.LGPLv3 included in the packaging of this file. Please review the
    following information to ensure the GNU Lesser General Public License
    requirements will be met: https://www.gnu.org/licenses/lgpl.html and
    http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#ifndef __JasonQt_File_h__
#define __JasonQt_File_h__

// C++ lib import
#include <functional>

// Qt lib import
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

namespace JasonQt_File
{

void foreachFileFromDirectory(const QFileInfo &directory, const std::function<void(const QFileInfo &)> &each, const bool &recursion = false);

bool writeFile(const QFileInfo &targetFilePath, const QByteArray &data, const bool &cover = true);

std::pair<bool, QByteArray> readFile(const QFileInfo &filePath);

bool copyFile(const QFileInfo &sourcePath, const QFileInfo &targetPath, const bool &cover = true);

bool copyDirectory(const QFileInfo &sourceDirectory, const QFileInfo &targetDirectory, const bool &cover = true);

bool copy(const QFileInfo &source, const QFileInfo &target, const bool &cover = true);

}

#endif//__JasonQt_File_h__