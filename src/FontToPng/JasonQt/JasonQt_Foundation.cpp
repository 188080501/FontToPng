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

#include "JasonQt_Foundation.h"

// Funcation define
void JasonQt_EventLoopSleep(const int &delay)
{
    QEventLoop eventLoop;
    QTimer::singleShot(delay, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void JasonQt_ForeachWidget(QWidget *parent, const std::function<void (QWidget *)> &each, const bool &recursive)
{
    QObjectList children = parent->children();
    for(auto &now: children)
    {
        if(now->inherits("QWidget"))
        {
            each(qobject_cast<QWidget *>(now));
            if(recursive)
            {
                JasonQt_ForeachWidget(parent, each, recursive);
            }
        }
    }
}

void JasonQt_TableWidgetSetHorizontalHeaderLabels(QTableWidget *tableWidget, const QStringList &stringList)
{
    tableWidget->setColumnCount(stringList.size());
    tableWidget->setHorizontalHeaderLabels(stringList);
}

void JasonQt_TableWidgetSetColumnWidth(QTableWidget *tableWidget, const QVector<int> &columnWidth)
{
    int column = -1;
    foreach(auto &&now, columnWidth)
    {
        tableWidget->setColumnWidth(++column, now);
    }
}

void JasonQt_TableWidgetAddString(QTableWidget *tableWidget, const QStringList &stringList)
{
    int x = 0, y = tableWidget->rowCount();
    tableWidget->setRowCount(y + 1);
    QTableWidgetItem *item;

    foreach(QString Now, stringList)
    {
        item = new QTableWidgetItem(Now);
        item->setSelected(false);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        tableWidget->setItem(y, x++, item);
    }
}

void JasonQt_TreeWidgetSetHorizontalHeaderLabels(QTreeWidget *treeWidget, const QStringList &stringList)
{
    treeWidget->setColumnCount(stringList.size());
    treeWidget->setHeaderLabels(stringList);
}

void JasonQt_TreeWidgetSetColumnWidth(QTreeWidget *treeWidget, const QVector<int> &columnWidth)
{
    int column = -1;
    foreach(auto &&now, columnWidth)
    {
        ++column;
        if(now > 0)
        {
            treeWidget->setColumnWidth(column, now);
        }
        else if(!now)
        {
            continue;
        }
        else
        {
            treeWidget->setColumnWidth(column, treeWidget->headerItem()->text(column).size() * abs(now));
        }
    }
}

void JasonQt_SetWidgetColor(QWidget *label, const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::WindowText, color);
    label->setPalette(palette);
}

void JasonQt_TexetEditMoveCursorToEnd(QTextEdit *textEdit)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
}

void JasonQt_TextEditInsertTextToEnd(QTextEdit *textEdit, const QString &string)
{
    textEdit->insertPlainText(string);
    JasonQt_TexetEditMoveCursorToEnd(textEdit);
}

QString JasonQt_HashString(const QString &Key)
{
    return JasonQt_HashString(Key.toLatin1());
}

QString JasonQt_HashString(const QByteArray &Key)
{
    return QCryptographicHash::hash(Key, QCryptographicHash::Sha1).toHex();
}

QPoint JasonQt_GetWidgetGlobalPos(const QWidget *widget)
{
    if(!widget)
    {
        return QPoint(-1,-1);
    }

    float px = 0;
    float py = 0;
    QWidget const *parent = widget;
    QWidget const *preParent = NULL;

    do
    {
        QPoint p = parent->pos();
        px += p.x();
        py += p.y();
        preParent = parent;
        parent = parent->parentWidget();
    }
    while(parent);

    QSize size = preParent->frameSize();
    QSize size2 = preParent->size();
    px = px + (size.width() - size2.width()) / 2;
    py = py + (size.height() - size2.height() - (size.width() - size2.width()) / 2);

    QPoint pr(px, py);
    return pr;
}

QRect JasonQt_GetWidgetGlobalGeometry(const QWidget *widget)
{
    return QRect(JasonQt_GetWidgetGlobalPos(widget), widget->size());
}

QWidget *JasonQt_TopParentWidget(QWidget *widget)
{
    QWidget *parent = widget;

    while(parent->parentWidget())
    {
        parent = parent->parentWidget();
    }

    return parent;
}

const QWidget *JasonQt_TopParentWidget(const QWidget *widget)
{
    QWidget const *parent = widget;

    while(parent->parentWidget())
    {
        parent = parent->parentWidget();
    }

    return parent;
}

#if !(defined Q_OS_IOS) && !(defined Q_OS_ANDROID) && !(defined Q_OS_WINPHONE)
bool JasonQt_SingleApplication(const QString &flag)
{
    static QSharedMemory *shareMem = NULL;

    if(shareMem)
    {
        return true;
    }

    shareMem = new QSharedMemory("JasonQt_SingleApplication_Control_" + flag);

    volatile short i = 2;
    while (i--)
    {
        if (shareMem->attach(QSharedMemory::ReadOnly))
        {
            shareMem->detach();
        }
    }

    if(shareMem->create(1))
    {
        return true;
    }

    return false;
}
#else
bool JasonQt_SingleApplication(const QString &)
{
    return true;
}
#endif

QString JasonQt_ByteArrayToHexString(const QByteArray &data)
{
    QString buf(data.toHex());
    for(int Now = 1; Now < data.size(); Now++)
    {
        buf.insert(Now * 2 + Now - 1, ' ');
    }
    return buf;
}

QByteArray JasonQt_PixmapToByteArray(const QPixmap &pixmap, const QString &format)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, format.toLatin1().data());
    return bytes;
}

QByteArray JasonQt_ImageToByteArray(const QImage &image, const QString &format)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, format.toLatin1().data());
    return bytes;
}

QPixmap JasonQt_ByteArrayToPixmap(const QByteArray &byteArray)
{
    QPixmap Pixmap;
    Pixmap.loadFromData(byteArray);
    return Pixmap;
}

void JasonQt_LineEditSetToIPLineEdit(QLineEdit *lineEdit)
{
    lineEdit->setValidator(new QRegExpValidator(QRegExp("^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])[.](\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])[.](\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])[.](\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$")));
}

void JasonQt_LineEditSetToazAZ09LineEdit(QLineEdit *lineEdit)
{
    lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-zA-Z]+")));
}

void JasonQt_LineEditSetTo09LineEdit(QLineEdit *lineEdit)
{
    lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
}

void JasonQt_WidgetSetToTransparent(QWidget *target)
{
    target->setAttribute(Qt::WA_TranslucentBackground, true);
    target->setAttribute(Qt::WA_NoSystemBackground, false);
    target->setWindowFlags(Qt::FramelessWindowHint);
    target->setStyleSheet(target->styleSheet() + QString("\nQWidget#%1 { background-color: transparent; }").arg(target->objectName()));
    target->setUpdatesEnabled(true);
}

QString JasonQt_RandString(const int &stringLength, const bool &autoSetSeed)
{
    if(autoSetSeed)
    {
        srand(QDateTime::currentDateTime().toTime_t());
    }

    QString buf;
    for(auto index = 0; (index + 1) < stringLength; index += 2)
    {
        buf.append(QByteArray().append(char(rand() % 256)).toHex());
    }
    if(stringLength % 2)
    {
        buf.append((rand() % 10) + '0');
    }
    return buf;
}

void JasonQt_WaitForSignal(const QObject *sender, const char *signal)
{
    QEventLoop eventLoop;
    QObject::connect(sender, signal, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

QJsonObject JasonQt_JsonFilter(const QJsonObject &source, const QSet<QString> &leftKey)
{
    QJsonObject data;

    for(const auto &key: leftKey)
    {
        auto buf = source.find(key);
        if(buf != source.end())
        {
            data[buf.key()] = buf.value();
        }
    }

    return data;
}

void JasonQt_TimerCallback(const int &interval, const std::function<void ()> &callback, const bool &callbackOnStart)
{
    auto timer = new QTimer;
    QObject::connect(timer, &QTimer::timeout, [=]()
    {
        callback();
    });
    timer->start(interval);

    if(callbackOnStart)
    {
        callback();
    }
}

void JasonQt_TimerCallback(const int &interval, const std::function<void (QTimer *)> &callback, const bool &callbackOnStart)
{
    auto timer = new QTimer;
    QObject::connect(timer, &QTimer::timeout, [=]()
    {
        callback(timer);
    });
    timer->start(interval);

    if(callbackOnStart)
    {
        callback(timer);
    }
}

// JasonQt_BeforMain
JasonQt_BeforMain::JasonQt_BeforMain(const std::function<void ()> &funcation)
{
    funcation();
    delete this;
}

// JasonQt_ReadyExit
JasonQt_ReadyExit::JasonQt_ReadyExit(const QString &message):
    m_message(message)
{ }

// JasonQt_ShowInformationMessageBoxFromOtherThread
JasonQt_ShowInformationMessageBoxFromOtherThread::JasonQt_ShowInformationMessageBoxFromOtherThread(const QString &title, const QString &message):
    m_title(title),
    m_message(message)
{ }

void JasonQt_ShowInformationMessageBoxFromOtherThread::show(const QString &title, const QString &message)
{
    QEventLoop eventLoop;
    auto messageBox = new JasonQt_ShowInformationMessageBoxFromOtherThread(title, message);
    connect(messageBox, &JasonQt_ShowInformationMessageBoxFromOtherThread::destroyed, &eventLoop, &QEventLoop::quit);
    messageBox->readyShow();
    eventLoop.exec();
}

void JasonQt_ShowInformationMessageBoxFromOtherThread::readyShow()
{
    this->moveToThread(qApp->thread());
    QTimer::singleShot(0, this, SLOT(onShow()));
}

void JasonQt_ShowInformationMessageBoxFromOtherThread::onShow()
{
    QMessageBox::information(NULL, m_title, m_message);
    this->deleteLater();
}

// JasonQt_ConnectionManage
JasonQt_ConnectionManage::JasonQt_ConnectionManage()
{ }

JasonQt_ConnectionManage::~JasonQt_ConnectionManage()
{
    disconnectAll();
    if(m_vectorData)
    {
        delete m_vectorData;
    }
    if(m_mapData)
    {
        delete m_mapData;
    }
}

void JasonQt_ConnectionManage::push(const QMetaObject::Connection &connection)
{
    if(!m_vectorData)
    {
        m_vectorData = new QVector<QMetaObject::Connection>;
    }

    m_vectorData->push_back(connection);
}

void JasonQt_ConnectionManage::insert(const QString &key, const QMetaObject::Connection &connection)
{
    if(!m_mapData)
    {
        m_mapData = new QMap<QString, QMetaObject::Connection>;
    }

    m_mapData->insert(key, connection);
}

void JasonQt_ConnectionManage::remove(const QString &key)
{
    m_mapData->erase(m_mapData->find(key));
}

void JasonQt_ConnectionManage::disconnectAll()
{
    if(m_vectorData)
    {
        for(auto &connection: *m_vectorData)
        {
            QObject::disconnect(connection);
        }
        m_vectorData->clear();
    }
    if(m_mapData)
    {
        for(auto &connection: *m_mapData)
        {
            QObject::disconnect(connection);
        }
        m_mapData->clear();
    }
}

void JasonQt_ConnectionManage::deleteLater()
{
    delete this;
}

// JasonQt_ThreadHelper
JasonQt_ThreadHelper::JasonQt_ThreadHelper(QMutex *&mutexForWait):
    m_mutexForWait(mutexForWait)
{
    connect(this, &JasonQt_ThreadHelper::readyRun, this, &JasonQt_ThreadHelper::onRun, Qt::QueuedConnection);
}

void JasonQt_ThreadHelper::run(const std::function<void ()> &callback)
{
    m_mutex.lock();

    m_waitCallbacks.push_back(callback);

    m_mutex.unlock();

    emit readyRun();
}

void JasonQt_ThreadHelper::onRun()
{
    if(!m_waitCallbacks.isEmpty())
    {
        m_mutex.lock();

        auto callback = m_waitCallbacks.first();
        m_waitCallbacks.pop_front();

        m_mutex.unlock();

        callback();

        if(m_mutexForWait)
        {
            m_mutexForWait->unlock();
        }
    }
}

// JasonQt_Thread
JasonQt_Thread::JasonQt_Thread()
{
    this->QThread::start();
}

JasonQt_Thread::~JasonQt_Thread()
{
    while(!this->isRunning())
    {
        QThread::msleep(2);
    }

    this->quit();
    this->wait();
}

void JasonQt_Thread::start(const std::function<void ()> &callback)
{
    this->waitForRunning();
    m_helper->run(callback);
}

void JasonQt_Thread::waitForStart(const std::function<void ()> &callback)
{
    if(m_mutexForWait)
    {
        qDebug("JasonQt_Thread::start: error");
        return;
    }

    m_mutexForWait = new QMutex;
    m_mutexForWait->lock();

    this->start(callback);

    if(m_mutexForWait)
    {
        m_mutexForWait->lock();
        m_mutexForWait->unlock();
        delete m_mutexForWait;
        m_mutexForWait = NULL;
    }
}

void JasonQt_Thread::waitForRunning()
{
    while(!m_helper)
    {
        QThread::msleep(2);
    }
}

void JasonQt_Thread::run()
{
    m_helper = new JasonQt_ThreadHelper(m_mutexForWait);
    this->exec();
    delete m_helper;
    m_helper = NULL;

    QEventLoop eventLoop;
    while(eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents));
}

// JasonQt_InvokeFromThreadHelper
void JasonQt_InvokeFromThreadHelper::addCallback(const std::function<void ()> &callback)
{
    m_mutex.lock();
    m_waitCallbacks.push_back(callback);
    m_mutex.unlock();
}

void JasonQt_InvokeFromThreadHelper::onRun()
{
    if(!m_waitCallbacks.isEmpty())
    {
        m_mutex.lock();

        auto callback = m_waitCallbacks.first();
        m_waitCallbacks.pop_front();

        m_mutex.unlock();

        callback();
    }
}

// JasonQt_InvokeFromThread
QMutex JasonQt_InvokeFromThread::g_mutex;
QMap< QThread *, JasonQt_InvokeFromThreadHelper * > JasonQt_InvokeFromThread::g_helpers;

void JasonQt_InvokeFromThread::invoke(QThread *thread, const std::function<void ()> &callback)
{
    if(!(thread->isRunning()))
    {
        qDebug() << "JasonQt_InvokeFromThread::invoke: Target thread" << thread << "is not running!";
        return;
    }

    g_mutex.lock();

    auto it = g_helpers.find(thread);

    if(it == g_helpers.end())
    {
        auto helper = new JasonQt_InvokeFromThreadHelper;
        helper->moveToThread(thread);

        QObject::connect(thread, &QThread::finished, [=]()
        {
            g_mutex.lock();

            auto it = g_helpers.find(thread);
            if(it != g_helpers.end())
            {
                g_helpers.erase(it);
            }

            g_mutex.unlock();
        });

        g_helpers.insert( thread, helper );
        it = g_helpers.find(thread);
    }

    it.value()->addCallback(callback);

    QMetaObject::invokeMethod(it.value(), "onRun", Qt::QueuedConnection);

    g_mutex.unlock();
}

void JasonQt_InvokeFromThread::waitForInvoke(QThread *thread, const std::function<void ()> &callback)
{
    if(!(thread->isRunning()))
    {
        qDebug() << "JasonQt_InvokeFromThread::waitForInvoke: Target thread" << thread << "is not running!";
        return;
    }

    g_mutex.lock();

    auto it = g_helpers.find(thread);

    if(it == g_helpers.end())
    {
        auto helper = new JasonQt_InvokeFromThreadHelper;
        helper->moveToThread(thread);

        QObject::connect(thread, &QThread::finished, [=]()
        {
            g_mutex.lock();

            auto it = g_helpers.find(thread);
            if(it != g_helpers.end())
            {
                g_helpers.erase(it);
            }

            g_mutex.unlock();
        });

        g_helpers.insert( thread, helper );
        it = g_helpers.find(thread);
    }

    it.value()->addCallback([&]()
    {
        g_mutex.unlock();
        callback();
    });

    QMetaObject::invokeMethod(it.value(), "onRun", Qt::QueuedConnection);

    g_mutex.lock();
    g_mutex.unlock();
}

// JasonQt_WaitForOtherThread
int JasonQt_WaitForOtherThread::wait()
{
    m_mutex.lock();
    m_mutex.lock();
    m_mutex.unlock();
    return m_flag;
}

void JasonQt_WaitForOtherThread::finish(const int &flag)
{
    m_flag = flag;
    m_mutex.unlock();
}
