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

#include "DatabaseSettingsPage.h"

#include <QPointer>
#include <QScopedPointer>

class Database;
namespace Ui
{
class DatabaseSettingsPageEncryption;
}

class DatabaseSettingsPageEncryption: public DatabaseSettingsPage
{
Q_OBJECT

public:
    explicit DatabaseSettingsPageEncryption(QWidget* parent = nullptr);
    Q_DISABLE_COPY(DatabaseSettingsPageEncryption);
    ~DatabaseSettingsPageEncryption() override;

    void initializePage() override;
    void uninitializePage() override;
    bool save() override;
    inline bool hasAdvancedMode() const override { return true; }
    void setAdvancedMode(bool advanced) override;

private slots:
    void transformRoundsBenchmark();
    void kdfChanged(int index);
    void memoryChanged(int value);
    void parallelismChanged(int value);
    void updateDecryptionTime(int value);
    void updateFormatCompatibility(int index);

private:
    void setupAlgorithmComboBox();
    void setupKdfComboBox();
    void loadKdfParameters();

    const QScopedPointer<Ui::DatabaseSettingsPageEncryption> m_ui;
};


#endif //KEEPASSXC_DATABASESETTINGSWIDGETENCRYPTION_H
