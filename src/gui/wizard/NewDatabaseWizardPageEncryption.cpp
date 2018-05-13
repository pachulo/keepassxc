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

#include "NewDatabaseWizardPageEncryption.h"
#include "ui_NewDatabaseWizardPageEncryption.h"
#include "../DatabaseSettingsWidgetEncryption.h"
#include "core/AsyncTask.h"
#include "core/Database.h"
#include "core/Metadata.h"
#include "crypto/kdf/Argon2Kdf.h"
#include "format/KeePass2.h"

#include <QSlider>

NewDatabaseWizardPageEncryption::NewDatabaseWizardPageEncryption(Database* db, QWidget* parent)
    : QWizardPage(parent)
    , m_ui(new Ui::NewDatabaseWizardPageEncryption())
    , m_db(db)
    , m_advancedSettings(new DatabaseSettingsWidgetEncryption())
{
    m_ui->setupUi(this);

    m_ui->stackedWidget->addWidget(m_advancedSettings);

    m_ui->compatibilitySelection->addItem(tr("KDBX 4.0 (recommended)"), KeePass2::KDF_ARGON2.toByteArray());
    m_ui->compatibilitySelection->addItem(tr("KDBX 3.1"), KeePass2::KDF_AES_KDBX3.toByteArray());

    connect(m_ui->decryptionTimeSlider, SIGNAL(valueChanged(int)), SLOT(updateDecryptionTime(int)));
    connect(m_ui->advancedSettingsButton, SIGNAL(clicked()), SLOT(toggleAdvancedSettings()));
    connect(m_ui->compatibilitySelection, SIGNAL(currentIndexChanged(int)), SLOT(updateFormatCompatibility(int)));
}

NewDatabaseWizardPageEncryption::~NewDatabaseWizardPageEncryption()
{
}

void NewDatabaseWizardPageEncryption::initializePage()
{
    updateFormatCompatibility(0);
    m_ui->decryptionTimeSlider->setValue(1000);
    updateDecryptionTime(m_ui->decryptionTimeSlider->value());
    m_advancedSettings->initializePage(m_db);
}

void NewDatabaseWizardPageEncryption::initializePage(Database* db)
{
    m_db = db;
    initializePage();
}

bool NewDatabaseWizardPageEncryption::validatePage()
{
    if (m_db) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        auto kdf = m_db->kdf();
        int time = m_ui->decryptionTimeSlider->value();
        int rounds = AsyncTask::runAndWaitForFuture([&kdf, time]() { return kdf->benchmark(time); });
        QApplication::restoreOverrideCursor();

        kdf->setRounds(rounds);
    }

    return true;
}

void NewDatabaseWizardPageEncryption::toggleAdvancedSettings()
{
    if (m_ui->stackedWidget->currentIndex() == 0) {
        m_advancedSettings->initializePage(m_db);
        m_ui->stackedWidget->setCurrentIndex(1);
        m_ui->advancedSettingsButton->setText(tr("Simple Settings"));
    } else {
        m_ui->stackedWidget->setCurrentIndex(0);
        m_ui->advancedSettingsButton->setText(tr("Advanced Settings"));
    }
}

void NewDatabaseWizardPageEncryption::updateDecryptionTime(int value)
{
    if (value < 1000) {
        m_ui->decryptionTimeValueLabel->setText(tr("%1 ms", "milliseconds", value).arg(value));
    } else {
        m_ui->decryptionTimeValueLabel->setText(tr("%1 s", "seconds", value / 1000).arg(value / 1000.0, 0, 'f', 1));
    }
}

void NewDatabaseWizardPageEncryption::updateFormatCompatibility(int index)
{
    if (m_db) {
        auto kdfUuid = Uuid(m_ui->compatibilitySelection->itemData(index).toByteArray());
        auto kdf = KeePass2::uuidToKdf(kdfUuid);
        m_db->changeKdf(kdf);

        if (kdf->uuid() == KeePass2::KDF_ARGON2) {
            auto argon2Kdf = kdf.staticCast<Argon2Kdf>();
            argon2Kdf->setMemory(128 * 1024);
            argon2Kdf->setParallelism(static_cast<quint32>(QThread::idealThreadCount()));
        }
    }
}
