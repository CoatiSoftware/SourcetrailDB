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

#ifndef SOURCETRAIL_ELEMENT_COMPONENT_KIND_H
#define SOURCETRAIL_ELEMENT_COMPONENT_KIND_H

namespace sourcetrail
{
/**
 * Enum providing all possible values for kinds of node and edge components that can be stored to the Sourcetrail database.
 */
enum class ElementComponentKind : int
{
	IS_AMBIGUOUS = 1 << 0
};

int elementComponentKindToInt(ElementComponentKind kind);
ElementComponentKind intToElementComponentKind(int i);
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_ELEMENT_COMPONENT_KIND_H
