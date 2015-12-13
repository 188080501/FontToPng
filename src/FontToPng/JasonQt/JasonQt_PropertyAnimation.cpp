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

#include "JasonQt_PropertyAnimation.h"

using namespace JasonQt_PropertyAnimation;

// OpacityWidget_Node
QMap<QWidget *,OpacityWidgetNode *> OpacityWidgetNode::g_opacityWidgetNode;
QMap<QWidget *, bool> OpacityWidgetNode::g_opacityWidgetToShow;

OpacityWidgetNode::OpacityWidgetNode(QWidget *target, const qreal &defaultOpacity):
    QWidget(target->parentWidget()),
    m_image(target->size() * QWindow().devicePixelRatio(), QImage::Format_ARGB32),
    m_opacity(defaultOpacity)
{
    memset(m_image.bits(), '\0', m_image.size().width() * m_image.size().height() * 4);
    m_image.setDevicePixelRatio(QWindow().devicePixelRatio());

    auto befor = target->testAttribute(Qt::WA_TranslucentBackground);
    target->setAttribute(Qt::WA_TranslucentBackground, true);
    target->render(&m_image);
    target->setAttribute(Qt::WA_TranslucentBackground, befor);

    this->setGeometry(target->geometry());
    this->show();
}

void OpacityWidgetNode::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setOpacity(m_opacity);
    painter.drawImage(0, 0, m_image);
}

// JasonQt_PropertyAnimation_Control_Node
QMap< const QObject *, QMap< QByteArray, ControlNode * > > ControlNode::m_node;

ControlNode::ControlNode(QObject *target, QPropertyAnimation *propertyAnimation)
{
    m_target = target;
    m_propertyAnimation = propertyAnimation;

    connect(m_propertyAnimation, &QPropertyAnimation::finished, this, &ControlNode::finish);
}

ControlNode::~ControlNode()
{
    if(m_timer)
    {
        delete m_timer;
    }

    delete m_propertyAnimation;
}

void ControlNode::start()
{
    m_propertyAnimation->stop();
    m_propertyAnimation->start();

    if(m_lifeCycle)
    {
        if(m_timer)
        {
            delete m_timer;
        }
        m_timer = new QTimer;
        connect(m_timer, &QTimer::timeout, this, &ControlNode::delete_);
        m_timer->start(m_lifeCycle);
    }
}

void ControlNode::finish()
{
    if(m_disposable)
    {
        this->delete_();
        return;
    }
    if(m_loop)
    {
        m_propertyAnimation->start();
    }
}

void ControlNode::delete_()
{
    auto now = m_node.find(m_target);
    if(now != m_node.end())
    {
        auto Now2 = now->find(m_propertyAnimation->propertyName());
        if(Now2 != now->end())
        {
            now->erase(Now2);
        }
    }
    this->deleteLater();
}

// JasonQt_PropertyAnimation_Control_Class
void Control::startSize(QWidget *target, const QSize &start, const QSize &end, const int &duration, const QEasingCurve &easingCurve, const bool &disposable, const int &lifeCycle, const bool &loop)
{
    auto node = at((QObject *)target, "size");

    node->m_propertyAnimation->setStartValue(start);
    node->m_propertyAnimation->setEndValue(end);
    node->m_propertyAnimation->setDuration(duration);
    node->m_propertyAnimation->setEasingCurve(easingCurve);

    node->m_disposable = disposable;
    node->m_lifeCycle = lifeCycle;
    node->m_loop = loop;

    ControlNode::m_node[(QObject *)target]["size"] = node;

    node->start();
}

void Control::startPos(QWidget *target, const QPoint &start, const QPoint &end, const int &duration, const QEasingCurve &easingCurve, const bool &disposable, const int &lifeCycle, const bool &loop)
{
    auto node = at((QObject *)target, "pos");

    node->m_propertyAnimation->setStartValue(start);
    node->m_propertyAnimation->setEndValue(end);
    node->m_propertyAnimation->setDuration(duration);
    node->m_propertyAnimation->setEasingCurve(easingCurve);

    node->m_disposable = disposable;
    node->m_lifeCycle = lifeCycle;
    node->m_loop = loop;

    ControlNode::m_node[(QObject *)target]["pos"] = node;

    node->start();
}

void Control::startGeometry(QWidget *target, const QRect &start, const QRect &end, const int &duration, const QEasingCurve &easingCurve, const bool &disposable, const int &lifeCycle, const bool &loop)
{
    auto node = at((QObject *)target, "geometry");

    node->m_propertyAnimation->setStartValue(start);
    node->m_propertyAnimation->setEndValue(end);
    node->m_propertyAnimation->setDuration(duration);
    node->m_propertyAnimation->setEasingCurve(easingCurve);

    node->m_disposable = disposable;
    node->m_lifeCycle = lifeCycle;
    node->m_loop = loop;

    ControlNode::m_node[(QObject *)target]["geometry"] = node;

    node->start();
}

void Control::startOpacity(QWidget *target, const qreal &start, const qreal &end, const int &duration, const QEasingCurve &easingCurve, const bool &disposable, const int &lifeCycle, const bool &loop)
{
    auto node = at((QObject *)target, "windowOpacity");

    node->m_propertyAnimation->setStartValue(start);
    node->m_propertyAnimation->setEndValue(end);
    node->m_propertyAnimation->setDuration(duration);
    node->m_propertyAnimation->setEasingCurve(easingCurve);

    node->m_disposable = disposable;
    node->m_lifeCycle = lifeCycle;
    node->m_loop = loop;

    ControlNode::m_node[(QObject *)target]["windowOpacity"] = node;

    node->start();
}

void Control::startOpacityToShow(QWidget *target_, const int &duration, const QEasingCurve &easingCurve, const QObject *onFinishReceiver, const char *onFinishMethod)
{
    if(!OpacityWidgetNode::g_opacityWidgetNode.contains(target_))
    {
        OpacityWidgetNode::g_opacityWidgetNode[target_] = new OpacityWidgetNode(target_, (target_->isVisible()) ? (1.0) : (0.0));
    }

    auto target = OpacityWidgetNode::g_opacityWidgetNode[target_];
    target_->hide();

    ControlNode *node = at((QObject *)target, "OpacityWidget");

    node->m_propertyAnimation->setStartValue(target->getOpacity());
    node->m_propertyAnimation->setEndValue(1.0);
    node->m_propertyAnimation->setDuration(duration);
    node->m_propertyAnimation->setEasingCurve(easingCurve);

    OpacityWidgetNode::g_opacityWidgetToShow[target_] = true;

    QObject::connect(node, &QObject::destroyed, [=]()
    {
        if(OpacityWidgetNode::g_opacityWidgetNode.contains(target_))
        {
            if(OpacityWidgetNode::g_opacityWidgetToShow[target_])
            {
                target_->show();
            }
            else
            {
                target_->hide();
            }

            delete OpacityWidgetNode::g_opacityWidgetNode[target_];
            OpacityWidgetNode::g_opacityWidgetToShow.erase(OpacityWidgetNode::g_opacityWidgetToShow.find(target_));
            OpacityWidgetNode::g_opacityWidgetNode.erase(OpacityWidgetNode::g_opacityWidgetNode.find(target_));
        }
    });
    if(onFinishReceiver)
    {
        QObject::connect(node, SIGNAL(destroyed()), onFinishReceiver, onFinishMethod);
    }
    ControlNode::m_node[(QObject *)target]["OpacityWidget"] = node;

    node->start();
}

void Control::startOpacityToHide(QWidget *target_, const int &duration, const QEasingCurve &easingCurve, const QObject *onFinishReceiver, const char *onFinishMethod)
{
    if(!OpacityWidgetNode::g_opacityWidgetNode.contains(target_))
    {
        OpacityWidgetNode::g_opacityWidgetNode[target_] = new OpacityWidgetNode(target_, (target_->isHidden()) ? (0.0) : (1.0));
    }

    auto target = OpacityWidgetNode::g_opacityWidgetNode[target_];
    target_->hide();

    auto node = at((QObject *)target, "OpacityWidget");

    node->m_propertyAnimation->setStartValue(target->getOpacity());
    node->m_propertyAnimation->setEndValue(0.0);
    node->m_propertyAnimation->setDuration(duration);
    node->m_propertyAnimation->setEasingCurve(easingCurve);

    OpacityWidgetNode::g_opacityWidgetToShow[target_] = false;

    QObject::connect(node, &QObject::destroyed, [=]()
    {
        if(OpacityWidgetNode::g_opacityWidgetNode.contains(target_))
        {
            if(OpacityWidgetNode::g_opacityWidgetToShow[target_])
            {
                target_->show();
            }
            else
            {
                target_->hide();
            }

            delete OpacityWidgetNode::g_opacityWidgetNode[target_];
            OpacityWidgetNode::g_opacityWidgetToShow.erase(OpacityWidgetNode::g_opacityWidgetToShow.find(target_));
            OpacityWidgetNode::g_opacityWidgetNode.erase(OpacityWidgetNode::g_opacityWidgetNode.find(target_));
        }
    });
    if(onFinishReceiver)
    {
        QObject::connect(node, SIGNAL(destroyed()), onFinishReceiver, onFinishMethod);
    }
    ControlNode::m_node[(QObject *)target]["OpacityWidget"] = node;

    node->start();
}

void Control::stop(const QWidget *target, const QByteArray &propertyName)
{
    auto now = ControlNode::m_node.find((QObject *)target);
    if(now != ControlNode::m_node.end())
    {
        for(auto Now2: *now)
        {
            if(!propertyName.isEmpty())
            {
                if(Now2->m_propertyAnimation->propertyName() != propertyName)
                {
                    continue;
                }
            }

            auto Befor = Now2->m_loop;
            Now2->m_loop = false;

            Now2->m_propertyAnimation->stop();
            Now2->finish();

            Now2->m_loop = Befor;
        }
    }
}

ControlNode *Control::at(QObject *target, const QByteArray &propertyName)
{
    auto now = ControlNode::m_node.find(target);
    if(now != ControlNode::m_node.end())
    {
        auto Now2 = now->find(propertyName);
        if(Now2 != now->end())
        {
            return *Now2;
        }
    }

    return new ControlNode(target, new QPropertyAnimation((QObject *)target, propertyName));
}
