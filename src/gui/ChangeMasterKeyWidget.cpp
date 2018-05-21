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
#include "masterkey/PasswordEditWidget.h"
#include "masterkey/KeyFileEditWidget.h"
#include "masterkey/YubiKeyEditWidget.h"

#include <QPushButton>

ChangeMasterKeyWidget::ChangeMasterKeyWidget(QWidget* parent)
    : DialogyWidget(parent)
    , m_ui(new Ui::ChangeMasterKeyWidget())
{
    m_ui->setupUi(this);

    connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(generateKey()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));

#ifndef WITH_XC_YUBIKEY
    m_ui->yubiKeyEditWidget->setVisible(false);
#endif
}

ChangeMasterKeyWidget::~ChangeMasterKeyWidget()
{
}

void ChangeMasterKeyWidget::clearForms()
{
}

QSharedPointer<CompositeKey> ChangeMasterKeyWidget::newMasterKey()
{
    return QSharedPointer<CompositeKey>::create();
}

QLabel* ChangeMasterKeyWidget::headlineLabel()
{
    return m_ui->headlineLabel;
}

void ChangeMasterKeyWidget::generateKey()
{
    emit editFinished(true);
}

void ChangeMasterKeyWidget::reject()
{
    emit editFinished(false);
}

void ChangeMasterKeyWidget::setCancelEnabled(bool enabled)
{
    m_ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(enabled);
}
