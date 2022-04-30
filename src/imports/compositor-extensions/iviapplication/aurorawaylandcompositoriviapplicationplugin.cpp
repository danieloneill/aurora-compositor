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

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>

#include <LiriAuroraCompositor/aurorawaylandquickextension.h>
#include <LiriAuroraCompositor/aurorawaylandiviapplication.h>
#include <LiriAuroraCompositor/aurorawaylandivisurface.h>

namespace Aurora {

namespace Compositor {

/*!
    \qmlmodule Aurora.Compositor.IviApplication
    \title Qt Wayland IviApplication Extension
    \ingroup qmlmodules
    \brief Provides a Qt API for the IviApplication shell extension.

    \section2 Summary
    IviApplication is a shell extension suitable for lightweight compositors,
    for example in In-Vehicle Infotainment (IVI) systems.

    IviApplication corresponds to the Wayland \c ivi_application interface.

    \section2 Usage
    To use this module, import it like this:
    \qml
    import Aurora.Compositor.IviApplication
    \endqml
*/

Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(WaylandIviApplication)

class WaylandCompositorIviApplicationPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Aurora.Compositor.IviApplication"));
        defineModule(uri);
    }

    static void defineModule(const char *uri)
    {
        qmlRegisterModule(uri, 1, 0);
        qmlRegisterType<WaylandIviApplicationQuickExtension>(uri, 1, 0, "IviApplication");
        qmlRegisterType<WaylandIviSurface>(uri, 1, 0, "IviSurface");
    }
};

} // namespace Compositor

} // namespace Aurora

#include "aurorawaylandcompositoriviapplicationplugin.moc"