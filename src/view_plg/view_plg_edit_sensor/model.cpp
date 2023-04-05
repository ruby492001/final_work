#include "model.h"
#include "agent_event.h"


int EditSensorModel::columnCount( const QModelIndex& ) const
{
     return 4;
}


QVariant EditSensorModel::data( const QModelIndex& index, int role ) const
{
     if( role != Qt::DisplayRole || !index.isValid() )
     {
          return {};
     }
     switch( index.column() )
     {
          case 0:
          {
               return sensors_.at( index.row() ).clientSensorId >> 32;
          }
          case 1:
          {
               return ( sensors_.at( index.row() ).clientSensorId & 0xffffffff );
          }
          case 2:
          {
               return sensors_.at( index.row() ).userName;
          }
          case 3:
          {
               return toString( sensors_.at( index.row() ).eventType );
          }
          default:
          {
               break;
          }
     }
     return {};
}


QModelIndex EditSensorModel::index( int row, int column, const QModelIndex& parent ) const
{
     if( parent.isValid() )
     {
          return {};
     }
     return createIndex( row, column, ( void* )( &sensors_.at( row ) ) );
}


QModelIndex EditSensorModel::parent( const QModelIndex& ) const
{
     return {};
}


int EditSensorModel::rowCount( const QModelIndex& parent ) const
{
     if( parent.isValid() )
     {
          return 0;
     }
     return sensors_.count();
}


QVariant EditSensorModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
     if( role != Qt::DisplayRole || orientation != Qt::Horizontal )
     {
          return {};
     }
     switch( section )
     {
          case 0:
          {
               return tr( "Клиент" );
          }
          case 1:
          {
               return tr( "Датчик" );
          }
          case 2:
          {
               return tr( "Имя" );
          }
          case 3:
          {
               return tr( "Тип данных" );
          }
          default:
          {
               return QVariant();
          }
     }
}


Qt::ItemFlags EditSensorModel::flags( const QModelIndex& index ) const
{
     if( index.column() != 2 )
     {
          return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
     }
     return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


bool EditSensorModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
     if( index.column() != 2 || role != Qt::EditRole )
     {
          return false;
     }
     SensorParam* t = static_cast< SensorParam* >( index.internalPointer() );
     if( !t )
     {
          return false;
     }
     t->userName = value.toString();
     emit sDataChanged( *t );
     dataChanged( index, index );
     return true;
}


void EditSensorModel::onSetSensorList( const QList<SensorParam>& sensors )
{
     beginResetModel();
     sensors_ = sensors;
     endResetModel();
}
