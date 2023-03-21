#pragma once


#include <QtCore>


#define INVALID_PLUGIN_ID 0


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

private:
     QVariantList variables_;
};


QDataStream& operator<<( QDataStream& out, const ViewArgs& myObj );
QDataStream& operator>>( QDataStream& in, ViewArgs& myObj );


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
     const ViewArgs* at( quint32 idx );
     ViewCommand& operator=( const ViewCommand& rhs );

     friend QDataStream& operator<<( QDataStream& out, const ViewCommand& myObj );
     friend QDataStream& operator>>( QDataStream& in, ViewCommand& myObj );

private:
     quint32 plgId_;
     quint32 command_;
     QList< ViewArgs > arguments_;
};


QDataStream& operator<<( QDataStream& out, const ViewCommand& myObj );
QDataStream& operator>>( QDataStream& in, ViewCommand& myObj );