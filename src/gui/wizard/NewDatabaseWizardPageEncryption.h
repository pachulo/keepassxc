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

#ifndef KEEPASSXC_NEWDATABASEWIZARDPAGEENCRYPTION_H
#define KEEPASSXC_NEWDATABASEWIZARDPAGEENCRYPTION_H

#include <QPointer>
#include <QScopedPointer>
#include <QWizardPage>

class Database;
class DatabaseSettingsWidgetEncryption;
namespace Ui
{
class NewDatabaseWizardPageEncryption;
}

class NewDatabaseWizardPageEncryption : public QWizardPage
{
Q_OBJECT

public:
    explicit NewDatabaseWizardPageEncryption(Database* db = nullptr, QWidget* parent = nullptr);
    Q_DISABLE_COPY(NewDatabaseWizardPageEncryption);
    ~NewDatabaseWizardPageEncryption() override;

    void initializePage() override;
    void initializePage(Database* db);
    bool validatePage() override;

private slots:
    void toggleAdvancedSettings();
    void updateDecryptionTime(int value);
    void updateFormatCompatibility(int index);

private:
    const QScopedPointer<Ui::NewDatabaseWizardPageEncryption> m_ui;
    QPointer<Database> m_db;
    QPointer<DatabaseSettingsWidgetEncryption> m_advancedSettings;
};

#endif //KEEPASSXC_NEWDATABASEWIZARDPAGEENCRYPTION_H
