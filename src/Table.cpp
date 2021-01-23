#include "Table.hpp"

void migmgr::Table::create_field(std::string name, field_type ft, field_flag fg)
{
    migrations.back().fields.push_back({name, ft, fg, 0});
    fields.push_back({name, ft, fg});
}

migmgr::Table::Table(std::string nm) : name(nm) 
{
    migrations.push_back({0, 0});
}
