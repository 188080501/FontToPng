#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_ui->tableWidget->horizontalHeader()->setHidden(true);
    m_ui->tableWidget->verticalHeader()->setHidden(true);
    m_ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->tableWidget->setCurrentItem(NULL);
    m_ui->tableWidget->setShowGrid(false);
    m_ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_ui->comboBox, (void(QComboBox::*)(int))&QComboBox::currentIndexChanged, this, &MainWindow::refresh);
    connect(m_ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::refresh);

    JasonQt_PropertyAnimation::Control::startOpacity(this, 0, 1, 200);

    QTimer::singleShot(300, [=]()
    {
        this->setEnabled(false);

        this->beginFonts();
        this->refresh();

        this->setEnabled(true);
    });
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    JasonQt_PropertyAnimation::Control::startOpacity(this, 1, 0, 200);
    JasonQt_EventLoopSleep(210);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_ui->tableWidget->move((this->width() - this->width() / 101 * 101) / 2, 0);
    m_ui->tableWidget->resize(this->width() / 101 * 101, this->height() - 100);

    this->refresh();
}

void MainWindow::beginFonts()
{
    this->addFontAwesome();
    this->addMaterialDesign();
    this->addGlyphiconsHalflings();
    this->addIconFont();
    this->addEntypo();
    this->addFoundation();
    this->addOpenIconic();
    this->addTypicons();

    int fontCount = 0;
    int indexForFontMaker = 0;
    int indexForFontNodes;
    int iconCount = 0;

    for(auto &maker: m_fontMakers)
    {
        QVector< FontNode > fontNodes;
        indexForFontNodes = 0;

        if(maker.begin())
        {
            for(const auto &flag: maker.getFlags())
            {
                auto iconNode = new Ui::IconNode;
                auto iconNodeWidget = new WidgetHelper;
                iconNode->setupUi(iconNodeWidget);

                connect(iconNode->pushButton, &QPushButton::clicked, [=]()
                {
                    this->iconExport(indexForFontMaker, indexForFontNodes);
                });

                connect(iconNodeWidget, &WidgetHelper::firstPaint, [=]()
                {
                    iconNode->labelName->setText(flag);
                    auto image = maker.paintFontFromFlag(flag, iconNode->labelPreview->size(), QColor(0, 0, 0), QPointF(0, 0));
                    iconNode->labelPreview->setPixmap(QPixmap::fromImage(image));
                });

                fontNodes.push_back( { flag, iconNodeWidget } );

                indexForFontNodes++;
                iconCount++;
            }
        }

        m_fontDatas.push_back({
                                  maker.fontName(),
                                  fontNodes
                              });

        m_ui->comboBox->addItem(maker.fontName());

        indexForFontMaker++;
        fontCount += fontNodes.size();
    }

    qDebug() << "Icon count:" << iconCount;
}

void MainWindow::iconExport(const int &fontMakerIndex, const int &fontNodesIndex)
{
    auto &&maker = m_fontMakers[fontMakerIndex];
    auto &&node = m_fontDatas[fontMakerIndex].fontNodes[fontNodesIndex];

    auto iconExport = new Ui::IconExport;
    auto iconExportDialog = new QDialog;
    iconExport->setupUi(iconExportDialog);

    iconExport->labelFontName->setText(QString("%1\n%2").arg(m_fontDatas[fontMakerIndex].name, node.flag));

    auto makeImage = [&]()->QImage
    {
        if(iconExport->doubleSpinBoxZoom->value() == 1)
        {
            auto image = maker.paintFontFromFlag(node.flag,
                                                 iconExport->labelPreview->size(),
                                                 QColor(iconExport->lineEditColor->text()),
                                                 QPointF(iconExport->spinBoxOffsetX->value(), iconExport->spinBoxOffsetY->value()));
            MainWindow::drawFrame(image);
            return image;
        }
        else
        {
            QImage image(iconExport->labelPreview->size(), QImage::Format_ARGB32);
            MainWindow::clearImage(image);

            QImage image2 = maker.paintFontFromFlag(node.flag,
                                                    iconExport->labelPreview->size() * iconExport->doubleSpinBoxZoom->value(),
                                                    QColor(iconExport->lineEditColor->text()),
                                                    QPointF(iconExport->spinBoxOffsetX->value(), iconExport->spinBoxOffsetY->value()));

            QPainter painter;
            painter.begin(&image);
            painter.drawImage((image.width() - image2.width()) / 2.0, (image.height() - image2.height()) / 2, image2);
            painter.end();

            MainWindow::drawFrame(image);
            return image;
        }
    };

    auto refreshPreview = [&]()
    {
        iconExport->labelPreview->setGeometry((iconExport->widgetPaintCenter->width() / 2) - (iconExport->spinBoxTargetSize->value() / 2),
                                              (iconExport->widgetPaintCenter->height() / 2) - (iconExport->spinBoxTargetSize->value() / 2),
                                              iconExport->spinBoxTargetSize->value(),
                                              iconExport->spinBoxTargetSize->value());

        iconExport->labelPreview->setPixmap(QPixmap::fromImage(makeImage()));
    };

    refreshPreview();

    connect(iconExport->lineEditColor,      &QLineEdit::textChanged, [&](){ refreshPreview(); });
    connect(iconExport->spinBoxTargetSize,  (void(QSpinBox::*)(int))&QSpinBox::valueChanged, [&](){ refreshPreview(); });
    connect(iconExport->spinBoxOffsetX,     (void(QSpinBox::*)(int))&QSpinBox::valueChanged, [&](){ refreshPreview(); });
    connect(iconExport->spinBoxOffsetY,     (void(QSpinBox::*)(int))&QSpinBox::valueChanged, [&](){ refreshPreview(); });
    connect(iconExport->doubleSpinBoxZoom,  (void(QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged, [&](){ refreshPreview(); });
    connect(iconExport->pushButtonCancel,   &QPushButton::clicked, iconExportDialog, &QDialog::reject);

    connect(iconExport->pushButtonExport, &QPushButton::clicked, [&]()
    {
        auto filePath = QFileDialog::getSaveFileName(NULL, "Please choose save path", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "*.png");
        if(filePath.isEmpty()) { return; }

        if(!makeImage().save(filePath))
        {
            QMessageBox::information(NULL, "Error", "Save png file fail");
        }
    });

    iconExportDialog->exec();

    delete iconExport;
    iconExportDialog->deleteLater();
}

void MainWindow::refresh()
{
    static bool processing = false;
    static bool waitProcess = false;

    if(processing)
    {
        waitProcess = true;
        MainWindow::eventLoop();
        return;
    }

    auto timeFlag = QDateTime::currentMSecsSinceEpoch();

    const auto &&columnCount = this->width() / 101;
    const auto &&comboBoxCurrentText = m_ui->comboBox->currentText();
    const auto &&searchText = m_ui->lineEdit->text();

    if(!columnCount) { return; }

    processing = true;

    // Ready widget
    QVector< QWidget * > targetWidgets;
    QVector< QWidget * > sourceWidgets;

    for(const auto &fontData: m_fontDatas)
    {
        for(const auto &node: fontData.fontNodes)
        {
            auto &&currentWidget = node.nodeWidget;

            if(currentWidget->parent())
            {
                currentWidget->hide();
                currentWidget->setParent(NULL);
            }

            if(((comboBoxCurrentText == "All") || (comboBoxCurrentText == fontData.name)) && (searchText.isEmpty() || (node.flag.indexOf(searchText) > -1)))
            {
                auto &&newWidget = new QWidget;
                newWidget->resize(100, 120);

                targetWidgets.push_back(newWidget);
                sourceWidgets.push_back(currentWidget);
            }
        }

        qDebug() << "Ready->" << QDateTime::currentMSecsSinceEpoch() - timeFlag;
        timeFlag = QDateTime::currentMSecsSinceEpoch();
    }

    timeFlag = QDateTime::currentMSecsSinceEpoch();

    // Add to table
    const auto &&rowCount = (targetWidgets.size() / columnCount) + !!(targetWidgets.size() % columnCount);

    m_ui->tableWidget->clear();

    m_ui->tableWidget->setColumnCount(columnCount);
    m_ui->tableWidget->setRowCount(rowCount);

    int now = 0;

    timeFlag = QDateTime::currentMSecsSinceEpoch();

    for(auto row = 0; (row < rowCount) && !waitProcess; row++)
    {
        m_ui->tableWidget->setRowHeight(row, 121);

        for(auto column = 0; (column < columnCount) && !waitProcess; column++)
        {
            if(!row)
            {
                m_ui->tableWidget->setColumnWidth(column, 101);
            }

            if(now >= targetWidgets.size()) { continue; }

            sourceWidgets[now]->setParent(targetWidgets[now]);
            sourceWidgets[now]->show();

            m_ui->tableWidget->setCellWidget(row, column, targetWidgets[now]);

            if(++now >= targetWidgets.size()) { continue; }
        }

        if(!(row % 4))
        {
            MainWindow::eventLoop();
        }
    }

    qDebug() << "Show->" << QDateTime::currentMSecsSinceEpoch() - timeFlag;

    processing = false;

    if(waitProcess)
    {
        waitProcess = false;
        this->refresh();
    }
}

void MainWindow::addFontAwesome()
{
    QSharedPointer< int > fontId(new int(-1));
    QSharedPointer< QHash< QString, ushort > > mappingForChar(new QHash< QString, ushort >);
    QSharedPointer< QHash< ushort, OffsetExData > > mappingOffsetEx(new QHash< ushort, OffsetExData >);

    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   *fontId = QFontDatabase::addApplicationFont(":/FontAwesome/FontAwesome/FontAwesome.ttf");
                                   if(*fontId == -1) { return false; }
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "FontAwesome";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;
                                   auto datas = JasonQt_File::readFile( { ":/FontAwesome/FontAwesome/FontAwesome.css" } ).second.split('\n');

                                   for(const auto &data: datas)
                                   {
                                       auto buf2 = data.split(':');
                                       if(buf2.size() == 2)
                                       {
                                           const auto &&unicode = buf2[1].toUShort(0, 16);
                                           const auto &&name = QString("%1(%2)").arg(buf2[0], QString::number(unicode, 16));

                                           (*mappingForChar)[name] = unicode;
                                           buf.push_back(name);
                                       }
                                   }

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardTtfFontPaintFontFromFlag(flag, targetSize, color, offset, fontId, mappingForChar, mappingOffsetEx);
                               }
                           });
}

void MainWindow::addMaterialDesign()
{
    QSharedPointer< int > fontId(new int(-1));
    QSharedPointer< QHash< QString, ushort > > mappingForChar(new QHash< QString, ushort >);
    QSharedPointer< QHash< ushort, OffsetExData > > mappingOffsetEx(new QHash< ushort, OffsetExData >);

    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   *fontId = QFontDatabase::addApplicationFont(":/MaterialDesign/MaterialDesign/MaterialDesign.ttf");
                                   if(*fontId == -1) { return false; }
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "MaterialDesign";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;
                                   auto datas = JasonQt_File::readFile( { ":/MaterialDesign/MaterialDesign/MaterialDesign.txt" } ).second.split('\n');

                                   for(const auto &data: datas)
                                   {
                                       auto buf2 = data.split(':');
                                       if(buf2.size() == 2)
                                       {
                                           const auto &&unicode = buf2[0].toUShort(0, 16);
                                           const auto &&name = QString("%1(%2)").arg(buf2[1], QString::number(unicode, 16));

                                           (*mappingForChar)[name] = unicode;
                                           buf.push_back(name);
                                       }
                                   }

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardTtfFontPaintFontFromFlag(flag, targetSize, color, offset, fontId, mappingForChar, mappingOffsetEx);
                               }
                           });
}

void MainWindow::addGlyphiconsHalflings()
{
    QSharedPointer< int > fontId(new int(-1));
    QSharedPointer< QHash< QString, ushort > > mappingForChar(new QHash< QString, ushort >);
    QSharedPointer< QHash< ushort, OffsetExData > > mappingOffsetEx(new QHash< ushort, OffsetExData >);

    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   *fontId = QFontDatabase::addApplicationFont(":/GlyphiconsHalflings/GlyphiconsHalflings/GlyphiconsHalflings.ttf");
                                   if(*fontId == -1) { return false; }
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "GlyphiconsHalflings";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;
                                   auto datas = JasonQt_File::readFile( { ":/GlyphiconsHalflings/GlyphiconsHalflings/GlyphiconsHalflings.txt" } ).second.split('\n');

                                   for(const auto &data: datas)
                                   {
                                       auto buf2 = data.split(':');
                                       if(buf2.size() == 2)
                                       {
                                           const auto &&unicode = buf2[0].toUShort(0, 16);
                                           const auto &&name = QString("%1(%2)").arg(buf2[1], QString::number(unicode, 16));

                                           (*mappingForChar)[name] = unicode;
                                           buf.push_back(name);
                                       }
                                   }

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardTtfFontPaintFontFromFlag(flag, targetSize, color, offset, fontId, mappingForChar, mappingOffsetEx);
                               }
                           });
}

void MainWindow::addIconFont()
{
    QSharedPointer< int > fontId(new int(-1));
    QSharedPointer< QHash< QString, ushort > > mappingForChar(new QHash< QString, ushort >);
    QSharedPointer< QHash< ushort, OffsetExData > > mappingOffsetEx(new QHash< ushort, OffsetExData >);

    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   *fontId = QFontDatabase::addApplicationFont(":/IconFont/IconFont/IconFont.ttf");
                                   if(*fontId == -1) { return false; }
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "IconFont";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;
                                   auto datas = JasonQt_File::readFile( { ":/IconFont/IconFont/IconFont.txt" } ).second.split('\n');

                                   for(const auto &data: datas)
                                   {
                                       auto buf2 = data.split(':');
                                       if(buf2.size() == 2)
                                       {
                                           const auto &&unicode = buf2[0].toUShort(0, 16);
                                           const auto &&name = QString("%1(%2)").arg(buf2[1], QString::number(unicode, 16));

                                           (*mappingForChar)[name] = unicode;
                                           buf.push_back(name);
                                       }
                                   }

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardTtfFontPaintFontFromFlag(flag, targetSize, color, offset, fontId, mappingForChar, mappingOffsetEx);
                               }
                           });
}

void MainWindow::addEntypo()
{
    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "Entypo";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;

                                   JasonQt_File::foreachFileFromDirectory( { ":/Entypo/Entypo/" },
                                       [&](const QFileInfo &fileInfo)
                                       {
                                           buf.push_back(fileInfo.baseName());
                                       }
                                   );

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardSvgFontPaintFontFromFlag(flag, targetSize, color, offset, ":/Entypo/Entypo/%1.svg");
                               }
                           });
}

void MainWindow::addFoundation()
{
    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "Foundation";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;

                                   JasonQt_File::foreachFileFromDirectory( { ":/Foundation/Foundation/" },
                                       [&](const QFileInfo &fileInfo)
                                       {
                                           buf.push_back(fileInfo.baseName());
                                       }
                                   );

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardSvgFontPaintFontFromFlag(flag, targetSize, color, offset, ":/Foundation/Foundation/%1.svg");
                               }
                           });
}

void MainWindow::addOpenIconic()
{
    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "OpenIconic";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;

                                   JasonQt_File::foreachFileFromDirectory( { ":/OpenIconic/OpenIconic/" },
                                       [&](const QFileInfo &fileInfo)
                                       {
                                           buf.push_back(fileInfo.baseName());
                                       }
                                   );

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardSvgFontPaintFontFromFlag(flag, targetSize, color, offset, ":/OpenIconic/OpenIconic/%1.svg");
                               }
                           });
}

void MainWindow::addTypicons()
{
    m_fontMakers.push_back({
                               [=]()->bool
                               {
                                   return true;
                               },
                               [=]()->QString
                               {
                                   return "Typicons";
                               },
                               [=]()->QVector< QString >
                               {
                                   QVector< QString > buf;

                                   JasonQt_File::foreachFileFromDirectory( { ":/Typicons/Typicons/" },
                                       [&](const QFileInfo &fileInfo)
                                       {
                                           buf.push_back(fileInfo.baseName());
                                       }
                                   );

                                   return buf;
                               },
                               [=](const QString &flag, const QSizeF &targetSize, const QColor &color, const QPointF &offset)->QImage
                               {
                                   return MainWindow::standardSvgFontPaintFontFromFlag(flag, targetSize, color, offset, ":/Typicons/Typicons/%1.svg");
                               }
                           });
}

void MainWindow::eventLoop()
{
    static QEventLoop *eventLoop = NULL;
    if(!eventLoop)
    {
        eventLoop = new QEventLoop;
    }

    while(eventLoop->processEvents());
}

void MainWindow::drawFrame(QImage &image)
{
    QPainter painter;
    painter.begin(&image);
    painter.drawRect(0, 0, image.width() - 1, image.height() - 1);
    painter.end();
}

void MainWindow::clearImage(QImage &image)
{
    memset(image.bits(), 0, image.byteCount());
}

MainWindow::OffsetExData MainWindow::getOffsetPercentage(const QImage &source, const QSizeF &originallySize, const qreal &minimumScale)
{
    OffsetExData buf;
    qreal xStart = -1;
    qreal xEnd = source.width();
    qreal yStart = -1;
    qreal yEnd = source.height();

    for(auto x = 0; (x < source.width()) && (xStart == -1); x++)
    {
        for(auto y = 0; (y < source.height()) && (xStart == -1); y++)
        {
            if(source.pixel(x, y))
            {
                xStart = x;
                break;
            }
        }
    }

    for(auto x = source.width() - 1; (x >= 0) && (xEnd == source.width()); x--)
    {
        for(auto y = 0; (y < source.height()) && (xEnd == source.width()); y++)
        {
            if(source.pixel(x, y))
            {
                xEnd = x;
                break;
            }
        }
    }

    for(auto y = 0; (y < source.height()) && (yStart == -1); y++)
    {
        for(auto x = 0; (x < source.width()) && (yStart == -1); x++)
        {
            if(source.pixel(x, y))
            {
                yStart = y;
                break;
            }
        }
    }

    for(auto y = source.height() - 1; (y >= 0) && (yEnd == source.height()); y--)
    {
        for(auto x = 0; (x < source.width()) && (yEnd == source.height()); x++)
        {
            if(source.pixel(x, y))
            {
                yEnd = y;
                break;
            }
        }
    }

    buf.xOffset = (source.width() - xEnd - xStart) / originallySize.width() / 2;
    buf.yOffset = (source.height() - yEnd - yStart) / originallySize.height() / 2;

    if((xEnd - xStart) > (originallySize.width() * minimumScale))
    {
        if(((yEnd - yStart) > (originallySize.width() * minimumScale)) && ((yEnd - yStart) > (xEnd - xStart)))
        {
            buf.scale = (originallySize.height() * minimumScale) / (yEnd - yStart);
            buf.xOffset *= buf.scale;
            buf.yOffset *= buf.scale;
        }
        else
        {
            buf.scale = (originallySize.width() * minimumScale) / (xEnd - xStart);
            buf.xOffset *= buf.scale;
            buf.yOffset *= buf.scale;
        }
    }

    if((buf.xOffset >= -0.0015) && (buf.xOffset <= 0.0015)) { buf.xOffset = 0; }
    if((buf.yOffset >= -0.0015) && (buf.yOffset <= 0.0015)) { buf.yOffset = 0; }

    if(buf.xOffset >= 0.9985) { buf.xOffset = 1; }
    if(buf.xOffset <= -0.9985) { buf.xOffset = -1; }
    if(buf.yOffset >= 0.9985) { buf.yOffset = 1; }
    if(buf.yOffset <= -0.9985) { buf.yOffset = -1; }

    return buf;
}

QImage MainWindow::standardTtfFontPaintFontFromFlag(const QString &flag, const QSizeF &targetSize, const QColor &color_, const QPointF &offset,
                                                    const QSharedPointer<int> &fontId,
                                                    const QSharedPointer<QHash<QString, ushort> > &mappingForChar,
                                                    const QSharedPointer<QHash<ushort, MainWindow::OffsetExData> > &mappingOffsetEx)
{
    auto &&currentChar = QChar((*mappingForChar)[flag]);

    if(!mappingOffsetEx->contains(currentChar.unicode()))
    {
        QPainter painter;
        QImage image(1000, 1000, QImage::Format_ARGB32);

        MainWindow::clearImage(image);

        QFont font;
        font.setFamily(QFontDatabase::applicationFontFamilies(*fontId).first());
        font.setPixelSize(800);

        painter.begin(&image);

        painter.setFont(font);
        painter.drawText(QRectF(100, 100, 800, 800), Qt::AlignCenter, QString().append(currentChar));

        painter.end();

        (*mappingOffsetEx)[currentChar.unicode()] = getOffsetPercentage(image, QSizeF(800, 800));
    }

    auto &&offsetData = (*mappingOffsetEx)[currentChar.unicode()];

    {
        auto color = (color_.isValid()) ? (color_) : (QColor(0, 0, 0));

        QPainter painter;
        QImage image(targetSize.width(), targetSize.height(), QImage::Format_ARGB32);

        MainWindow::clearImage(image);

        QFont font;
        font.setFamily(QFontDatabase::applicationFontFamilies(*fontId).first());
        font.setPixelSize(targetSize.width() * offsetData.scale);

        painter.begin(&image);

        painter.setFont(font);
        painter.setPen(color);
        painter.translate(offset);
        painter.translate(targetSize.width() * offsetData.scale * offsetData.xOffset,
                          targetSize.height() * offsetData.scale * offsetData.yOffset);

        painter.drawText(QRectF(0, 0, targetSize.width(), targetSize.height()),
                         Qt::AlignCenter, QString().append(currentChar));

        painter.end();

        return image;
    }
}

QImage MainWindow::standardSvgFontPaintFontFromFlag(const QString &flag, const QSizeF &targetSize, const QColor &color_, const QPointF &offset,
                                                    const QString &filePath)
{
    auto color = (color_.isValid()) ? (color_) : (QColor(0, 0, 0));

    QSvgRenderer renderer(filePath.arg(flag));

    QPainter painter;
    QImage image(targetSize.width(), targetSize.height(), QImage::Format_ARGB32);

    MainWindow::clearImage(image);

    painter.begin(&image);

    painter.translate(offset);
    renderer.render(&painter, QRectF(QPointF(0, 0), targetSize));

    painter.end();

    if(color != QColor(0, 0, 0, 255))
    {
        for(auto y = 0; y < image.height(); y++)
        {
            for(auto x = 0; x < image.width(); x++)
            {
                auto alpha = *(image.bits() + (x * 4) + ((y * image.width()) * 4) + 3);
                if(alpha)
                {
                    image.setPixel(x, y, qRgba(color.red(), color.green(), color.blue(), alpha * (color.alpha() / 255.0)));
                }
            }
        }
    }

    return image;
}
