/*
 * Copyright 2018 Coati Software KG
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
	SourcetrailException(std::string message): m_message(message) {}
	virtual ~SourcetrailException() = default;
	std::string getMessage() const
	{
		return m_message;
	}

private:
	std::string m_message;
};
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_SOURCETRAIL_EXCEPTION_H
