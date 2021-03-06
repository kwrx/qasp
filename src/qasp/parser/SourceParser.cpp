/*                                                                      
 * GPL-3.0 License 
 * 
 * Copyright (C) 2021 Antonino Natale
 * This file is part of QASP.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "SourceParser.hpp"
#include "ParserException.hpp"
#include "../Context.hpp"
#include "../utils/Performance.hpp"

#include <iostream>
#include <fstream>
#include <tuple>
#include <iterator>
#include <string>
#include <sstream>
#include <filesystem>

#include <qasp/qasp.h>


using namespace qasp;
using namespace qasp::parser;


struct Token {

    char tk_type;
    char tk_value;
    uint32_t tk_line;
    uint32_t tk_column;
    const std::string tk_source;

    Token(char type, char value, uint32_t line, uint32_t column, std::string source)
        : tk_type(type)
        , tk_value(value)
        , tk_line(line)
        , tk_column(column)
        , tk_source(std::move(source)) {}

};


#define EXPECT(it, token)       \
    ((*(it)).tk_type == token)

#define VALUE(it)               \
    ((*(it)).tk_value)

#define GOOD(it)                \
    ((it) != tokens.end())



static std::string parseValue(const std::vector<Token>::iterator& it) {

    switch(it->tk_type) {

        case TK_ANNOTATION:
            return "%@";

        case TK_BODY:
            return ":-";

        default:
            return { VALUE(it) };
            
    }

}


static std::vector<Program> parseSources(const std::vector<std::string>& sources, std::vector<Program>& programs, std::optional<Program>& constraint) { __PERF_TIMING(parsing);

    std::vector<Token> tokens;

    auto tokenize = [&](std::istream& fd, const std::string& source) {

        LOG(__FILE__, INFO) << "Reading source from " << source << std::endl;


        uint32_t line = 1;
        uint32_t column = 1;


        while(fd.good()) {

            char ch;
            switch((ch = fd.get())) {

                case EOF:
                    break;

                case '%':

                    if(fd.good() && fd.get() == '@') {
                     
                        tokens.emplace_back(TK_ANNOTATION, 0, line, column++, source);

                        LOG(__FILE__, TRACE) << "<LEXER> Found annotation"
                                             << " in " << source
                                             << " at " << line << ":" << column << std::endl;

                    } else {

                        while(fd.good() && fd.get() != '\n')
                            ;
                        
                        ch = '\n';

                    }

                    break;

                case ':':

                    if(fd.good() && fd.get() == '-') {
                        
                        tokens.emplace_back(TK_BODY, 0, line, column, source);
                        column += 2;

                    } else {

                        tokens.emplace_back(TK_SOURCE, ':', line, column++, source);
                        fd.unget();

                    }
                                   
                    break;

                case '(':

                    tokens.emplace_back(TK_LEFT_PAREN, '(', line, column, source);
                    break;

                case ')':
                    
                    tokens.emplace_back(TK_RIGHT_PAREN, ')', line, column, source);
                    break;

                case '.':
                    
                    tokens.emplace_back(TK_DOT, '.', line, column, source);
                    break;

                case ',':
                    
                    tokens.emplace_back(TK_COMMA, ',', line, column, source);
                    break;

                case 't': // HACK(kwrx): workaround for 'not' keyword

                    if(fd.good() && fd.get() == ' ') {

                        tokens.emplace_back(TK_SOURCE, 't', line, column, source);
                        tokens.emplace_back(TK_SOURCE, ' ', line, column, source);

                        column += 2;

                    } else {

                        tokens.emplace_back(TK_SOURCE, 't', line, column++, source);
                        fd.unget();

                    }

                    break;

                 case ' ':

                    while(fd.good() && (ch = fd.get()) == ' ')
                        column++;

                    fd.unget();
                    break;

                default:
                    tokens.emplace_back(TK_SOURCE, ch, line, column, source);
                    break;
                
            }

            column++;

            if(ch == '\n') {
                column = 1;
                line++;
            }
                
        }

    };


    for(const auto& source : sources) {

        if(source != "-") {

            std::ifstream fd(source, std::ifstream::in);
        
            if(unlikely(fd.fail())) {

                LOG(__FILE__, ERROR) << "Error reading " << source << std::endl;

                throw std::invalid_argument("no such file or directory: " + source);

            }

            tokenize(fd, source);

        } else {
            tokenize(std::cin, "<STDIN>");
        }

    }



    for(auto it = tokens.begin(); it != tokens.end(); it++) {


        switch((*it).tk_type) {
            
            case TK_ANNOTATION: {

                std::ostringstream identifier;
                std::ostringstream source;


#ifdef DEBUG
                auto begin = it;
#endif

                while(GOOD(++it) && EXPECT(it, TK_SOURCE) && isalpha(VALUE(it))) {
                    identifier << VALUE(it);
                }

                if(unlikely(identifier.str().size() == 0))
                    break;


                if(VALUE(it) != '\n') {
                   
                    while(GOOD(++it) && EXPECT(it, TK_SOURCE) && isspace(VALUE(it)) && VALUE(it) != '\n') {
                        // Just skip...
                    }

                    if(VALUE(it) != '\n')
                        break;

                }


                while(GOOD(++it) && !EXPECT(it, TK_ANNOTATION))
                    source << parseValue(it);

                it--;



#ifdef DEBUG
                LOG(__FILE__, TRACE) << "<PARSER> Found annotation with identifier: #" << programs.size() + 1 << " @" << identifier.str()
                                     //<< " and source: \n" << source.str() << "\n"
                                     << " in " << (*begin).tk_source
                                     << " at " << (*begin).tk_line << ":" << (*begin).tk_column << std::endl;
#endif

                
                if(identifier.str() == ANNOTATION_EXISTS)
                    programs.emplace_back(programs.size() + 1, ProgramType::TYPE_EXISTS, source.str());

                else if(identifier.str() == ANNOTATION_FORALL)
                    programs.emplace_back(programs.size() + 1, ProgramType::TYPE_FORALL, source.str());

                else if(identifier.str() == ANNOTATION_CONSTRAINTS)
                    constraint.emplace(programs.size() + 1, ProgramType::TYPE_CONSTRAINTS, source.str());
                    
                else {

#ifdef DEBUG
                    LOG(__FILE__, ERROR) << "Unexpected annotation: " << identifier.str()
                                         << " in " << (*begin).tk_source
                                         << " at " << (*begin).tk_line << ":" << (*begin).tk_column << std::endl;
#endif

                }


            }


            default:
                continue;

        }

        
        throw ParserException((*it).tk_source, (*it).tk_line, (*it).tk_column, VALUE(it)); 

    }


    return programs;

}


Context Parser::parse(const qasp::Options& options) const {

    std::vector<Program> programs;
    std::optional<Program> constraint;

    parseSources(this->sources(), programs, constraint);
    
    
    std::ostringstream source;

    for(const auto& program : programs) {
        
        source << program.source();
        source << std::endl;

    }

    LOG(__FILE__, TRACE) << "Source: " << std::endl 
                         << source.str() << std::endl;



    return Context(source.str(), std::move(programs), std::move(constraint));

}