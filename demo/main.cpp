#include <atomic>
#include <boost/core/null_deleter.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <example.hpp>
#include <iostream>

std::atomic<bool> a = true;
void my_handler(int s) {
  a = false;
  ++s;
}

void init_logging() {
  boost::shared_ptr<logging::core> core = logging::core::get();
  boost::shared_ptr<sinks::text_file_backend> backend =
      boost::make_shared<sinks::text_file_backend>(
          keywords::file_name = MISC "log",
          keywords::rotation_size = 5 * 1024 * 1024,
          keywords::format =
              (expr::stream << expr::format_date_time<boost::posix_time::ptime>(
                                   "TimeStamp", "%Y-%m-%d %H:%M:%S")
                            << ": <" << logging::trivial::severity << "> "
                            << expr::smessage),
          keywords::time_based_rotation =
              sinks::file::rotation_at_time_point(12, 0, 0));

  typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
  boost::shared_ptr<sink_t> sink(new sink_t(backend));
  core->add_sink(sink);
}

void init_console() {
  typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
  boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>(
      keywords::format = expr::stream
                         << expr::format_date_time<boost::posix_time::ptime>(
                                "TimeStamp", "%Y-%m-%d %H:%M:%S")
                         << ": <" << logging::trivial::severity << "> "
                         << expr::smessage);

  boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
  sink->locked_backend()->add_stream(stream);
  logging::core::get()->add_sink(sink);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    return 0;
  }
  size_t n;
  std::string filename;
  if (argc == 2) {
    filename = argv[1];
    n = std::thread::hardware_concurrency();
  } else {
    filename = argv[1];
    std::stringstream convert(argv[2]);
    convert >> n;
  }
  init_console();
  init_logging();
  BOOST_LOG_TRIVIAL(info) << "An informational severity message";
  std::ofstream out(filename);
  json array;
  start_threads(n, array);

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = my_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
  while (a)
    ;
  out << array;
  std::cout << "done";
}
