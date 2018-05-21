/*
 *  Copyright (C) 2018 KeePassXC Team <team@keepassxc.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "YubiKeyEditWidget.h"
#include "ui_YubiKeyEditWidget.h"
#include "gui/MessageBox.h"
#include "gui/MainWindow.h"
#include "keys/CompositeKey.h"
#include "keys/YkChallengeResponseKey.h"
#include "config-keepassx.h"

#include <QtConcurrent>

YubiKeyEditWidget::YubiKeyEditWidget(QWidget* parent)
    : KeyComponentWidget(parent)
    , m_compUi(new Ui::YubiKeyEditWidget())
{
    setComponentName(tr("YubiKey Challenge Response"));
}

YubiKeyEditWidget::~YubiKeyEditWidget()
{
}

bool YubiKeyEditWidget::addToCompositeKey(QSharedPointer<CompositeKey> key)
{
    Q_ASSERT(m_compEditWidget);
    if (!m_isValid || !m_compEditWidget) {
        return false;
    }

    int selectionIndex = m_compUi->comboChallengeResponse->currentIndex();
    int comboPayload = m_compUi->comboChallengeResponse->itemData(selectionIndex).toInt();

    if (0 == comboPayload) {
        MessageBox::critical(KEEPASSXC_MAIN_WINDOW,
                             tr("Error communicating with YubiKey"),
                             tr("Error communicating with YubiKey, please ensure it's plugged in."),
                             QMessageBox::Button::Ok);
        return false;
    }

    // read blocking mode from LSB and slot index number from second LSB
    bool blocking = static_cast<bool>(comboPayload & 1u);
    int slot = comboPayload >> 1u;
    auto crKey = QSharedPointer<YkChallengeResponseKey>(new YkChallengeResponseKey(slot, blocking));
    key->addChallengeResponseKey(crKey);

    return true;
}

bool YubiKeyEditWidget::validate(QString& errorMessage) const
{
    if (!m_isValid) {
        errorMessage = tr("No YubiKey detected, please ensure its plugged in.");
    }
    return m_isValid;
}

QWidget* YubiKeyEditWidget::componentEditWidget()
{
    m_compEditWidget = new QWidget();
    m_compUi->setupUi(m_compEditWidget);

    QSizePolicy sp = m_compUi->yubikeyProgress->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    m_compUi->yubikeyProgress->setSizePolicy(sp);
    m_compUi->yubikeyProgress->setVisible(false);

#ifdef WITH_XC_YUBIKEY
    connect(m_compUi->buttonRedetectYubikey, SIGNAL(clicked()), SLOT(pollYubikey()));

    connect(YubiKey::instance(), SIGNAL(detected(int, bool)), SLOT(yubikeyDetected(int, bool)), Qt::QueuedConnection);
    connect(YubiKey::instance(), SIGNAL(notFound()), SLOT(noYubikeyFound()), Qt::QueuedConnection);

    pollYubikey();
#endif

    return m_compEditWidget;
}

void YubiKeyEditWidget::pollYubikey()
{
#ifdef WITH_XC_YUBIKEY
    if (!m_compEditWidget) {
        return;
    }
    m_compUi->buttonRedetectYubikey->setEnabled(false);
    m_compUi->comboChallengeResponse->setEnabled(false);
    m_compUi->comboChallengeResponse->clear();
    m_compUi->yubikeyProgress->setVisible(true);

    // YubiKey init is slow, detect asynchronously to not block the UI
    QtConcurrent::run(YubiKey::instance(), &YubiKey::detect);
#endif
}

void YubiKeyEditWidget::yubikeyDetected(int slot, bool blocking)
{
#ifdef WITH_XC_YUBIKEY
    if (!m_compEditWidget) {
        return;
    }
    YkChallengeResponseKey yk(slot, blocking);
    m_compUi->comboChallengeResponse->clear();
    // add detected YubiKey to combo box and encode blocking mode in LSB, slot number in second LSB
    m_compUi->comboChallengeResponse->addItem(yk.getName(), QVariant((slot << 1u) | blocking));
    m_compUi->comboChallengeResponse->setEnabled(true);
    m_compUi->buttonRedetectYubikey->setEnabled(true);
    m_compUi->yubikeyProgress->setVisible(false);
    m_isValid = true;
#endif
}

void YubiKeyEditWidget::noYubikeyFound()
{
#ifdef WITH_XC_YUBIKEY
    if (!m_compEditWidget) {
        return;
    }
    m_compUi->comboChallengeResponse->clear();
    m_compUi->comboChallengeResponse->setEnabled(false);
    m_compUi->comboChallengeResponse->addItem(tr("No YubiKey inserted."));
    m_compUi->buttonRedetectYubikey->setEnabled(true);
    m_compUi->yubikeyProgress->setVisible(false);
    m_isValid = false;
#endif
}
