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

#include "PasswordEditWidget.h"
#include "ui_PasswordEditWidget.h"
#include "core/FilePath.h"
#include "keys/PasswordKey.h"
#include "keys/CompositeKey.h"

PasswordEditWidget::PasswordEditWidget(QWidget* parent)
    : KeyComponentWidget(parent)
    , m_compUi(new Ui::PasswordEditWidget())
{
    setComponentName(tr("Password"));
}

PasswordEditWidget::~PasswordEditWidget()
{
}

bool PasswordEditWidget::addToCompositeKey(QSharedPointer<CompositeKey> key)
{
    key->addKey(QSharedPointer<PasswordKey>::create(m_compUi->enterPasswordEdit->text()));
    return true;
}

QWidget* PasswordEditWidget::componentEditWidget()
{
    m_compEditWidget = new QWidget();
    m_compUi->setupUi(m_compEditWidget);
    m_compUi->togglePasswordButton->setIcon(filePath()->onOffIcon("actions", "password-show"));
    m_compUi->repeatPasswordEdit->enableVerifyMode(m_compUi->enterPasswordEdit);

    connect(m_compUi->togglePasswordButton, SIGNAL(toggled(bool)), m_compUi->enterPasswordEdit, SLOT(setShowPassword(bool)));

    return m_compEditWidget;
}

bool PasswordEditWidget::validate(QString& errorMessage) const
{
    if (m_compUi->enterPasswordEdit->text().isEmpty()) {
        errorMessage = tr("Password cannot be empty.");
        return false;
    }

    if (m_compUi->enterPasswordEdit->text() != m_compUi->repeatPasswordEdit->text()) {
        errorMessage = tr("Passwords do not match.");
        return false;
    }

    return true;
}
