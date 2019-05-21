
#include <libpq-fe.h>
//vcpkg integrate install
//vcpkg install libpq:x86-windows

https://devblogs.microsoft.com/cppblog/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/


Import data to postgresql:
"C:\\Program Files\\PostgreSQL\\9.6\\bin\\psql.exe" --command " "\\copy public.data_value (date, externalid, \"number\", constant, digits, decimals) FROM 'C:/Temp/data.csv' DELIMITER E'\\t' CSV HEADER ENCODING 'UTF8' QUOTE '\"' ESCAPE '''';""
