#include "boost/date_time/posix_time/posix_time.hpp"

class Timer
{
	public:
		Timer() : started( boost::posix_time::microsec_clock::local_time() ) {}
		double time() { return (boost::posix_time::microsec_clock::local_time().time_of_day() - started.time_of_day()).total_nanoseconds() / 1000000000.0; }
		void reset() { started = boost::posix_time::microsec_clock::local_time(); }
	private:
		boost::posix_time::ptime started;
};