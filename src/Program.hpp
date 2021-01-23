#pragma once
#ifndef MIGRATIONMGR_PROGRAM_HPP
#define MIGRATIONMGR_PROGRAM_HPP

#include <deque>
#include <cstdint>
#include <string>
#include "Database.hpp"
#include "CLuaH.hpp"
#include "CLuaFunctions.hpp"

namespace migmgr
{
    class Program
    {
        std::string                 config_script;
        std::string                 migrations_directory;
        std::deque<Database>        dbs;

        CLuaH::scriptStorage        scripts;

        bool loaded;

        static int registerFunctions(lua_State* L);
        static int registerGlobals(lua_State* L);

        // Lua functions
        static int select_database(lua_State* L);
        static int create_table(lua_State* L);
        static int alter_table(lua_State* L);
        static int field_id(lua_State* L);
        static int field_timestamps(lua_State* L);
        static int create_field(lua_State *L);
        static int dumptbl(lua_State *L);

    public:
        bool load();
        void run();

        static Program &prog();

    private:
        Program() noexcept;
    };
}

#endif
