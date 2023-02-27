#pragma once

#include <QtCore>

class TestLib: public QObject
{
     Q_OBJECT
public:
     TestLib(){};
     QString getString();
};