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

#ifndef AURORA_COMPOSITOR_WAYLANDIVISURFACE_H
#define AURORA_COMPOSITOR_WAYLANDIVISURFACE_H

#include <LiriAuroraCompositor/WaylandShellSurface>
#include <LiriAuroraCompositor/aurorawaylandquickchildren.h>

struct wl_resource;

namespace Aurora {

namespace Compositor {

class WaylandIviSurfacePrivate;
class WaylandSurface;
class WaylandIviApplication;
class WaylandSurfaceRole;
class WaylandResource;

class LIRIAURORACOMPOSITOR_EXPORT WaylandIviSurface : public WaylandShellSurfaceTemplate<WaylandIviSurface>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandIviSurface)
    Q_WAYLAND_COMPOSITOR_DECLARE_QUICK_CHILDREN(WaylandIviSurface)
    Q_PROPERTY(Aurora::Compositor::WaylandSurface *surface READ surface NOTIFY surfaceChanged)
    Q_PROPERTY(uint iviId READ iviId NOTIFY iviIdChanged)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Q_MOC_INCLUDE("aurorawaylandsurface.h")
#endif
public:
    WaylandIviSurface();
    WaylandIviSurface(WaylandIviApplication *application, WaylandSurface *surface, uint iviId, const WaylandResource &resource);
    ~WaylandIviSurface();

    Q_INVOKABLE void initialize(Aurora::Compositor::WaylandIviApplication *iviApplication,
                                Aurora::Compositor::WaylandSurface *surface,
                                uint iviId, const Aurora::Compositor::WaylandResource &resource);

    WaylandSurface *surface() const;
    uint iviId() const;

    static const struct wl_interface *interface();
    static QByteArray interfaceName();
    static WaylandSurfaceRole *role();
    static WaylandIviSurface *fromResource(::wl_resource *resource);

    Q_INVOKABLE void sendConfigure(const QSize &size);

#if LIRI_FEATURE_aurora_compositor_quick
    WaylandQuickShellIntegration *createIntegration(WaylandQuickShellSurfaceItem *item) override;
#endif

Q_SIGNALS:
    void surfaceChanged();
    void iviIdChanged();

private:
    QScopedPointer<WaylandIviSurfacePrivate> const d_ptr;

    void initialize() override;
};

} // namespace Compositor

} // namespace Aurora

#endif // AURORA_COMPOSITOR_WAYLANDIVISURFACE_H