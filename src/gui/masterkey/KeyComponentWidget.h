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

#ifndef KEEPASSXC_KEYCOMPONENTWIDGET_H
#define KEEPASSXC_KEYCOMPONENTWIDGET_H

#include <QStackedWidget>
#include <QScopedPointer>

namespace Ui
{
class KeyComponentWidget;
}
class CompositeKey;

class KeyComponentWidget : public QStackedWidget
{
    Q_OBJECT
    Q_PROPERTY(QString componentName READ m_componentName READ componentName WRITE setComponentName NOTIFY nameChanged)
    Q_PROPERTY(bool componentAdded READ m_isComponentAdded READ componentAdded WRITE setComponentAdded NOTIFY componentAddChanged)

public:
    enum Page
    {
        AddNew = 0,
        Edit = 1,
        LeaveOrRemove = 2
    };

    explicit KeyComponentWidget(QWidget* parent = nullptr);
    explicit KeyComponentWidget(const QString& name, QWidget* parent = nullptr);
    Q_DISABLE_COPY(KeyComponentWidget);
    ~KeyComponentWidget();

    /**
     * Add the new key component to the given \link CompositeKey.
     * A caller should always check first with \link validate() if
     * the new key data is actually valid before adding it to a CompositeKey.
     *
     * @param key CompositeKey to add new key to
     * @return true if added successfully
     */
    virtual bool addToCompositeKey(CompositeKey& key) = 0;

    /**
     * Validate key component data to check if key component
     * may be added to a CompositeKey.
     *
     * @param errorMessage error message in case data is not valid
     * @return true if data is valid
     */
    virtual bool validate(QString& errorMessage) const = 0;

    void setComponentName(const QString& name);
    QString componentName() const;
    void setComponentAdded(bool added);
    bool componentAdded() const;
    void changeVisiblePage(Page page);
    Page visiblePage() const;

protected:
    /**
     * Construct and return an instance of the key component edit widget
     * which is to be inserted into the component management UI.
     * Since previous widgets will be destroyed, every successive call to
     * this function must return a new widget.
     *
     * @return edit widget instance
     */
    virtual QWidget* componentEditWidget() = 0;

signals:
    void nameChanged(const QString& newName);
    void componentAddChanged(bool added);
    void componentAddRequested();
    void componentEditRequested();
    void editCanceled();
    void componentRemovalRequested();

private slots:
    void updateComponentName(const QString& name);
    void updateAddStatus(bool added);
    void doAdd();
    void doEdit();
    void doRemove();
    void cancelEdit();
    void reset();

private:
    bool m_isComponentAdded = false;
    Page m_previousPage = Page::AddNew;
    QString m_componentName;

    const QScopedPointer<Ui::KeyComponentWidget> m_ui;
};

#endif //KEEPASSXC_KEYCOMPONENTWIDGET_H
