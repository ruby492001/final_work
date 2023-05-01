#include "delegate.h"


QStringList places;
QStringList types;
QStringList measure;

QWidget* Delegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
     Q_UNUSED( option )
     QComboBox* editor = new QComboBox( parent );
     connect( editor,  QOverload<int>::of( &QComboBox::activated ),
             this, &Delegate::commitAndCloseEditor );
     editor->setEditable( true );
     editor->addItem( "" );
     switch( index.column() )
     {
          case 4:
          {
               editor->addItems( places );
               break;
          }
          case 5:
          {
               editor->addItems( types );
               break;
          }
          case 6:
          {
               editor->addItems( measure );
               break;
          }
          default:
               break;
     }
     return editor;
}

void Delegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
     QComboBox* cb = qobject_cast<QComboBox*>(editor);
     if (!cb)
          throw std::logic_error("editor is not a combo box");

     QString value = index.data(Qt::EditRole).toString();
     int idx = cb->findText(value);
     if (idx >= 0)
          cb->setCurrentIndex(idx);
     cb->showPopup();
}


void Delegate::commitAndCloseEditor()
{
     QComboBox* editor = qobject_cast< QComboBox * >( sender() );
     emit commitData( editor );
     emit closeEditor( editor );
}
