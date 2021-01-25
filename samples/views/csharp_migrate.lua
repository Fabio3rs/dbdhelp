using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;

namespace ConsoleApp1
{
    class Migrate
    {
        public void set_migration_val(int n)
        {

        }

        public void exec_migration()
        {
            SQLMGR sql = new SQLMGR();

            String sqlstr = "select * from migrations";
            DataTable dt = new DataTable();
            SqlDataAdapter da = new SqlDataAdapter(sqlstr, SQLMGR.connect());
            da.Fill(dt);

            int lastmigration = Convert.ToInt32(dt.Rows[0]["last"]);

<?lua
            for k,p in ipairs(migratedata) do
?>

            if (lastmigration < {{! k !}})
            {
                TableField[] create_for_{{! p["table"] !}} = new TableField[]
                {
<?lua
                for i,l in ipairs(p["fields"]) do
?>
                    new TableField({{! l["name"] !}}, {{! l["type"] !}}, 0, false, true, true, true, true)
<?lua
end
?>
                };
                sql.create_table({{! p["table"] !}}, create_for_{{! p["table"] !}});

                set_migration_val({{! k !}});
            }
            
<?lua
end
?>
        }
    }
}



