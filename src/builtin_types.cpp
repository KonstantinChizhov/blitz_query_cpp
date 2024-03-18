#include <type_system/type_system_classes.hpp>
#include <type_system/schema.hpp>
#include <type_system/schema_parser.hpp>
#include <iostream>

namespace blitz_query_cpp
{
    std::string_view introspection_schema = R""""(

scalar Int
scalar Float
scalar String
scalar Boolean
scalar Long

directive @skip(if: Boolean!) on FIELD | FRAGMENT_SPREAD | INLINE_FRAGMENT
directive @include(if: Boolean!) on FIELD | FRAGMENT_SPREAD | INLINE_FRAGMENT
directive @deprecated(reason: String = "No longer supported") on FIELD
directive @specifiedBy(url: String!) on SCALAR


scalar UUID @specifiedBy(url: "https://tools.ietf.org/html/rfc4122")

"""
Represents an ISO-8601 compliant date time type.
"""
scalar DateTime  @specifiedBy(url: "https://scalars.graphql.org/andimarek/date-time")


"A GraphQL Schema containes all available types and directives on the server and the entry points for query, mutation, and subscription operations."
type __Schema {
  description: String
  "A list of all types in this schema."
  types: [__Type!]!
  "The root type for query operations."
  queryType: __Type!
  "The root type for mutation operations."
  mutationType: __Type
  "The root type for subscription operations."
  subscriptionType: __Type
  "A list of all directives in this schema."
  directives: [__Directive!]!
}


type __Type {
  kind: __TypeKind!
  name: String
  description: String
  "must be non-null for OBJECT and INTERFACE, otherwise null."
  fields(includeDeprecated: Boolean = false): [__Field!]
  "must be non-null for OBJECT and INTERFACE, otherwise null."
  interfaces: [__Type!]
  "must be non-null for INTERFACE and UNION, otherwise null."
  possibleTypes: [__Type!]
  "must be non-null for ENUM, otherwise null."
  enumValues(includeDeprecated: Boolean = false): [__EnumValue!]
  "must be non-null for INPUT_OBJECT, otherwise null."
  inputFields(includeDeprecated: Boolean = false): [__InputValue!]
  "must be non-null for NON_NULL and LIST, otherwise null."
  ofType: __Type
  "may be non-null for custom SCALAR, otherwise null."
  specifiedByURL: String
}

enum __TypeKind {
  SCALAR
  OBJECT
  INTERFACE
  UNION
  ENUM
  INPUT_OBJECT
  LIST
  NON_NULL
}

type __Field {
  name: String!
  description: String
  args(includeDeprecated: Boolean = false): [__InputValue!]!
  type: __Type!
  isDeprecated: Boolean!
  deprecationReason: String
}

type __InputValue {
  name: String!
  description: String
  type: __Type!
  defaultValue: String
  isDeprecated: Boolean!
  deprecationReason: String
}

type __EnumValue {
  name: String!
  description: String
  isDeprecated: Boolean!
  deprecationReason: String
}

type __Directive {
  name: String!
  description: String
  locations: [__DirectiveLocation!]!
  args(includeDeprecated: Boolean = false): [__InputValue!]!
  isRepeatable: Boolean!
}

enum __DirectiveLocation {
  QUERY
  MUTATION
  SUBSCRIPTION
  FIELD
  FRAGMENT_DEFINITION
  FRAGMENT_SPREAD
  INLINE_FRAGMENT
  VARIABLE_DEFINITION
  SCHEMA
  SCALAR
  OBJECT
  FIELD_DEFINITION
  ARGUMENT_DEFINITION
  INTERFACE
  UNION
  ENUM
  ENUM_VALUE
  INPUT_OBJECT
  INPUT_FIELD_DEFINITION
})"""";

    bool add_introspection_types(schema_t &schema)
    {
        schema_parser_t parser;
        bool res = parser.parse(schema, introspection_schema);
        if(!res)
        {
            std::cerr << "ERROR: Failed to add introspection types: " <<  parser.get_error_msg() << std::endl;
        }
        return res;
    }

}