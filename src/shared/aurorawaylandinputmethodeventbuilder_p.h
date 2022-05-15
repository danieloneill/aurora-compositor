/****************************************************************************
**
** Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef AURORA_COMPOSITOR_WAYLANDINPUTMETHODEVENTBUILDER_H
#define AURORA_COMPOSITOR_WAYLANDINPUTMETHODEVENTBUILDER_H

#include <QInputMethodEvent>
#include <private/qglobal_p.h>

namespace Aurora {

namespace Compositor {

class WaylandInputMethodEventBuilder
{
public:
    WaylandInputMethodEventBuilder() = default;
    ~WaylandInputMethodEventBuilder();

    void reset();

    void setCursorPosition(int32_t index, int32_t anchor);
    void setDeleteSurroundingText(uint32_t beforeLength, uint32_t afterLength);

    void addPreeditStyling(uint32_t index, uint32_t length, uint32_t style);
    void setPreeditCursor(int32_t index);

    QInputMethodEvent *buildCommit(const QString &text);
    QInputMethodEvent *buildPreedit(const QString &text);

    static int indexFromWayland(const QString &text, int length, int base = 0);
    static int indexToWayland(const QString &text, int length, int base = 0);

    static int trimmedIndexFromWayland(const QString &text, int length, int base = 0);
private:
    QPair<int, int> replacementForDeleteSurrounding();

    int32_t m_anchor = 0;
    int32_t m_cursor = 0;
    uint32_t m_deleteBefore = 0;
    uint32_t m_deleteAfter = 0;

    int32_t m_preeditCursor = 0;
    QList<QInputMethodEvent::Attribute> m_preeditStyles;
};

struct WaylandInputMethodContentType {
    uint32_t hint = 0;
    uint32_t purpose = 0;

    static WaylandInputMethodContentType convert(Qt::InputMethodHints hints);
    static WaylandInputMethodContentType convertV4(Qt::InputMethodHints hints);
};


} // namespace Compositor

} // namespace Aurora

#endif // AURORA_COMPOSITOR_WAYLANDINPUTMETHODEVENTBUILDER_H
