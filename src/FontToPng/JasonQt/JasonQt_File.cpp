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

#include "JasonQt_File.h"

using namespace JasonQt_File;

void JasonQt_File::foreachFileFromDirectory(const QFileInfo &directory, const std::function<void(const QFileInfo &)> &each, const bool &recursion)
{
    for(const auto &now: QDir(directory.filePath()).entryInfoList(QDir::Files))
    {
        each(now);
    }

    if(recursion)
    {
        for(const auto &now: QDir(directory.filePath()).entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot))
        {
            foreachFileFromDirectory(now.filePath(), each, recursion);
        }
    }
}

bool JasonQt_File::writeFile(const QFileInfo &targetFilePath, const QByteArray &data, const bool &cover)
{
    if(!targetFilePath.dir().isReadable())
    {
        if(!QDir().mkpath(targetFilePath.path()))
        {
            return false;
        }
    }

    if(targetFilePath.isFile() && !cover)
    {
        return true;
    }

    QFile file(targetFilePath.filePath());
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(data);
    file.waitForBytesWritten(10000);

    return true;
}

std::pair<bool, QByteArray> JasonQt_File::readFile(const QFileInfo &filePath)
{
    QFile file(filePath.filePath());

    if(!file.open(QIODevice::ReadOnly)) { return { false, "Open file error" }; }

    return { true, file.readAll() };
}

bool JasonQt_File::copyFile(const QFileInfo &sourcePath, const QFileInfo &targetPath, const bool &cover)
{
    if(sourcePath.filePath()[sourcePath.filePath().size() - 1] == '/')
    {
        return false;
    }

    if(targetPath.filePath()[targetPath.filePath().size() - 1] == '/')
    {
        return false;
    }

    if(!targetPath.dir().isReadable())
    {
        if(!QDir().mkpath(targetPath.path()))
        {
            return false;
        }
    }

    if(targetPath.isFile() && !cover)
    {
        return true;
    }

    return QFile::copy(sourcePath.filePath(), targetPath.filePath());
}

bool JasonQt_File::copyDirectory(const QFileInfo &sourceDirectory, const QFileInfo &targetDirectory, const bool &cover)
{
    try
    {
        if(!sourceDirectory.isDir())
        {
            throw false;
        }

        if(sourceDirectory.filePath()[sourceDirectory.filePath().size() - 1] != '/')
        {
            throw false;
        }

        if(targetDirectory.filePath()[targetDirectory.filePath().size() - 1] != '/')
        {
            throw false;
        }

        JasonQt_File::foreachFileFromDirectory(sourceDirectory, [&](const QFileInfo &info)
        {
            const auto &&path = info.path().mid(sourceDirectory.path().size());
            if(!JasonQt_File::copyFile(info, targetDirectory.path() + "/" + ((path.isEmpty()) ? ("") : (path + "/")) + info.fileName(), cover))
            {
                throw false;
            }
        }, true);
    }
    catch(const bool &error)
    {
        return error;
    }

    return true;
}

bool JasonQt_File::copy(const QFileInfo &source, const QFileInfo &target, const bool &cover)
{
    if(source.isFile())
    {
        return JasonQt_File::copyFile(source, target, cover);
    }
    else if(source.isDir())
    {
        return JasonQt_File::copyDirectory(source, target, cover);
    }

    return false;
}
