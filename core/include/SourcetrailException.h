#ifndef SOURCETRAIL_SOURCETRAIL_EXCEPTION_H
#define SOURCETRAIL_SOURCETRAIL_EXCEPTION_H

#include <string>

namespace sourcetrail
{
	/**
	 * Exception type that is thrown by the SourcetrailDBWriter API.
	 */
	class SourcetrailException
	{
	public:
		SourcetrailException(std::string message) : m_message(message) {}
		virtual ~SourcetrailException() = default;
		std::string getMessage() const { return m_message; }

	private:
		std::string m_message;
	};
}

#endif // SOURCETRAIL_SOURCETRAIL_EXCEPTION_H
