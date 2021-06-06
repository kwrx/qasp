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

#include "Parser.hpp"
#include "ParserException.hpp"
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


#if defined(HAVE_MODE_LOOK_AHEAD)

static std::string parsePredicates(const std::vector<Token>& tokens, std::vector<Token>::iterator& it, Predicates& predicates) {


    #define VALID_NAME(it)          \
        (EXPECT(it, TK_SOURCE) && (isalnum(VALUE(it)) || VALUE(it) == '_'))

    #define VALID_EXTENSIONS(it)    \
        (EXPECT(it, TK_SOURCE) || EXPECT(it, TK_DOT))



    std::ostringstream source;

    auto sign = PREDICATE_POSITIVE;
    auto begin = it;



    do {

        if(EXPECT(it, TK_BODY)) {

            
            do {

                while(GOOD(++it) && isspace(VALUE(it)))
                    ;

                if(unlikely(!VALID_NAME(it)))
                    break; // FIXME: parse aggregate functions



                std::ostringstream name;
                std::ostringstream extensions;


                name << VALUE(it);

                while(GOOD(++it) && VALID_NAME(it)) {
                    name << VALUE(it);
                }

                if(EXPECT(it, TK_LEFT_PAREN)) {

                    while(GOOD(++it) && VALID_EXTENSIONS(it)) {
                        extensions << VALUE(it);
                    }

                    if(unlikely(!EXPECT(it, TK_RIGHT_PAREN))) {

                        LOG(__FILE__, ERROR) << "Expected a RIGHT_PAREN after extensions list" << std::endl;

                        throw ParserException((*it).tk_source, (*it).tk_line, (*it).tk_column, VALUE(it));

                    }

                } else {

                    if(unlikely(!EXPECT(it, TK_DOT) && !EXPECT(it, TK_COMMA))) {

                        if(name.str() == "not") {
                            sign *= PREDICATE_NEGATIVE;
                            continue;
                        }

                        LOG(__FILE__, WARN) << "Expected a LEFT_PAREN, DOT or COMMA after predicate name" << std::endl;
                        break;

                    }

                }

                // FIXME
                // LOG(__FILE__, TRACE) << "<PARSER> Found Predicate with name: " << name.str()
                //                      << " and extensions: (" << extensions.str() << ")"
                //                      << " in " << (*begin).tk_source
                //                      << " at " << (*begin).tk_line << ":" << (*begin).tk_column << std::endl;



                Predicate&& predicate = Predicate(name.str(), extensions.str(), sign);

                if(!predicates.contains(predicate))
                    predicates.emplace_back(predicate);
                
            


                sign = PREDICATE_POSITIVE;


                if(EXPECT(it, TK_COMMA))
                    continue;

                else if(EXPECT(it, TK_DOT))
                    break;

                else {

                    LOG(__FILE__, ERROR) << "Expected a DOT or COMMA after predicate" << std::endl;
                    break;

                }


            } while(true);

        }

    } while(0);


    for(; begin != it; begin++) {
        source << parseValue(begin);
    }
        
    source << parseValue(begin);

    return source.str();

}

#endif

static std::vector<Program> parseSources(const std::vector<std::string>& sources) { __PERF_TIMING(parsing);

    std::vector<Token> tokens;
    std::vector<Program> programs;


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
                Predicates predicates;


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


                while(GOOD(++it) && !EXPECT(it, TK_ANNOTATION)) {

#if defined(HAVE_MODE_LOOK_AHEAD)
                    source << parsePredicates(tokens, it, predicates);
#else
                    source << parseValue(it);
#endif

                }

                it--;



#ifdef DEBUG
                LOG(__FILE__, TRACE) << "<PARSER> Found annotation with identifier: #" << programs.size() + 1 << " @" << identifier.str()
                                     //<< " and source: \n" << source.str() << "\n"
                                     << " in " << (*begin).tk_source
                                     << " at " << (*begin).tk_line << ":" << (*begin).tk_column << std::endl;
#endif

                
                if(identifier.str() == ANNOTATION_EXISTS)
                    programs.emplace_back(programs.size() + 1, ProgramType::TYPE_EXISTS, source.str(), predicates);
                else if(identifier.str() == ANNOTATION_FORALL)
                    programs.emplace_back(programs.size() + 1, ProgramType::TYPE_FORALL, source.str(), predicates);
                else if(identifier.str() == ANNOTATION_CONSTRAINTS)
                    programs.emplace_back(programs.size() + 1, ProgramType::TYPE_CONSTRAINTS, source.str(), predicates);
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


Program Parser::parse(const qasp::Options& options) const {

    std::vector<Program> programs = parseSources(this->sources());
    std::ostringstream source;

    for(const auto& program : programs) {
        
        source << program.source();
        source << std::endl;

    }

    LOG(__FILE__, TRACE) << "Source: " << std::endl 
                         << source.str() << std::endl;



    for(auto it = programs.rbegin(); it != programs.rend(); it++) {

        if(unlikely(it->type() == ProgramType::TYPE_CONSTRAINTS))
            continue;

        it->last(true);
        break;

    }


    return Program(-1, ProgramType::TYPE_COMMON, source.str(), {}, programs);

}