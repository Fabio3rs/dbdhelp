#include "Database.hpp"

int migmgr::Database::create_table_if_not_exists(std::string name)
{
    for (int i = 0; i < tables.size(); i++)
    {
        Table &tb = tables[i];
        if (tb.name == name)
            return i;
    }

    tables.push_back({name});
    tables.back().nicename = name;
    return tables.size() - 1;
}

