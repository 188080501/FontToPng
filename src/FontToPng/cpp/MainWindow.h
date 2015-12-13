#ifndef __MainWingow_h__
#define __MainWingow_h__

// Qt lib import
#include <QMainWindow>
#include <QFontDatabase>
#include <QSvgRenderer>
#include <QFileDialog>
#include <QStandardPaths>

// JasonQt lib import
#include "JasonQt_Foundation.h"
#include "JasonQt_File.h"
#include "JasonQt_PropertyAnimation.h"

// UI import
#include "ui_MainWindow.h"
#include "ui_IconNode.h"
#include "ui_IconExport.h"

class WidgetHelper: public QWidget
{
    Q_OBJECT

private:
    bool m_firstPaint = true;

public:
    void paintEvent(QPaintEvent *)
    {
        if(m_firstPaint)
        {
            m_firstPaint = false;
            emit firstPaint();
        }
    }

signals:
    void firstPaint();
};

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    struct OffsetExData
    {
        qreal xOffset = 0;
        qreal yOffset = 0;
        qreal scale = 1;
    };

    struct FontNode
    {
        QString flag;
        QWidget *nodeWidget;
    };

    struct FontData
    {
        QString name;
        QVector< FontNode > fontNodes;
    };

    struct FontMaker
    {
        std::function< bool() > begin;
        std::function< QString() > fontName;
        std::function< QVector< QString >() > getFlags;
        std::function< QImage(const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset) > paintFontFromFlag;
    };

    QVector< FontMaker > m_fontMakers;
    QVector< FontData > m_fontDatas;

private:
    Ui::MainWindow *m_ui;

public:
    explicit MainWindow(QWidget *parent = NULL);

    ~MainWindow();

    void closeEvent(QCloseEvent *);

    void resizeEvent(QResizeEvent *);

    void beginFonts();

    void iconExport(const int &fontMakerIndex, const int &fontNodesIndex);

public slots:
    void refresh();

private:
    void addFontAwesome();

    void addMaterialDesign();

    void addGlyphiconsHalflings();

    void addIconFont();

    void addEntypo();

    void addFoundation();

    void addOpenIconic();

    void addTypicons();

    static void eventLoop();

    static void drawFrame(QImage &image);

    static void clearImage(QImage &image);

    static OffsetExData getOffsetPercentage(const QImage &source, const QSizeF &originallySize, const qreal &minimumScale = 0.85);

    static QImage standardTtfFontPaintFontFromFlag(const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset,
                                                   const QSharedPointer< int > &fontId,
                                                   const QSharedPointer< QHash< QString, ushort > > &mappingForChar,
                                                   const QSharedPointer< QHash< ushort, OffsetExData > > &mappingOffsetEx);

    static QImage standardSvgFontPaintFontFromFlag(const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset,
                                                   const QString &filePath);
};

#endif//__MainWingow_h__
