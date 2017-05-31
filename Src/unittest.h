// --------------------------------
// Author: Alain Lanthier, 2017
// --------------------------------
#ifndef _UNITTEST_H
#define _UNITTEST_H

#include <vector>
#include <string>
#include <sstream>

namespace unit_test
{
	template <typename T> class TTest; // Test on a Type

	template <typename T>
	class TTest
	{
	public:
		TTest() {}
		TTest(std::string name) : _name(name) {}

		bool run()
		{
			_status = true;
			_has_except = false;
			for(auto &v : m_checks_def)
			{
				try
				{
					bool result = checkT(v.pT, v.f, v.id);
					v.result = result;
					v.is_except = false;
					if (result == false)  _status = false;
				}
				catch (...)
				{
					v.result = false;
					v.is_except = true;
					_status = false;
					_has_except = true;
				}
			}
			return _status;
		}

		struct check_test_def
		{
			T*          pT;                 // a concrete object of T
			bool        (T::*f)(uint32_t);  // member function of T to test
			uint32_t    id;
			std::string msg_err;
			bool        result;
			bool        is_except;
		};

		void reset() { m_checks_def.clear(); }

		void add(T* p, bool (T::*f)(uint32_t), uint32_t id, std::string msg_err)
		{
			m_checks_def.push_back(check_test_def{ p, f, id, msg_err, true, false});
		}

		size_t num_failed() const
		{
			size_t n = 0;
			for (auto &v : m_checks_def)
			{
				if (v.result == false) n++;
			}
			return n;
		}

		size_t num_except() const
		{
			size_t n = 0;
			for (auto &v : m_checks_def)
			{
				if (v.is_except == true) n++ ;
			}
			return n;
		}

		std::string report(bool detailed = false) const
		{
			std::stringstream s;
			s << "Number check:  " << m_checks_def.size() << "\n";
			s << "Number failed: " << num_failed() << "\n";
			s << "Number except: " << num_except() << "\n";
			if (detailed)
			{
				size_t n = 0;
				for (auto &v : m_checks_def)
				{
					if (v.result == false)
					{
						if (!v.is_except)
							s << "Failed error : (id=" << v.id << ") (msg=" << v.msg_err << ")\n";
						else
							s << "Failed except: (id=" << v.id << ") (msg=" << v.msg_err << ")\n";
					}
				}
			}
			return s.str();
		}

		std::vector<check_test_def> get_checks() const { return m_checks_def; }
		bool status()       const   { return _status };
		bool has_except()   const   { return _has_except; }
		std::string name()  const   { return _name; }

	private:
		bool checkT(T* p, bool (T::*f)(uint32_t), uint32_t id) const
		{
			return (p->*f)(id);
		}

	private:
		std::string _name;
		bool        _status;
		bool        _has_except;
		std::vector<check_test_def> m_checks_def;
	};

}

#endif
