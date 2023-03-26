#pragma once


#include <QtCore>


#define INVALID_PLUGIN_ID 0

class ViewCommand;

/// @brief сериализует cmd в формате NetworkPackage для передачи по сети
QByteArray serializeViewCmd( const ViewCommand& cmd );


/// @brief десериализует ViewCommand из потока data, полученном по сети. Если данные удалось десериализовать, они удаляются из массива
/// остальная часть массива не трогается. Если данные десериализовать не получилось, массив остается без изменения
/// @return Валидный ViewCommand, если пришли все части. Невалидный ViewCommand, если получены не все данные
ViewCommand deserializeViewCmd( QByteArray& data );



class ViewArgs: public QObject
{
     Q_OBJECT
public:
     ViewArgs( const QVariantList& variables = QVariantList() );
     ViewArgs( const ViewArgs& rhs );
     virtual ~ViewArgs();

     void addVariable( const QVariant& variable );
     quint32 count() const;
     const QVariant* at( quint32 idx ) const;

     ViewArgs& operator=( const ViewArgs& rhs);

     friend QDataStream& operator<<( QDataStream& out, const ViewArgs& myObj );
     friend QDataStream& operator>>( QDataStream& in, ViewArgs& myObj );
     friend QDebug& operator<<( QDebug& d, const ViewArgs& cmd );
private:
     QVariantList variables_;
};


QDataStream& operator<<( QDataStream& out, const ViewArgs& myObj );
QDataStream& operator>>( QDataStream& in, ViewArgs& myObj );
QDebug& operator<<( QDebug& d, const ViewArgs& cmd );

Q_DECLARE_METATYPE( ViewArgs )


class ViewCommand: public QObject
{
     Q_OBJECT
public:
     ViewCommand( quint32 pluginId = INVALID_PLUGIN_ID, quint32 command = 0, const QList<ViewArgs>& args = QList<ViewArgs>() );
     ViewCommand( const ViewCommand& other );
     virtual ~ViewCommand();

     quint32 plgId() const;
     quint32 cmd() const;
     bool isValid() const;

     void addArg( const ViewArgs& arg );
     quint32 count() const;
     const ViewArgs* at( quint32 idx ) const;
     ViewCommand& operator=( const ViewCommand& rhs );

     friend QDataStream& operator<<( QDataStream& out, const ViewCommand& myObj );
     friend QDataStream& operator>>( QDataStream& in, ViewCommand& myObj );

     friend QDebug& operator<<( QDebug& d, const ViewCommand& cmd );
private:
     quint32 plgId_;
     quint32 command_;
     QList< ViewArgs > arguments_;
};


QDataStream& operator<<( QDataStream& out, const ViewCommand& myObj );
QDataStream& operator>>( QDataStream& in, ViewCommand& myObj );
QDebug& operator<<( QDebug& d, const ViewCommand& cmd );

Q_DECLARE_METATYPE( ViewCommand )
Q_DECLARE_METATYPE( QList<ViewArgs> )

