#pragma once
#ifndef MIGRATIONMGR_DATABASE_HPP
#define MIGRATIONMGR_DATABASE_HPP

#include <deque>
#include <cstdint>
#include <string>
#include "Table.hpp"

namespace migmgr
{
    class Database
    {
        friend class Program;
        std::string name;
        std::deque<Table> tables;

    public:
        int create_table_if_not_exists(std::string name);

        Database(std::string nm) : name(nm) {};
    };
}

#endif
