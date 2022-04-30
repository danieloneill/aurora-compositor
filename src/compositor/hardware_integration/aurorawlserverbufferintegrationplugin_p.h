/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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

#ifndef AURORA_COMPOSITOR_WAYLANDSERVERBUFFERINTEGRATIONPLUGIN_H
#define AURORA_COMPOSITOR_WAYLANDSERVERBUFFERINTEGRATIONPLUGIN_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Aurora API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <LiriAuroraCompositor/liriauroracompositorglobal.h>

#include <QtCore/qplugin.h>
#include <QtCore/qfactoryinterface.h>
#include <QtCore/private/qglobal_p.h>

namespace Aurora {

namespace Compositor {

namespace Internal {

class ServerBufferIntegration;

#define AuroraServerBufferIntegrationFactoryInterface_iid "org.qt-project.Qt.Compositor.AuroraServerBufferIntegrationFactoryInterface.5.3"

class LIRIAURORACOMPOSITOR_EXPORT ServerBufferIntegrationPlugin : public QObject
{
    Q_OBJECT
public:
    explicit ServerBufferIntegrationPlugin(QObject *parent = nullptr);
    ~ServerBufferIntegrationPlugin() override;

    virtual ServerBufferIntegration *create(const QString &key, const QStringList &paramList) = 0;
};

}

} // namespace Compositor

} // namespace Aurora

#endif // AURORA_COMPOSITOR_WAYLANDSERVERBUFFERINTEGRATIONPLUGIN_H