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

#include "NewDatabaseWizard.h"
#include "NewDatabaseWizardPageMetaData.h"
#include "NewDatabaseWizardPageEncryption.h"
#include "../ChangeMasterKeyWidget.h"

#include "core/Global.h"
#include "core/Database.h"
#include "core/Group.h"
#include "core/FilePath.h"
#include "format/KeePass2.h"

#include <QVBoxLayout>

NewDatabaseWizard::NewDatabaseWizard(QWidget* parent)
    : QWizard(parent)
    , m_pages()
{
    setWizardStyle(QWizard::MacStyle);
    setOption(QWizard::WizardOption::HaveHelpButton, false);

    m_pages << new NewDatabaseWizardPageMetaData()
            << new NewDatabaseWizardPageEncryption();

    for (auto const& page: asConst(m_pages)) {
        addPage(page);
    }

//    m_changeKeyPage->setLayout(new QVBoxLayout());
//    m_changeKeyPage->layout()->addWidget(m_changeKeyWidget);
//    m_changeKeyPage->setTitle(tr("Set Master Key"));
//    m_changeKeyPage->setSubTitle(tr("As a last step, you need to set a strong master key to protect your database."));
//    addPage(m_changeKeyPage);

    // TODO: change image
    setPixmap(QWizard::BackgroundPixmap, filePath()->applicationIcon().pixmap(512, 512));
}

NewDatabaseWizard::~NewDatabaseWizard()
{
}

bool NewDatabaseWizard::validateCurrentPage()
{
    bool returnVal = QWizard::validateCurrentPage();

//    if (currentPage() == m_changeKeyPage && m_db) {
//        // TODO: set up and integrate change master key widget properly
//        m_db->setKey(m_changeKeyWidget->newMasterKey());
//    }

    return returnVal;
}

Database* NewDatabaseWizard::takeDatabase()
{
    return m_db.take();
}

void NewDatabaseWizard::initializePage(int id)
{
    if (id == startId()) {
        m_db.reset(new Database());
        m_db->rootGroup()->setName(tr("Root", "Root group"));

        CompositeKey emptyKey;
        m_db->setKey(emptyKey);
        m_db->setCipher(KeePass2::CIPHER_AES);
        m_db->setKdf(KeePass2::uuidToKdf(KeePass2::KDF_ARGON2));
    }

    m_pages[id]->setDatabase(m_db.data());
    m_pages[id]->initializePage();
}
