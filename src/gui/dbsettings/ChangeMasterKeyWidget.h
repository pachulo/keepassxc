/*
 *  Copyright (C) 2012 Felix Geyer <debfx@fobos.de>
 *  Copyright (C) 2017 KeePassXC Team <team@keepassxc.org>
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

#ifndef KEEPASSX_CHANGEMASTERKEYWIDGET_H
#define KEEPASSX_CHANGEMASTERKEYWIDGET_H

#include "DatabaseSettingsPage.h"

#include <QScopedPointer>

class QLabel;
namespace Ui
{
    class ChangeMasterKeyWidget;
}

class KeyComponentWidget;
class Key;
class CompositeKey;
class ChallengeResponseKey;

class ChangeMasterKeyWidget : public DatabaseSettingsPage
{
    Q_OBJECT

public:
    explicit ChangeMasterKeyWidget(QWidget* parent = nullptr);
    Q_DISABLE_COPY(ChangeMasterKeyWidget);
    ~ChangeMasterKeyWidget() override;

    void load(Database* db) override;
    bool hasAdvancedMode() const override;

public slots:
    void initializePage() override;
    void uninitializePage() override;
    bool save() override;
    void discard() override;

private:
    bool addToCompositeKey(KeyComponentWidget* widget,
                           QSharedPointer<CompositeKey>& newKey,
                           QSharedPointer<Key>& oldKey);
    bool addToCompositeKey(KeyComponentWidget* widget,
                           QSharedPointer<CompositeKey>& newKey,
                           QSharedPointer<ChallengeResponseKey>& oldKey);

    const QScopedPointer<Ui::ChangeMasterKeyWidget> m_ui;
};

#endif // KEEPASSX_CHANGEMASTERKEYWIDGET_H
