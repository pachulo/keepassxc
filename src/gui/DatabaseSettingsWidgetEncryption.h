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

#ifndef KEEPASSXC_DATABASESETTINGSWIDGETENCRYPTION_H
#define KEEPASSXC_DATABASESETTINGSWIDGETENCRYPTION_H

#include <QPointer>
#include <QScopedPointer>
#include <QWizardPage>

class Database;
namespace Ui
{
class DatabaseSettingsWidgetEncryption;
}

class DatabaseSettingsWidgetEncryption: public QWizardPage
{
Q_OBJECT

public:
    explicit DatabaseSettingsWidgetEncryption(Database* db = nullptr, QWidget* parent = nullptr);
    Q_DISABLE_COPY(DatabaseSettingsWidgetEncryption);
    ~DatabaseSettingsWidgetEncryption() override;

    void initializePage() override;
    void initializePage(Database* db);
    bool validatePage() override;

private slots:
    void transformRoundsBenchmark();
    void kdfChanged(int index);
    void memoryChanged(int value);
    void parallelismChanged(int value);

private:
    void setupAlgorithmComboBox();
    void setupKdfComboBox();
    void setupKdfParameterFields();

    const QScopedPointer<Ui::DatabaseSettingsWidgetEncryption> m_ui;
    QPointer<Database> m_db;
};


#endif //KEEPASSXC_DATABASESETTINGSWIDGETENCRYPTION_H
