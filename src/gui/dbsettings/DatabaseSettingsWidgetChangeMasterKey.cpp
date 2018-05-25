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

#include "DatabaseSettingsWidgetChangeMasterKey.h"
#include "core/Database.h"
#include "keys/PasswordKey.h"
#include "keys/FileKey.h"
#include "keys/YkChallengeResponseKey.h"
#include "gui/MessageBox.h"
#include "gui/masterkey/PasswordEditWidget.h"
#include "gui/masterkey/KeyFileEditWidget.h"
#include "gui/masterkey/YubiKeyEditWidget.h"

#include <QVBoxLayout>

DatabaseSettingsWidgetChangeMasterKey::DatabaseSettingsWidgetChangeMasterKey(QWidget* parent)
    : DatabaseSettingsWidget(parent)
    , m_passwordEditWidget(new PasswordEditWidget(this))
    , m_keyFileEditWidget(new KeyFileEditWidget(this))
#ifdef WITH_XC_YUBIKEY
    , m_yubiKeyEditWidget(new YubiKeyEditWidget(this))
#endif
{
    setLayout(new QVBoxLayout());
    layout()->addWidget(m_passwordEditWidget);
    layout()->addWidget(m_keyFileEditWidget);
#ifdef WITH_XC_YUBIKEY
    layout()->addWidget(m_yubiKeyEditWidget);
#endif
}

DatabaseSettingsWidgetChangeMasterKey::~DatabaseSettingsWidgetChangeMasterKey()
{
}

void DatabaseSettingsWidgetChangeMasterKey::load(Database* db)
{
    DatabaseSettingsWidget::load(db);

    for (const auto& key: m_db->key()->keys()) {
        if (key->uuid() == PasswordKey::UUID) {
            m_passwordEditWidget->setComponentAdded(true);
        } else if (key->uuid() == FileKey::UUID) {
            m_keyFileEditWidget->setComponentAdded(true);
        }
    }

#ifdef WITH_XC_YUBIKEY
    for (const auto& key: m_db->key()->challengeResponseKeys()) {
        if (key->uuid() == YkChallengeResponseKey::UUID) {
            m_yubiKeyEditWidget->setComponentAdded(true);
        }
    }
#endif
}

void DatabaseSettingsWidgetChangeMasterKey::initialize()
{
    blockSignals(true);
    m_passwordEditWidget->setComponentAdded(false);
    m_keyFileEditWidget->setComponentAdded(false);
#ifdef WITH_XC_YUBIKEY
    m_yubiKeyEditWidget->setComponentAdded(false);
#endif
    blockSignals(false);
}

void DatabaseSettingsWidgetChangeMasterKey::uninitialize()
{
}

bool DatabaseSettingsWidgetChangeMasterKey::save()
{
    auto newKey = QSharedPointer<CompositeKey>::create();

    QSharedPointer<Key> passwordKey;
    QSharedPointer<Key> fileKey;
    QSharedPointer<ChallengeResponseKey> ykCrKey;

    for (const auto& key: m_db->key()->keys()) {
        if (key->uuid() == PasswordKey::UUID) {
            passwordKey = key;
        } else if (key->uuid() == FileKey::UUID) {
            fileKey = key;
        }
    }

    for (const auto& key: m_db->key()->challengeResponseKeys()) {
        if (key->uuid() == YkChallengeResponseKey::UUID) {
            ykCrKey = key;
        }
    }

    if (!addToCompositeKey(m_passwordEditWidget, newKey, passwordKey)) {
        return false;
    }

    if (!addToCompositeKey(m_keyFileEditWidget, newKey, fileKey)) {
        return false;
    }

#ifdef WITH_XC_YUBIKEY
    if (!addToCompositeKey(m_yubiKeyEditWidget, newKey, ykCrKey)) {
        return false;
    }
#endif

    if (newKey->keys().isEmpty() && newKey->challengeResponseKeys().isEmpty()) {
        MessageBox::critical(this, tr("No encryption key added"),
                             tr("You must add at least one encryption key to secure your database!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    if (m_passwordEditWidget->visiblePage() == KeyComponentWidget::AddNew) {
        auto answer = MessageBox::warning(this, tr("No password set"),
                                          tr("WARNING! You have not set a password. Using a database without "
                                             "a password is strongly discouraged!\n\n"
                                             "Are you sure you want to continue without a password?"),
                                          QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (answer != QMessageBox::Yes) {
            return false;
        }
    }

    m_db->setKey(newKey);

    emit editFinished(true);
    return true;
}

void DatabaseSettingsWidgetChangeMasterKey::discard()
{
    emit editFinished(false);
}

bool DatabaseSettingsWidgetChangeMasterKey::addToCompositeKey(KeyComponentWidget* widget, QSharedPointer<CompositeKey>& newKey,
                                              QSharedPointer<Key>& oldKey)
{
    if (widget->visiblePage() == KeyComponentWidget::Edit) {
        QString error = tr("Unknown error");
        if (!widget->validate(error) || !widget->addToCompositeKey(newKey)) {
            QMessageBox::critical(this, tr("Failed to change master key"), error, QMessageBox::Ok);
            return false;
        }
    } else if (widget->visiblePage() == KeyComponentWidget::LeaveOrRemove) {
        Q_ASSERT(oldKey);
        newKey->addKey(oldKey);
    }
    return true;
}

bool DatabaseSettingsWidgetChangeMasterKey::addToCompositeKey(KeyComponentWidget* widget, QSharedPointer<CompositeKey>& newKey,
                                              QSharedPointer<ChallengeResponseKey>& oldKey)
{
    if (widget->visiblePage() == KeyComponentWidget::Edit) {
        QString error = tr("Unknown error");
        if (!widget->validate(error) || !widget->addToCompositeKey(newKey)) {
            QMessageBox::critical(this, tr("Failed to change master key"), error, QMessageBox::Ok);
            return false;
        }
    } else if (widget->visiblePage() == KeyComponentWidget::LeaveOrRemove) {
        Q_ASSERT(oldKey);
        newKey->addChallengeResponseKey(oldKey);
    }
    return true;
}
