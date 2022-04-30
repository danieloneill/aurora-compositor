/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
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

#include "aurorawaylandtextinputv4.h"
#include "aurorawaylandtextinputv4_p.h"

#include <LiriAuroraCompositor/WaylandCompositor>
#include <LiriAuroraCompositor/private/aurorawaylandseat_p.h>

#include "aurorawaylandsurface.h"
#include "aurorawaylandview.h"
#include "aurorawaylandinputmethodeventbuilder_p.h"

#include <QGuiApplication>
#include <QInputMethodEvent>
#include <qpa/qwindowsysteminterface.h>

#if LIRI_FEATURE_aurora_xkbcommon
#include <LiriAuroraXkbCommonSupport/private/qxkbcommon_p.h>
#endif

namespace Aurora {

namespace Compositor {

Q_DECLARE_LOGGING_CATEGORY(qLcWaylandCompositorTextInput)

WaylandTextInputV4ClientState::WaylandTextInputV4ClientState()
{
}

Qt::InputMethodQueries WaylandTextInputV4ClientState::updatedQueries(const WaylandTextInputV4ClientState &other) const
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Qt::InputMethodQueries queries;

    if (hints != other.hints)
        queries |= Qt::ImHints;
    if (cursorRectangle != other.cursorRectangle)
        queries |= Qt::ImCursorRectangle;
    if (surroundingText != other.surroundingText)
        queries |= Qt::ImSurroundingText | Qt::ImCurrentSelection;
    if (cursorPosition != other.cursorPosition)
        queries |= Qt::ImCursorPosition | Qt::ImCurrentSelection;
    if (anchorPosition != other.anchorPosition)
        queries |= Qt::ImAnchorPosition | Qt::ImCurrentSelection;

    return queries;
}

Qt::InputMethodQueries WaylandTextInputV4ClientState::mergeChanged(const WaylandTextInputV4ClientState &other) {

    Qt::InputMethodQueries queries;

    if ((other.changedState & Qt::ImHints) && hints != other.hints) {
        hints = other.hints;
        queries |= Qt::ImHints;
    }

    if ((other.changedState & Qt::ImCursorRectangle) && cursorRectangle != other.cursorRectangle) {
        cursorRectangle = other.cursorRectangle;
        queries |= Qt::ImCursorRectangle;
    }

    if ((other.changedState & Qt::ImSurroundingText) && surroundingText != other.surroundingText) {
        surroundingText = other.surroundingText;
        queries |= Qt::ImSurroundingText | Qt::ImCurrentSelection;
    }

    if ((other.changedState & Qt::ImCursorPosition) && cursorPosition != other.cursorPosition) {
        cursorPosition = other.cursorPosition;
        queries |= Qt::ImCursorPosition | Qt::ImCurrentSelection;
    }

    if ((other.changedState & Qt::ImAnchorPosition) && anchorPosition != other.anchorPosition) {
        anchorPosition = other.anchorPosition;
        queries |= Qt::ImAnchorPosition | Qt::ImCurrentSelection;
    }

    return queries;
}

WaylandTextInputV4Private::WaylandTextInputV4Private(WaylandCompositor *compositor)
    : compositor(compositor)
    , currentState(new WaylandTextInputV4ClientState)
    , pendingState(new WaylandTextInputV4ClientState)
{
}

void WaylandTextInputV4Private::sendInputMethodEvent(QInputMethodEvent *event)
{
    Q_Q(WaylandTextInputV4);
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    if (!focusResource || !focusResource->handle)
        return;

    bool needsDone = false;

    const QString &newPreeditString = event->preeditString();

    // Current cursor shape is only line. It means both cursorBegin
    // and cursorEnd will be the same values.
    int32_t preeditCursorPos = newPreeditString.length();

    if (event->replacementLength() > 0 || event->replacementStart() < 0) {
        if (event->replacementStart() <= 0 && (event->replacementLength() >= -event->replacementStart())) {
            const int selectionStart = qMin(currentState->cursorPosition, currentState->anchorPosition);
            const int selectionEnd = qMax(currentState->cursorPosition, currentState->anchorPosition);
            const int before = WaylandInputMethodEventBuilder::indexToWayland(currentState->surroundingText, -event->replacementStart(), selectionStart + event->replacementStart());
            const int after = WaylandInputMethodEventBuilder::indexToWayland(currentState->surroundingText, event->replacementLength() + event->replacementStart(), selectionEnd);
            send_delete_surrounding_text(focusResource->handle, before, after);
            needsDone = true;
        } else {
            qCWarning(qLcWaylandCompositorTextInput) << "Not yet supported case of replacement. Start:" << event->replacementStart() << "length:" << event->replacementLength();
        }
        preeditCursorPos = event->replacementStart() + event->replacementLength();
    }

    if (currentPreeditString != newPreeditString) {
        currentPreeditString = newPreeditString;
        send_preedit_string(focusResource->handle, currentPreeditString, preeditCursorPos, preeditCursorPos);
        needsDone = true;
    }
    if (!event->commitString().isEmpty()) {
        send_commit_string(focusResource->handle, event->commitString());
        needsDone = true;
    }

    if (needsDone)
        send_done(focusResource->handle, serial);
}


void WaylandTextInputV4Private::sendKeyEvent(QKeyEvent *event)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_Q(WaylandTextInputV4);

    if (!focusResource || !focusResource->handle)
        return;

    send_commit_string(focusResource->handle, event->text());

    send_done(focusResource->handle, serial);
}

QVariant WaylandTextInputV4Private::inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO << property;

    switch (property) {
    case Qt::ImHints:
        return QVariant(static_cast<int>(currentState->hints));
    case Qt::ImCursorRectangle:
        return currentState->cursorRectangle;
    case Qt::ImFont:
        // Not supported
        return QVariant();
    case Qt::ImCursorPosition:
        qCDebug(qLcWaylandCompositorTextInput) << currentState->cursorPosition;
        return currentState->cursorPosition;
    case Qt::ImSurroundingText:
        qCDebug(qLcWaylandCompositorTextInput) << currentState->surroundingText;
        return currentState->surroundingText;
    case Qt::ImCurrentSelection:
        return currentState->surroundingText.mid(qMin(currentState->cursorPosition, currentState->anchorPosition),
                                                 qAbs(currentState->anchorPosition - currentState->cursorPosition));
    case Qt::ImMaximumTextLength:
        // Not supported
        return QVariant();
    case Qt::ImAnchorPosition:
        qCDebug(qLcWaylandCompositorTextInput) << currentState->anchorPosition;
        return currentState->anchorPosition;
    case Qt::ImAbsolutePosition:
        // We assume the surrounding text is our whole document for now
        return currentState->cursorPosition;
    case Qt::ImTextAfterCursor:
        if (argument.isValid())
            return currentState->surroundingText.mid(currentState->cursorPosition, argument.toInt());
        return currentState->surroundingText.mid(currentState->cursorPosition);
    case Qt::ImTextBeforeCursor:
        if (argument.isValid())
            return currentState->surroundingText.left(currentState->cursorPosition).right(argument.toInt());
        return currentState->surroundingText.left(currentState->cursorPosition);

    default:
        return QVariant();
    }
}

void WaylandTextInputV4Private::setFocus(WaylandSurface *surface)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;
    Q_Q(WaylandTextInputV4);

    if (focusResource && focus) {
        // sync before leave
        // IBUS commits by itself but qtvirtualkeyboard doesn't
        // And when handling chinese input, it is required to commit
        // before leaving the focus.
        if (qgetenv("QT_IM_MODULE") != QByteArrayLiteral("ibus")
                || qApp->inputMethod()->locale().language() == QLocale::Chinese) {
            qApp->inputMethod()->commit();
        }

        qApp->inputMethod()->hide();
        inputPanelVisible = false;
        send_leave(focusResource->handle, focus->resource());
        currentPreeditString.clear();
    }

    if (focus != surface)
        focusDestroyListener.reset();

    Resource *resource = surface ? resourceMap().value(surface->waylandClient()) : 0;
    if (resource && surface) {
        send_enter(resource->handle, surface->resource());

        if (focus != surface)
            focusDestroyListener.listenForDestruction(surface->resource());
    }

    focus = surface;
    focusResource = resource;
}

void WaylandTextInputV4Private::zwp_text_input_v4_bind_resource(Resource *resource)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_UNUSED(resource);
}

void WaylandTextInputV4Private::zwp_text_input_v4_destroy_resource(Resource *resource)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    if (focusResource == resource)
        focusResource = nullptr;
}

void WaylandTextInputV4Private::zwp_text_input_v4_destroy(Resource *resource)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    wl_resource_destroy(resource->handle);
}

void WaylandTextInputV4Private::zwp_text_input_v4_enable(Resource *resource)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_Q(WaylandTextInputV4);

    pendingState.reset(new WaylandTextInputV4ClientState);

    enabledSurfaces.insert(resource, focus);
    emit q->surfaceEnabled(focus);

    serial = 0;
    inputPanelVisible = true;
    qApp->inputMethod()->show();
}

void WaylandTextInputV4Private::zwp_text_input_v4_disable(PrivateServer::zwp_text_input_v4::Resource *resource)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_Q(WaylandTextInputV4);

    WaylandSurface *s = enabledSurfaces.take(resource);
    emit q->surfaceDisabled(s);

    // When reselecting a word by setFocus
    if (qgetenv("QT_IM_MODULE") != QByteArrayLiteral("ibus")
            || qApp->inputMethod()->locale().language() == QLocale::Chinese) {
        qApp->inputMethod()->commit();
    }
    qApp->inputMethod()->reset();
    pendingState.reset(new WaylandTextInputV4ClientState);
}

void WaylandTextInputV4Private::zwp_text_input_v4_set_cursor_rectangle(Resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO << x << y << width << height;

    Q_Q(WaylandTextInputV4);

    if (resource != focusResource)
        return;

    pendingState->cursorRectangle = QRect(x, y, width, height);

    pendingState->changedState |= Qt::ImCursorRectangle;
}

void WaylandTextInputV4Private::zwp_text_input_v4_commit(Resource *resource)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_Q(WaylandTextInputV4);

    if (resource != focusResource) {
        qCDebug(qLcWaylandCompositorTextInput) << "OBS: Disabled surface!!";
        return;
    }

    serial = serial < UINT_MAX ? serial + 1U : 0U;

    // Just increase serials and ignore empty commits
    if (!pendingState->changedState) {
        qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO << "pendingState is not changed";
        return;
    }

    // Selection starts.
    // But since qtvirtualkeyboard with hunspell does not reset its preedit string,
    // compositor forces to reset inputMethod.
    if ((currentState->cursorPosition == currentState->anchorPosition)
            && (pendingState->cursorPosition != pendingState->anchorPosition))
        qApp->inputMethod()->reset();

    // Enable reselection
    // This is a workaround to make qtvirtualkeyboad's state empty by clearing State::InputMethodClick.
    if (currentState->surroundingText == pendingState->surroundingText && currentState->cursorPosition != pendingState->cursorPosition)
        qApp->inputMethod()->invokeAction(QInputMethod::Click, pendingState->cursorPosition);

    Qt::InputMethodQueries queries = currentState->mergeChanged(*pendingState.data());
    pendingState.reset(new WaylandTextInputV4ClientState);

    if (queries) {
        qCDebug(qLcWaylandCompositorTextInput) << "QInputMethod::update() after commit with" << queries;

        qApp->inputMethod()->update(queries);
    }
}

void WaylandTextInputV4Private::zwp_text_input_v4_set_content_type(Resource *resource, uint32_t hint, uint32_t purpose)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO << hint << purpose;

    if (resource != focusResource)
        return;

    pendingState->hints = Qt::ImhNone;

    if ((hint & content_hint_completion) == 0)
        pendingState->hints |= Qt::ImhNoPredictiveText;
    if ((hint & content_hint_auto_capitalization) == 0)
        pendingState->hints |= Qt::ImhNoAutoUppercase;
    if ((hint & content_hint_lowercase) != 0)
        pendingState->hints |= Qt::ImhPreferLowercase;
    if ((hint & content_hint_uppercase) != 0)
        pendingState->hints |= Qt::ImhPreferUppercase;
    if ((hint & content_hint_hidden_text) != 0)
        pendingState->hints |= Qt::ImhHiddenText;
    if ((hint & content_hint_sensitive_data) != 0)
        pendingState->hints |= Qt::ImhSensitiveData;
    if ((hint & content_hint_latin) != 0)
        pendingState->hints |= Qt::ImhLatinOnly;
    if ((hint & content_hint_multiline) != 0)
        pendingState->hints |= Qt::ImhMultiLine;

    switch (purpose) {
    case content_purpose_normal:
        break;
    case content_purpose_alpha:
        pendingState->hints |= Qt::ImhUppercaseOnly | Qt::ImhLowercaseOnly;
        break;
    case content_purpose_digits:
        pendingState->hints |= Qt::ImhDigitsOnly;
        break;
    case content_purpose_number:
        pendingState->hints |= Qt::ImhFormattedNumbersOnly;
        break;
    case content_purpose_phone:
        pendingState->hints |= Qt::ImhDialableCharactersOnly;
        break;
    case content_purpose_url:
        pendingState->hints |= Qt::ImhUrlCharactersOnly;
        break;
    case content_purpose_email:
        pendingState->hints |= Qt::ImhEmailCharactersOnly;
        break;
    case content_purpose_name:
    case content_purpose_password:
        break;
    case content_purpose_date:
        pendingState->hints |= Qt::ImhDate;
        break;
    case content_purpose_time:
        pendingState->hints |= Qt::ImhTime;
        break;
    case content_purpose_datetime:
        pendingState->hints |= Qt::ImhDate | Qt::ImhTime;
        break;
    case content_purpose_terminal:
    default:
        break;
    }

    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO << pendingState->hints;

    pendingState->changedState |= Qt::ImHints;
}

void WaylandTextInputV4Private::zwp_text_input_v4_set_surrounding_text(Resource *resource, const QString &text, int32_t cursor, int32_t anchor)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO << text << cursor << anchor;

    if (resource != focusResource)
        return;

    pendingState->surroundingText = text;
    pendingState->cursorPosition = WaylandInputMethodEventBuilder::indexFromWayland(text, cursor);
    pendingState->anchorPosition = WaylandInputMethodEventBuilder::indexFromWayland(text, anchor);

    pendingState->changedState |= Qt::ImSurroundingText | Qt::ImCursorPosition | Qt::ImAnchorPosition;
}

void WaylandTextInputV4Private::zwp_text_input_v4_set_text_change_cause(Resource *resource, uint32_t cause)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_UNUSED(resource);
    Q_UNUSED(cause);
}

WaylandTextInputV4::WaylandTextInputV4(WaylandObject *container, WaylandCompositor *compositor)
    : WaylandCompositorExtensionTemplate(container, *new WaylandTextInputV4Private(compositor))
{
    connect(&d_func()->focusDestroyListener, &WaylandDestroyListener::fired,
            this, &WaylandTextInputV4::focusSurfaceDestroyed);
}

WaylandTextInputV4::~WaylandTextInputV4()
{
}

void WaylandTextInputV4::sendInputMethodEvent(QInputMethodEvent *event)
{
    Q_D(WaylandTextInputV4);

    d->sendInputMethodEvent(event);
}

void WaylandTextInputV4::sendKeyEvent(QKeyEvent *event)
{
    Q_D(WaylandTextInputV4);

    d->sendKeyEvent(event);
}

QVariant WaylandTextInputV4::inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const
{
    const Q_D(WaylandTextInputV4);

    return d->inputMethodQuery(property, argument);
}

WaylandSurface *WaylandTextInputV4::focus() const
{
    const Q_D(WaylandTextInputV4);

    return d->focus;
}

void WaylandTextInputV4::setFocus(WaylandSurface *surface)
{
    Q_D(WaylandTextInputV4);

    d->setFocus(surface);
}

void WaylandTextInputV4::focusSurfaceDestroyed(void *)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_D(WaylandTextInputV4);

    d->focusDestroyListener.reset();

    d->focus = nullptr;
    d->focusResource = nullptr;
}

bool WaylandTextInputV4::isSurfaceEnabled(WaylandSurface *surface) const
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    const Q_D(WaylandTextInputV4);

    return d->enabledSurfaces.values().contains(surface);
}

void WaylandTextInputV4::add(::wl_client *client, uint32_t id, int version)
{
    qCDebug(qLcWaylandCompositorTextInput) << Q_FUNC_INFO;

    Q_D(WaylandTextInputV4);

    d->add(client, id, version);
}

const wl_interface *WaylandTextInputV4::interface()
{
    return WaylandTextInputV4Private::interface();
}

QByteArray WaylandTextInputV4::interfaceName()
{
    return WaylandTextInputV4Private::interfaceName();
}

} // namespace Compositor

} // namespace Aurora