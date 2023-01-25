// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef AURORA_COMPOSITOR_WAYLANDIVIAPPLICATION_H
#define AURORA_COMPOSITOR_WAYLANDIVIAPPLICATION_H

#include <LiriAuroraCompositor/WaylandCompositorExtension>
#include <LiriAuroraCompositor/WaylandSurface>
#include <LiriAuroraCompositor/WaylandShell>
#include <LiriAuroraCompositor/WaylandResource>
#include <LiriAuroraCompositor/WaylandIviSurface>
#include <QtCore/QSize>

namespace Aurora {

namespace Compositor {

class WaylandIviApplicationPrivate;

class LIRIAURORACOMPOSITOR_EXPORT WaylandIviApplication : public WaylandShellTemplate<WaylandIviApplication>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandIviApplication)

public:
    WaylandIviApplication();
    WaylandIviApplication(WaylandCompositor *compositor);
    ~WaylandIviApplication();

    void initialize() override;

    static const struct wl_interface *interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void iviSurfaceRequested(Aurora::Compositor::WaylandSurface *surface, uint iviId, const Aurora::Compositor::WaylandResource &resource);
    void iviSurfaceCreated(Aurora::Compositor::WaylandIviSurface *iviSurface);

private:
    QScopedPointer<WaylandIviApplicationPrivate> const d_ptr;
};

} // namespace Compositor

} // namespace Aurora

#endif // AURORA_COMPOSITOR_WAYLANDIVIAPPLICATION_H
