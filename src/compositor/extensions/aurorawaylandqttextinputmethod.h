/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
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

#ifndef AURORA_COMPOSITOR_WAYLANDQTTEXTINPUTMETHOD_H
#define AURORA_COMPOSITOR_WAYLANDQTTEXTINPUTMETHOD_H

#include <LiriAuroraCompositor/aurorawaylandcompositorextension.h>

struct wl_client;

class QInputMethodEvent;
class QKeyEvent;

namespace Aurora {

namespace Compositor {

class WaylandSurface;
class WaylandQtTextInputMethodPrivate;

class LIRIAURORACOMPOSITOR_EXPORT WaylandQtTextInputMethod : public WaylandCompositorExtensionTemplate<WaylandQtTextInputMethod>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandQtTextInputMethod)
public:
    explicit WaylandQtTextInputMethod(WaylandObject *container, WaylandCompositor *compositor);
    ~WaylandQtTextInputMethod() override;

    WaylandSurface *focusedSurface() const;
    void setFocus(WaylandSurface *surface);

    bool isSurfaceEnabled(WaylandSurface *surface) const;

    void add(::wl_client *client, uint32_t id, int version);

    static const struct wl_interface *interface();
    static QByteArray interfaceName();

    QVariant inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const;
    void sendInputMethodEvent(QInputMethodEvent *event);
    void sendKeyEvent(QKeyEvent *event);

Q_SIGNALS:
    void updateInputMethod(Qt::InputMethodQueries queries);
    void surfaceEnabled(Aurora::Compositor::WaylandSurface *surface);
    void surfaceDisabled(Aurora::Compositor::WaylandSurface *surface);

private:
    QScopedPointer<WaylandQtTextInputMethodPrivate> const d_ptr;

private Q_SLOTS:
    void sendVisibleChanged();
    void sendKeyboardRectangleChanged();
    void sendInputDirectionChanged();
    void sendLocaleChanged();
    void focusSurfaceDestroyed();
};

} // namespace Compositor

} // namespace Aurora

#endif // AURORA_COMPOSITOR_WAYLANDQTTEXTINPUTMETHOD_H