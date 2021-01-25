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

#ifndef SOURCETRAIL_NAME_HIERARCHY_H
#define SOURCETRAIL_NAME_HIERARCHY_H

#include <string>
#include <vector>

namespace sourcetrail
{
/**
 * Struct that represents a single hierarchical element that is part of a symbol's name.
 *
 *  prefix: optional prefix used for unique identification and shown in tooltips
 *  name: name represented by this element
 *  postfix: optional prefix used for unique identification and shown in tooltips
 */
struct NameElement
{
	std::string prefix;
	std::string name;
	std::string postfix;
};

/**
 * Struct that represents an entire name of a symbol.
 *
 *  nameDelimiter: delimiter added between name elements
 *  nameElements: all name elements that make up the hierarchy
 */
struct NameHierarchy
{
	std::string nameDelimiter;
	std::vector<NameElement> nameElements;
};

/**
 * Converts a NameHierarchy to a JSON string
 *
 *  param: nameHierarchy - the name hierarchy to convert to JSON string
 *
 *  return: a JSON object of the form:
 *    {
 *      "name_delimiter" : "."
 *      "name_elements" : [
 *        {
 *          "prefix" : "",
 *          "name" : "",
 *          "postfix" : ""
 *        },
 *        ...
 *      ]
 *    }
 */
std::string serializeNameHierarchyToJson(const NameHierarchy& nameHierarchy);

/**
 * Converts a JSON string to a NameHierarchy
 *
 *  param: serializedNameHierarchy - JSON object string of the form:
 *    {
 *      "name_delimiter" : "."
 *      "name_elements" : [
 *        {
 *          "prefix" : "",
 *          "name" : "",
 *          "postfix" : ""
 *        },
 *        ...
 *      ]
 *    }
 *  param: error - optional pointer to a string, where an error message will be set
 *
 *  return: NameHierarchy object. Empty on failure.
 */
NameHierarchy deserializeNameHierarchyFromJson(const std::string& serializedNameHierarchy, std::string* error = nullptr);

/**
 * INTERNAL: Converts a NameHierarchy to a string in Sourcetrail database format
 */
std::string serializeNameHierarchyToDatabaseString(const NameHierarchy& nameHierarchy);
}	 // namespace sourcetrail

#endif	  // SOURCETRAIL_NAME_HIERARCHY_H
