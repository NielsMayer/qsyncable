#include <QUuid>
#include <QtQml>
#include "qsuuid.h"

/*! \qmltype Uuid
    \inqmlmodule QSyncable
    \brief Uuid Generator

 */

QSUuid::QSUuid(QObject *parent) : QObject(parent)
{

}

/*! \qmlmethod Uuid::create()

  Call this function to generate a random Uuid in string format.
  It use the result from QUuid::createUuid().
 */

QString QSUuid::create() const
{
    return QUuid::createUuid().toString().replace(QRegExp("[{}]"), "");
}


static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    QSUuid* object = new QSUuid();

    return object;
}


class QSUuidRegistionHelper {

public:
    QSUuidRegistionHelper() {
        qmlRegisterSingletonType<QSUuid>("QSyncable", 1, 0, "Uuid", provider);
    }
};

static QSUuidRegistionHelper registerHelper;

