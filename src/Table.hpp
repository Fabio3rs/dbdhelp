#pragma once
#ifndef MIGRATIONMGR_TABLE_HPP
#define MIGRATIONMGR_TABLE_HPP

#include <deque>
#include <vector>
#include <cstdint>
#include <string>
#include <string>

namespace migmgr
{
    enum field_type{integer, string, decimal, date, datetime, timestamp};
    enum field_flag{normal, primary_key};

    struct Field
    {
        std::string name;
        field_type ft;
        field_flag fg;
    };

    struct MField
    {
        std::string name;
        field_type ft;
        field_flag fg;
        int migtype;
    };

    struct Migration
    {
        int migtype;
        int identifier;
        std::vector<MField> fields;
    };

    class Table
    {
        friend class Database;
        friend class Program;
        std::string name;
        
        std::vector<Field> fields;
        std::vector<Migration> migrations;
        
    public:
        void create_field(std::string name, field_type ft, field_flag fg);
        
        Table(std::string nm);
    };
}

#endif
