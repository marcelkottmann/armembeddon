// Copyright (C) 2022-2025 Exaloop Inc. <https://exaloop.io>

#include <Arduino.h>

#include "codon_rt_lib.h"
#include "gc/gc.h"
#include <atomic>
#include <new>

#define USE_STANDARD_MALLOC 0

int seq_flags;

struct SeqMutex {
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

SEQ_FUNC void seq_exc_init(int flags)
{
  // armembeddon: currently only dummy impl
}

SEQ_FUNC void seq_init(int flags)
{
#if !USE_STANDARD_MALLOC
  gc_init();
#endif

  seq_exc_init(flags);
  seq_flags = flags;
}

// SEQ_FUNC seq_int_t seq_pid() { return (seq_int_t)getpid(); }

// SEQ_FUNC seq_int_t seq_time() {
//   auto duration = std::chrono::system_clock::now().time_since_epoch();
//   seq_int_t nanos =
//       std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
//   return nanos;
// }

// SEQ_FUNC seq_int_t seq_time_monotonic() {
//   auto duration = std::chrono::steady_clock::now().time_since_epoch();
//   seq_int_t nanos =
//       std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
//   return nanos;
// }

// SEQ_FUNC seq_int_t seq_time_highres() {
//   auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
//   seq_int_t nanos =
//       std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
//   return nanos;
// }

static void copy_time_c_to_seq(struct tm *x, seq_time_t *output)
{
  output->year = x->tm_year;
  output->yday = x->tm_yday;
  output->sec = x->tm_sec;
  output->min = x->tm_min;
  output->hour = x->tm_hour;
  output->mday = x->tm_mday;
  output->mon = x->tm_mon;
  output->wday = x->tm_wday;
  output->isdst = x->tm_isdst;
}

static void copy_time_seq_to_c(seq_time_t *x, struct tm *output)
{
  output->tm_year = x->year;
  output->tm_yday = x->yday;
  output->tm_sec = x->sec;
  output->tm_min = x->min;
  output->tm_hour = x->hour;
  output->tm_mday = x->mday;
  output->tm_mon = x->mon;
  output->tm_wday = x->wday;
  output->tm_isdst = x->isdst;
}

SEQ_FUNC bool seq_localtime(seq_int_t secs, seq_time_t *output)
{
  struct tm result;
  time_t now = (secs >= 0 ? secs : time(nullptr));
  if (now == (time_t)-1 || !localtime_r(&now, &result))
    return false;
  copy_time_c_to_seq(&result, output);
  return true;
}

SEQ_FUNC bool seq_gmtime(seq_int_t secs, seq_time_t *output)
{
  struct tm result;
  time_t now = (secs >= 0 ? secs : time(nullptr));
  if (now == (time_t)-1 || !gmtime_r(&now, &result))
    return false;
  copy_time_c_to_seq(&result, output);
  return true;
}

SEQ_FUNC seq_int_t seq_mktime(seq_time_t *time)
{
  struct tm result;
  copy_time_seq_to_c(time, &result);
  return mktime(&result);
}

SEQ_FUNC void seq_sleep(double secs)
{
  delay((uint32_t)(secs * 1000.0));
}

extern char **environ;
SEQ_FUNC char **seq_env() { return environ; }

/*
 * GC
 */

SEQ_FUNC void *seq_alloc(uint64_t n)
{
#if USE_STANDARD_MALLOC
  return malloc(n);
#else
  return gc_malloc(n);
#endif
}

SEQ_FUNC void *seq_alloc_atomic(uint64_t n)
{
#if USE_STANDARD_MALLOC
  return malloc(n);
#else
  return gc_malloc_atomic(n);
#endif
}

// SEQ_FUNC void *seq_alloc_uncollectable(size_t n) {
// #if USE_STANDARD_MALLOC
//   return malloc(n);
// #else
//   return GC_MALLOC_UNCOLLECTABLE(n);
// #endif
// }

// SEQ_FUNC void *seq_alloc_atomic_uncollectable(size_t n) {
// #if USE_STANDARD_MALLOC
//   return malloc(n);
// #else
//   return 
// #endif
// }

SEQ_FUNC void *seq_realloc(void* p, uint64_t newsize, uint64_t oldsize)
{
#if USE_STANDARD_MALLOC
  return realloc(p, newsize);
#else
  return gc_realloc(p, newsize);
#endif
}

// SEQ_FUNC void seq_free(void *p) {
// #if USE_STANDARD_MALLOC
//   free(p);
// #else
//   GC_FREE(p);
// #endif
// }

// SEQ_FUNC void seq_register_finalizer(void *p, void (*f)(void *obj, void *data)) {
// #if !USE_STANDARD_MALLOC
//   GC_REGISTER_FINALIZER(p, f, nullptr, nullptr, nullptr);
// #endif
// }

// SEQ_FUNC void seq_gc_add_roots(void *start, void *end) {
// #if !USE_STANDARD_MALLOC
//   GC_add_roots(start, end);
// #endif
// }

// SEQ_FUNC void seq_gc_remove_roots(void *start, void *end) {
// #if !USE_STANDARD_MALLOC
//   GC_remove_roots(start, end);
// #endif
// }

// SEQ_FUNC void seq_gc_clear_roots() {
// #if !USE_STANDARD_MALLOC
//   GC_clear_roots();
// #endif
// }

// SEQ_FUNC void seq_gc_exclude_static_roots(void *start, void *end) {
// #if !USE_STANDARD_MALLOC
//   GC_exclude_static_roots(start, end);
// #endif
// }

seq_str_t create_seq_str(seq_int_t len)
{
  auto *p = (char *)seq_alloc_atomic(len);
  return {(seq_int_t)len, p};
}

static seq_str_t string_conv(const String &s)
{
  seq_str_t result = create_seq_str((seq_int_t)s.length());
  memcpy(result.str, s.c_str(), result.len);
  return result;
}

static seq_str_t string_conv(const char *cstr, seq_int_t len)
{
  seq_str_t result = create_seq_str((seq_int_t)len);
  memcpy(result.str, cstr, result.len);
  return result;
}

template <typename T>
std::string default_format(T n)
{
  return std::string("armembeddon: default_format not implemented");
}

template <>
std::string default_format(double n)
{
  return std::to_string(n);
}

template <typename T>
seq_str_t fmt_conv(T n, seq_str_t format, bool *error)
{
  return string_conv(String("armembeddon: fmt_conv not implemented"));
  // *error = false;
  // try {
  //   if (format.len == 0) {
  //     return string_conv(default_format(n));
  //   } else {
  //     auto locale = std::locale("en_US.UTF-8");
  //     std::string fstr(format.str, format.len);
  //     return string_conv(fmt::format(
  //         locale, fmt::runtime(fmt::format(FMT_STRING("{{:{}}}"), fstr)), n));
  //   }
  // } catch (const std::runtime_error &f) {
  //   *error = true;
  //   return string_conv(f.what());
  // }
}

SEQ_FUNC seq_str_t seq_str_int(seq_int_t n, seq_str_t format, bool *error)
{
  String s = "";
  if (n < 0)
  {
    s += "-";
    n = -n;
  }
  uint32_t high_int = (uint32_t)(n / 1000000000);
  if (high_int != 0)
  {
    s += String(high_int);
  }
  s += String((uint32_t)(n % 1000000000)); // low part
  return string_conv(s);
}

SEQ_FUNC seq_str_t seq_str_uint(seq_int_t value, seq_str_t format, bool *error)
{
  if (value == 0)
    return string_conv("0", 1);

  String result = "";
  char buf[10]; // up to 10 digits per chunk (fits in uint32_t)

  // Split into chunks of up to 1e9 (1,000,000,000)
  // to safely handle 64-bit values without overflowing 32-bit math
  uint32_t base = 1000000000UL;
  uint32_t parts[3]; // max 3 chunks needed for full 64-bit range
  int count = 0;

  while (value > 0)
  {
    parts[count++] = value % base;
    value /= base;
  }

  // Print highest chunk without leading zeros
  result += String(parts[count - 1]);

  // Remaining chunks with zero padding to 9 digits each
  for (int i = count - 2; i >= 0; --i)
  {
    snprintf(buf, sizeof(buf), "%09lu", (unsigned long)parts[i]);
    result += buf;
  }

  return string_conv(result);
}


SEQ_FUNC seq_str_t seq_str_float(double d, seq_str_t format, bool *error)
{
  return string_conv(String(d));
}

SEQ_FUNC seq_str_t seq_str_ptr(void *p, seq_str_t format, bool *error)
{
  return string_conv(String((uint32_t)p, 16));
}

SEQ_FUNC seq_str_t seq_str_str(seq_str_t s, seq_str_t format, bool *error)
{
  return s;
}

SEQ_FUNC seq_int_t seq_int_from_str(seq_str_t s, const char **e, int base)
{
  return std::stoi(std::string(s.str, s.len));
}

SEQ_FUNC double seq_float_from_str(seq_str_t s, const char **e)
{
  return std::stof(std::string(s.str, s.len));
}

/*
 * General I/O
 */

SEQ_FUNC seq_str_t seq_check_errno()
{
  if (errno)
  {
    std::string msg = strerror(errno);
    auto *buf = (char *)seq_alloc_atomic(msg.size());
    memcpy(buf, msg.data(), msg.size());
    return {(seq_int_t)msg.size(), buf};
  }
  return {0, nullptr};
}

SEQ_FUNC void seq_print(seq_str_t str) { seq_print_full(str, stdout); }

// static std::ostringstream capture;
// static std::mutex captureLock;

SEQ_FUNC void seq_print_full(seq_str_t str, FILE *fo)
{
  // if ((seq_flags & SEQ_FLAG_CAPTURE_OUTPUT) && (fo == stdout || fo == stderr)) {
  //   captureLock.lock();
  //   capture.write(str.str, str.len);
  //   captureLock.unlock();
  // } else {
  //   fwrite(str.str, 1, (size_t)str.len, fo);
  // }
  Serial.write((const uint8_t *)str.str, (size_t)str.len);
}

// std::string codon::runtime::getCapturedOutput()
// {
//   std::string result = capture.str();
//   capture.str("");
//   return result;
// }

SEQ_FUNC void *seq_stdin() { return stdin; }

SEQ_FUNC void *seq_stdout() { return stdout; }

SEQ_FUNC void *seq_stderr() { return stderr; }

SEQ_FUNC ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
  Serial.println(stream == stdin ? "STDIN" : "NOT STDIN");
  return 0;
}

// Dummy implementation for Codon personality function
_Unwind_Reason_Code seq_personality(
    int version,
    _Unwind_Action actions,
    uint64_t exceptionClass,
    _Unwind_Exception *exceptionObject,
    _Unwind_Context *context)
{
  (void)version;
  (void)actions;
  (void)exceptionClass;
  (void)exceptionObject;
  (void)context;
  return _URC_NO_REASON;
}

SEQ_FUNC void seq_terminate(void *exc)
{
}


// SEQ_FUNC void *seq_lock_new()
// {
//   auto *memory = (SeqMutex *)seq_alloc_atomic(sizeof(SeqMutex));
//   return new (memory) SeqMutex();
// }

// SEQ_FUNC bool seq_lock_acquire(void *lock, bool block, double timeout)
// {
//   auto *m = (SeqMutex *)lock;

//   if (!block)
//     return !m->flag.test_and_set(std::memory_order_acquire);

//   if (timeout < 0.0) {
//     while (m->flag.test_and_set(std::memory_order_acquire))
//       delayMicroseconds(50);
//     return true;
//   }

//   const unsigned long timeout_us = (unsigned long)(timeout * 1000000.0);
//   const unsigned long start = micros();

//   while (m->flag.test_and_set(std::memory_order_acquire)) {
//     if (timeout_us == 0)
//       return false;

//     if ((unsigned long)(micros() - start) >= timeout_us)
//       return false;

//     delayMicroseconds(50);
//   }

//   return true;
// }

// SEQ_FUNC void seq_lock_release(void *lock)
// {
//   auto *m = (SeqMutex *)lock;
//   m->flag.clear(std::memory_order_release);
// }
