#include <processing/parse_document.hpp>
#include <parser/parser.hpp>

namespace blitz_query_cpp
{

    bool parse_document::process(query_context &context)
    {
        parser_t parser{context.document};
        if (!parser.parse())
        {
            return context.report_error(parser.get_error_msg());
        }
        return true;
    }

}
