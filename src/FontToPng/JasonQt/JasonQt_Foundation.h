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

#ifndef __JasonQt_Foundation_h__
#define __JasonQt_Foundation_h__

#if (__cplusplus < 201103)
#   error("Plwase add c++11 config on pro file")
#endif

#ifndef QT_CORE_LIB
#   error("Plwaer add core in pro file")
#endif

#ifndef QT_GUI_LIB
#   error("Plwaer add gui in pro file")
#endif

// C++ lib import
#include <set>
#include <unordered_set>
#include <functional>

// Qt lib import
#include <QApplication>
#include <QObject>
#include <QMetaMethod>
#include <QCryptographicHash>
#include <QMutex>
#include <QSettings>
#include <QSharedMemory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <QBuffer>
#include <QThread>
#include <QDateTime>
#include <QTime>
#include <QHash>
#include <QVariant>

#include <QKeyEvent>
#include <QImage>
#include <QPainter>
#include <QDesktopWidget>
#include <QAbstractButton>
#include <QTextCursor>
#include <QTabWidget>
#include <QTabBar>
#include <QTableWidget>
#include <QTreeWidget>
#include <QWidget>
#include <QMessageBox>
#include <QPalette>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>

// Macro define
#define SingleNew(p, ...)                                                           \
    if(p) { delete p;  }                                                            \
    p = new decltype(*p)(__VA_ARGS__);

#define PropertyDeclare(Type, name, setName, ...)                                   \
    private:                                                                        \
    Type m_ ## name __VA_ARGS__;                                                    \
    public:                                                                         \
    inline const Type &name() const { return m_ ## name; }                          \
    inline void setName(const Type &name) { m_ ## name = name; }                    \
    private:

#define PropertyDeclareWithSlot(Type, name, setName, ...)                           \
    private:                                                                        \
    Type m_ ## name __VA_ARGS__;                                                    \
    public Q_SLOTS:                                                                 \
    Type name() const { return m_ ## name; }                                        \
    void setName(const Type &name) { m_ ## name = name; }                           \
    private:

#define PtrPropertyDeclare(Type, name, setName, ...)                                \
    private:                                                                        \
    Type *m_ ## name __VA_ARGS__;                                                   \
    public:                                                                         \
    inline const Type *name() const                                                 \
        { return m_ ## name; }                                                      \
    inline void setName(const Type &name)                                           \
        { if(m_ ## name) { delete m_ ## name; }                                     \
        m_ ## name = new Type(name); }                                              \
    private:

#define PrimaryKeyPropertyDeclare(ClassName, Type, name, ...)                       \
    Type Name __VA_ARGS__;                                                          \
    ClassName() = default;                                                          \
    ClassName(const Type &name ## _): name(name ## _) { }                           \
    operator QVariant() const { return name; }                                      \
    operator Type() const { return name; }

#define DeleteClassCopy(ClassName)                                                  \
    ClassName(const ClassName &) = delete;                                          \
    ClassName &operator=(const ClassName &) = delete;

#define JsonObjectMapToJsonArrayAndReturn(Map, ...)                                 \
    {                                                                               \
        QJsonArray buf;                                                             \
        for(const auto &data: Map)                                                  \
        {                                                                           \
            buf.append( { data __VA_ARGS__ } );                                     \
        }                                                                           \
        return buf;                                                                 \
    }

#define JsonObjectMapToJsonArray(Map, ...)                                          \
    [&]()                                                                           \
    {                                                                               \
        QJsonArray buf;                                                             \
        for(const auto &data: Map)                                                  \
        {                                                                           \
            buf.append( { data __VA_ARGS__ } );                                     \
        }                                                                           \
        return buf;\
    }()

#define JasonArrayToJsonObjectMap(Array, Target, ...)                               \
    for(const auto &data: Array)                                                    \
    {                                                                               \
        Target.insert(__VA_ARGS__(data.toObject()));                                \
    }

#define rforeach(variable, container)                                               \
for(JasonQt_RforeachContainer<__typeof__((container))> _container_((container));    \
    _container_.__howMuch__; _container_.__howMuch__--)                             \
    for(variable = *(--_container_.__now__); ; __extension__ ({ break;}))

template <typename T>
class JasonQt_RforeachContainer {
public:
    inline JasonQt_RforeachContainer(const T& t) : __now__(t.end()), __howMuch__(t.size()){ }
    typename T::const_iterator __now__;
    int __howMuch__;
};

// Funcation define
void JasonQt_EventLoopSleep(const int &delay);

void JasonQt_ForeachWidget(QWidget *parent, const std::function<void(QWidget *)> &each, const bool &recursive = false);

void JasonQt_TableWidgetSetHorizontalHeaderLabels(QTableWidget *tableWidget, const QStringList &stringList);

void JasonQt_TableWidgetSetColumnWidth(QTableWidget *tableWidget, const QVector<int> &columnWidth);

void JasonQt_TableWidgetAddString(QTableWidget *tablwWidget, const QStringList &string);

void JasonQt_TreeWidgetSetHorizontalHeaderLabels(QTreeWidget *treeWidget, const QStringList &ttringList);

void JasonQt_TreeWidgetSetColumnWidth(QTreeWidget *treeWidget, const QVector<int> &columnWidth);

void JasonQt_SetWidgetColor(QWidget *label, const QColor &color);

void JasonQt_TexetEditMoveCursorToEnd(QTextEdit *textEdit);

void JasonQt_TextEditInsertTextToEnd(QTextEdit *textEdit, const QString &string);

QString JasonQt_HashString(const QString &Key);

QString JasonQt_HashString(const QByteArray &Key);

QPoint JasonQt_GetWidgetGlobalPos(const QWidget *widget);

QRect JasonQt_GetWidgetGlobalGeometry(const QWidget *widget);

QWidget *JasonQt_TopParentWidget(QWidget *widget);

const QWidget *JasonQt_TopParentWidget(const QWidget *widget);

bool JasonQt_SingleApplication(const QString &flag);

QString JasonQt_ByteArrayToHexString(const QByteArray &data);

QByteArray JasonQt_PixmapToByteArray(const QPixmap &pixmap, const QString &format);

QByteArray JasonQt_ImageToByteArray(const QImage &image, const QString &format);

QPixmap JasonQt_ByteArrayToPixmap(const QByteArray &byteArray);

void JasonQt_LineEditSetToIPLineEdit(QLineEdit *lineEdit);

void JasonQt_LineEditSetToazAZ09LineEdit(QLineEdit *lineEdit);

void JasonQt_LineEditSetTo09LineEdit(QLineEdit *lineEdit);

void JasonQt_WidgetSetToTransparent(QWidget *target);

QString JasonQt_RandString(const int &stringLength = 40, const bool &autoSetSeed = true);

void JasonQt_WaitForSignal(const QObject *sender, const char *signal);

QJsonObject JasonQt_JsonFilter(const QJsonObject &source, const QSet<QString> &leftKey);

void JasonQt_TimerCallback(const int &interval, const std::function< void() > &callback, const bool &callbackOnStart = false);

void JasonQt_TimerCallback(const int &interval, const std::function< void(QTimer *) > &callback, const bool &callbackOnStart = false);

// Template funcation define
template <typename FunType>
bool waitBoolSignal(const typename QtPrivate::FunctionPointer<FunType>::Object *sender, FunType signal);

// Class define
class JasonQt_BeforMain
{
public:
    JasonQt_BeforMain(const std::function<void()> &funcation);
};

class JasonQt_ReadyExit
{
private:
    QString m_message;

public:
    JasonQt_ReadyExit(const QString &message);

    inline const QString &message() const { return m_message; }

    inline operator QString() const { return m_message; }
};

class JasonQt_ShowInformationMessageBoxFromOtherThread: public QObject
{
    Q_OBJECT

private:
    const QString m_title;
    const QString m_message;

public:
    JasonQt_ShowInformationMessageBoxFromOtherThread(const QString &title, const QString &message);

    static void show(const QString &title, const QString &message);

private:
    void readyShow();

private slots:
    void onShow();
};

template<class DataType>
class JasonQt_BlockWidget
{
private:
    DataType m_exitData;
    QEventLoop m_eventLoop;

public:
    JasonQt_BlockWidget() = default;

    JasonQt_BlockWidget(const DataType &defaultExitData);

    inline DataType exitData() const { return m_exitData; }

    inline void setExitData(const DataType &exitData) { m_exitData = exitData; }

    void exit(const DataType &exitdata);

    DataType exec();
};

class JasonQt_ConnectionManage
{
private:
    QVector<QMetaObject::Connection> *m_vectorData = NULL;
    QMap<QString, QMetaObject::Connection> *m_mapData = NULL;

public:
    JasonQt_ConnectionManage();

    ~JasonQt_ConnectionManage();

    void push(const QMetaObject::Connection &connection);

    void insert(const QString &key, const QMetaObject::Connection &connection);

    void remove(const QString &key);

    void disconnectAll();

    void deleteLater();
};

class JasonQt_ThreadHelper: public QObject
{
    Q_OBJECT

private:
    QMutex m_mutex;
    QList< std::function<void()> > m_waitCallbacks;
    QMutex *&m_mutexForWait;

public:
    JasonQt_ThreadHelper(QMutex *&mutexForWait);

    void run(const std::function<void()> &callback);

public slots:
    void onRun();

signals:
    void readyRun();
};

class JasonQt_Thread: public QThread
{
private:
    JasonQt_ThreadHelper *m_helper = NULL;
    QMutex *m_mutexForWait = NULL;

public:
    JasonQt_Thread();

    ~JasonQt_Thread();

    void start(const std::function< void() > &callback);

    void waitForStart(const std::function< void() > &callback);

    void waitForRunning();

private:
    void run();
};

class JasonQt_InvokeFromThreadHelper: public QObject
{
    Q_OBJECT

private:
    QMutex m_mutex;
    QList< std::function<void()> > m_waitCallbacks;

public:
    void addCallback(const std::function<void()> &callback);

public slots:
    void onRun();
};

class JasonQt_InvokeFromThread
{
private:
    static QMutex g_mutex;
    static QMap< QThread *, JasonQt_InvokeFromThreadHelper * > g_helpers;

public:
    static void invoke(QThread *thread, const std::function<void()> &callback);

    static void waitForInvoke(QThread *thread, const std::function<void()> &callback);
};

class JasonQt_WaitForOtherThread
{
private:
    QMutex m_mutex;
    int m_flag = 0;

public:
    int wait();

    void finish(const int &flag = 0);
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~TemplateFunctionDefine~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

template <typename Func1>
bool waitBoolSignal(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal)
{
    QEventLoop eventLoop;

    QObject::connect(
                sender,
                signal,
                sender,
                [&](const bool &flag)
    {
        eventLoop.exit(flag);
    },
    Qt::DirectConnection);

    return eventLoop.exec();
}

// JasonQt_BlockWidget_Class
template<class DataType>
JasonQt_BlockWidget<DataType>::JasonQt_BlockWidget(const DataType &defaultExitData):
    m_exitData(defaultExitData)
{ }

template<class DataType>
void JasonQt_BlockWidget<DataType>::exit(const DataType &exitdata)
{
    m_exitData = exitdata;
    m_eventLoop.exit();
}

template<class DataType>
DataType JasonQt_BlockWidget<DataType>::exec()
{
    m_eventLoop.exec();
    return m_exitData;
}

#endif//__JasonQt_Foundation_h__
