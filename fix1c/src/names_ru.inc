// Russian names visible from 1C and messages. This file is UTF-8; build.cmd (gen_names.py) converts it
// to CP1251 into Out\gen\names_ru_cp1251.inc, and that copy is what the compiler includes
// (1C compares method names as CP1251 bytes, VC6 puts string bytes into the DLL as is).

// ---- our type, methods and properties
#define RU_TYPE_NAME        "ИсправлениеODBC"
#define RU_ENABLE           "Включить"
#define RU_DISABLE          "Выключить"
#define RU_ENABLED          "Включено"
#define RU_VERSION          "Версия"
#define RU_INFO             "Сведения"
#define RU_CHECK_OBJECT     "ПроверитьОбъект"
#define RU_REFRESH_ON       "ОбновлениеВключено"
#define RU_ERROR_TEXT       "ТекстОшибки"

// ---- messages
#define MSG_NO_1CPP         "1cpp.dll не загружена: сначала ЗагрузитьВнешнююКомпоненту(\"1cpp.dll\")"
#define MSG_NO_RS           "не удалось создать ODBCRecordset"
#define MSG_BAD_SLOT        "место CallAsFunc в таблице методов не сходится (по коду %d, ожидалось %d): исправление не включено"
#define MSG_BAD_VTBL        "таблица методов ODBCRecordset или её CallAsFunc не в 1cpp.dll: исправление не включено"
#define MSG_NO_METHOD       "у ODBCRecordset нет метода ВыполнитьИнструкцию (ExecuteStatement)"
#define MSG_PROTECT         "не удалось открыть таблицу методов ODBCRecordset на запись (VirtualProtect %lu)"
#define MSG_FOREIGN         "CallAsFunc у ODBCRecordset уже подменён не этой компонентой: исправление не включено"
#define MSG_NOT_OURS        "CallAsFunc у ODBCRecordset сейчас не наш: выключать нечего"
#define MSG_EXCEPTION       "исключение в методе %s"

// ---- Сведения()
#define INFO_FORMAT         "%s; включено: %s; 1cpp.dll 0x%08lX-0x%08lX; таблица методов 0x%08lX; CallAsFunc: место %d (по коду %d), исходный 0x%08lX; ВыполнитьИнструкцию — метод %d; вызовов ВыполнитьИнструкцию %ld, с существующей ТЗ %ld, из них с ошибкой %ld"
#define INFO_YES            "да"
#define INFO_NO             "нет"
