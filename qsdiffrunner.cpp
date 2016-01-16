#include <QHash>
#include <QDebug>
#include "qsdiffrunner.h"

static QList<QSChange> merge(const QList<QSChange> list) {

    if (list.size() <= 1) {
        return list;
    }

    QList<QSChange> res;

    QSChange prev;

    for (int i = 0 ; i < list.size() ; i++) {
        QSChange current = list.at(i);

        if (prev.isNull()) {
            prev = current;
            continue;
        }

        if (prev.canMerge(current)) {
            prev = prev.merge(current);
        }
    }

    if (!prev.isNull()) {
        res << prev;
    }

    return res;
}

QSDiffRunner::QSDiffRunner()
{

}

QString QSDiffRunner::keyField() const
{
    return m_keyField;
}

void QSDiffRunner::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
}

/*! \fn QList<QSChange> QSDiffRunner::run(const QVariantList &previous, const QVariantList &current)

    Call this function to compare previous and current list, then return a
    list of changes required to transform from previous to current with
    the minimum number of steps. It uses an algorithm with O(n) runtime.
 */

QList<QSChange> QSDiffRunner::compare(const QVariantList &previous, const QVariantList &current)
{
    QList<QSChange> res;
    QList<QSChange> updates;
    QVariantList prevList;
    QVariantList tmp;

    QHash<QString, int> currentHashTable;
    QHash<QString, int> prevHashTable;
    QVariantMap item;
    int offset = 0;

    /* Step 1 - Check Removal */
    for (int i = 0 ; i < current.size() ; i++) {
        item = current.at(i).toMap();
        QString key = item[m_keyField].toString();
        currentHashTable[key] = i;
    }

    prevList = previous;

    tmp.clear();
    for (int i = prevList.size() - 1 ; i >= 0 ; i--) {
        item = prevList.at(i).toMap();
        QString key = item[m_keyField].toString();

        if (!currentHashTable.contains(key)) {
            // The item is removed.
            QSChange change;
            change.setType(QSChange::Remove);
            change.setFrom(i);
            change.setTo(i);
            res << change;
            prevList.removeAt(i); //@FIXME
        }
    }

    /* Step 2 - Compare to find move and update */

    // Build index table
    for (int i = 0 ; i < prevList.size() ; i++) {
        item = prevList.at(i).toMap();
        QString key = item[m_keyField].toString();

        prevHashTable[key] = i;
    }

    for (int i = 0 ; i < current.size() ; i++) {
        item = current.at(i).toMap();
        QString key = item[m_keyField].toString();

        if (!prevHashTable.contains(key)) {
            offset++;
            res << QSChange(QSChange::Insert, i, i, 1, item);
        } else {
            int prevPos = prevHashTable[key];
            int expectedPos = prevPos + offset;

            if (expectedPos != i) {
                QSChange change(QSChange::Move, prevPos, i, 1);
                res << change;

                offset++;
            }
        }
    }

    //@TODO handle update changes
    return merge(res);
}
