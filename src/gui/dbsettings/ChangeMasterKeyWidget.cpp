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

#include "ChangeMasterKeyWidget.h"
#include "ui_ChangeMasterKeyWidget.h"
#include "config-keepassx.h"
#include "core/Database.h"
#include "keys/PasswordKey.h"
#include "keys/FileKey.h"
#include "keys/YkChallengeResponseKey.h"
#include "gui/MessageBox.h"
#include "gui/masterkey/PasswordEditWidget.h"
#include "gui/masterkey/KeyFileEditWidget.h"
#include "gui/masterkey/YubiKeyEditWidget.h"

ChangeMasterKeyWidget::ChangeMasterKeyWidget(QWidget* parent)
    : DatabaseSettingsPage(parent)
    , m_ui(new Ui::ChangeMasterKeyWidget())
{
    m_ui->setupUi(this);

    connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(save()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), SLOT(discard()));

#ifndef WITH_XC_YUBIKEY
    m_ui->yubiKeyEditWidget->setVisible(false);
#endif
}

ChangeMasterKeyWidget::~ChangeMasterKeyWidget()
{
}

void ChangeMasterKeyWidget::load(Database* db)
{
    DatabaseSettingsPage::load(db);

    for (const auto& key: m_db->key()->keys()) {
        if (key->uuid() == PasswordKey::UUID) {
            m_ui->passwordEditWidget->setComponentAdded(true);
        } else if (key->uuid() == FileKey::UUID) {
            m_ui->keyFileEditWidget->setComponentAdded(true);
        }
    }

    for (const auto& key: m_db->key()->challengeResponseKeys()) {
        if (key->uuid() == YkChallengeResponseKey::UUID) {
            m_ui->yubiKeyEditWidget->setComponentAdded(true);
        }
    }
}

void ChangeMasterKeyWidget::initializePage()
{
    blockSignals(true);
    m_ui->passwordEditWidget->setComponentAdded(false);
    m_ui->keyFileEditWidget->setComponentAdded(false);
    m_ui->yubiKeyEditWidget->setComponentAdded(false);
    blockSignals(false);
}

void ChangeMasterKeyWidget::uninitializePage()
{
}

bool ChangeMasterKeyWidget::save()
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

    if (!addToCompositeKey(m_ui->passwordEditWidget, newKey, passwordKey)) {
        return false;
    }

    if (!addToCompositeKey(m_ui->keyFileEditWidget, newKey, fileKey)) {
        return false;
    }

    if (!addToCompositeKey(m_ui->yubiKeyEditWidget, newKey, ykCrKey)) {
        return false;
    }

    if (newKey->keys().isEmpty() && newKey->challengeResponseKeys().isEmpty()) {
        MessageBox::critical(this, tr("No encryption key added"),
                             tr("You must add at least one encryption key to secure your database!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    if (m_ui->passwordEditWidget->visiblePage() == KeyComponentWidget::AddNew) {
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

void ChangeMasterKeyWidget::discard()
{
    emit editFinished(false);
}

bool ChangeMasterKeyWidget::hasAdvancedMode() const
{
    return false;
}

bool ChangeMasterKeyWidget::addToCompositeKey(KeyComponentWidget* widget, QSharedPointer<CompositeKey>& newKey,
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

bool ChangeMasterKeyWidget::addToCompositeKey(KeyComponentWidget* widget, QSharedPointer<CompositeKey>& newKey,
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
