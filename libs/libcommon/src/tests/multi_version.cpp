#include <string.h>
#include <iostream>
#include <statdaemons/threadpool.hpp>
#include <boost/bind.hpp>
#include <Yandex/MultiVersion.h>


typedef std::string T;
typedef MultiVersion<T> MV;
typedef std::vector<T> Results;


void thread1(MV & x, T & result)
{
	MV::Version v = x.get();
	result = *v;
}

void thread2(MV & x, const char * result)
{
	x.set(new T(result));
}


int main(int argc, char ** argv)
{
	try
	{
		const char * s1 = "Hello!";
		const char * s2 = "Goodbye!";
		
		size_t n = 1000;
		MV x(new T(s1));
		Results results(n);

		boost::threadpool::pool tp(8);
		for (size_t i = 0; i < n; ++i)
		{
			tp.schedule(boost::bind(thread1, boost::ref(x), boost::ref(results[i])));
			tp.schedule(boost::bind(thread2, boost::ref(x), (rand() % 2) ? s1 : s2));
		}
		tp.wait();

		for (size_t i = 0; i < n; ++i)
			std::cerr << results[i] << " ";
		std::cerr << std::endl;
	}
	catch (const Poco::Exception & e)
	{
		std::cerr << e.message() << std::endl;
		throw;
	}
	
	return 0;
}
