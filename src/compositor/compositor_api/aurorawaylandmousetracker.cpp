/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCursor>
#include <QImage>
#include <QPixmap>

#include "aurorawaylandmousetracker_p.h"

namespace Aurora {

namespace Compositor {

class WaylandMouseTrackerPrivate
{
    Q_DECLARE_PUBLIC(WaylandMouseTracker)
public:
    WaylandMouseTrackerPrivate(WaylandMouseTracker *self)
        : q_ptr(self)
    {
        QImage cursorImage(64,64,QImage::Format_ARGB32);
        cursorImage.fill(Qt::transparent);
        cursorPixmap = QPixmap::fromImage(cursorImage);
    }
    void handleMousePos(const QPointF &mousePos)
    {
        Q_Q(WaylandMouseTracker);
        bool xChanged = mousePos.x() != this->mousePos.x();
        bool yChanged = mousePos.y() != this->mousePos.y();
        if (xChanged || yChanged) {
            this->mousePos = mousePos;
            if (xChanged)
                emit q->mouseXChanged();
            if (yChanged)
                emit q->mouseYChanged();
        }
    }

    void setHovered(bool hovered)
    {
        Q_Q(WaylandMouseTracker);
        if (this->hovered == hovered)
            return;
        this->hovered = hovered;
        emit q->hoveredChanged();
    }

    QPointF mousePos;
    bool windowSystemCursorEnabled = false;
    QPixmap cursorPixmap;
    bool hovered = false;

private:
    WaylandMouseTracker *q_ptr = nullptr;
};

WaylandMouseTracker::WaylandMouseTracker(QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new WaylandMouseTrackerPrivate(this))
{
    Q_D(WaylandMouseTracker);
    setFiltersChildMouseEvents(true);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
#if QT_CONFIG(cursor)
    setCursor(QCursor(d->cursorPixmap));
#endif
}

WaylandMouseTracker::~WaylandMouseTracker()
{
}

qreal WaylandMouseTracker::mouseX() const
{
    Q_D(const WaylandMouseTracker);
    return d->mousePos.x();
}
qreal WaylandMouseTracker::mouseY() const
{
    Q_D(const WaylandMouseTracker);
    return d->mousePos.y();
}

#if QT_CONFIG(cursor)
void WaylandMouseTracker::setWindowSystemCursorEnabled(bool enable)
{
    Q_D(WaylandMouseTracker);
    if (d->windowSystemCursorEnabled != enable) {
        d->windowSystemCursorEnabled = enable;
        if (enable) {
            unsetCursor();
        } else {
            setCursor(QCursor(d->cursorPixmap));
        }
        emit windowSystemCursorEnabledChanged();
    }
}

bool WaylandMouseTracker::windowSystemCursorEnabled() const
{
    Q_D(const WaylandMouseTracker);
    return d->windowSystemCursorEnabled;
}
#endif

bool WaylandMouseTracker::hovered() const
{
    Q_D(const WaylandMouseTracker);
    return d->hovered;
}

bool WaylandMouseTracker::childMouseEventFilter(QQuickItem *item, QEvent *event)
{
    Q_D(WaylandMouseTracker);
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        d->handleMousePos(mapFromItem(item, mouseEvent->position()));
#else
        d->handleMousePos(mapFromItem(item, mouseEvent->localPos()));
#endif
    } else if (event->type() == QEvent::HoverMove) {
        QHoverEvent *hoverEvent = static_cast<QHoverEvent *>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        d->handleMousePos(mapFromItem(item, hoverEvent->position()));
#else
        d->handleMousePos(mapFromItem(item, hoverEvent->posF()));
#endif
    }
    return false;
}

void WaylandMouseTracker::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(WaylandMouseTracker);
    QQuickItem::mouseMoveEvent(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    d->handleMousePos(event->position());
#else
    d->handleMousePos(event->localPos());
#endif
}

void WaylandMouseTracker::hoverMoveEvent(QHoverEvent *event)
{
    Q_D(WaylandMouseTracker);
    QQuickItem::hoverMoveEvent(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    d->handleMousePos(event->position());
#else
    d->handleMousePos(event->posF());
#endif
}

void WaylandMouseTracker::hoverEnterEvent(QHoverEvent *event)
{
    Q_D(WaylandMouseTracker);
    Q_UNUSED(event);
    d->setHovered(true);
}

void WaylandMouseTracker::hoverLeaveEvent(QHoverEvent *event)
{
    Q_D(WaylandMouseTracker);
    Q_UNUSED(event);
    d->setHovered(false);
}

} // namespace Compositor

} // namespace Aurora