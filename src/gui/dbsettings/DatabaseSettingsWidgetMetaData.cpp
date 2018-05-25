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

#include "DatabaseSettingsWidgetMetaData.h"
#include "ui_DatabaseSettingsWidgetMetaData.h"
#include "core/Database.h"
#include "core/Metadata.h"

DatabaseSettingWidgetMetaData::DatabaseSettingWidgetMetaData(QWidget* parent)
    : DatabaseSettingsWidget(parent)
    , m_ui(new Ui::DatabaseSettingsWidgetMetaData())
{
    m_ui->setupUi(this);
}

DatabaseSettingWidgetMetaData::~DatabaseSettingWidgetMetaData()
{
}

void DatabaseSettingWidgetMetaData::initialize()
{
    Metadata* meta = m_db->metadata();
    auto name = meta->name();
    m_ui->databaseName->setText(name.isEmpty() ? tr("Passwords") : name);
    m_ui->databaseDescription->setText(meta->description());
    m_ui->databaseDefaultUsername->setText(meta->defaultUserName());

    m_ui->databaseName->setFocus();
    m_ui->databaseName->selectAll();
}

void DatabaseSettingWidgetMetaData::uninitialize()
{
}

bool DatabaseSettingWidgetMetaData::save()
{
    Metadata* meta = m_db->metadata();
    meta->setName(m_ui->databaseName->text());
    meta->setDescription(m_ui->databaseDescription->text());
    meta->setDefaultUserName(m_ui->databaseDefaultUsername->text());

    return true;
}
