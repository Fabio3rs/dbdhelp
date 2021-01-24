print("AAAAA")
select_database("test_db")

migration = create_table("users")
field_id(migration)
create_field(migration, "login", "string")
create_field(migration, "password", "string")
field_timestamps(migration)



print("DUMP DA TABELA INTEIRA")
dumptbl(migration)

print("migration end")

