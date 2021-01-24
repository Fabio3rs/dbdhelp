using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Data;

namespace ConsoleApp1
{
    struct TableField
    {
        public String name;
        public int type;
        public int length;
        public bool nullable, primaryk, index, unsigned, autoincrement;

        public TableField(String n, int t, int l, bool nb, bool pk, bool ind, bool uns, bool ai)
        {
            name = n;
            type = t;
            length = l;
            nullable = nb;
            primaryk = pk;
            index = ind;
            unsigned = uns;
            autoincrement = ai;
        }
    }

	class SQLMGR
    {
        static private String strConn = "Data Source=svraddress;Database=testdb;User Id=dbuser;Password=dbpasswd;Integrated Security = false;"
            + " Connect Timeout=5;";
        String fieldsToSqlTbl(TableField[] Fields)
        {
            String result = "";

            bool comma = false;

            foreach (TableField tb in Fields)
            {
                if (comma)
                {
                    result += ",\n";
                }
                comma = true;

                String line = "\t";
                line += tb.name;

                switch (tb.type)
                {
                    case 0:
                        line += " int";
                        break;
                    case 1:
                        line += " varchar";
                        break;
                    case 2:
                        line += " decimal";
                        break;
                    case 3:
                        line += " date";
                        break;
                    case 4:
                        line += " datetime";
                        break;
                    case 5:
                        line += " timestamp";
                        break;
                }

                if (tb.length > 1)
                {
                    line += "(" + tb.length.ToString() + ")";
                }

                if (tb.nullable)
                {
                    line += " NULL";
                }
                else
                {
                    line += " NOT NULL";
                }

                if (tb.autoincrement && tb.primaryk)
                {
                    line += " IDENTITY (1, 1)";
                }

                result += line;
            }

            return result;
        }

        static public SqlConnection connect()
        {
            SqlConnection conn = new SqlConnection(strConn);
            conn.Open();

            return conn;
        }

        public bool create_table(String name, TableField[] Fields)
        {
            using (SqlConnection conn = connect())
            {
                String sql = "CREATE TABLE dbo." + name + "\n\t(" + fieldsToSqlTbl(Fields) + "\t);\n";

                SqlCommand cm = new SqlCommand(sql, conn);

                Console.WriteLine(cm.ExecuteNonQuery().ToString());
            }
            return false;
        }

        public bool alter_table(String name, TableField[] Fields)
        {
            using (SqlConnection conn = connect())
            {
                String sql = "ALTER TABLE dbo." + name + " ADD\n\t" + fieldsToSqlTbl(Fields) + "\t;\n";

                SqlCommand cm = new SqlCommand(sql, conn);

                Console.WriteLine(cm.ExecuteNonQuery().ToString());
            }
            return false;
        }
    }
}

