#ifndef CPP4_3DVIEWER_V2_0_SCOPE_TIMER_SCOPE_TIMER_H_
#define CPP4_3DVIEWER_V2_0_SCOPE_TIMER_SCOPE_TIMER_H_

/// \file
/// \brief Простая RAII-обёртка для измерения времени выполнения участка кода.

#include <QDebug>
#include <QString>
#include <chrono>
#include <utility>

namespace s21 {

inline QString toQString(const char* s) { return QString::fromUtf8(s); }
inline QString toQString(const QString& s) { return s; }

class ScopeTimer {
 public:
  explicit ScopeTimer(QString name, double warn_ms = 500.0)
      : name_(std::move(name)),
        warn_ms_(warn_ms),
        start_(std::chrono::steady_clock::now()) {}

  ~ScopeTimer() noexcept {
    const auto end = std::chrono::steady_clock::now();
    const double elapsed =
        std::chrono::duration<double, std::milli>(end - start_).count();

    if (elapsed >= warn_ms_) {
      qWarning().nospace() << "[ScTIMER][WARN] " << name_ << " took " << elapsed
                           << " ms";
    } else {
      qDebug().nospace() << "[ScTIMER] " << name_ << " took " << elapsed
                         << " ms";
    }
  }

 private:
  QString name_;
  double warn_ms_;
  std::chrono::steady_clock::time_point start_;
};

}  // namespace s21

#ifndef NDEBUG
#define SCOPE_TIMER(name) \
  ::s21::ScopeTimer scope_timer_##__LINE__(::s21::toQString(name), 500.0)
#define SCOPE_TIMER_WARN(name, w) \
  ::s21::ScopeTimer scope_timer_##__LINE__(::s21::toQString(name), (w))
#else
#define SCOPE_TIMER(name) \
  do {                    \
  } while (0)
#define SCOPE_TIMER_WARN(name, w) \
  do {                            \
  } while (0)
#endif

#endif  // CPP4_3DVIEWER_V2_0_SCOPE_TIMER_SCOPE_TIMER_H_
