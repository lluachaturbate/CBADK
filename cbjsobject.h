#ifndef CBJSOBJECT_H
#define CBJSOBJECT_H

#include <QObject>
#include <QScriptable>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QScriptEngine>


//! The cbjs object. @note Why there are 2 different objects exposed to the JS engine idk. Why not just cb.arrayContains? @sa CBObjectBase and CBObjectImpl
class CBJSObject : public QObject, QScriptable
{
    Q_OBJECT
public:
    explicit CBJSObject(QObject *parent = 0) : QObject(parent) {}

    //! Returns true if array contains at least once instance of object.
    Q_INVOKABLE bool arrayContains(const QScriptValue& array, const QScriptValue& object) const
    {
        if (array.isArray())
            return array.toVariant().toList().contains(object.toVariant());
        context()->throwError(QScriptContext::TypeError, "cbjs.arrayContains(): Argument mismatch. Expected an array and an object.");
        return false;
    }

    //! Returns new array with all instances of object removed.
    Q_INVOKABLE QScriptValue arrayRemove(QScriptValue array, const QScriptValue& object) const
    {
        if (array.isArray())
        {
            QVariant o = object.toVariant();
            QScriptValue newarray = engine()->newArray();
            int idx = -1;
            QScriptValueIterator it(array);
            while (it.hasNext())
            {
                it.next();
                if (it.value().toVariant() != o && it.name() != "length")
                    newarray.setProperty(++idx, it.value());
            }
            return newarray;
        }
        else
            context()->throwError(QScriptContext::TypeError, "cbjs.arrayRemove(): Argument mismatch. Expected an array and an object.");
        return array;
    }
};

#endif // CBJSOBJECT_H
