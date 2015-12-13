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

#ifndef __JasonQt_PropertyAnimation_h__
#define __JasonQt_PropertyAnimation_h__

// Qt lib import
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWindow>

namespace JasonQt_PropertyAnimation
{

class OpacityWidgetNode: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal OpacityWidget READ getOpacity WRITE setOpacity)

private:
    QImage m_image;
    qreal m_opacity;

public:
    static QMap<QWidget *,OpacityWidgetNode *> g_opacityWidgetNode;
    static QMap<QWidget *, bool> g_opacityWidgetToShow;

public:
    OpacityWidgetNode(QWidget *target, const qreal &DefaultOpacity);

    inline qreal getOpacity() const { return m_opacity; }

    inline void setOpacity(const qreal &Opacity) { m_opacity = Opacity; this->repaint(); }

    void paintEvent(QPaintEvent *);
};

class ControlNode: public QObject
{
    Q_OBJECT

public:
    QObject *m_target = NULL;
    QPropertyAnimation *m_propertyAnimation = NULL;

    bool m_disposable = true;
    int m_lifeCycle = 0;
    bool m_loop = false;

    QTimer *m_timer = NULL;

public:
    static QMap<const QObject *, QMap<QByteArray, ControlNode *> > m_node;

public:
    ControlNode(QObject *m_target, QPropertyAnimation *m_propertyAnimation);

    ~ControlNode();

public slots:
    void start();

    void finish();

    void delete_();
};

class Control
{
public:
    static void startSize(QWidget *target, const QSize &start, const QSize &end, const int &duration, const QEasingCurve &easingCurve = QEasingCurve::Linear, const bool &disposable = true, const int &lifeCycle = 0, const bool &loop = false);

    static void startPos(QWidget *target, const QPoint &start, const QPoint &end, const int &duration, const QEasingCurve &easingCurve = QEasingCurve::Linear, const bool &disposable = true, const int &lifeCycle = 0, const bool &loop = false);

    static void startGeometry(QWidget *target, const QRect &start, const QRect &end, const int &duration, const QEasingCurve &easingCurve = QEasingCurve::Linear, const bool &disposable = true, const int &lifeCycle = 0, const bool &loop = false);

    static void startOpacity(QWidget *target, const qreal &start, const qreal &end, const int &duration, const QEasingCurve &easingCurve = QEasingCurve::Linear, const bool &disposable = true, const int &lifeCycle = 0, const bool &loop = false);

    static void startOpacityToShow(QWidget *target, const int &duration, const QEasingCurve &easingCurve = QEasingCurve::Linear, const QObject *onFinishReceiver = NULL, const char *onFinishMethod = NULL);

    static void startOpacityToHide(QWidget *target, const int &duration, const QEasingCurve &easingCurve = QEasingCurve::Linear, const QObject *onFinishReceiver = NULL, const char *onFinishMethod = NULL);

    static void stop(const QWidget *target, const QByteArray &PropertyName = QByteArray());

    static ControlNode *at(QObject *target, const QByteArray &PropertyName);
};

}

#endif//__JasonQt_PropertyAnimation_h__
