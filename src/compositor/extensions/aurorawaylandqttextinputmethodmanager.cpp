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

#include "aurorawaylandqttextinputmethodmanager.h"
#include "aurorawaylandqttextinputmethodmanager_p.h"

#include <LiriAuroraCompositor/WaylandCompositor>
#include <LiriAuroraCompositor/WaylandSeat>

#include "aurorawaylandqttextinputmethod.h"

namespace Aurora {

namespace Compositor {

WaylandQtTextInputMethodManagerPrivate::WaylandQtTextInputMethodManagerPrivate()
{
}

void WaylandQtTextInputMethodManagerPrivate::text_input_method_manager_v1_get_text_input_method(Resource *resource, uint32_t id, struct ::wl_resource *seatResource)
{
    Q_Q(WaylandQtTextInputMethodManager);
    WaylandCompositor *compositor = static_cast<WaylandCompositor *>(q->extensionContainer());
    WaylandSeat *seat = WaylandSeat::fromSeatResource(seatResource);
    WaylandQtTextInputMethod *textInput = WaylandQtTextInputMethod::findIn(seat);
    if (textInput == nullptr)
        textInput = new WaylandQtTextInputMethod(seat, compositor);
    textInput->add(resource->client(), id, wl_resource_get_version(resource->handle));
    WaylandClient *client = WaylandClient::fromWlClient(compositor, resource->client());
    WaylandClient::TextInputProtocols p = client->textInputProtocols();
    client->setTextInputProtocols(p.setFlag(WaylandClient::TextInputProtocol::QtTextInputMethodV1));

    if (!textInput->isInitialized())
        textInput->initialize();
}

/*!
  \qmltype QtTextInputMethodManager
  \instantiates WaylandQtTextInputMethodManager
  \inqmlmodule Aurora.Compositor
  \since 6.0
  \brief Provides access to input methods in the compositor.

  The \c QtTextInputMethodManager corresponds to the \c qt-text-input-method-manager interface
  in the \c qt-text-input-method-unstable-v1 extension protocol. It is specifically designed
  to be used with a Qt-based input method, such as Qt Virtual Keyboard.

  Instantiating this as child of a \l WaylandCompositor adds it to the list of interfaces available
  to the client. If a client binds to it, then it will be used to communciate text input to
  that client.
*/

/*!
   \class WaylandQtTextInputMethodManager
   \inmodule AuroraCompositor
   \since 6.0
   \brief Provides access to input methods in the compositor.

   The \c WaylandQtTextInputMethodManager class corresponds to the \c qt-text-input-method-manager interface
   in the \c qt-text-input-method-unstable-v1 extension protocol. It is specifically designed
   to be used with a Qt-based input method, such as Qt Virtual Keyboard.

  Instantiating this as child of a \l WaylandCompositor adds it to the list of interfaces available
  to the client. If a client binds to it, then it will be used to communciate text input to
  that client.
*/

WaylandQtTextInputMethodManager::WaylandQtTextInputMethodManager()
    : WaylandCompositorExtensionTemplate<WaylandQtTextInputMethodManager>(*new WaylandQtTextInputMethodManagerPrivate)
{
}

WaylandQtTextInputMethodManager::WaylandQtTextInputMethodManager(WaylandCompositor *compositor)
    : WaylandCompositorExtensionTemplate<WaylandQtTextInputMethodManager>(compositor, *new WaylandQtTextInputMethodManagerPrivate)
{
}

void WaylandQtTextInputMethodManager::initialize()
{
    Q_D(WaylandQtTextInputMethodManager);

    WaylandCompositorExtensionTemplate::initialize();
    WaylandCompositor *compositor = static_cast<WaylandCompositor *>(extensionContainer());
    if (compositor == nullptr) {
        qWarning() << "Failed to find WaylandCompositor when initializing WaylandQtTextInputMethodManager";
        return;
    }

    d->init(compositor->display(), 1);
}

const wl_interface *WaylandQtTextInputMethodManager::interface()
{
    return WaylandQtTextInputMethodManagerPrivate::interface();
}

QByteArray WaylandQtTextInputMethodManager::interfaceName()
{
    return WaylandQtTextInputMethodManagerPrivate::interfaceName();
}

} // namespace Compositor

} // namespace Aurora