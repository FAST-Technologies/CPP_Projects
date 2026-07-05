#ifndef CPP4_3DVIEWER_V2_0_PARSER_PARSER_H_
#define CPP4_3DVIEWER_V2_0_PARSER_PARSER_H_

/// \file
/// \brief Парсер формата Wavefront OBJ (и смежных директив) в структуру
/// ObjModel. \details
///  - Разбирает строки OBJ: v/vt/vn/f, а также o/g/mtllib/usemtl/s.
///  - Пишет диагностический лог в файл при включённом флаге \c log.
///  - Не бросает исключений наружу; ошибки отражаются в возвращаемом значении.

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../geometry_types.h"
#include "../objloader/objloader.h"

namespace s21 {

/// \brief Класс парсинга OBJ-данных в модель.
/// \details
///  - Методы \c parse* обрабатывают отдельные типы строк.\n
///  - \c parseObjString очищает модель и наполняет её данными; возвращает \c
///  true, если удалось получить хотя бы одну грань.
/// \thread_safety Не потокобезопасен.
class Parser {
 public:
  /// \brief Инициализирует подсистему логирования парсера.
  Parser();

  /// \brief Завершает работу подсистемы логирования.
  ~Parser();

  /// \brief Парсит OBJ из строки.
  /// \param data Текст OBJ.
  /// \param model Выходная модель; в начале очищается.
  /// \param log Вести ли диагностический лог в файл.
  /// \return \c true, если получены валидные грани; иначе \c false.
  static bool parseObjString(const std::string& data, ObjModel& model,
                             bool log = true);

 private:
  // --- Логирование ---
  static std::ofstream logFile_;  ///< Дескриптор файла лога.
  static constexpr const char* logFileName_ =
      "parser_logs.txt";  ///< Имя файла лога.

  /// \brief Удаляет хвостовой комментарий, начинающийся с '#'.
  static void removedSharpComments(std::string& line, bool log);

  /// \brief Обрабатывает строку вершины \c v.
  static void parseVLine(const std::string& line, ObjModel& model, bool log);

  /// \brief Обрабатывает строку текстурных координат \c vt.
  static void parseVTLine(const std::string& line, ObjModel& model, bool log);

  /// \brief Обрабатывает строку нормали \c vn.
  static void parseVNLine(const std::string& line, ObjModel& model, bool log);

  /// \brief Обрабатывает строку грани \c f (включая триангуляцию).
  static void parseFLine(const std::string& line, ObjModel& model, bool log);

  /// \brief Имя объекта \c o.
  static void parseOLine(const std::string& line, ObjModel& model, bool log);

  /// \brief Файл материалов \c mtllib.
  static void parseMtllibLine(const std::string& line, ObjModel& model,
                              bool log);

  /// \brief Группа \c g.
  static void parseGLine(const std::string& line, ObjModel& model, bool log);

  /// \brief Активный материал \c usemtl.
  static void parseUsemtlLine(const std::string& line, ObjModel& model,
                              bool log);

  /// \brief Параметры сглаживания \c s.
  static void parseSLine(const std::string& line, ObjModel& model, bool log);

  /// \brief Открывает файл лога (append) только если он еще не открыт.
  static void openLogFile();

  /// \brief Закрывает файл лога, если он открыт.
  static void closeLogFile();

  /// \brief Пишет строку в лог при разрешённом \p logEnabled.
  static void logger(const std::string& message, bool logEnabled);
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_PARSER_PARSER_H_