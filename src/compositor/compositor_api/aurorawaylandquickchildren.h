// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#pragma once

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
#if LIRI_FEATURE_aurora_compositor_quick
#include <QtQml/QQmlListProperty>
#include <QtCore/QList>
#endif

namespace Aurora {

namespace Compositor {

#if LIRI_FEATURE_aurora_compositor_quick
#define AURORA_COMPOSITOR_DECLARE_QUICK_CHILDREN(className) \
        Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false) \
        Q_CLASSINFO("DefaultProperty", "data") \
    public: \
        QQmlListProperty<QObject> data() \
        { \
            return QQmlListProperty<QObject>(this, &m_children); \
        } \
    private: \
        QList<QObject *> m_children;
#else
#define AURORA_COMPOSITOR_DECLARE_QUICK_CHILDREN(className)
#endif

} // namespace Compositor

} // namespace Aurora

