/****************************************************************************
**
** Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "aurorawaylandquickoutput.h"
#include "aurorawaylandquickcompositor.h"
#include "aurorawaylandquickitem_p.h"

namespace Aurora {

namespace Compositor {

WaylandQuickOutput::WaylandQuickOutput()
{
}

WaylandQuickOutput::WaylandQuickOutput(WaylandCompositor *compositor, QWindow *window)
    : WaylandOutput(compositor, window)
{
}

void WaylandQuickOutput::initialize()
{
    WaylandOutput::initialize();

    QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
    if (!quickWindow) {
        qWarning("Initialization error: Could not locate QQuickWindow on initializing WaylandQuickOutput %p.\n", this);
        return;
    }
    connect(quickWindow, &QQuickWindow::beforeSynchronizing,
            this, &WaylandQuickOutput::updateStarted,
            Qt::DirectConnection);

    connect(quickWindow, &QQuickWindow::afterRendering,
            this, &WaylandQuickOutput::doFrameCallbacks);
}

void WaylandQuickOutput::classBegin()
{
}

void WaylandQuickOutput::componentComplete()
{
    if (!compositor()) {
        for (QObject *p = parent(); p != nullptr; p = p->parent()) {
            if (auto c = qobject_cast<WaylandCompositor *>(p)) {
                setCompositor(c);
                break;
            }
        }
    }
}

void WaylandQuickOutput::update()
{
    if (!m_updateScheduled) {
        //don't qobject_cast since we have verified the type in initialize
        static_cast<QQuickWindow *>(window())->update();
        m_updateScheduled = true;
    }
}

/*!
 * \qmlproperty bool AuroraCompositor::WaylandOutput::automaticFrameCallback
 *
 * This property holds whether the WaylandOutput automatically sends frame
 * callbacks when rendering.
 *
 * The default is true.
 */
bool WaylandQuickOutput::automaticFrameCallback() const
{
    return m_automaticFrameCallback;
}

void WaylandQuickOutput::setAutomaticFrameCallback(bool automatic)
{
    if (m_automaticFrameCallback == automatic)
        return;

    m_automaticFrameCallback = automatic;
    emit automaticFrameCallbackChanged();
}

static bool itemZOrder_sort(QQuickItem *lhs, QQuickItem *rhs)
{
    return lhs->z() < rhs->z();
}

static QList<QQuickItem *> paintOrderChildItems(QQuickItem *rootItem)
{
    const auto childItems = rootItem->childItems();

    // If none of the items have set Z then the paint order list is the same as
    // the childItems list.  This is by far the most common case.
    bool haveZ = false;
    for (int i = 0; i < childItems.count(); ++i) {
        if (childItems.at(i)->z() != 0.) {
            haveZ = true;
            break;
        }
    }
    if (haveZ) {
        const auto sortedChildItems = new QList<QQuickItem*>(childItems);
        std::stable_sort(sortedChildItems->begin(), sortedChildItems->end(), itemZOrder_sort);
        return *sortedChildItems;
    }

    return childItems;
}

static QQuickItem* clickableItemAtPosition(QQuickItem *rootItem, const QPointF &position)
{
    if (!rootItem->isEnabled() || !rootItem->isVisible())
        return nullptr;

    QList<QQuickItem *> paintOrderItems = paintOrderChildItems(rootItem);
    auto negativeZStart = paintOrderItems.crend();
    for (auto it = paintOrderItems.crbegin(); it != paintOrderItems.crend(); ++it) {
        if ((*it)->z() < 0) {
            negativeZStart = it;
            break;
        }
        QQuickItem *item = clickableItemAtPosition(*it, rootItem->mapToItem(*it, position));
        if (item)
            return item;
    }

    if (rootItem->contains(position) && rootItem->acceptedMouseButtons() != Qt::NoButton)
        return rootItem;

    for (auto it = negativeZStart; it != paintOrderItems.crend(); ++it) {
        QQuickItem *item = clickableItemAtPosition(*it, rootItem->mapToItem(*it, position));
        if (item)
            return item;
    }

    return nullptr;
}

QQuickItem *WaylandQuickOutput::pickClickableItem(const QPointF &position)
{
    QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
    if (!quickWindow)
        return nullptr;

    return clickableItemAtPosition(quickWindow->contentItem(), position);
}

/*!
 * \internal
 */
void WaylandQuickOutput::updateStarted()
{
    m_updateScheduled = false;

    if (!compositor())
        return;

    frameStarted();
}

void WaylandQuickOutput::doFrameCallbacks()
{
    if (m_automaticFrameCallback)
        sendFrameCallbacks();
}
} // namespace Compositor

} // namespace Aurora