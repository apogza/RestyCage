#include "serialized_query.h"

SerializedQuery::SerializedQuery() {}

void SerializedQuery::setParams(QList<SerializedParameter> &params)
{
    m_params = params;
}
